#!/bin/bash
#  65536 131072

for i in 512 1024 2048 4096 8192 16384 32768; do
	./build/imgc ${i} ${i} images/${i}x${i}.bmp &
done
