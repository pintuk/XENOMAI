/* switch_control.c
 *
 * Copyright (c) 2017 Samsung Electronics, Inc.
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
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
//#include <linux/reboot.h>
//#include <sys/reboot.h>
#include "gpioutils.h"
#include "switch_control.h"
//#include "motor_control.h"
#include "led_control.h"
//#include "proxi_sensor.h"


volatile int terminate_switch = 0;
volatile int ledon = 0;
pthread_mutex_t mlock;

void *switch_read_func(void *arg)
{
        int v = 0;
	int on = 0;

        while (!terminate_switch) {
		pthread_mutex_lock(&mlock);
               switch_read(&v);
                if (v == 1) {
			printf("Switch pressed...\n");
			//led_write(LED_RED_PIN, 0);
			ledon = !ledon;
			led_write(LED_RED_PIN, ledon);
			if (ledon == 1)
				printf("RED LED - ON\n");
			else
				printf("RED LED - OFF\n");
                } else {
			//led_write(LED_RED_PIN, 0);
		}
		usleep(10*1000);
		pthread_mutex_unlock(&mlock);
        }

	return NULL;
}

void switch_init(void)
{
	int ret;

	terminate_switch = 0;

        gpio_export(SWITCH_PIN);
        gpio_direction(SWITCH_PIN, 0);
	pthread_mutex_init(&mlock, NULL);
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
