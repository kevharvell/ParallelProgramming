simd.p4.o:	simd.p4.h  simd.p4.cpp
	g++  -c  simd.p4.cpp -o simd.p4.o

project4:	project4.cpp simd.p4.o
	g++  -o arraymult  arraymult.cpp simd.p4.o  -lm  -fopenmp