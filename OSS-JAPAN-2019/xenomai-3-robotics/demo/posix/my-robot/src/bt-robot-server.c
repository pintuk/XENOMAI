/* bt-robot-server.c
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
#include "motor_control.h"
#include "bt_serial.h"
#include "proxi_sensor.h"
#include "ultrasonic_sensor.h"
#include "led_control.h"
#include "switch_control.h"
//#include "ldr_control.h"

void disconnect(void);

void disconnect(void)
{
	//proxi_uninit();
	//ldr_uninit();
	ultra_uninit();
	motor_uninit();
	led_uninit();
	system("sudo poweroff");
}

void connect(void)
{
	led_init();
	motor_init();
	//proxi_init();
//	ldr_init();
	ultra_init();
}

int main(void)
{
	int fd = -1;
	char data[255];
	struct termios old;
	int ret = -1;
	int isconnected = 0;
	char val;

	serial_init(&fd, &old);
	if (fd < 0) {
		fprintf(stderr, "ERROR: serial_init failed\n");
		return -1;
	}
#if 1
	connect();
	isconnected = 1;

	/* Turn on RED LED */
	led_write(LED_RED_PIN, 1);
	//led_write(LED_GREEN_PIN, 0);
	//switch_init();

	while (1) {
		char val;

		ret = read(fd, &data, 255);
		if (ret < 0) {
			fprintf(stderr, "ERROR: %s: read failed\n", __func__);
			continue;
		}

		val = data[0];
		if ((val == 'f' || val == 'F') && isconnected == 0) {
			printf("Robot connected....\n");
			connect();
			isconnected = 1;
			/* Turn on GREEN LED */
			led_write(LED_RED_PIN, 1);
			//led_write(LED_GREEN_PIN, 1);
		} else if ((val == 'g' || val == 'G') && isconnected == 1) {
			printf("Robot disconnected....\n");
			/* Turn on RED LED */
			led_write(LED_RED_PIN, 0);
			//led_write(LED_GREEN_PIN, 0);
			disconnect();
			isconnected = 0;
		} else {
			if (isconnected == 1 && val != 'F' && val != 'G') {
				//printf("Command send to motor : %c\n", val);
				send_to_motor(val);
			}
		}
	}
#endif
	if (isconnected == 1)
		disconnect();

	//switch_uninit();
	serial_uninit(fd, old);
	return 0;
}
