#!/bin/bash

for i in 1024 4096 16384 32768; do
	for j in 1024 4096 16384 32768; do
		./build/imgc ${i} ${j} images/${i}x${j}.bmp &
	done
done
wait