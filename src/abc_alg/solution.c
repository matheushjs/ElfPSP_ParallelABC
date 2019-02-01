#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <movelem.h>
#include <random.h>

#include "solution.h"

// Documented in header file
Solution Solution_blank(int hpSize){
	Solution retval;
	retval.position = malloc(sizeof(MovElem) * (hpSize - 1));
	return retval;
}

// Documented in header file
Solution Solution_copy(Solution sol, int hpSize){
	Solution retval;
	retval.fitness = sol.fitness;
	retval.idle_iterations = sol.idle_iterations;

	int chainSize = hpSize - 1;

	retval.position = malloc(sizeof(MovElem) * chainSize);
	memcpy(retval.position, sol.position, sizeof(MovElem) * chainSize);

	return retval;
}

// Documented in header file
void Solution_free(Solution sol){
	free(sol.position);
}

// Documented in header file
Solution Solution_random(int hpSize){
	Solution sol;
	int nMovements = hpSize - 1;

	sol.idle_iterations = 0;

	// Generate random MovElem *
	sol.position = malloc(sizeof(MovElem) * nMovements);
	int i;
	for(i = 0; i < nMovements; i++)
		sol.position[i] = MovElem_random();

	sol.fitness = FITNESS_MIN;

	return sol;
}

// Documented in header file
Solution Solution_perturb_relative(Solution perturb, Solution other, int hpSize){
	int chainSize = hpSize - 1;
	int pos1 = urandom_max(chainSize);
	int pos2 = urandom_max(chainSize);

	pos2 = pos1;

	Solution retval = Solution_copy(perturb, hpSize);
	unsigned char elem1 = MovElem_to_number(retval.position[pos1]);
	unsigned char elem2 = MovElem_to_number(other.position[pos2]);

	char distance = elem2 - (char) elem1;

	// Generate a number in [0, distance)
	double aux = drandom_x() * (double) distance;

	// Fit the number in the discrete space [0, distance]
	char delta = (char) round(aux);

	retval.position[pos1] = MovElem_from_number(elem1 + delta);
	retval.idle_iterations = 0;
	retval.fitness = FITNESS_MIN;

	return retval;
}

