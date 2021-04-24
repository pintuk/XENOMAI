/* proxi_sensor.c
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
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "gpioutils.h"
#include "proxi_sensor.h"
#include "motor_control.h"
#include "led_control.h"


volatile int terminate_proxi = 0;

void *proxi_read_func(void *arg)
{
	int v = -1;
	char val = 'b';

	while (!terminate_proxi) {
		proxi_read(&v);
		if (v == 0) {
			fprintf(stderr, "obstacle detected by robot: v: %d\n", v);
			send_to_motor(val);
			v = 1;
			sleep(1);
		}
		usleep(100 * 1000);
	}
	return NULL;
}


void proxi_init()
{
	int ret;

	gpio_export(PROXI_PORT);
	gpio_direction(PROXI_PORT, 0);

	terminate_proxi = 0;

	ret =  pthread_create(&proxi_tid, NULL, proxi_read_func, NULL);
	if (ret < 0) {
		fprintf(stderr, "%s: pthread_create error: %s\n",
		__func__, (char *)strerror(errno));
	}
}

void proxi_uninit()
{
	terminate_proxi = 1;
	pthread_detach(proxi_tid);
	gpio_unexport(PROXI_PORT);
}

void proxi_read(int *v)
{
	gpio_get_value(PROXI_PORT, v);
}

