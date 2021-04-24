/* gpioutils.c
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
 * MERCHANTABILITY or FITNESS FOR A PARTI
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int gpio_export(int num)
{
	char buf[64];
	int fd;
	int ret;

	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to export gpio: %d\n", num);
		return -1;
	}
	sprintf(buf, "%d", num);
	ret = write(fd, buf, strlen(buf));
	if (ret < 0) {
		fprintf(stderr, "%s: Failed to write\n", __func__);
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int gpio_unexport(int num)
{
	char buf[255];
	int fd;
	int ret;

	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to unexport gpio: %d\n", num);
		return -1;
	}
	sprintf(buf, "%d", num);
	ret = write(fd, buf, strlen(buf));
	if (ret < 0) {
		fprintf(stderr, "%s: Failed to write\n", __func__);
		close(fd);
		return -1;
	}
	close(fd);

	return 0;
}

int gpio_direction(int num, int dir)
{
	char path[64];
	int fd;
	int ret;

	sprintf(path, "/sys/class/gpio/gpio%d/direction", num);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to set dir: %d for gpio: %d\n",
						dir, num);
		return -1;
	}
	if (dir == 0) {
		ret = write(fd, "in", 2);
		if (ret < 0) {
			fprintf(stderr, "%s: Failed to write\n", __func__);
			close(fd);
			return -1;
		}
	} else {
		ret = write(fd, "out", 3);
		if (ret < 0) {
			fprintf(stderr, "%s: Failed to write\n", __func__);
			close(fd);
			return -1;
		}
	}

	close(fd);

	return 0;
}

int gpio_set_value(int num, int value)
{
	char path[64];
	char buf[255];
	int fd;
	int ret;

	sprintf(path, "/sys/class/gpio/gpio%d/value", num);
	fd = open(path, O_WRONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to set value: %d for gpio: %d\n",
						value, num);
		return -1;
	}
	sprintf(buf, "%d", value);
	ret = write(fd, buf, strlen(buf));
	if (ret < 0) {
		fprintf(stderr, "%s: Failed to write\n", __func__);
		close(fd);
		return -1;
	}
	
	close(fd);

	return 0;
}

int gpio_get_value(int num, int *value)
{
	char path[64];
	char buf[255];
	int fd;
	int ret;

	sprintf(path, "/sys/class/gpio/gpio%d/value", num);
	fd = open(path, O_RDONLY);
	if (fd < 0) {
		fprintf(stderr, "Failed to get value for gpio: %d\n", num);
		fprintf(stderr, "%s: ERROR: %s\n", __func__, strerror(errno));
		return -1;
	}
	ret = read(fd, buf, 255);
	if (ret < 0) {
		fprintf(stderr, "%s: Failed to read\n", __func__);
		close(fd);
		return -1;
	}
	*value = atoi(buf);
	close(fd);

	return 0;
}

