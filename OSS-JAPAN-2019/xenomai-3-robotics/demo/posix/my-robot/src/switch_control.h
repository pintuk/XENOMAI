/* switch_control.h
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

#ifndef __SWITCH_CONTROL_H
#define __SWITCH_CONTROL_H
#include <pthread.h>

pthread_t switch_tid;

#define SWITCH_PIN 12


void switch_init(void);
void switch_uninit(void);
void switch_read(int *s);


#endif

