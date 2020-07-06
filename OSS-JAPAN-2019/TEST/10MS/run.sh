#!/bin/bash

TASK=$1
TYPE=$2
LOOPS=10000
EXECNAME=none
HEADER=none

if [ "$TYPE" = "" -o "$TASK" = "" ]; then
	echo "usage: sudo ./run.sh <task-name={normal/xeno-native/xeno-posix}> <load/no-load>"
	exit -1
fi

if [ "$TASK" = "normal" ]; then
	EXECNAME=simple_task_normal_linux_10ms.out
	HEADER=NORMAL-POSIX
elif [ "$TASK" = "xeno-posix" ]; then
	EXECNAME=simple_task_xeno_posix_10ms.out
	HEADER=XENO-POSIX
elif [ "$TASK" = "xeno-native" ]; then
	EXECNAME=simple_task_xeno_native_10ms.out
	HEADER=XENO-NATIVE
fi

rm -f output.txt
if [ "$TYPE" = "load" ]; then
	hackbench --pipe 100 -s 100 --process 10000 -l 100000 &
	sleep 60
	./$EXECNAME $LOOPS > output-$TASK.txt
	echo "done"
	kill -9 `pidof hackbench`
	sleep 5
	./plot-graph.sh $TASK-simple-task-sleep-with-hackbench-plot.png output-$TASK.txt $HEADER-TASK-HACKBENCH
else
	./$EXECNAME $LOOPS > output-$TASK.txt
	echo "done"
	sleep 5
	./plot-graph.sh $TASK-simple-task-sleep-with-noload-plot.png output-$TASK.txt $HEADER-TASK-NO-LOAD
fi
