/* bt_serial.c
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
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include "bt_serial.h"


void serial_init(int *fd, struct termios *old)
{
	struct termios newtio;

	*fd = open(DEV_NAME, O_RDWR | O_NOCTTY);
	if (*fd < 0) {
		fprintf(stderr, "ERROR: %s: open failed: %s\n",
			__func__, strerror(errno));
		return;
	}
	tcgetattr(*fd, old);
	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;
	newtio.c_lflag = 0;
	newtio.c_cc[VTIME] = 0;
	newtio.c_cc[VMIN] = 1;
	tcflush(*fd, TCIFLUSH);
	tcsetattr(*fd, TCSANOW, &newtio);
}

void serial_uninit(int fd, struct termios old)
{
	tcsetattr(fd, TCSANOW, &old);
	close(fd);
}
