#!/bin/bash

IMAGE_FILE=$1
DATA_FILE=$2
HEADER_TITLE=$3
MACHINE="Raspberry Pi 3"

if [ "$IMAGE_FILE" = "" ]; then
	echo "Usage: please specific output image filename !!!"
	exit -1
fi

max=`grep "Max Latency" $DATA_FILE | awk -F': ' '{print $2}' | awk '{print $1}'`
min=`grep "Min Latency" $DATA_FILE | awk -F': ' '{print $2}' | awk '{print $1}'`
avg=`grep "Avg Latency" $DATA_FILE | awk -F': ' '{print $2}' | awk '{print $1}'`
niteration=`grep "Iteration" $DATA_FILE | awk -F', ' '{print $2}' | awk -F': ' '{print $2}'`
duration=`grep "duration" $DATA_FILE | awk -F', ' '{print $3}' | awk -F': ' '{print $2}'`

echo "set title \"$HEADER_TITLE\nSleep Duration: $duration\nMin Latency: $min us, Max Latency: $max us, Avg Latency: $avg us\"" > plotcmd
echo "set terminal png" >> plotcmd
echo "set xlabel \"Number of latency samples ($niteration)\"" >> plotcmd
echo "set ylabel \"Latency (us)\"" >> plotcmd
#echo "set logscale x" >> plotcmd
echo "set autoscale" >> plotcmd
echo "set output \"$IMAGE_FILE\"" >> plotcmd
echo -n "plot \"$DATA_FILE\" using 1:3 title \"$MACHINE\" with histeps" >> plotcmd
gnuplot -persist < plotcmd

rm -f plotcmd
