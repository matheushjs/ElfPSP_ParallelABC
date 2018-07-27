#ifndef FITNESS_GYRATION_C_H
#define FITNESS_GYRATION_C_H

#include <int3d.h>
#include <hpchain.h>
#include <utils.h>
#include <movchain.h>
#include <fitness/fitness.h>

#include <math.h>
#include <stdlib.h>
#include <string.h>

/* Include FitnessCalc structures and routines, as well as some math structures */
#include "fitness_structures.c.h"


/* coords - the coordinates for the beads
 * size   - the number of beads
 * center - the center points (baricenter) for the beads.
 *
 * Returns the gyration radius for the given beads.
 */
static
double calc_gyration(const int3d *coords, int size, DPoint center){
	double gyr = 0;

	// First we perform the sum of squares
	int i;
	for(i = 0; i < size; i++){
		int3d elem = coords[i];

		// Perform sum of squares
		gyr += dsquare(elem.x - center.x);
		gyr += dsquare(elem.y - center.y);
		gyr += dsquare(elem.z - center.z);
	}

	// Final touches
	gyr = sqrt(gyr / size);

	return gyr;
}

/* coordsSC - the coordinates for all side chain beads
 * hpChain - the string representing the types of the side chain beads
 * hpSize - the number of side chain beads
 * centerH and centerP are the center points (baricenter) for H beads and P beads respectively.
 *
 * Returns a DPair where the first element is the gyration for H beads.
 * The second element is gyration for P beads.
 */
static
DPair calc_gyration_joint(const int3d *coordsSC, const HPElem * hpChain, int hpSize, DPoint centerH, DPoint centerP){
	DPair gyr = { 0, 0 };
	int countH = 0, countP = 0;

	// First we perform the sum of squares
	int i;
	for(i = 0; i < hpSize; i++){
		int3d elem = coordsSC[i];

		// Perform sum of squares
		if(hpChain[i] == 'H'){
			gyr.first += dsquare(elem.x - centerH.x);
			gyr.first += dsquare(elem.y - centerH.y);
			gyr.first += dsquare(elem.z - centerH.z);

			countH++;
		} else {
			gyr.second += dsquare(elem.x - centerP.x);
			gyr.second += dsquare(elem.y - centerP.y);
			gyr.second += dsquare(elem.z - centerP.z);

			countP++;
		}
	}

	// Final touches
	gyr.first = sqrt(gyr.first / countH);
	gyr.second = sqrt(gyr.second / countP);

	// Because we would have divided by 0 above
	if(countP == 0) gyr.second = 1;

	return gyr;
}


/* Calculate MaxRG_H which is the radius of gyration for the hydrophobic beads
 *   considering the protein completely unfolded
 *
 * hpChain - the string representing the types of the side chain beads
 * hpSize - the number of side chain beads
 */
static
double calc_max_gyration(const HPElem * hpChain, int hpSize){
	// First we get the sum of the X coordinates of the H beads
	int xCoordSum = 0, countH = 0;
	int i;
	for(i = 0; i < hpSize; i++){
		if(hpChain[i] == 'H'){
			xCoordSum += i;
			countH += 1;
		}
	}
	// Then we get the center of mass of these H beads
	double xCoordCenter = xCoordSum / (double) countH;

	// Get the sum of squares
	double maxRG_H = 0;
	for(i = 0; i < hpSize; i++){ // Remaining beads
		if(hpChain[i] == 'H'){
			maxRG_H += dsquare(fabs(i - xCoordCenter) + 1); // The 1 represents the addition on the
														   // X coordinate resulting from the bending
		}
	}

	// Final touches
	return sqrt(maxRG_H / countH);
}

#endif // FITNESS_GYRATION_C_H

