#ifndef FITNESS_FITCALC_STRUCTURE_H
#define FITNESS_FITCALC_STRUCTURE_H

#include <int3d.h>
#include <hpchain.h>
#include <utils.h>
#include <movchain.h>
#include <fitness/fitness.h>

#include <stdlib.h>

/**********************************
 *    FitnessCalc Procedures      *
 **********************************/

#define MAX_POINTERS 64 // Max number of different chunks of memory allocated
#define MAX_MEMORY 4E8  // Max total size of memory allocated

/* So that memory allocated can be reused throughout calls to
 *   fitness calculation functions, we have this structure.
 */
typedef struct _FitnessCalc {
	const HPElem * hpChain;
	int hpSize;
	void *space3d;
	int axisSize;
	double maxGyration;
} FitnessCalc;

static FitnessCalc FIT_BUNDLE[MAX_POINTERS] = { {0, 0, NULL, 0, 0} };
static int MEM_USED = 0;

// We need this declaration. It's implemented later.
static double calc_max_gyration(const HPElem * hpChain, int hpSize);

void FitnessCalc_initialize(int threadId, const HPElem * hpChain, int hpSize){
	if(threadId >= MAX_POINTERS){
		error_print("%s", "Invallid index given.\n");
		exit(EXIT_FAILURE);
	}

	if(FIT_BUNDLE[threadId].space3d != NULL){
		error_print("%s", "Double initialization.\n");
		exit(EXIT_FAILURE);
	}

	FIT_BUNDLE[threadId].hpChain = hpChain;
	FIT_BUNDLE[threadId].hpSize = hpSize;

	int axisSize = (hpSize+3)*2;
	int spaceSize = axisSize * axisSize * axisSize;

	/*
	 * RACE CONDITION HERE
	 */
	MEM_USED += spaceSize;

	// Failsafe for memory usage
	if(MEM_USED > MAX_MEMORY){
		error_print("Will not allocate more than %g memory.\n", (double) MAX_MEMORY);
		exit(EXIT_FAILURE);
	}

	FIT_BUNDLE[threadId].axisSize = axisSize;
	FIT_BUNDLE[threadId].space3d = malloc(spaceSize * sizeof(char));
	FIT_BUNDLE[threadId].maxGyration = calc_max_gyration(hpChain, hpSize);

	debug_print("Space allocated: %lf GiB\n", spaceSize * sizeof(char) / 1024.0 / 1024.0 / 1024.0);
}

void FitnessCalc_cleanup(int threadId){
	// No checks will be done
	free(FIT_BUNDLE[threadId].space3d);
	FIT_BUNDLE[threadId].space3d = NULL;

	int axisSize = FIT_BUNDLE[threadId].axisSize;
	MEM_USED -= axisSize * axisSize * axisSize;
}

/* Returns the FitnessCalc with index 'threadId'
 */
static inline
FitnessCalc FitnessCalc_get(int threadId){
	if(FIT_BUNDLE[threadId].space3d == NULL){
		error_print("%s", "FitnessCalc must be initialized.\n");
		exit(EXIT_FAILURE);
	}
	return FIT_BUNDLE[threadId];
}



typedef struct {
	double x;
	double y;
	double z;
} DPoint;

typedef struct {
	double first;
	double second;
} DPair;

typedef struct {
	int first;
	int second;
} IPair;


typedef struct {
	int first;
	int second;
	int third;
} ITriple;

static
double dsquare(double a){
	return a * a;
}

#endif // FITNESS_FITCALC_STRUCTURE_H

