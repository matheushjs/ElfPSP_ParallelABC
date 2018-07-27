
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
#define AXIS_SIZE(HPSIZE) (HPSIZE+3)*2

/* Include FitnessCalc structures and routines, as well as some math structures */
#include "fitness_structures.c.h"

/* Include gyration calculation procedures */
#include "fitness_gyration.c.h"

/* Include collision/contact counting routines */
#if COUNTING_QUADRATIC == 0
	#include "fitness_collisions_linear.c.h"
	#include "fitness_contacts_linear.c.h"
#elif COUNTING_QUADRATIC == 1
	#include "fitness_collisions_quadratic.c.h"
	#include "fitness_contacts_quadratic.c.h"
#endif

double FitnessCalc_run(int threadId, const int3d *coordsBB, const int3d *coordsSC){
	int i;
	FitnessCalc fitCalc = FitnessCalc_get(threadId);

	// H is the energy related to different kinds of contacts among side-chain and backbone beads.
	double H = 0; // Free energy of the protein

	struct Calculations {
		ITriple ss_contacts; // Order: HH, PP, HP
		IPair   bs_contacts; // Order: HB, PB
		int     bb_contacts;
		int     collisions;
	} calc;

	calc.ss_contacts = count_contacts_ss(threadId, coordsSC, fitCalc.hpChain, fitCalc.hpSize);
	calc.bs_contacts = count_contacts_bs(threadId, coordsBB, coordsSC, fitCalc.hpChain, fitCalc.hpSize);
	calc.bb_contacts = count_contacts_bb(threadId, coordsBB, fitCalc.hpSize);
	calc.collisions  = count_collisions(threadId, coordsBB, coordsSC, fitCalc.hpSize);

#if EPS_HH != 0
	H += EPS_HH * calc.ss_contacts.first;
#endif
#if EPS_PP != 0
	H += EPS_PP * calc.ss_contacts.second;
#endif
#if EPS_HP != 0
	H += EPS_HP * calc.ss_contacts.third;
#endif
#if EPS_HB != 0
	H += EPS_HB * calc.bs_contacts.first;
#endif
#if EPS_PB != 0
	H += EPS_PB * calc.bs_contacts.second;
#endif
#if EPS_BB != 0
	H += EPS_BB * calc.bb_contacts;
#endif

	debug_print("Contacts Energy (H): %lf\n", H);

	double penalty = PENALTY_VALUE * calc.collisions;
	debug_print("Penalty: %lf\n", penalty);

// Then we calculate the mass center for H beads and P beads (we'll need for gyration)
// Sum all coordinates for P and H beads
	int3d sumP = int3d_make(0, 0, 0);
	int3d sumH = int3d_make(0, 0, 0);
	int countP = 0;
	int countH = 0;
	for(i = 0; i < fitCalc.hpSize; i++){
		if(fitCalc.hpChain[i] == 'H'){
			sumH = int3d_add(sumH, coordsSC[i]);
			countH ++;
		} else /* bead is Polar */ {
			sumP = int3d_add(sumP, coordsSC[i]);
			countP ++;
		}
	}

// Get mass centers for H and P
	DPoint centerH = { sumH.x / (double) countH,
					   sumH.y / (double) countH,
					   sumH.z / (double) countH };
	DPoint centerP = { sumP.x / (double) countP,
					   sumP.y / (double) countP,
					   sumP.z / (double) countP };

	debug_print("Center for 'H': (%lf,%lf,%lf)\n", centerH.x, centerH.y, centerH.z);
	debug_print("Center for 'P': (%lf,%lf,%lf)\n", centerP.x, centerP.y, centerP.z);

// Calculate the gyration for both bead types
	DPair RG_HP = calc_gyration_joint(coordsSC, fitCalc.hpChain, fitCalc.hpSize, centerH, centerP);
	debug_print("Gyr Pair (H,P): (%lf, %lf)\n", RG_HP.first, RG_HP.second);

// Calculate max gyration of H beads
	double maxRG_H = fitCalc.maxGyration;
	debug_print("maxRG_H: %lf\n", maxRG_H);

// Calculate RadiusG_H
	double radiusG_H = maxRG_H - RG_HP.first;
	debug_print("radiusG_H: %lf\n", radiusG_H);

// Calculate RadiusG_P
	double radiusG_P;
	if(RG_HP.second >= RG_HP.first || countP == 0){
		radiusG_P = 1;
	} else {
		radiusG_P = 1 / (1 - (RG_HP.second - RG_HP.first));
	}
	debug_print("radiusG_P: %lf\n", radiusG_P);

	return (H - penalty) * radiusG_H * radiusG_P;
}

double FitnessCalc_run2(int threadId, const MovElem * chain){
	int3d *coordsBB, *coordsSC;

	FitnessCalc fitCalc = FitnessCalc_get(threadId);
	int chainSize = fitCalc.hpSize - 1;

	MovChain_build_3d(chain, chainSize, &coordsBB, &coordsSC);
	double fit = FitnessCalc_run(threadId, coordsBB, coordsSC);
	free(coordsBB);
	free(coordsSC);
	return fit;
}

void FitnessCalc_measures(int threadId, const MovElem *chain, int *Hcontacts_p, int *collisions_p, double *bbGyration_p){
	int3d *coordsBB, *coordsSC;

	FitnessCalc fitCalc = FitnessCalc_get(threadId);
	int chainSize = fitCalc.hpSize - 1;

	MovChain_build_3d(chain, chainSize, &coordsBB, &coordsSC);

	ITriple ss_contacts = count_contacts_ss(threadId, coordsSC, fitCalc.hpChain, fitCalc.hpSize);
	int collisions  = count_collisions(threadId, coordsBB, coordsSC, fitCalc.hpSize);

	if(Hcontacts_p){
		*Hcontacts_p = ss_contacts.first;
	}

	if(collisions_p){
		*collisions_p = collisions;
	}

	if(bbGyration_p){
		int3d sum = int3d_make(0, 0, 0);
		int i;

		// Sum coordinates
		for(i = 0; i < fitCalc.hpSize; i++){
			sum = int3d_add(sum, coordsBB[i]);
		}

		// Get center
		DPoint center = {  sum.x / (double) fitCalc.hpSize,
						   sum.y / (double) fitCalc.hpSize,
						   sum.z / (double) fitCalc.hpSize };

		*bbGyration_p = calc_gyration(coordsBB, fitCalc.hpSize, center);
	}

	free(coordsBB);
	free(coordsSC);
}
