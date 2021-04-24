/* switch_control.c
 *
 * Copyright (c) 2019 SONY Electronics, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <linux/reboot.h>
#include <sys/reboot.h>
#include "gpioutils.h"
#include "switch_control.h"
#include "motor_control.h"
#include "led_control.h"
#include "proxi_sensor.h"

extern void disconnect(void);

volatile int terminate_switch = 0;

void *switch_read_func(void *arg)
{
        int v = 0;

        while (!terminate_switch) {
               switch_read(&v);
                if (v == 1) {
			fprintf(stderr, "Switch enabled, turning off robot\n");
			sync();
			disconnect();
			system("sudo poweroff");
			//led_uninit();
			//reboot(LINUX_REBOOT_CMD_POWER_OFF);
			//printf("after reboot\n");
                }
		sleep(1);
        }

	return NULL;
}

void switch_init(void)
{
	int ret;

	terminate_switch = 0;

        gpio_export(SWITCH_PIN);
        gpio_direction(SWITCH_PIN, 0);
        ret =  pthread_create(&switch_tid, NULL, switch_read_func, NULL);
        if (ret < 0) {
                fprintf(stderr, "%s: pthread_create error: %s\n",
                __func__, (char *)strerror(errno));
        }

}

void switch_uninit(void)
{
	terminate_switch = 1;
	pthread_detach(switch_tid);
        gpio_unexport(SWITCH_PIN);
}

void switch_read(int *s)
{
        gpio_get_value(SWITCH_PIN, s);
}
