#!/bin/bash
# array sizes
for s in 1024 2048
do
	echo NUM_ELEMENTS = $s
	g++ -DNUM_ELEMENTS=$s -o first first.cpp /usr/lib/x86_64-linux-gnu/libOpenCL.so -lm  -fopenmp
	./first
done
