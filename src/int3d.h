#ifndef INT3D_H
#define INT3D_H

/** \file int3d.h Routines for mathematical manipulation of vectors and points in the integer space. */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifndef INT3D_SOURCE_FILE
	#define INT3D_INLINE inline
#else // If it's source file, declare extern inline
	#define INT3D_INLINE extern inline
#endif

/** Type for representing a tridimensional coordinate. */
typedef struct int3d_ {
	int x; /**< Coordinate x. */
	int y; /**< Coordinate y. */
	int z; /**< Coordinate z. */
} int3d;

/** Creates an int3d. */
INT3D_INLINE
int3d int3d_make(int x, int y, int z){
	int3d num;
	num.x = x;
	num.y = y;
	num.z = z;
	return num;
}

/** Adds two int3d objects. */
INT3D_INLINE
int3d int3d_add(int3d a, int3d b){
	int3d res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

/** Verifies if `a` and `b` are within a distance of exactly 1 from each other. */
INT3D_INLINE
bool int3d_isDist1(int3d a, int3d b){
	int dx = abs(a.x - b.x);
	int dy = abs(a.y - b.y);
	int dz = abs(a.z - b.z);
	int sum = dx + dy + dz;
	return sum == 1 ? true : false;
}

/** Verifies if `a` and `b` are equal. */
INT3D_INLINE
bool int3d_equal(int3d a, int3d b){
	if(a.x != b.x || a.y != b.y || a.z != b.z)
		return false;
	return true;
}

/** Prints the given int3d object, without placing a newline. */
INT3D_INLINE
void int3d_print(int3d a, FILE *fp){
	fprintf(fp, "%d,%d,%d", a.x, a.y, a.z);
}

#endif // INT3D_H

