#!/bin/bash
# array sizes
for s in 16000 32000 64000 128000 256000 512000
do
	for t in 16 32 64
	do
		echo NUMTRIALS = $s
		echo BLOCKSIZE = $t
		nvcc -DNUMTRIALS=$s -DBLOCKSIZE=$t -o arrayMul  arrayMul.cu
		./arrayMul
	done
done
