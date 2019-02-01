#include "fitness_private.h"
#include "fitness.h"
#include "gyration.h"

double FitnessCalc_run(const int3d *coordsBB, const int3d *coordsSC){
	int i;
	FitnessCalc fitCalc = FitnessCalc_get();

	// H is the energy related to different kinds of contacts among side-chain and backbone beads.
	double H = 0; // Free energy of the protein

	BeadMeasures measures = proteinMeasures(coordsBB, coordsSC, fitCalc.hpChain, fitCalc.hpSize);

	// Keep summing on energy
	H += EPS_HH * measures.hh;
	H += EPS_PP * measures.pp;
	H += EPS_HP * measures.hp;
	H += EPS_HB * measures.hb;
	H += EPS_PB * measures.pb;
	H += EPS_BB * measures.bb;

	double penalty = PENALTY_VALUE * measures.collisions;

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

// Calculate the gyration for both bead types
	DPair RG_HP = calc_gyration_joint(coordsSC, fitCalc.hpChain, fitCalc.hpSize, centerH, centerP);

// Calculate max gyration of H beads
	double maxRG_H = fitCalc.maxGyration;

// Calculate RadiusG_H
	double radiusG_H = maxRG_H - RG_HP.first;

// Calculate RadiusG_P
	double radiusG_P;
	if(RG_HP.second >= RG_HP.first || countP == 0){
		radiusG_P = 1;
	} else {
		radiusG_P = 1 / (1 - (RG_HP.second - RG_HP.first));
	}

	return (H - penalty) * radiusG_H * radiusG_P;
}

double FitnessCalc_run2(const MovElem * chain){
	int3d *coordsBB, *coordsSC;

	FitnessCalc fitCalc = FitnessCalc_get();
	int chainSize = fitCalc.hpSize - 1;

	MovChain_build_3d(chain, chainSize, &coordsBB, &coordsSC);
	double fit = FitnessCalc_run(coordsBB, coordsSC);
	free(coordsBB);
	free(coordsSC);
	return fit;
}

void FitnessCalc_measures(const MovElem *chain, int *Hcontacts_p, int *collisions_p, double *bbGyration_p){
	int3d *coordsBB, *coordsSC;

	FitnessCalc fitCalc = FitnessCalc_get();
	int chainSize = fitCalc.hpSize - 1;

	MovChain_build_3d(chain, chainSize, &coordsBB, &coordsSC);

	BeadMeasures measures = proteinMeasures(coordsBB, coordsSC, fitCalc.hpChain, fitCalc.hpSize);

	if(Hcontacts_p){
		*Hcontacts_p = measures.hh;
	}

	if(collisions_p){
		*collisions_p = measures.collisions;
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
