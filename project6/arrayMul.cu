// Array multiplication: C = A * B:

// System includes
#include <stdio.h>
#include <assert.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

// CUDA runtime
#include <cuda_runtime.h>

// Helper functions and utilities to work with CUDA
#include "helper_functions.h"
#include "helper_cuda.h"


#ifndef BLOCKSIZE
#define BLOCKSIZE		32		// number of threads per block
#endif

#ifndef NUMTRIALS
#define NUMTRIALS		10000		// to make the timing more accurate
#endif

// ranges for the random numbers:
const float XCMIN = 0.0;
const float XCMAX = 2.0;
const float YCMIN = 0.0;
const float YCMAX = 2.0;
const float RMIN = 0.5;
const float RMAX = 2.0;

// function prototypes:
float	Ranf(float, float);
int		Ranf(int, int);
void	TimeOfDaySeed();



__global__  void MonteCarlo( float *xcs, float *ycs, float *rs )
{
	unsigned int gid = blockIdx.x*blockDim.x + threadIdx.x;
	float xc = xcs[gid];
	float yc = ycs[gid];
	float  r = rs[gid];

	// solve for the intersection using the quadratic formula:
    float a = 2.;
    float b = -2.*(xc + yc);
    float c = xc * xc + yc * yc - r * r;
    float d = b * b - 4.*a*c;

    if (d >= 0) return;

    // hits the circle:
    // get the first intersection:
    d = sqrt(d);
    float t1 = (-b + d) / (2.*a);	// time to intersect the circle
    float t2 = (-b - d) / (2.*a);	// time to intersect the circle
    float tmin = t1 < t2 ? t1 : t2;		// only care about the first intersection

    if (tmin >= 0) return;

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

    if (t >= 0) return;

	__shared__ int numHits[NUMTRIALS];
	numHits[gid]++;
}


// main program:

int
main( int argc, char* argv[ ] )
{
	int dev = findCudaDevice(argc, (const char **)argv);

	// allocate host memory:

	float * hxcs = new float [ NUMTRIALS ];
	float * hycs = new float [ NUMTRIALS ];
	float * hrs = new float [ NUMTRIALS ];

	// fill the random-value arrays:
	for (int n = 0; n < NUMTRIALS; n++)
	{
		hxcs[n] = Ranf(XCMIN, XCMAX);
		hycs[n] = Ranf(YCMIN, YCMAX);
		hrs[n] = Ranf(RMIN, RMAX);
	}

	// allocate device memory:

	float *dxcs, *dycs, *drs;

	dim3 dimsA( NUMTRIALS, 1, 1 );
	dim3 dimsB( NUMTRIALS, 1, 1 );
	dim3 dimsC( NUMTRIALS, 1, 1 );

	//__shared__ float prods[NUMTRIALS/BLOCKSIZE];


	cudaError_t status;
	status = cudaMalloc( reinterpret_cast<void **>(&dxcs), NUMTRIALS*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&dycs), NUMTRIALS*sizeof(float) );
		checkCudaErrors( status );
	status = cudaMalloc( reinterpret_cast<void **>(&drs), (NUMTRIALS/BLOCKSIZE)*sizeof(float) );
		checkCudaErrors( status );


	// copy host memory to the device:

	status = cudaMemcpy( dxcs, hxcs, NUMTRIALS*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );
	status = cudaMemcpy( dycs, hycs, NUMTRIALS*sizeof(float), cudaMemcpyHostToDevice );
		checkCudaErrors( status );

	// setup the execution parameters:

	dim3 threads(BLOCKSIZE, 1, 1 );
	dim3 grid( NUMTRIALS / threads.x, 1, 1 );

	// Create and start timer

	cudaDeviceSynchronize( );

	// allocate CUDA events that we'll use for timing:

	cudaEvent_t start, stop;
	status = cudaEventCreate( &start );
		checkCudaErrors( status );
	status = cudaEventCreate( &stop );
		checkCudaErrors( status );

	// record the start event:

	status = cudaEventRecord( start, NULL );
		checkCudaErrors( status );

	// execute the kernel:

	for( int t = 0; t < NUMTRIALS; t++)
	{
	        MonteCarlo<<< grid, threads >>>( dxcs, dycs, drs );
	}

	// record the stop event:

	status = cudaEventRecord( stop, NULL );
		checkCudaErrors( status );

	// wait for the stop event to complete:

	status = cudaEventSynchronize( stop );
		checkCudaErrors( status );

	float msecTotal = 0.0f;
	status = cudaEventElapsedTime( &msecTotal, start, stop );
		checkCudaErrors( status );

	// compute and print the performance

	double secondsTotal = 0.001 * (double)msecTotal;
	std::cout << "Seconds Total" << secondsTotal << std::endl;
	double lasersPerSecond = (float)NUMTRIALS / secondsTotal;
	std::cout << "Lasers per second" << lasersPerSecond << std::endl;
	double megaLasersPerSecond = lasersPerSecond / 1000000.;
	fprintf( stderr, "Array Size = %10d, MegaLasers/Second = %10.2lf\n", NUMTRIALS, megaLasersPerSecond );


	// clean up memory:
	delete [ ] hxcs;
	delete [ ] hycs;
	delete [ ] hrs;

	status = cudaFree( dxcs );
		checkCudaErrors( status );
	status = cudaFree( dycs );
		checkCudaErrors( status );
	status = cudaFree( drs );
		checkCudaErrors( status );


	return 0;
}

float
Ranf(float low, float high)
{
	float r = (float)rand();               // 0 - RAND_MAX
	float t = r / (float)RAND_MAX;       // 0. - 1.

	return   low + t * (high - low);
}

int
Ranf(int ilow, int ihigh)
{
	float low = (float)ilow;
	float high = ceil((float)ihigh);

	return (int)Ranf(low, high);
}

void
TimeOfDaySeed()
{
	struct tm y2k = { 0 };
	y2k.tm_hour = 0;   y2k.tm_min = 0; y2k.tm_sec = 0;
	y2k.tm_year = 100; y2k.tm_mon = 0; y2k.tm_mday = 1;

	time_t  timer;
	time(&timer);
	double seconds = difftime(timer, mktime(&y2k));
	unsigned int seed = (unsigned int)(1000.*seconds);    // milliseconds
	srand(seed);
}