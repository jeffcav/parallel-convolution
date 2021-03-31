#!/bin/bash
TIMEFORMAT='%3R'

function serial() {
	echo ""
	echo "serial"
	for i in 512 1024 2048 4096 16384 32768; do
		echo -ne "${i} "; time ./build/serial images/${i}x${i}.bmp images/out/serial_${i}x${i}.bmp
	done
}

function omp() {
	echo ""
	echo "omp"
	for i in 512 1024 2048 4096 16384 32768; do
		echo -ne "${i} "; time ./build/parallel-omp images/${i}x${i}.bmp images/out/omp_${i}x${i}.bmp
	done
}

function mpi() {
	echo ""
	echo "mpi-only"
	for p in 2 4 8; do
		echo ""
		for i in 512 1024 2048 4096 16384 32768; do
			echo -ne "${i} ${p} "; time mpirun -n $p ./build/parallel-mpi images/${i}x${i}.bmp images/out/mpi_${i}x${i}.bmp
		done
	done
}

function mpiomp() {
	echo ""
	echo "mpi-omp"
	for p in 2 4 8; do
		echo ""
		for i in 512 1024 2048 4096 16384 32768; do
			echo -ne "${i} ${p} "; time mpirun -n $p ./build/parallel-mpi-omp images/${i}x${i}.bmp images/out/mpiomp_${i}x${i}.bmp
		done
	done
}

if [ "$1" == "serial" ]
	then serial
elif [ "$1" == "mpi" ]
	then mpi
elif [ "$1" == "omp" ]
	then omp
elif [ "$1" == "mpiomp" ]
	then mpiomp
fi
