/* bt_serial.h
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

#ifndef __BT_SERIAL_H
#define __BT_SERIAL_H

#include <termios.h>

#define DEV_NAME        "/dev/ttyS0"
#define BAUDRATE        B9600

void serial_init(int *fd, struct termios *old);
void serial_uninit(int fd, struct termios old);


#endif

