#!/bin/bash
TIMEFORMAT='%3R'

echo "OpenMP"
echo -ne "image size\tavg time (s)\n"
echo "--------------------------"
for i in 1024 4096 16384 32768; do
	for j in 1024 4096 16384 32768; do
		sum=0
		for k in 1 2 3 4 5 6 7 8; do

			raw_time=$( { time ./build/parallel-omp images/${i}x${j}.bmp images/out/omp_${i}x${j}.bmp; } 2>&1 )
			raw_dot_time=$(echo $raw_time | tr , .)
			sum=$(echo "scale=3; $raw_dot_time + $sum" | bc)

		done
		avg=$(echo "scale=3; ${sum} / 8" | bc)
		echo -ne "${i}x${j} \t${avg}\n"
	done
done

