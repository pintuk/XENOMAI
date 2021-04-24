#include <linux/module.h>
#include <linux/printk.h>
#include <linux/kobject.h>
#include <linux/init.h>
#include <linux/gpio.h>		// gpio macros
#include <linux/interrupt.h>
#include <linux/cpumask.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/err.h>

#define GPIO_LED	17
#define GPIO_SWITCH	18

#define WORK_INTERVAL       (50*HZ/100)

static unsigned int ledon = 0;
static unsigned int irqno;
static int counter = 0;
static int switch_value = 0;
//volatile int pressed = 0;

static DEFINE_SPINLOCK(slock);

#if 0
static void led_switch_tasklet_handler(unsigned long val)
{
	unsigned long flags;

	spin_lock_irqsave(&slock, flags);
	ledon = !ledon;
	gpio_set_value(GPIO_LED, ledon);
	pr_info("[led-switch]: %s: ledon = %d\n", __func__, ledon);
	spin_unlock_irqrestore(&slock, flags);
}
static DECLARE_TASKLET(my_tasklet, led_switch_tasklet_handler, (unsigned long)&taskdata);
#endif

static void led_switch_work_handler(struct work_struct *work)
{
	unsigned long flags;
	spin_lock_irqsave(&slock, flags);
	ledon = !ledon;
	if (switch_value == 1) {
		gpio_set_value(GPIO_LED, ledon);
		counter = 0;
	} else {
		//ledon = 0;
		//gpio_set_value(GPIO_LED, ledon);
		counter = 0;
	}
	pr_info("[led-switch]: %s: Signal! switch_val: %d, ledon : %d\n", __func__, switch_value, ledon);
	spin_unlock_irqrestore(&slock, flags);
}
static DECLARE_DELAYED_WORK(switch_work, led_switch_work_handler);

static irq_handler_t led_switch_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned long flags;

	spin_lock_irqsave(&slock, flags);
	counter++;
	switch_value = gpio_get_value(GPIO_SWITCH);
	pr_info("[led-switch]: %s: interrupt counter: %d\n", __func__, counter);
	spin_unlock_irqrestore(&slock, flags);
	//tasklet_schedule(&my_tasklet);
	schedule_delayed_work(&switch_work, (unsigned long)WORK_INTERVAL);

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
			pr_info("[led-switch]: %s: gpio_set_debounce ret: %d\n", __func__, ret);
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
	cancel_delayed_work_sync(&switch_work);
	pr_info("[led-switch]: %s: DONE!\n", __func__);
}

static int __init gpio_switch_led_init(void)
{
	int ret;

	gpio_init(GPIO_SWITCH, "switch-gpio", 0);
	gpio_init(GPIO_LED, "led-gpio", 1);
	ledon = 0;

	irqno = gpio_to_irq(GPIO_SWITCH);
	pr_info("[led-switch]: %s: GPIO(%d) set to IRQ(%d)\n", __func__, (int)GPIO_SWITCH, irqno);

	ret = request_irq(irqno, (irq_handler_t) led_switch_irq_handler,
			IRQF_TRIGGER_RISING | IRQF_ONESHOT, "led-switch-gpio", NULL);
	if (ret < 0) {
		pr_err("[led-switch]: %s: request_irq failed, ret = %d\n", __func__, ret);
		return -1;
	}
	pr_info("[led-switch]: %s: DONE!\n", __func__);

	return 0;
}

static void __exit gpio_switch_led_exit(void)
{
	gpio_uninit(GPIO_SWITCH);
	gpio_uninit(GPIO_LED);
	free_irq(irqno, NULL);
	pr_info("[led-switch]: %s: DONE!\n", __func__);
}
module_init(gpio_switch_led_init);
module_exit(gpio_switch_led_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Pintu Kumar");
MODULE_DESCRIPTION("GPIO Switch Controlled LED Driver");
