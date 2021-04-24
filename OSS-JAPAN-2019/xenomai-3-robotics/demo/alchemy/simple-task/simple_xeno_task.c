/*
 * simple_xeno_task.c
 *
 * This is a simple userspace test program to demonstrate latency measurement
 * across real-time task, using Xenomai alchemy skin.
 * It supports the following functionality:
 * Multi-threading latency measurement capability.
 * Options to specify number of samples for each task.
 * Measure latency difference for each sample and store results.
 * Display latency data in the form of cyclictest results.
 * Helpful in easily plotting a graphical view of latency data
 * using a simple script and gnuplot command.
 *
 * Copyright 2018 by Pintu Kumar <pintu.kumar@sony.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <unistd.h>
#include <limits.h>
#include <sys/mman.h>

//#define ALCHEMY_API

#if defined(ALCHEMY_API)
#include <alchemy/task.h>
#include <alchemy/timer.h>
#else
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#endif

#define MAX_THREADS	1
#define ONE_SECOND	1000000000ULL
#define NANO_SECOND	1000000000ULL
#define MICRO_SECOND	1000000.00
#define MILLI_SECOND	1000.00

#define MILLI_SEC(x)    ((x) * MICRO_SECOND)
#define MICRO_SEC(x)    ((x) * MILLI_SECOND)

/* Task sleep duration in micro-seconds */
#define SLEEP_DURATION	(100)

/* This denotes the number of samples */
int n;

struct task_data {
	float *latency;
	float min_latency;
	float max_latency;
	float avg_latency;
} data[MAX_THREADS+1];


/* This function returns time difference in micro-seconds */
#if defined(ALCHEMY_API)
static inline float timediff(RTIME t1, RTIME t2)
#else
static inline float timediff(struct timespec t1, struct timespec t2)
#endif
{
	float diff;

#if defined(ALCHEMY_API)
	diff = (t2 - t1) / MILLI_SECOND;
#else
	diff = (t2.tv_sec - t1.tv_sec) * NANO_SECOND;
	diff += (t2.tv_nsec - t1.tv_nsec) / MILLI_SECOND;
#endif

	return diff;
}

/* This is a dummy function, which actually does nothing
 * It just allows us to sleep for a duration specified by value.
 */
static void do_something(int value, int taskno)
{
#if defined(ALCHEMY_API)
	int ret;
	//RTIME delay;
#else
	struct timespec tm;
#endif

	/* Sometimes rt_task_wait_period may also fail.
	 * At that time huge negative latency can be seen.
	 * Thus it is better to check failure case.
	 */
#if defined(ALCHEMY_API)
	ret = rt_task_wait_period(NULL);
	if (ret < 0)
		rt_printf("rt_task_wait_period - failed, task: %d\n", taskno);
#else
	tm.tv_sec = 0;
	tm.tv_nsec = MICRO_SEC(value);
	clock_nanosleep(CLOCK_MONOTONIC, 0, &tm, NULL);
#endif

	/* We can also use below to create delay.
	 * Latency values might differ slightly.
	 */
	//delay = MICRO_SEC(value);
	//rt_task_sleep(delay);
}

/* This is the real task function that gets called when rt task starts.
 * The argument here indicates the task number.
 */
#if defined(ALCHEMY_API)
static void TaskFunction(void *arg)
#else
static void *TaskFunction(void *arg)
#endif
{
	int i, taskno;
	float max, min, avg_latency, max_latency, min_latency;
	float diff, total_diff = 0.0;
#if defined(ALCHEMY_API)
	int ret;
	RTIME prev, now;
#else
	struct timespec prev, now;
#endif

	if (!arg)
		goto end;

	taskno = *(int *)arg;

	max = (float)INT_MIN;
	min = (float)INT_MAX;

#if defined(ALCHEMY_API)
	ret = rt_task_set_periodic(NULL, TM_NOW, MICRO_SEC(SLEEP_DURATION));
	if (ret < 0)
		printf("rt_task_set_periodic - failed, task: %d\n", taskno);
#endif

	printf("Task: %d, Iteration: %d, Sleep duration: %d us\n", taskno, n,
					(int)SLEEP_DURATION);

	for (i = 1; i <= n; i++) {
#if defined(ALCHEMY_API)
		prev = rt_timer_read();
#else
		clock_gettime(CLOCK_MONOTONIC, &prev);
#endif
		do_something(SLEEP_DURATION, taskno);
#if defined(ALCHEMY_API)
		now = rt_timer_read();
#else
		clock_gettime(CLOCK_MONOTONIC, &now);
#endif
		diff = timediff(prev, now);
		data[taskno].latency[i] = diff;
		total_diff += diff;

		if (diff > max)
			max = diff;
		if (diff < min)
			min = diff;
	}

	avg_latency = (total_diff / n) - (SLEEP_DURATION);
	data[taskno].avg_latency = avg_latency;
	max_latency = max - (SLEEP_DURATION);
	data[taskno].max_latency = max_latency;
	min_latency = min - (SLEEP_DURATION);
	data[taskno].min_latency = min_latency;

end:
#if !defined(ALCHEMY_API)
	return NULL;
#endif
}

static void show_latency(void)
{
	int i, j;

	for (i = 0; i < MAX_THREADS; i++) {
		for (j = 1; j <= n; j++) {
			float latency;

			if (data[i].latency == NULL)
				continue;

			latency = data[i].latency[j] - (SLEEP_DURATION);
			printf("%d %5.3f %5.3f\n",
					j, data[i].latency[j], latency);
		}
		printf("Task[%d] - Min Latency: %5.3f us\n",
					i, data[i].min_latency);
		printf("Task[%d] - Max Latency: %5.3f us\n",
					i, data[i].max_latency);
		printf("Task[%d] - Avg Latency: %5.3f us\n",
					i, data[i].avg_latency);
		printf("\n");

		if (data[i].latency != NULL)
			free(data[i].latency);
	}
}

int main(int argc, char *argv[])
{
	int i;
#if defined(ALCHEMY_API)
	RT_TASK tid[MAX_THREADS+1];
#else
	pthread_t tid[MAX_THREADS+1];
	struct sched_param param;
	pthread_attr_t attr;
#endif

	if (argc < 2) {
		printf("Usage: ./%s <number of iterations>\n", argv[0]);
		return -1;
	}

	mlockall(MCL_CURRENT|MCL_FUTURE);

	n = atoi(argv[1]);

	if (n <= 0)
		return -1;

	/* Create thread with same thread function */
	for (i = 0; i < MAX_THREADS; i++) {
		int err;
		int priority;
		char name[32];

		data[i].latency = (float *)malloc(sizeof(float)*n+1);
		if (data[i].latency == NULL) {
			printf("Cannot allocate memory for: data[%d]\n", i);
			return -1;
		}

		/* Caution: Do not choose priority greater than 99.
		 * So make sure to adjust the MAX_THREADS value accordingly
		 */
		priority = 99 - i;
		snprintf(name, sizeof(name), "task%d", i);
		printf("main: creating task name: %s, priority: %d\n",
					name, priority);

#if defined(ALCHEMY_API)
		/* Note: Here T_JOINABLE is important.
		 * If you dont specify this then rt_task_join will not wait
		 */
		err = rt_task_create(&tid[i], name, 0, priority, T_JOINABLE);
		if (err) {
			printf("rt_task_create - Failed, err = %d\n", err);
			return -1;
		}

		err = rt_task_start(&tid[i], &TaskFunction, &i);
		if (err) {
			printf("rt_task_start - Failed, err = %d\n", err);
			rt_task_delete(&tid[i]);
			return -1;
		}
#else
		memset(&attr, 0, sizeof(attr));
		memset(&param, 0, sizeof(param));
		/* Set thread attributes to set specific policy */
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
		pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
		pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
		pthread_attr_setschedparam(&attr, &param);

		param.sched_priority = priority;
		pthread_attr_setschedparam(&attr, &param);
		err = pthread_create(&tid[i], &attr, TaskFunction, (void *)&i);
		if (err) {
			printf("pthread_create - Failed, err = %d\n", err);
			return -1;
		}
#endif
	}

	/* Synchronize the completion of each thread. */
	for (i = 0; i < MAX_THREADS; i++) {
#if defined(ALCHEMY_API)
		/* Note: It should be used along with T_JOINABLE option */
		rt_task_join(&tid[i]);
#else
		pthread_join(tid[i], NULL);
#endif
	}

	/* Display latency measurement data for each task */
	show_latency();

	printf("ALL FINISHED.......!!!\n");

	return 0;
}
