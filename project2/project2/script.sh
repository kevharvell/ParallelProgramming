#!/bin/bash
# number of threads:
for t in 1 2 4 6 8
do
	echo NUMT = $t
	# number of subdivisions:
	for s in 100 1000 10000 20000 40000 80000 160000 320000 640000 1280000
	do
		echo NUMNODES = $s
		/usr/local/common/gcc-8.2.0/bin/g++ -DNUMNODES=$s -DNUMT=$t project2.cpp -o project2 -lm -fopenmp
		./project2
	done
done