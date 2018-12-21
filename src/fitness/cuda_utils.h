#ifndef _CUDA_UTILS_H
#define _CUDA_UTILS_H

/* Divides 'dividend' by 'divisor', rounding up.
 */
inline
int divisionCeil(int dividend, int divisor){
	return (dividend + divisor - 1) / divisor;
}

// Returns the first power of 2 that is >= 'base'.
inline
int higherEqualPow2(int base){
	int result = 1;
	while(result < base) result <<= 1;
	return result;
}

// Returns the last power of 2 that is < 'base'
inline
int lowerStrictPow2(int base){
	int result = 1;
	while(result < base) result <<= 1; // Get a result such that result >= base
	return result >> 1; // Then divide the result by 2 so that result < base
}

#endif
