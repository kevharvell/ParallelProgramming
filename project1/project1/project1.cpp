#include "pch.h"
#include <iostream>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

// setting the number of threads:
#ifndef NUMT
#define NUMT		1
#endif

// setting the number of trials in the monte carlo simulation:
#ifndef NUMTRIALS
#define NUMTRIALS	1000000
#endif

// how many tries to discover the maximum performance:
#ifndef NUMTRIES
#define NUMTRIES	10
#endif

// ranges for the random numbers:
const float XCMIN = -1.0;
const float XCMAX = 1.0;
const float YCMIN = 0.0;
const float YCMAX = 2.0;
const float RMIN = 0.5;
const float RMAX = 2.0;

// function prototypes:
float	Ranf(float, float);
int		Ranf(int, int);
void	TimeOfDaySeed();

// main program:
int
main(int argc, char *argv[])
{
#ifndef _OPENMP
	fprintf(stderr, "No OpenMP support!\n");
	return 1;
#endif

	TimeOfDaySeed();		// seed the random number generator

	omp_set_num_threads(NUMT);	// set the number of threads to use in the for-loop:`

	// better to define these here so that the rand() calls don't get into the thread timing:
	float *xcs = new float[NUMTRIALS];
	float *ycs = new float[NUMTRIALS];
	float * rs = new float[NUMTRIALS];

	// fill the random-value arrays:
	for (int n = 0; n < NUMTRIALS; n++)
	{
		xcs[n] = Ranf(XCMIN, XCMAX);
		ycs[n] = Ranf(YCMIN, YCMAX);
		rs[n] = Ranf(RMIN, RMAX);
	}

	// get ready to record the maximum performance and the probability:
	float maxPerformance = 0.;      // must be declared outside the NUMTRIES loop
	float currentProb;              // must be declared outside the NUMTRIES loop

	// looking for the maximum performance:
	for (int t = 0; t < NUMTRIES; t++)
	{
		double time0 = omp_get_wtime();

		int numHits = 0;
#pragma omp parallel for default(none) shared(xcs,ycs,rs) reduction(+:numHits)
		for (int i = 0; i < NUMTRIALS; i++)
		{
			// randomize the location and radius of the circle:
			float xc = xcs[i];
			float yc = ycs[i];
			float  r = rs[i];

			// solve for the intersection using the quadratic formula:
			float a = 2.;
			float b = -2.*(xc + yc);
			float c = xc * xc + yc * yc - r * r;
			float d = b * b - 4.*a*c;

			if (d < 0) continue;

			// hits the circle:
			// get the first intersection:
			d = sqrt(d);
			float t1 = (-b + d) / (2.*a);	// time to intersect the circle
			float t2 = (-b - d) / (2.*a);	// time to intersect the circle
			float tmin = t1 < t2 ? t1 : t2;		// only care about the first intersection

			if (tmin < 0) continue;

			// where does it intersect the circle?
			float xcir = tmin;
			float ycir = tmin;

			// get the unitized normal vector at the point of intersection:
			float nx = xcir - xc;
			float ny = ycir - yc;
			float n = sqrt( nx*nx + ny*ny );
			nx /= n;	// unit vector
			ny /= n;	// unit vector

			// get the unitized incoming vector:
			float inx = xcir - 0.;
			float iny = ycir - 0.;
			float in = sqrt( inx*inx + iny*iny );
			inx /= in;	// unit vector
			iny /= in;	// unit vector

			// get the outgoing (bounced) vector:
			float dot = inx*nx + iny*ny;
			float outx = inx - 2.*nx*dot;	// angle of reflection = angle of incidence`
			float outy = iny - 2.*ny*dot;	// angle of reflection = angle of incidence`

			// find out if it hits the infinite plate:
			float t = ( 0. - ycir ) / outy;

			if (t < 0) continue;

			numHits++;
		}
		std::cout << "number of hits: " << numHits;
	}
}