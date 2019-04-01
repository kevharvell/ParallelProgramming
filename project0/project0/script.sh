#!/bin/bash
# number of threads:
for t in 1 4
do
	echo NUMT = $t
	# number of subdivisions:
	for s in 2 4 8 16 32 64 128 256 512 1024 2048 3072 4096
	do
		echo ARRAYSIZE = $s
		g++ -DARRAYSIZE=$s -DNUMT=$t project0.cpp -o project0 -lm -fopenmp
		./prog
	done
done