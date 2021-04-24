/* ldr_control.c
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
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "gpioutils.h"
#include "ldr_control.h"

#define LDR_DARK_THRESHOLD	2000

volatile int terminate_ldr = 0;

void *ldr_read_func(void *arg)
{
	int v = 0;

	while (!terminate_ldr) {
	       ldr_read(&v);
		if (v > LDR_DARK_THRESHOLD)
			fprintf(stderr, "Too dark: %d\n",v);
		sleep(1);
	}
	usleep(100 * 1000);

	return NULL;
}

void ldr_init(void)
{
	int ret;

	terminate_ldr = 0;

	gpio_export(LDR_PIN);
	ret =  pthread_create(&ldr_tid, NULL, ldr_read_func, NULL);
	if (ret < 0) {
		fprintf(stderr, "%s: pthread_create error: %s\n",
		__func__, (char *)strerror(errno));
	}

}

void ldr_uninit(void)
{
	terminate_ldr = 1;
	pthread_detach(ldr_tid);
	gpio_unexport(LDR_PIN);
}

void ldr_read(int *s)
{
	int count = 0;
	gpio_direction(LDR_PIN, 1);
	gpio_set_value(LDR_PIN, 0);
	usleep(100000);
	gpio_direction(LDR_PIN, 0);
	while (1) {
		int v = 0;
		gpio_get_value(LDR_PIN, &v);
		if (v == 0)
			count++;
		else
			break;
	}
	*s = count;
}
