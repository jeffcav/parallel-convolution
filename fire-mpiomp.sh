#!/bin/bash
TIMEFORMAT='%3R'
export OMP_NUM_THREADS=2

echo "Serial execution"
echo -ne "nprocs\timgsize\t\trun\ttime(s)\n"
echo "---------------------------------------"

for p in 2 4 8; do
	for i in 1024 4096 16384 32768; do
		for j in 1024 4096 16384 32768; do
			sum=0
			for k in 1 2 3 4 5 6 7 8; do
				raw_time=$( { time mpiexec -n ${p} ./build/parallel-mpi-omp images/${i}x${j}.bmp images/out/mpiomp_${i}x${j}.bmp; } 2>&1 )
				raw_dot_time=$(echo $raw_time | tr , .)
				sum=$(echo "scale=3; $raw_dot_time + $sum" | bc)

				sleep 1

				#echo -ne "${p}\t${i}x${j}\t${k}\t${raw_time}\n"
			done
			avg=$(echo "scale=3; ${sum} / 8" | bc)
			echo -ne "${p}\t${i}x${j}\t${avg}\n"
		done
	done
done
