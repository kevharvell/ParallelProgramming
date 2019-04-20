#!/bin/bash
# number of threads:
for t in 1 2 4 6 8
do
	echo NUMT = $t
	# number of subdivisions:
	for s in 100 200 400 800 1600 3200 6400
	do
		echo NUMNODES = $s
		/usr/local/common/gcc-8.2.0/bin/g++ -DNUMNODES=$s -DNUMT=$t project2.cpp -o project2 -lm -fopenmp
		./project2
	done
done