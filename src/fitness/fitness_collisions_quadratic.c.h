#ifndef FITNESS_COLLISIONS_QUADRATIC_C_H
#define FITNESS_COLLISIONS_QUADRATIC_C_H

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
 * 'hpSize' should be the number of coordinates in each coordinate vector.
 */
static
int count_collisions(int threadId, const int3d *coordsBB, const int3d *coordsSC, int hpSize){
	// The number of collisions is the number of beads in the same 3D point minus 1.

	int i, j;
	int collisions = 0;

	for(i = 0; i < hpSize; i++){
		// for the i-th backbone beads, compare it to the following BB beads,
		//   and then with all the SC beads
		int count = 0;
		int3d bead = coordsBB[i];

		// Check following backbone beads
		for(j = i+1; j < hpSize; j++){
			if(int3d_equal(bead, coordsBB[j]))
				count++;
		}

		// Check all SC beads
		for(j = 0; j < hpSize; j++){
			if(int3d_equal(bead, coordsSC[j]))
				count++;
		}

		// Say the current bead has 'count' == 2
		// That means this bead collides with 2 more
		// Which means the final 'collisions' should be incremented by 2
		//
		// This bead will increment 'collisions' by 1
		// When the next colliding bead is analyzed, its 'count' will be 1, incrementing 'collisions' once again
		// The final 'collisions' will be incremented by 2, as desired.
		if(count >= 1)
			collisions++;
	}

	for(i = 0; i < hpSize-1; i++){
		// Now for each i-th side chain bead, compare it to the following SC beads
		int count = 0;
		int3d bead = coordsSC[i];

		for(j = i+1; j < hpSize; j++){
			if(int3d_equal(bead, coordsSC[j]))
				count++;
		}

		if(count >= 1)
			collisions++;
	}

	return collisions;
}

#endif // FITNESS_COLLISIONS_QUADRATIC_C_H

