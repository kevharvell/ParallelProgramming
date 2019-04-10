#!/bin/bash
# number of threads:
for t in 1 2 4 6 8 10 12 14 16 18 20
do
	echo NUMT = $t
	# number of subdivisions:
	for s in 100000 1000000 10000000
	do
		echo NUMTRIALS = $s
		/usr/local/common/gcc-8.2.0/bin/g++ -DNUMTRIALS=$s -DNUMT=$t project1.cpp -o project1 -lm -fopenmp
		./project1
	done
done