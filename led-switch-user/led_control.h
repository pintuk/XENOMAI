/* led_control.h
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

#ifndef __LED_CONTROL_H
#define __LED_CONTROL_H


#define LED_RED_PIN	17
#define LED_GREEN_PIN	21


void led_init(void);
void led_uninit(void);
void led_write(int num, int value);


#endif
