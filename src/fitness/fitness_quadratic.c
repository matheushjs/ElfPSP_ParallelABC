
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
#include "gyration.h"

static FitnessCalc FIT_BUNDLE = {0, 0, NULL, 0, 0};

void FitnessCalc_initialize(const HPElem * hpChain, int hpSize){
	FIT_BUNDLE.hpChain = hpChain;
	FIT_BUNDLE.hpSize = hpSize;
	FIT_BUNDLE.maxGyration = calc_max_gyration(hpChain, hpSize);
}

void FitnessCalc_cleanup(){
	return;
}

/* Returns the FitnessCalc
 */
static inline
FitnessCalc FitnessCalc_get(){
	return FIT_BUNDLE;
}



/* Counts the number of conflicts among the protein beads.
 */
static
int count_collisions(const int3d *beads, int nBeads){
	int i, j;
	int collisions = 0;

	for(i = 0; i < nBeads; i++){
		int3d bead = beads[i];

		// Check following backbone beads
		for(j = i+1; j < nBeads; j++){
			if(int3d_equal(bead, beads[j]))
				collisions++;
		}
	}

	return collisions;
}

/* Counts the number of contacts among the protein beads.
 */
static
int count_contacts(const int3d *beads, int nBeads){
	int i, j;
	int contacts = 0;

	for(i = 0; i < nBeads; i++){
		int3d bead = beads[i];

		// Check following backbone beads
		for(j = i+1; j < nBeads; j++){
			if(int3d_isDist1(bead, beads[j]))
				contacts++;
		}
	}

	return contacts;
}

BeadMeasures proteinMeasures(const int3d *BBbeads, const int3d *SCbeads, const HPElem *hpChain, int hpSize){
	int i;

	// Create vectors with desired coordinates of beads
	int3d *coordsAll = malloc(sizeof(int3d) * hpSize * 2);
	int    sizeAll = 0;
	int3d *coordsBB  = malloc(sizeof(int3d) * hpSize);
	int    sizeBB  = 0;
	int3d *coordsHB  = malloc(sizeof(int3d) * hpSize * 2);
	int    sizeHB  = 0;
	int3d *coordsPB  = malloc(sizeof(int3d) * hpSize * 2);
	int    sizePB  = 0;
	int3d *coordsHH  = malloc(sizeof(int3d) * hpSize);
	int    sizeHH  = 0;
	int3d *coordsHP  = malloc(sizeof(int3d) * hpSize);
	int    sizeHP  = 0;
	int3d *coordsPP  = malloc(sizeof(int3d) * hpSize);
	int    sizePP  = 0;

	for(i = 0; i < hpSize; i++){
		coordsAll[sizeAll++] = BBbeads[i];
		coordsBB[sizeBB++]   = BBbeads[i];
		coordsHB[sizeHB++]   = BBbeads[i];
		coordsPB[sizePB++]   = BBbeads[i];
	}

	for(i = 0; i < hpSize; i++){
		coordsAll[sizeAll++] = SCbeads[i];
		coordsHP[sizeHP++]  = SCbeads[i];
		if(hpChain[i] == 'H'){
			coordsHH[sizeHH++] = SCbeads[i];
			coordsHB[sizeHB++] = SCbeads[i];
		} else {
			coordsPP[sizePP++] = SCbeads[i];
			coordsPB[sizePB++] = SCbeads[i];
		}
	}

	BeadMeasures retval;

	retval.hh = count_contacts(coordsHH, sizeHH);
	retval.pp = count_contacts(coordsPP, sizePP);
	retval.hp = count_contacts(coordsHP, sizeHP) - retval.hh - retval.pp; // HP = all - HH - PP
	retval.bb = count_contacts(coordsBB, sizeBB);
	retval.hb = count_contacts(coordsHB, sizeHB) - retval.hh - retval.bb; // HB = all - HH - BB
	retval.pb = count_contacts(coordsPB, sizePB) - retval.pp - retval.bb; // PB = all - PP - BB
	retval.collisions = count_collisions(coordsAll, sizeAll);

	free(coordsAll);
	free(coordsBB);
	free(coordsHB);
	free(coordsPB);
	free(coordsHH);
	free(coordsHP);
	free(coordsPP);

	return retval;
}

/* Include FitnessCalc_run and FitnessCalc_measures */
#include "fitness_run.c.h"
