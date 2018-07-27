#ifndef FITNESS_COLLISIONS_LINEAR_C_H
#define FITNESS_COLLISIONS_LINEAR_C_H

/* IMPORTANT NOTE
 * The signature of functions in "fitness_collisions_linear.c.h" and
 *   "fitness_collisions_quadratic.c.h" match each other.
 */

#include <int3d.h>
#include <utils.h>

/* Include FitnessCalc structures and routines, as well as some math structures */
/* We might need to access the FIT_BUNDLE array */
#include "fitness_structures.c.h"


/* Counts the number of conflicts among the protein beads.
 * 'space3d' is 3D lattice whose axis has size axisSize (positive + negative sides of the axis).
 */
static
int count_collisions(int threadId, const int3d *coordsBB, const int3d *coordsSC, int hpSize){
	int i, collisions;

	// Get space3d associated with that thread
	char *space3d = FIT_BUNDLE[threadId].space3d;
	int axisSize = FIT_BUNDLE[threadId].axisSize;
	
	collisions = 0;

	// Reset space
	for(i = 0; i < hpSize; i++){
		int idx = COORD3D(coordsSC[i], axisSize);
		space3d[idx] = 0;
		idx = COORD3D(coordsBB[i], axisSize);
		space3d[idx] = 0;
	}

	// Place beads in the space (actually calculate the collisions at the same time)
	for(i = 0; i < hpSize; i++){
		int idx = COORD3D(coordsSC[i], axisSize);
		if(space3d[idx]){
			collisions++;
		}
		space3d[idx]++;

		idx = COORD3D(coordsBB[i], axisSize);
		if(space3d[idx]){
			collisions++;
		}
		space3d[idx]++;
	}

	return collisions;
}

#endif // FITNESS_COLLISIONS_LINEAR_C_H
