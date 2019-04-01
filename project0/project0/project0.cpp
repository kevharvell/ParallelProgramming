#include <omp.h>
#include <stdio.h>
#include <math.h>
#include <limits>


#define NUMT	         4
#define ARRAYSIZE       50000	// you decide
#define NUMTRIES        20		// you decide

float A[ARRAYSIZE];
float B[ARRAYSIZE];
float C[ARRAYSIZE];

int main()
{
#ifndef _OPENMP
	fprintf(stderr, "OpenMP is not supported here -- sorry.\n");
	return 1;
#endif

	omp_set_num_threads(NUMT);
	fprintf(stderr, "Using %d threads\n", NUMT);

	double maxMegaMults = 0.;
	double fastestTime = std::numeric_limits<double>::infinity();

	for (int t = 0; t < NUMTRIES; t++)
	{
		double time0 = omp_get_wtime();

#pragma omp parallel for
		for (int i = 0; i < ARRAYSIZE; i++)
		{
			C[i] = A[i] * B[i];
		}

		double time1 = omp_get_wtime();
		double timeElapsed = time1 - time0;
		double megaMults = (double)ARRAYSIZE / (time1 - time0) / 1000000.;
		if (megaMults > maxMegaMults)
			maxMegaMults = megaMults;
		if (timeElapsed < fastestTime)
			fastestTime = timeElapsed;
	}

	printf("Peak Performance = %8.2lf MegaMults/Sec\n", maxMegaMults);
	printf("Fastest Time = %8.2lf seconds\n", fastestTime);

	// note: %lf stands for "long float", which is how printf prints a "double"
	//        %d stands for "decimal integer", not "double"

	return 0;
}
