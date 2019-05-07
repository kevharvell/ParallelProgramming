#!/bin/bash
# array sizes
for s in 1000 2000 4000 8000 16000 32000 64000 128000 256000 512000 1024000 2048000 4096000 8192000
do
	echo ARRAY_SIZE = $s
	g++ -DARRAY_SIZE=$s -o project4  project4.cpp simd.p4.o  -lm  -fopenmp
	./project4
done
