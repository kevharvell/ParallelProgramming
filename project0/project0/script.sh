#!/bin/bash
# number of threads:
for t in 1 4
do
	echo NUMT = $t
	# number of subdivisions:
	for s in 10000 50000 10000
	do
		echo ARRAYSIZE = $s
		/usr/local/common/gcc-8.2.0/bin/g++ -DARRAYSIZE=$s -DNUMT=$t project0.cpp -o project0 -lm -fopenmp
		./project0
	done
done