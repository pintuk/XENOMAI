/* ldr_control.h
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

#ifndef __LDR_CONTROL_H
#define __LDR_CONTROL_H
#include <pthread.h>

pthread_t ldr_tid;

#define LDR_PIN		4

void ldr_init(void);
void ldr_uninit(void);
void ldr_read(int *v);


#endif
