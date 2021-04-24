#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/init.h>
#include <linux/gpio.h>         // gpio macros
#include <linux/interrupt.h>
#include <linux/cpumask.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/err.h>

#define GPIO_LED	17
#define GPIO_PROXI	18

/* 10ms delay */
#define WORK_INTERVAL       (1*HZ/100)

static unsigned int ledon = 0;
static unsigned int irqno;
static int counter = 0;
static int sensor_value = 0;

static DEFINE_SPINLOCK(slock);

static void proxi_sensor_work_handler(struct work_struct *work)
{
	unsigned long flags;
	spin_lock_irqsave(&slock, flags);
	if (sensor_value == 0) {
		ledon = 1;
		gpio_set_value(GPIO_LED, ledon);
	} else {
		ledon = 0;
		gpio_set_value(GPIO_LED, ledon);
		counter = 0;
	}
	pr_info("[proxi-sensor]: %s: Signal! sensor_val: %d, ledon : %d\n", __func__, sensor_value, ledon);
	spin_unlock_irqrestore(&slock, flags);
}
static DECLARE_DELAYED_WORK(proxi_work, proxi_sensor_work_handler);

static irq_handler_t proxi_sensor_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned long flags;

	spin_lock_irqsave(&slock, flags);
	counter++;
	sensor_value = gpio_get_value(GPIO_PROXI);
	spin_unlock_irqrestore(&slock, flags);

	schedule_delayed_work(&proxi_work, (unsigned long)WORK_INTERVAL);
	//pr_info("[proxi-sensor]: %s: counter: %d\n", __func__, counter);
	//spin_unlock_irqrestore(&slock, flags);

	return (irq_handler_t) IRQ_HANDLED;
}

static void gpio_init(int gpiono, const char *name, bool type)
{
	int ret;

	gpio_request(gpiono, name);
	if (type == 0) {
		gpio_direction_input(gpiono);
		ret = gpio_set_debounce(gpiono, 200);
		if (ret) {
			pr_info("[proxi-sensor]: %s: gpio_set_debounce ret: %d\n", __func__, ret);
		}
	} else {
		gpio_direction_output(gpiono, false);
	}
	gpio_export(gpiono, false);
}

static void gpio_uninit(int gpiono)
{
	gpio_unexport(gpiono);
	gpio_set_value(gpiono, 0);
	gpio_free(gpiono);
}

static int __init gpio_proxi_sensor_init(void)
{
	int ret;

	gpio_init(GPIO_PROXI, "proxi-sensor-gpio", 0);
	gpio_init(GPIO_LED, "proxi-led-gpio", 1);
	ledon = 0;

	irqno = gpio_to_irq(GPIO_PROXI);
	pr_info("[proxi-sensor]: %s: GPIO(%d) set to IRQ(%d)\n", __func__, (int)GPIO_PROXI, irqno);

	ret = request_irq(irqno, (irq_handler_t) proxi_sensor_irq_handler,
			IRQF_TRIGGER_RISING | IRQF_ONESHOT, "proxi-sensor-gpio", NULL);
	if (ret < 0) {
		pr_err("[proxi-sensor]: %s: request_irq failed, ret = %d\n", __func__, ret);
		return -1;
	}
	pr_info("[proxi-sensor]: %s: DONE!\n", __func__);

	return 0;
}

static void __exit gpio_proxi_sensor_exit(void)
{
	gpio_uninit(GPIO_PROXI);
	gpio_uninit(GPIO_LED);
	free_irq(irqno, NULL);
	cancel_delayed_work_sync(&proxi_work);
	pr_info("[proxi-sensor]: %s: DONE!\n", __func__);
}
module_init(gpio_proxi_sensor_init);
module_exit(gpio_proxi_sensor_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pintu Kumar");
MODULE_DESCRIPTION("GPIO based proximity sensor Driver");
