
#include <int3d.h>
#include <hpchain.h>
#include <utils.h>
#include <movchain.h>
#include <fitness/fitness.h>
#include <config.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

#define COORD3D(V, AXIS) COORD(V.x, V.y, V.z, AXIS)
#define COORD(X, Y, Z, AXIS) ( (Z+AXIS/2) * (AXIS*AXIS) + (Y+AXIS/2) * (AXIS) + (X+AXIS/2))

/* Include FitnessCalc structures and routines, as well as some math structures */
#include "fitness_structures.c.h"

/* Include gyration calculation procedures */
#include "fitness_gyration.c.h"

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

/* Counts the number of H-H, P-P and H-P contacts in a O(n) method.
 *
 * Returns a triple where:
 *   result.first is the number of H-H contacts
 *   result.second is the number of P-P contacts
 *   result.triple is the number of H-P contacts
 *
 * Depending on the configuration of EPS_PP, EPS_HH, EPS_HP macros, the number of the
 *   respective contacts is not calculated and is always returned as 0.
 *
 * 'space3d' is 3D lattice whose axis has size axisSize (positive + negative sides of the axis).
 */
static
ITriple count_contacts_ss(int threadId, const int3d *coordsSC, const HPElem * hpChain, int hpSize){
	int i;

	// Get space3d associated with that thread
	char *space3d = FIT_BUNDLE[threadId].space3d;
	int axisSize = FIT_BUNDLE[threadId].axisSize;

	int HH = 0;
	int HP = 0;
	int PP = 0;

#if EPS_HH != 0 || EPS_HP != 0
	// Reset space
	for(i = 0; i < hpSize; i++){
		int3d a = coordsSC[i];
		space3d[COORD(a.x+1, a.y, a.z, axisSize)] = 0;
		space3d[COORD(a.x-1, a.y, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y+1, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y-1, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y, a.z+1, axisSize)] = 0;
		space3d[COORD(a.x, a.y, a.z-1, axisSize)] = 0;
		// Yes, there is no need to reset the point itself.
	}

	// Place H beads in the space
	for(i = 0; i < hpSize; i++){
		if(hpChain[i] == 'H'){
			int3d a = coordsSC[i];
			space3d[COORD(a.x, a.y, a.z, axisSize)]++;
		}
	}

	// Count HH and HP contacts
	for(i = 0; i < hpSize; i++){
		int3d a = coordsSC[i];
		int contacts = 0;
		contacts += space3d[COORD(a.x+1, a.y, a.z, axisSize)];
		contacts += space3d[COORD(a.x-1, a.y, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y+1, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y-1, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y, a.z+1, axisSize)];
		contacts += space3d[COORD(a.x, a.y, a.z-1, axisSize)];

		if(hpChain[i] == 'H'){
			HH += contacts;
		} else /* hpChain[i] == 'P' */ {
			HP += contacts;
		}
	}

	HH /= 2; // Because we count each H-H contact twice
#endif // EPS_HH != 0 || EPS_HP != 0

#if EPS_PP != 0
	// Reset space only for polar beads
	for(i = 0; i < hpSize; i++){
		if(hpChain[i] == 'P'){
			int3d a = coordsSC[i];
			space3d[COORD(a.x+1, a.y, a.z, axisSize)] = 0;
			space3d[COORD(a.x-1, a.y, a.z, axisSize)] = 0;
			space3d[COORD(a.x, a.y+1, a.z, axisSize)] = 0;
			space3d[COORD(a.x, a.y-1, a.z, axisSize)] = 0;
			space3d[COORD(a.x, a.y, a.z+1, axisSize)] = 0;
			space3d[COORD(a.x, a.y, a.z-1, axisSize)] = 0;
			// Yes, there is no need to reset the point itself.
		}
	}

	// Place P beads in the space
	for(i = 0; i < hpSize; i++){
		if(hpChain[i] == 'P'){
			int3d a = coordsSC[i];
			space3d[COORD(a.x, a.y, a.z, axisSize)]++;
		}
	}

	// Count PP contacts
	for(i = 0; i < hpSize; i++){
		if(hpChain[i] == 'P'){
			int3d a = coordsSC[i];
			PP += space3d[COORD(a.x+1, a.y, a.z, axisSize)];
			PP += space3d[COORD(a.x-1, a.y, a.z, axisSize)];
			PP += space3d[COORD(a.x, a.y+1, a.z, axisSize)];
			PP += space3d[COORD(a.x, a.y-1, a.z, axisSize)];
			PP += space3d[COORD(a.x, a.y, a.z+1, axisSize)];
			PP += space3d[COORD(a.x, a.y, a.z-1, axisSize)];
		}
	}

	PP /= 2; // Because we count each PP contact twice
#endif // EPS_PP != 0

	ITriple res = { HH, PP, HP };
	return res;
}


/* Counts the number of Backbone-Backbone contacts in a O(n) method.
 * 'space3d' is 3D lattice whose axis has size axisSize (positive + negative sides of the axis).
 */
static
int count_contacts_bb(int threadId, const int3d *coordsBB, int hpSize){
	int i;

	// Get space3d associated with that thread
	char *space3d = FIT_BUNDLE[threadId].space3d;
	int axisSize = FIT_BUNDLE[threadId].axisSize;

	// Reset space
	for(i = 0; i < hpSize; i++){
		int3d a = coordsBB[i];
		space3d[COORD(a.x+1, a.y, a.z, axisSize)] = 0;
		space3d[COORD(a.x-1, a.y, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y+1, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y-1, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y, a.z+1, axisSize)] = 0;
		space3d[COORD(a.x, a.y, a.z-1, axisSize)] = 0;
	}

	// Place the beads in the space
	for(i = 0; i < hpSize; i++){
		int3d a = coordsBB[i];
		space3d[COORD(a.x, a.y, a.z, axisSize)]++;
	}

	// Count contacts
	int contacts = 0;
	for(i = 0; i < hpSize; i++){
		int3d a = coordsBB[i];
		contacts += space3d[COORD(a.x+1, a.y, a.z, axisSize)];
		contacts += space3d[COORD(a.x-1, a.y, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y+1, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y-1, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y, a.z+1, axisSize)];
		contacts += space3d[COORD(a.x, a.y, a.z-1, axisSize)];
	}

	// Because we counted each contact twice
	contacts /= 2;

	// In a protein with N beads, there are N-1 adjacent contacts. We remove them.
	contacts -= hpSize - 1;

	return contacts;
}

/* Counts the number of B-H and B-P contacts in a O(n) method.
 *
 * Returns a pair where:
 *   result.first is the number of B-H contacts
 *   result.second is the number of B-P contacts
 *
 * Depending on the configuration of EPS_HB, EPS_HP macros, the number of the
 *   respective contacts is not calculated and is always returned as 0.
 *
 * 'space3d' is 3D lattice whose axis has size axisSize (positive + negative sides of the axis).
 */
static
IPair count_contacts_bs(int threadId, const int3d *coordsBB, const int3d *coordsSC, const HPElem * hpChain, int hpSize){
	int i;

	// Get space3d associated with that thread
	char *space3d = FIT_BUNDLE[threadId].space3d;
	int axisSize = FIT_BUNDLE[threadId].axisSize;

	int BH = 0;
	int BP = 0;

	// We will count the number of P and H beads.
	int Hcount = 0, Pcount = 0;


#if EPS_HB != 0 || EPS_PB != 0
	// Reset space around side-chain beads
	for(i = 0; i < hpSize; i++){
		int3d a = coordsSC[i];
		space3d[COORD(a.x+1, a.y, a.z, axisSize)] = 0;
		space3d[COORD(a.x-1, a.y, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y+1, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y-1, a.z, axisSize)] = 0;
		space3d[COORD(a.x, a.y, a.z+1, axisSize)] = 0;
		space3d[COORD(a.x, a.y, a.z-1, axisSize)] = 0;
		// Yes, there is no need to reset the point itself.
	}

	// Place backbone beads in the space
	for(i = 0; i < hpSize; i++){
		int3d a = coordsBB[i];
		space3d[COORD(a.x, a.y, a.z, axisSize)]++;
	}

	// Count HB and PB contacts
	for(i = 0; i < hpSize; i++){
		int3d a = coordsSC[i];
		int contacts = 0;
		contacts += space3d[COORD(a.x+1, a.y, a.z, axisSize)];
		contacts += space3d[COORD(a.x-1, a.y, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y+1, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y-1, a.z, axisSize)];
		contacts += space3d[COORD(a.x, a.y, a.z+1, axisSize)];
		contacts += space3d[COORD(a.x, a.y, a.z-1, axisSize)];

		if(hpChain[i] == 'H'){
			BH += contacts;
			Hcount += 1;
		} else /* hpChain[i] == 'P' */ {
			BP += contacts;
			Pcount += 1;
		}
	}

	// BH contacts include the contact from aminoacids of type H
	// So we subtract them
	BH -= Hcount;

	// Same for BP contacts
	BP -= Pcount;
#endif // EPS_HB != 0 || EPS_PB != 0

	IPair res = { BH, BP };
	return res;
}



/* Include FitnessCalc_run and FitnessCalc_measures */
#include "fitness_run.c.h"
