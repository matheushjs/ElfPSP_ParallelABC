#ifndef _FITNESS_PRIVATE_H
#define _FITNESS_PRIVATE_H

/* Header to be included just by files in the fitness/ directory
 */
#include <hpchain.h>

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

#endif
