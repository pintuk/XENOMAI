/* proxi_sensor.c
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
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include <sys/mman.h>
#include "gpioutils.h"
#include "ultrasonic_sensor.h"
#include "motor_control.h"
#include "led_control.h"


#define NO_OF_SAMPLES	10000
#define MAX_THREADS	1
/* Task sleep duration in milli-seconds */
#define SLEEP_DURATION  (100)
#define NANO_SECOND     1000000000ULL
#define MICRO_SECOND    1000000.00
#define MILLI_SECOND    1000.00

#define TO_MILLI_SEC(x)    ((x) / MICRO_SECOND)
#define TO_MICRO_SEC(x)    ((x) / MILLI_SECOND)

volatile int terminate_ultra = 0;

struct task_data {
        float *latency;
        float min_latency;
        float max_latency;
        float avg_latency;
} data[MAX_THREADS];

static inline long double timediff(struct timespec t1, struct timespec t2)
{
        long double diff = 0.00; 
	long double val1, val2;

	val1 = t1.tv_sec * NANO_SECOND + t1.tv_nsec;
	val2 = t2.tv_sec * NANO_SECOND + t2.tv_nsec;
	diff = (val2 - val1) / 1000.00;

	//diff = TO_MICRO_SEC(diff);

        return diff;
}

static void show_latency(int n)
{
        int i, j, taskno = 0;
	FILE *fp = NULL;

	fp = fopen("/home/pi/PINTU/ROBOT/xenomai-3-robotics/demo/posix/my-robot/latency_output.txt", "w+");
	if (fp == NULL) {
		printf("show_latency: Could not create file: latency_output.txt\n");
		return;
	}
	//printf("show_latency: file created: latency_output.txt\n");
	fprintf(fp, "Task: %d, Iteration: %d, Sleep duration: %d ms\n", taskno, n,
                                        (int)SLEEP_DURATION);

        for (i = 0; i < MAX_THREADS; i++) {
                for (j = 1; j <= n; j++) {
                        float latency;

                        if (data[i].latency == NULL)
                                continue;

                        latency = data[i].latency[j] - (SLEEP_DURATION);
                        fprintf(fp, "%d %5.3f %5.3f\n",
                                        j, data[i].latency[j], latency);
                }
                fprintf(fp, "Task[%d] - Min Latency: %5.3f ms\n",
                                        i, data[i].min_latency);
                fprintf(fp, "Task[%d] - Max Latency: %5.3f ms\n",
                                        i, data[i].max_latency);
                fprintf(fp, "Task[%d] - Avg Latency: %5.3f ms\n",
                                        i, data[i].avg_latency);
                fprintf(fp, "\n");

                //if (data[i].latency != NULL)
                  //      free(data[i].latency);
        }
	fclose(fp);
}

static long int calc_dist(void)
{
	struct timespec  start, end;
	long int dist = 0;
	int value;
	float diff;

	while (ultra_read(ECHO_PORT) == 0);

	clock_gettime(CLOCK_MONOTONIC, &start);
	while (ultra_read(ECHO_PORT) == 1);
	clock_gettime(CLOCK_MONOTONIC, &end);
	diff = timediff(start, end);
	//printf("diff = %f\n", diff);
	dist = (long int) (diff / 58);

	return dist;
}

int check_obstacle(long int *dist)
{
	float diff;
	struct timespec tm, start, end;

	ultra_write(TRIGGER_PORT, 1);
	//usleep(20);
	tm.tv_sec = 0;
	tm.tv_nsec = (20 * MILLI_SECOND);
	//clock_gettime(CLOCK_MONOTONIC, &start);
	clock_nanosleep(CLOCK_MONOTONIC, 0, &tm, NULL);
	//clock_gettime(CLOCK_MONOTONIC, &end);
	ultra_write(TRIGGER_PORT, 0);

	*dist = calc_dist();
	if (*dist <= 50) {
		//diff = timediff(start, end);
		//fprintf(stderr, "obstacle  dist: ultra diff: %3.2f us\n", diff);
		return 1;
	}

	return 0;
}

void *ultra_read_func(void *arg)
{
	int ret = 0;
	char val = 'e';
	struct timespec  start = {0,}, end = {0,}, tm;
	struct timespec t1, t2, t3;
	long double total_diff = 0.00, diff = 0.00, diff1 = 0.00, diff2 = 0.00;
	float max, min, avg_latency, max_latency, min_latency;
	long int dist = 0;
	static long int counter = 1;
	volatile int first_detected = 0;

	max = (float)INT_MIN;
	min = (float)INT_MAX;
	while (!terminate_ultra) {
		//clock_gettime(CLOCK_MONOTONIC, &start);
		ret = check_obstacle(&dist);
		//clock_gettime(CLOCK_MONOTONIC, &end);
		if (ret == 1) {
			send_to_motor(val);
			clock_gettime(CLOCK_MONOTONIC, &t3);
			diff1 = timediff(t1, t3);
			//diff2 = timediff(start, end);
			//fprintf(stderr, "obstacle detected by robot: dist: %ld cm [%ld us]\n", dist, (long)(dist*58));
			fprintf(stderr, "obstacle detected by robot: dist: %ld cm, dect_time: %5.3f ms\n", dist, (float)(diff1/1000.000));
			first_detected = 1;
		}
		tm.tv_sec = 0;
		tm.tv_nsec = (100 * MICRO_SECOND);
		clock_gettime(CLOCK_MONOTONIC, &t1);
		clock_nanosleep(CLOCK_MONOTONIC, 0, &tm, NULL);
		clock_gettime(CLOCK_MONOTONIC, &t2);
		if (first_detected == 1) {
			diff = timediff(t1, t2);
			diff = diff/1000.00;
			if (diff > max)
				max = diff;
			if (diff < min)
				min = diff;

			total_diff += diff;
			data[0].latency[counter] = diff;
			//usleep(100 * 1000);
			if (counter >= NO_OF_SAMPLES) {
				avg_latency = (total_diff / (float)NO_OF_SAMPLES) - (SLEEP_DURATION);
				//printf("avg_latency: %5.3f ; total_diff: %5.3f\n", avg_latency, total_diff);
				data[0].avg_latency = avg_latency;
				max_latency = max - (SLEEP_DURATION);
				data[0].max_latency = max_latency;
				min_latency = min - (SLEEP_DURATION);
				data[0].min_latency = min_latency;
				show_latency(NO_OF_SAMPLES);
				counter = 1;
				total_diff = 0;
			} else {
				counter++;
			}
		}
	}
	return NULL;
}


void ultra_init()
{
	int ret, i = 0;
	struct sched_param param;
	pthread_attr_t attr;

	gpio_export(ECHO_PORT);
	gpio_export(TRIGGER_PORT);
	gpio_direction(ECHO_PORT, 0);
	gpio_direction(TRIGGER_PORT, 1);

	memset(&attr, 0, sizeof(attr));
	memset(&param, 0, sizeof(param));

        /* Set thread attributes to set specific policy */
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
	pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
	pthread_attr_setschedparam(&attr, &param);
	param.sched_priority = 99;
	pthread_attr_setschedparam(&attr, &param);

	data[0].latency = (float *)malloc(sizeof(float)*NO_OF_SAMPLES);
	if (data[0].latency == NULL) {
		printf("ultra_init: Cannot allocate memory for: data[%d]\n", i);
		return;
	}

	ultra_write(TRIGGER_PORT, 0);
	sleep(2);
	terminate_ultra = 0;

	ret =  pthread_create(&ultra_tid, &attr, ultra_read_func, NULL);
	if (ret < 0) {
		fprintf(stderr, "%s: pthread_create error: %s\n",
		__func__, (char *)strerror(errno));
	}
}

void ultra_uninit()
{
	terminate_ultra = 1;
	pthread_detach(ultra_tid);
	if (data[0].latency != NULL)
		free(data[0].latency);

	gpio_unexport(TRIGGER_PORT);
	gpio_unexport(ECHO_PORT);
}

int ultra_read(int gpio)
{
	int value = 0;
	gpio_get_value(ECHO_PORT, &value);
	return value;
}

void ultra_write(int gpio, int v)
{
	gpio_set_value(gpio, v);
}
