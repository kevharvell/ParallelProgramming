#!/bin/bash
# array sizes
for s in 1024 2048 4096 8192 16384 32768 65536 131072 262144 524288 1048576 2097152 4194304 8388608
do
	for t in 8 16 32 64 128 256 512
	do
		echo NUM_ELEMENTS = $s
		echo LOCAL_SIZE = $t
		g++ -o first first.cpp /scratch/cuda-7.0/lib64/libOpenCL.so -lm -fopenmp
		./first
	done
done
