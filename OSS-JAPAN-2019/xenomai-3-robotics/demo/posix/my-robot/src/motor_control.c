/* motor_control.c
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
#include <unistd.h>
#include "gpioutils.h"
#include "motor_control.h"

void motor_stop(void)
{
	gpio_set_value(MOTORA_1, 0);
	gpio_set_value(MOTORA_2, 0);
	gpio_set_value(MOTORB_1, 0);
	gpio_set_value(MOTORB_2, 0);
	//printf("Motor Stopped....\n");
}

void send_to_motor(char c)
{
	switch (c) {
	case 'A':
	case 'a':
		motor_stop();
		gpio_set_value(MOTORA_1, 1);
		gpio_set_value(MOTORA_2, 0);
		gpio_set_value(MOTORB_1, 1);
		gpio_set_value(MOTORB_2, 0);
		//printf("Moving Forward....\n");
		break;

	case 'B':
	case 'b':
		motor_stop();
		gpio_set_value(MOTORA_1, 0);
		gpio_set_value(MOTORA_2, 1);
		gpio_set_value(MOTORB_1, 0);
		gpio_set_value(MOTORB_2, 1);
		//printf("Moving Back....\n");
		break;

	case 'C':
	case 'c':
		motor_stop();
		gpio_set_value(MOTORA_1, 0);
		gpio_set_value(MOTORA_2, 0);
		gpio_set_value(MOTORB_1, 1);
		gpio_set_value(MOTORB_2, 0);
		//printf("Moving Left....\n");
		//Delay for 200ms after left movement and then stop
		usleep(200 * 1000);
		motor_stop();
		break;

	case 'D':
	case 'd':
		motor_stop();
		gpio_set_value(MOTORA_1, 1);
		gpio_set_value(MOTORA_2, 0);
		gpio_set_value(MOTORB_1, 0);
		gpio_set_value(MOTORB_2, 0);
		printf("Moving Right....\n");
		//Delay for 200ms after right movement and then stop
		usleep(200 * 1000);
		motor_stop();
		break;

	case 'E':
	case 'e':
		//printf("Stopping....\n");
		motor_stop();
		break;
	}
}

void motor_init()
{
	gpio_export(MOTORA_1);
	gpio_direction(MOTORA_1, 1);

	gpio_export(MOTORA_2);
	gpio_direction(MOTORA_2, 1);

	gpio_export(MOTORB_1);
	gpio_direction(MOTORB_1, 1);

	gpio_export(MOTORB_2);
	gpio_direction(MOTORB_2, 1);

	printf("motor init - done\n");
}


void motor_uninit()
{
	motor_stop();

	gpio_unexport(MOTORA_1);

	gpio_unexport(MOTORA_2);

	gpio_unexport(MOTORB_1);

	gpio_unexport(MOTORB_2);
	printf("motor uninit - done\n");
}

