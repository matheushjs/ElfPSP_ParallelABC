#ifndef RANDOM_H
#define RANDOM_H

#undef MT_GENERATE_CODE_IN_HEADER
#define MT_GENERATE_CODE_IN_HEADER 0
#include "mtwist/mtwist.h"

// Seeds the mersenne twister random number generator
static inline
void random_seed(){
	mt_seed();
}

// Returns a random double within [0,1)
static inline
double drandom_x(){
	return mt_drand();
}

// Returns an unsigned integer within [0,max)
static inline
unsigned int urandom_max(unsigned int max){
	return drandom_x() * max;
}

#endif // RANDOM_H
