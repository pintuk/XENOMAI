/* led_control.c
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
#include "gpioutils.h"
#include "led_control.h"


void led_init(void)
{
	gpio_export(LED_RED_PIN);
	gpio_direction(LED_RED_PIN, 1);

	//gpio_export(LED_GREEN_PIN);
	//gpio_direction(LED_GREEN_PIN, 1);
}

void led_uninit(void)
{
	gpio_unexport(LED_RED_PIN);
	//gpio_unexport(LED_GREEN_PIN);
}

void led_write(int num, int value)
{
	gpio_set_value(num, value);
}

