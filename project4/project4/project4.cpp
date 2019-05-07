#include "pch.h"
#include "simd.p4.h"
#include <iostream>
#include <fstream>
#include <omp.h>
#include <time.h>

using std::cout;
using std::endl;

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	10
#endif

// how many tries to discover the maximum performance:
#ifndef ARRAY_SIZE
#define ARRAY_SIZE	1000
#endif

// arrays to perform multiplication/reduction on
float A[ARRAY_SIZE];
float B[ARRAY_SIZE];
float C[ARRAY_SIZE];

// function prototypes
void fillArray(float arr[], int len);
void displayArray(float arr[], int len);
void mul(float *, float *, float *, int);

int main()
{
	#ifndef _OPENMP
		std::fprintf(stderr, "No OpenMP support!\n");
		return 1;
	#endif

	// seed random
	srand(time(0));

	// get ready to record the maximum performance:
	float maxPerformance = 0.;      // must be declared outside the NUMTRIES loop

	// fill arrays with random values
	fillArray(A, ARRAY_SIZE);
	fillArray(B, ARRAY_SIZE);
	fillArray(C, ARRAY_SIZE);

	// looking for the maximum performance for SIMD Multiplication:
	for (int t = 0; t < NUMTRIES; t++)
	{
		double time0 = omp_get_wtime();
		SimdMul(A, B, C, ARRAY_SIZE);
		double time1 = omp_get_wtime();
		double megaMultsPerSecond = (double)ARRAY_SIZE / (time1 - time0) / 1000000.;
		if (megaMultsPerSecond > maxPerformance) {
			maxPerformance = megaMultsPerSecond;
		}
		
		
	}
	cout << "Max Performance SIMD Multiplication: " << maxPerformance << endl;

	maxPerformance = 0;
	// looking for the maximum performance for NON-SIMD Multiplication
	for (int t = 0; t < NUMTRIES; t++)
	{
		double time0 = omp_get_wtime();
		mul(A, B, C, ARRAY_SIZE);
		double time1 = omp_get_wtime();
		double megaMultsPerSecond = (double)ARRAY_SIZE / (time1 - time0) / 1000000.;
		if (megaMultsPerSecond > maxPerformance) {
			maxPerformance = megaMultsPerSecond;
		}

	}
	cout << "Max Performance NON-SIMD Multiplication: " << maxPerformance << endl;

	maxPerformance = 0;
	// looking for the maximum performance for NON-SIMD Multiplication
	for (int t = 0; t < NUMTRIES; t++)
	{
		double time0 = omp_get_wtime();
		SimdMulSum(A, B, ARRAY_SIZE);
		double time1 = omp_get_wtime();
		double megaMultsPerSecond = (double)ARRAY_SIZE / (time1 - time0) / 1000000.;
		if (megaMultsPerSecond > maxPerformance) {
			maxPerformance = megaMultsPerSecond;
		}

	}
	cout << "Max Performance SIMD Multiplication Reduction: " << maxPerformance << endl;

	maxPerformance = 0;
	// looking for the maximum performance for NON-SIMD Multiplication
	for (int t = 0; t < NUMTRIES; t++)
	{
		double time0 = omp_get_wtime();
		mulSum(A, B, ARRAY_SIZE);
		double time1 = omp_get_wtime();
		double megaMultsPerSecond = (double)ARRAY_SIZE / (time1 - time0) / 1000000.;
		if (megaMultsPerSecond > maxPerformance) {
			maxPerformance = megaMultsPerSecond;
		}

	}
	cout << "Max Performance NON-SIMD Multiplication Reduction: " << maxPerformance << endl;

	return 0;
}

void displayArray(float arr[], int len) {
	for (int i = 0; i < len; i++) {
		cout << arr[i] << "\t";
	}
	cout << endl;
}

void fillArray(float arr[], int len)
{
	for (int i = 0; i < len; i++) {
		arr[i] = (float) (rand() % 50);
	}
}

void mul(float A[], float B[], float C[], int len) {
	for (int i = 0; i < len; i++) {
		C[i] = A[i] * B[i];
	}
}

float
mulSum(float *a, float *b, int len)
{
	float sum;
	for (int i = 0; i < len; i++)
	{
		sum += a[i] * b[i];
	}
	return sum;
}
