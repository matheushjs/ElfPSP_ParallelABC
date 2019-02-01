#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "movchain.h"
#include "movelem.h"
#include "int3d.h"
#include "random.h"

void MovChain_set_element(MovElem * chain, int eleIdx, unsigned char bb, unsigned char sc){
	chain[eleIdx] = MovElem_make(bb, sc);
}

MovElem * MovChain_create(int size){
	MovElem * chain = malloc(sizeof(MovElem) * size);
	int i;
	for(i = 0; i < size; i++)
		MovChain_set_element(chain, i, FRONT, RIGHT);

	return chain;
}

// Given a predecessor vector and a movement, applies such
//   movement in the predecessor vector and returns the result.
static inline
int3d getNext(int3d pred, unsigned int movement){
	int *first, *second;
	int3d result;

	result = int3d_make(0, 0, 0);

	// Get first and second filled coordinates
	if(pred.x != 0){
		first = &result.y;
		second = &result.z;
	} else if(pred.y != 0) {
		first = &result.x;
		second = &result.z;
	} else /* z != 0 */ {
		first = &result.x;
		second = &result.y;
	}

	// Make the movement
	if(movement == FRONT){
		result = pred;
	} else if(movement == UP) {
		*first = 1; // fill first filled coordinate positively
	} else if(movement == DOWN) {
		*first = -1;
	} else if(movement == RIGHT) {
		*second = 1;
	} else /* movement == RIGHT */ {
		*second = -1;
	}

	return result;
}

void MovChain_build_3d(const MovElem * chain,
	int chainSize,
	int3d **coordsBB_p,
	int3d **coordsSC_p
){
	int3d *coordsBB;
	int3d *coordsSC;

	// Allocate sufficient space for the coordinates
	coordsBB = malloc(sizeof(int3d) * (chainSize + 1));
	coordsSC = malloc(sizeof(int3d) * (chainSize + 1));

	// Add initial BB
	// As a convention, the first backbone beads are at (1, 0, 0) and (2, 0, 0).
	coordsBB[0] = int3d_make(1, 0, 0);
	coordsBB[1] = int3d_make(2, 0, 0);

	// Place initial SC, which are exceptions.
	// The first MovChain element stores directions for the first 2 SC's.
	// All the other MovChain elements store for 1 SC and 1 BB.
	MovElem elem = chain[0];
	unsigned char mov1 = MovElem_getBB(elem);
	unsigned char mov2 = MovElem_getSC(elem);

	// predecessor vector and displacement vector
	int3d predVec, dispVec;

	// Add SC beads.
	// First predecessor vector is (-1, 0, 0) from BB[1] to BB[0].
	// Second is (1, 0, 0) from BB[0] to BB[1]. 
	coordsSC[0] = int3d_add(getNext(int3d_make(-1, 0, 0), mov1), coordsBB[0]);
	predVec = int3d_make(1, 0, 0); // Will feed the loop as the first predecessor vector
	coordsSC[1] = int3d_add(getNext(predVec, mov2), coordsBB[1]);

	// Iterate over the chain
	// There should be N+1 beads and N chain elements
	int i;
	for(i = 2; i <= chainSize; i++){ // i represents index of current bead being added
		MovElem elem = chain[i-1];
		mov1 = MovElem_getBB(elem);
		mov2 = MovElem_getSC(elem);

		// Get displacement vector for backbone
		dispVec = getNext(predVec, mov1);

		// Add next backbone bead
		int3d nextBead = int3d_add(dispVec, coordsBB[i-1]);
		coordsBB[i] = nextBead;

		// Update predecessor vector
		predVec = dispVec;

		// Get displacement vector for side chain
		dispVec = getNext(predVec, mov2);

		// Add next sidechain bead
		nextBead = int3d_add(dispVec, coordsBB[i]);
		coordsSC[i] = nextBead;

		// Predecessor vector is kept for next iteration.
	}

	*coordsBB_p = coordsBB;
	*coordsSC_p = coordsSC;
}

/* DEBUGGING PROCEDURES
*

void print_3d_coords(int3d *bbCo, int3d *scCo, int size){
	int i;
	for(i = 0; i < size; i++){
		printf("(%d,%d,%d), (%d,%d,%d)\n",
			   bbCo[i].x, bbCo[i].y, bbCo[i].z,
			   scCo[i].x, scCo[i].y, scCo[i].z);
	}
}

int main(int argc, char *argv[]){
	int size = 6;

	MovElem * chain = MovChain_create(size);

	MovChain_set_element(chain, 0, UP,    RIGHT);
	MovChain_set_element(chain, 1, UP,    LEFT);
	MovChain_set_element(chain, 2, FRONT, RIGHT);
	MovChain_set_element(chain, 3, LEFT,  UP);
	MovChain_set_element(chain, 4, RIGHT, DOWN);
	MovChain_set_element(chain, 5, DOWN,  FRONT);

	int3d *bbCo, *scCo;
	MovChain_build_3d(chain, size, &bbCo, &scCo);

	print_3d_coords(bbCo, scCo, size+1);
}

*
*/
