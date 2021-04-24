/* motor_control.h
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

#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H

#define MOTORA_1        17
#define MOTORA_2        18
#define MOTORB_1        22
#define MOTORB_2        23

void motor_init(void);
void motor_uninit(void);
void motor_stop(void);
void send_to_motor(char c);


#endif
