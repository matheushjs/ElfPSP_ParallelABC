#ifndef MOVCHAIN_H
#define MOVCHAIN_H

#include <stdio.h>
#include "movelem.h"
#include "int3d.h"

/** Changes the given 'chain' in position 'eleIdx'.
 * The element in that position becomes set with movement 'bb' for the
 *   backbone and 'sc' for the side chain.
 */
void MovChain_set_element(MovElem * chain, int eleIdx, unsigned char bb, unsigned char sc);

/** Creates a chain of movements with 'size' bytes.
 * Each byte carries 4 bits (MSB) representing backbone movement, and 4 bits (LSB) for sidechain movement.
 * Everything is initialized to FRONT-right
 */
MovElem * MovChain_create(int size);

/** Takes a chain of movements and returns the spatial position of BB and SC beads over the 3D space.
 * 'coordsBB_p' and 'coordsSC_p' are pointers to where we should store, respectively, the coordinates for
 *    the backbone beads and the side chain beads.
 */
void MovChain_build_3d(const MovElem * chain, // input
	int chainSize,    // input
	int3d **coordsBB_p, // output
	int3d **coordsSC_p  // output
);


#endif // MOVCHAIN_H
