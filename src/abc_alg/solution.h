#ifndef SOLUTION_H_
#define SOLUTION_H_

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <movelem.h>
#include <random.h>

#define FITNESS_MIN -1E9

#ifndef SOLUTION_SOURCE_CODE
	#define SOLUTION_INLINE inline
#else
	#define SOLUTION_INLINE extern inline
#endif

/** Encapsulates a solution, which is a protein conformation that is developed by a bee. */
typedef struct Solution_ {
	MovElem *position;    /**< Position of such solution */
	double fitness;       /**< Fitness of such solution */
	int idle_iterations;  /**< Number of iterations through which the food didn't improve */
} Solution;

/** Returns a Solution whose fields are all uninitialized, but with due memory allocated. */
SOLUTION_INLINE
Solution Solution_blank(int hpSize){
	Solution retval;
	retval.position = malloc(sizeof(MovElem) * (hpSize - 1));
	return retval;
}

/** Returns a deep copy (all memory recursively duplicated) of the given solution. */
SOLUTION_INLINE
Solution Solution_copy(Solution sol, int hpSize){
	Solution retval;
	retval.fitness = sol.fitness;
	retval.idle_iterations = sol.idle_iterations;

	int chainSize = hpSize - 1;

	retval.position = malloc(sizeof(MovElem) * chainSize);
	memcpy(retval.position, sol.position, sizeof(MovElem) * chainSize);

	return retval;
}

/** Frees memory allocated for given solution */
SOLUTION_INLINE
void Solution_free(Solution sol){
	free(sol.position);
}

/** Returns a Solution whose movement chain is uniformly random.
 * The returned Solution has its idle_positions set to 0.
 * The returned Solution won't have its fitness calculated.
 */
SOLUTION_INLINE
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

/** Chooses a random element ELEM1 in 'perturb'.
 * Then chooses a random element ELEM2 in 'other'.
 * Takes the distance DIST between ELEM1 and ELEM2
 * Changes 'perturb' so that its ELEM1 approaches ELEM2 by a random amount, from 0 to 100%.
 * The solution 'perturb' is returned.
 *
 * The returned Solution has its idle_positions set to 0.
 * The returned Solution won't have its fitness calculated.
 */
SOLUTION_INLINE
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

#endif
