/* ultrasonic_sensor.h
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

#ifndef __ULTRA_SENSOR_H
#define __ULTRA_SENSOR_H

#include <pthread.h>


#define ECHO_PORT	6
#define TRIGGER_PORT	5

pthread_t ultra_tid;

void ultra_init(void);
void ultra_uninit(void);
int ultra_read(int gpio);
void ultra_write(int gpio, int v);

#endif
