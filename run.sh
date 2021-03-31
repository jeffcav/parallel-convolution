#!/bin/bash
TIMEFORMAT='%3R'

echo ""
echo "serial"
for i in 512 1024 2048 4096 8192 16384 32768; do
	echo -ne "${i} "; time ./build/serial images/${i}x${i}.bmp images/out/serial_${i}x${i}.bmp
done

echo ""
echo "mpi-only"
for i in 512 1024 2048 4096 8192 16384; do
	for p in 2 4 8; do
		echo -ne "${i} ${p} "; time mpirun -n $p ./build/parallel-mpi images/${i}x${i}.bmp images/out/serial_${i}x${i}.bmp
	done
done

# echo ""
# echo "mpi+omp"
# for i in 512 1024 2048 4096 8192 16384; do
# 	echo -ne "${i} "; time mpirun -n 4 ./build/parallel-mpi-omp images/${i}x${i}.bmp images/out/serial_${i}x${i}.bmp
# done
