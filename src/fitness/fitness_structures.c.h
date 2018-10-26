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

// We need this declaration. It's implemented later.
static double calc_max_gyration(const HPElem * hpChain, int hpSize);

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

