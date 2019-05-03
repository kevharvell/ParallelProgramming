#include "pch.h"
#include <iostream>
#include <fstream>
#include <omp.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI 
#define M_PI 3.14159265358979323846
#endif

int	NowYear;			// 2019 - 2024
int	NowMonth;			// 0 - 11
int NumMonths;			// tracks total number of months

float	NowPrecip;		// inches of rain per month
float	NowTemp;		// temperature this month
float	NowHeight;		// grain height in inches
int		NowNumDeer;		// number of deer in the current population
int		NowNumHippies;	// number of hippies in the current population

const float GRAIN_GROWS_PER_MONTH = 8.0;
const float ONE_DEER_EATS_PER_MONTH = 0.5;

const float AVG_PRECIP_PER_MONTH = 6.0;	// average
const float AMP_PRECIP_PER_MONTH = 6.0;	// plus or minus
const float RANDOM_PRECIP = 2.0;	// plus or minus noise

const float AVG_TEMP = 50.0;	// average
const float AMP_TEMP = 20.0;	// plus or minus
const float RANDOM_TEMP = 10.0;	// plus or minus noise

const float MIDTEMP = 40.0;
const float MIDPRECIP = 10.0;

// function prototypes
void GrainDeer();
void Grain();
void Watcher();
void Hippies();

// parallel stuff
omp_lock_t	Lock;
int		NumInThreadTeam;
int		NumAtBarrier;
int		NumGone;
// function prototypes
void InitBarrier(int);
void WaitBarrier();

// math function prototypes
float Ranf(unsigned int*, float, float);
int Ranf(unsigned int*, int, int);
float SQR(float);

int main()
{
	// starting date and time:
	NowMonth = 0;
	NowYear = 2019;
	NumMonths = 1;

	// starting state (feel free to change this if you want):
	NowNumDeer = 1;
	NowHeight = 1.;
	NowNumHippies = 1;

	float ang = (30.*(float)NowMonth + 15.) * (M_PI / 180.);

	float temp = AVG_TEMP - AMP_TEMP * cos(ang);
	unsigned int seed = time(0);
	NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

	float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
	NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
	if (NowPrecip < 0.)
		NowPrecip = 0.;

	omp_init_lock(&Lock);

	omp_set_num_threads(3);	// same as # of sections
	InitBarrier(3);

	#pragma omp parallel sections
	{
		#pragma omp section
		{
			GrainDeer();
		}

		#pragma omp section
		{
			Grain();
		}

		#pragma omp section
		{
			Watcher();
		}

		#pragma omp section
		{
			Hippies();	// your own
		}
	}       // implied barrier -- all functions must return in order
		// to allow any of them to get past here
}

void GrainDeer()
{
	while (NowYear < 2025)
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		int NextNumDeer = NowNumDeer;
		if (NowNumDeer > NowHeight) {
			NextNumDeer--;
		}
		else if (NowNumDeer < NowHeight) {
			NextNumDeer++;
		}
		if (NextNumDeer < 0) NextNumDeer = 0;

		//printf("GrainDeer waiting at #1.\n");
		// DoneComputing barrier:
		WaitBarrier();
		//printf("GrainDeer resuming at #1.\n");

		NowNumDeer = NextNumDeer;
		
		//printf("GrainDeer waiting at #2.\n");
		// DoneAssigning barrier:
		WaitBarrier();
		//printf("GrainDeer resuming at #2.\n");

		
		//printf("GrainDeer waiting at #3.\n");
		// DonePrinting barrier:
		WaitBarrier();
		//printf("GrainDeer resuming at #2.\n");
	}
}

void Grain()
{
	while (NowYear < 2025)
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		float NextHeight = NowHeight;
		float tempFactor = exp(-SQR((NowTemp - MIDTEMP) / 10.));
		float precipFactor = exp(-SQR((NowPrecip - MIDPRECIP) / 10.));
		
		NextHeight += tempFactor * precipFactor * GRAIN_GROWS_PER_MONTH;
		NextHeight -= (float)NowNumDeer * ONE_DEER_EATS_PER_MONTH;
		if (NextHeight < 0) NextHeight = 0;

		//printf("Grain waiting at #1.\n");
		// DoneComputing barrier:
		WaitBarrier();
		//printf("Grain resuming at #1.\n");

		NowHeight = NextHeight;

		//printf("Grain waiting at #2.\n");
		// DoneAssigning barrier:
		WaitBarrier();
		//printf("Grain resuming at #2.\n");


		//printf("Grain waiting at #3.\n");
		// DonePrinting barrier:
		WaitBarrier();
		//printf("Grain resuming at #2.\n");
	}
}

void Watcher()
{
	while (NowYear < 2025)
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:

		//printf("Watcher waiting at #1.\n");
		// DoneComputing barrier:
		WaitBarrier();
		//printf("Watcher resuming at #1.\n");

		//printf("Watcher waiting at #2.\n");
		// DoneAssigning barrier:
		WaitBarrier();
		//printf("Watcher resuming at #2.\n");

		printf("%d/%d\n", NowMonth + 1, NowYear);
		printf("Temperature: %f\n", NowTemp);
		printf("Precipitation: %f\n", NowPrecip);
		printf("NowHeight: %f\n", NowHeight);
		printf("NowNumDear: %d\n", NowNumDeer);
		printf("NowNumHippies: %d\n", NowNumHippies);


		NumMonths++;
		if (NowMonth == 11) {
			NowMonth = 0;
			NowYear++;
		}
		else {
			NowMonth++;
		}

		// recalculate environment variables
		float ang = (30.*(float)NowMonth + 15.) * (M_PI / 180.);
		float temp = AVG_TEMP - AMP_TEMP * cos(ang);
		unsigned int seed = time(0);
		NowTemp = temp + Ranf(&seed, -RANDOM_TEMP, RANDOM_TEMP);

		float precip = AVG_PRECIP_PER_MONTH + AMP_PRECIP_PER_MONTH * sin(ang);
		NowPrecip = precip + Ranf(&seed, -RANDOM_PRECIP, RANDOM_PRECIP);
		if (NowPrecip < 0.)
			NowPrecip = 0.;

		//printf("Watcher waiting at #3.\n");
		// DonePrinting barrier:
		WaitBarrier();
		//printf("Watcher resuming at #3.\n");
	}
}

void Hippies()
{
	while (NowYear < 2025)
	{
		// compute a temporary next-value for this quantity
		// based on the current state of the simulation:
		int NextNumHippies = NowNumHippies;
		if (NowHeight > 5) {
			NextNumHippies++;
		}
		else if (NowHeight < 5) {
			NextNumHippies--;
		}
		if (NextNumHippies < 0) NextNumHippies = 0;

		//printf("Hippies waiting at #1.\n");
		// DoneComputing barrier:
		WaitBarrier();
		//printf("Hippies resuming at #1.\n");

		NowNumHippies = NextNumHippies;

		//printf("Hippies waiting at #2.\n");
		// DoneAssigning barrier:
		WaitBarrier();
		//printf("Hippies resuming at #2.\n");


		//printf("Hippies waiting at #3.\n");
		// DonePrinting barrier:
		WaitBarrier();
		//printf("Hippies resuming at #2.\n");
}

// specify how many threads will be in the barrier:
//	(also init's the Lock)

void
InitBarrier(int n)
{
	NumInThreadTeam = n;
	NumAtBarrier = 0;
	omp_init_lock(&Lock);
}


// have the calling thread wait here until all the other threads catch up:

void
WaitBarrier()
{
	omp_set_lock(&Lock);
	{
		NumAtBarrier++;
		if (NumAtBarrier == NumInThreadTeam)
		{
			NumGone = 0;
			NumAtBarrier = 0;
			// let all other threads get back to what they were doing
// before this one unlocks, knowing that they might immediately
// call WaitBarrier( ) again:
			while (NumGone != NumInThreadTeam - 1);
			omp_unset_lock(&Lock);
			return;
		}
	}
	omp_unset_lock(&Lock);

	while (NumAtBarrier != 0);	// this waits for the nth thread to arrive

#pragma omp atomic
	NumGone++;			// this flags how many threads have returned
}

float
Ranf(unsigned int *seedp, float low, float high)
{
	float r = (float)rand_r(seedp);              // 0 - RAND_MAX
	return(low + r * (high - low) / (float)RAND_MAX);
}

int
Ranf(unsigned int *seedp, int ilow, int ihigh)
{
	float low = (float)ilow;
	float high = (float)ihigh + 0.9999f;

	return (int)(Ranf(seedp, low, high));
}

float
SQR(float x)
{
	return x * x;
}
