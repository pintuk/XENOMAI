/* gpioutils.h
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

#ifndef __GPIOUTILS_H
#define __GPIOUTILS_H


int gpio_export(int num);
int gpio_unexport(int num);
int gpio_direction(int num, int dir);
int gpio_set_value(int num, int value);
int gpio_get_value(int num, int *value);

#endif
