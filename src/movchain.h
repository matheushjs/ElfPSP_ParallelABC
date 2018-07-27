#ifndef MOVCHAIN_H
#define MOVCHAIN_H

#include <stdio.h>

/* Type that holds 2 movements, one for the backbone and one for the side chain.
 * The backbone movement is on the most significant 4 bits of this type.
 * The side chain movement is on the least significant 4 bits.
 * The type's size is supposed to be 1 byte.
 *
 * MovChain is how we refer to an array of MovElem.
 */
typedef unsigned char MovElem;

/* Constants for making MovElem elements
 */
extern const unsigned char FRONT;
extern const unsigned char LEFT;
extern const unsigned char RIGHT;
extern const unsigned char UP;
extern const unsigned char DOWN;

/* Returns a MovElem whose value corresponds to a movement where 'bb' is applied to the backbone
 *   and 'sc' is applied to the side chain.
 */
MovElem MovElem_make(unsigned char bb, unsigned char sc);

/* Returns a uniformly random MovElem
 */
MovElem MovElem_random();

/* Returns the movement for the backbone (BB) or side chain (SC) stored in a MovElem.
 */
unsigned char MovElem_getSC(MovElem elem);
unsigned char MovElem_getBB(MovElem elem);

/* The functions below convert MovElem elements into numbers.
 * Each possible MovElem map into a single number in interval [0, 24].
 */
unsigned char MovElem_to_number(MovElem elem);
MovElem MovElem_from_number(unsigned char num);

/* Prints a movement in the format "%c,%c", without leading/trailing spaces.
 * Prints to file 'fp'.
 */
void MovElem_print(MovElem elem, FILE *fp);

/* Changes the given 'chain' in position 'eleIdx'.
 * The element in that position becomes set with movement 'bb' for the
 *   backbone and 'sc' for the side chain.
 */
void MovChain_set_element(MovElem * chain, int eleIdx, unsigned char bb, unsigned char sc);

/* Creates a chain of movements with 'size' bytes.
 * Each byte carries 4 bits (MSB) representing backbone movement, and 4 bits (LSB) for sidechain movement.
 * Everything is initialized to FRONT-right
 */
MovElem * MovChain_create(int size);



#include "int3d.h"

/* Takes a chain of movements and returns the spatial position of BB and SC beads over the 3D space.
 * 'coordsBB_p' and 'coordsSC_p' are pointers to where we should store, respectively, the coordinates for
 *    the backbone beads and the side chain beads.
 */
void MovChain_build_3d(const MovElem * chain, // input
	int chainSize,    // input
	int3d **coordsBB_p, // output
	int3d **coordsSC_p  // output
);


#endif // MOVCHAIN_H
