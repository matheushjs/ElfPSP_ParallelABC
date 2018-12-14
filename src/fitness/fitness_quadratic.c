
#include <int3d.h>
#include <hpchain.h>
#include <utils.h>
#include <movchain.h>
#include <fitness/fitness.h>
#include <config.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "fitness_private.h"

/* Include gyration calculation procedures */
#include "fitness_gyration.c.h"

static FitnessCalc FIT_BUNDLE[MAX_POINTERS] = { {0, 0, NULL, 0, 0} };

void FitnessCalc_initialize(int threadId, const HPElem * hpChain, int hpSize){
	if(threadId >= MAX_POINTERS){
		error_print("%s", "Invallid index given.\n");
		exit(EXIT_FAILURE);
	}

	FIT_BUNDLE[threadId].hpChain = hpChain;
	FIT_BUNDLE[threadId].hpSize = hpSize;
	FIT_BUNDLE[threadId].maxGyration = calc_max_gyration(hpChain, hpSize);
}

void FitnessCalc_cleanup(int threadId){
	return;
}

/* Returns the FitnessCalc with index 'threadId'
 */
static inline
FitnessCalc FitnessCalc_get(int threadId){
	return FIT_BUNDLE[threadId];
}




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

/* Counts the number of H-H, P-P and H-P contacts in a O(n^2) method.
 * For each bead, see if it has a contact with any of the following beads.
 *
 * Returns a triple where:
 *   result.first is the number of H-H contacts
 *   result.second is the number of P-P contacts
 *   result.triple is the number of H-P contacts
 *
 * Depending on the configuration of EPS_PP, EPS_HH, EPS_HP macros, the number of the
 *   respective contacts is not calculated and is always returned as 0.
 */
static
ITriple count_contacts_ss(int threadId, const int3d *coordsSC, const HPElem * hpChain, int hpSize){
	int i, j;

	int HH = 0;
	int PP = 0;
	int HP = 0;

	// For each bead
	for(i = 0; i < hpSize-1; i++){
		// Check all following beads for contacts
		for(j = i+1; j < hpSize; j++){
			int3d a = coordsSC[i];
			int3d b = coordsSC[j];
			int isAdjacent = int3d_isDist1(a, b);

#if EPS_HH != 0
			if(hpChain[i] == 'H' && hpChain[j] == 'H' && isAdjacent){
				HH += 1;
			}
#endif // EPS_HH != 0

#if EPS_HH != 0 && EPS_PP != 0
			else
#endif // EPS_HH != 0 && EPS_PP != 0

#if EPS_PP != 0
			if(hpChain[i] == 'P' && hpChain[j] == 'P' && isAdjacent){
				PP += 1;
			}
#endif // EPS_PP != 0

#if (EPS_HH != 0 || EPS_PP != 0) && EPS_HP != 0
			else
#endif // (EPS_HH != 0 || EPS_PP != 0) && EPS_HP != 0

#if EPS_HP != 0
			if(   (hpChain[i] == 'H' && hpChain[j] == 'P')
			||    (hpChain[i] == 'P' && hpChain[j] == 'H') ){
				if(isAdjacent) HP += 1;
			}
#endif // EPS_HP != 0
		}
	}

	ITriple res = { HH, PP, HP };
	return res;
}


/* Counts the number of Backbone-Backbone contacts in a O(n^2) method.
 * For each bead, see if it has a contact with any of the following beads.
 */
static
int count_contacts_bb(int threadId, const int3d *coordsBB, int hpSize){
	int i, j;

	int count = 0;

	for(i = 0; i < hpSize-1; i++){
		for(j = i+2; j < hpSize; j++){ // j = i+2 so that we don't count neighbor contacts
			int3d a = coordsBB[i];
			int3d b = coordsBB[j];
			if(int3d_isDist1(a, b)){
				count += 1;
			}
		}
	}

	return count;
}

/* Counts the number of B-H and B-P contacts in a O(n^2) method
 * For each backbone bead, check all the side-chain beads
 *
 * Returns a pair where:
 *		pair.first: number of B-H contacts
 *      pair.second: number of B-P contacts
 *
 * Depending on the macros EPS_BH and EPS_BP, no calculations are done and
 *   the number of contacts is returned as 0.
 */
static
IPair count_contacts_bs(int threadId, const int3d *coordsBB, const int3d *coordsSC, const HPElem * hpChain, int hpSize){
	int i, j;

	int BH = 0;
	int BP = 0;

	// For each backbone bead
	for(i = 0; i < hpSize; i++){
		// For each sidechain bead
		for(j = 0; j < hpSize; j++){
			// Skip the sidechain in the same aminoacid
			if(j == i) continue;

			int3d bb = coordsBB[i];
			int3d sc = coordsSC[j];
			int isAdjacent = int3d_isDist1(bb, sc);

#if EPS_HB != 0
			if(isAdjacent && hpChain[j] == 'H'){
				BH += 1;
			}
#endif // EPS_HB != 0

#if EPS_HP != 0 && EPS_HB != 0
			else
#endif // EPS_HP != 0 && EPS_HB != 0

#if EPS_HP != 0
			if(isAdjacent && hpChain[j] == 'P'){
				BP += 1;
			}
#endif // EPS_HP != 0
		}
	}

	IPair res = { BH, BP };
	return res;
}



/* Include FitnessCalc_run and FitnessCalc_measures */
#include "fitness_run.c.h"
