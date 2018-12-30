#ifndef _FITNESS_PRIVATE_H
#define _FITNESS_PRIVATE_H

/* Header to be included just by files in the fitness/ directory
 */
#include <hpchain.h>
#include <int3d.h>

/**********************************
 *    FitnessCalc Procedures      *
 **********************************/

#define MAX_MEMORY ((long int) 4*1E9) // Max total size of memory allocated

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

typedef struct {
	int hh, pp, hp, hb, pb, bb;
	int collisions;
} BeadMeasures;

FitnessCalc FitnessCalc_get(); // Returns the FIT_BUNDLE of the protein being assessed.
BeadMeasures proteinMeasures(const int3d *BBbeads, const int3d *SCbeads, const HPElem *hpChain, int hpSize);

#endif
