#ifndef FITNESS_CONTACTS_QUADRATIC_C_H
#define FITNESS_CONTACTS_QUADRATIC_C_H

/* IMPORTANT NOTE
 * The signature of all functions in "fitness_contacts_linear.c.h" and
 *   "fitness_contacts_quadratic.c.h" match each other.
 */

#include <int3d.h>
#include <hpchain.h>
#include <utils.h>
#include <movchain.h>
#include <fitness/fitness.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Include FitnessCalc structures and routines, as well as some math structures */
/* We might need to access the FIT_BUNDLE array */
#include "fitness_structures.c.h"


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

#endif // FITNESS_CONTACTS_QUADRATIC_C_H

