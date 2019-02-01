#ifndef _MOVELEM_H_
#define _MOVELEM_H_

#include <stdio.h>
#include <stdlib.h>
#include "random.h"

/** Type that holds 2 movements, one for the backbone and one for the side chain.
 * The backbone movement is on the most significant 4 bits of this type.
 * The side chain movement is on the least significant 4 bits.
 * The type's size is supposed to be 1 byte.
 *
 * MovChain is how we refer to an array of MovElem.
 */
typedef unsigned char MovElem;

/** Constants for making MovElem elements */
enum _MovUnits {
	FRONT = 0,
	LEFT  = 1,
	RIGHT = 2,
	UP    = 3,
	DOWN  = 4,
};

#ifndef MOVELEM_SOURCE_FILE
	#define MOVELEM_INLINE inline
#else
	#define MOVELEM_INLINE extern inline
#endif

/** Creates a MovElem.
 *
 * \param bb The backbone element (e.g. FRONT, LEFT)
 * \param sc The side-chain element (e.g. FRONT, LEFT)
 */
MOVELEM_INLINE
MovElem MovElem_make(unsigned char bb, unsigned char sc){
	return (bb << 4) | sc;
}

/** Returns a uniformly random MovElem. */
MOVELEM_INLINE
MovElem MovElem_random(){
	return MovElem_make(urandom_max(DOWN+1), urandom_max(DOWN+1));
}

/** Returns the movement for the backbone (BB) stored in a MovElem. */
MOVELEM_INLINE
unsigned char MovElem_getBB(MovElem elem){
	return elem >> 4;
}

/** Returns the movement for the side chain (SC) stored in a MovElem. */
MOVELEM_INLINE
unsigned char MovElem_getSC(MovElem elem){
	return elem & 0x0F;
}

/** Convert a MovElem to a number.
 * Each possible MovElem map into a single number in interval [0, 24].
 */
MOVELEM_INLINE
unsigned char MovElem_to_number(MovElem elem){
	unsigned char bb = MovElem_getBB(elem);
	unsigned char sc = MovElem_getSC(elem);
	return bb * 5 + sc;
}

/** Convert a number to MovElem.
 * Each possible MovElem map into a single number in interval [0, 24].
 */
MOVELEM_INLINE
MovElem MovElem_from_number(unsigned char num){
	return MovElem_make(num / 5, num % 5);
}

/** Prints a movement in the format "%c,%c", without leading/trailing spaces.
 * Prints to file 'fp'.
 */
MOVELEM_INLINE
void MovElem_print(MovElem elem, FILE *fp){
	static const char chars[] = {'F','L','R','U','D'};
	unsigned char bb = elem >> 4;
	unsigned char sc = elem & 0x0F;
	fprintf(fp, "%c,%c", chars[bb], chars[sc]);
}

#endif
