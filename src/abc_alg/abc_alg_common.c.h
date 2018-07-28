#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include <movchain.h>
#include <hpchain.h>
#include <fitness/fitness.h>
#include <random.h>
#include <config.h>
#include <utils.h>

#include "abc_alg.h"

#define FITNESS_MIN -1E9


/******************************************/
/****** SOLUTION PROCEDURES        ********/
/******************************************/

typedef struct _Solution {
	MovElem * position;   // Position of such solution
	double fitness;      // Fitness of such solution
	int idle_iterations; // Number of iterations through which the food didn't improve
} Solution;

/* Returns a Solution whose fields are all uninitialized, but with due memory allocated.
 */
static
Solution Solution_blank(int hpSize){
	Solution retval;
	retval.position = malloc(sizeof(MovElem) * (hpSize - 1));
	return retval;
}

/* Returns a deep copy (all memory recursively duplicated) of the given solution.
 */
static
Solution Solution_copy(Solution sol, int hpSize){
	Solution retval;
	retval.fitness = sol.fitness;
	retval.idle_iterations = sol.idle_iterations;

	int chainSize = hpSize - 1;

	retval.position = malloc(sizeof(MovElem) * chainSize);
	memcpy(retval.position, sol.position, sizeof(MovElem) * chainSize);

	return retval;
}

/* Frees memory allocated for given solution
 */
static
void Solution_free(Solution sol){
	free(sol.position);
}

/* Returns a Solution whose movement chain is uniformly random
 * The returned Solution has its idle_positions set to 0.
 * The returned Solution won't have its fitness calculated.
 */
static
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

/* Chooses a random element ELEM1 in 'perturb'.
 * Then chooses a random element ELEM2 in 'other'.
 * Takes the distance DIST between ELEM1 and ELEM2
 * Changes 'perturb' so that its ELEM1 approaches ELEM2 by a random amount, from 0 to 100%.
 * The solution 'perturb' is returned.
 *
 * The returned Solution has its idle_positions set to 0.
 * The returned Solution won't have its fitness calculated.
 */
static
Solution Solution_perturb_relative(Solution perturb, Solution other, int hpSize){
	int chainSize = hpSize - 1;
	int pos1 = urandom_max(chainSize);
	int pos2 = urandom_max(chainSize);

	pos2 = pos1;

	Solution retval = Solution_copy(perturb, hpSize);
	unsigned char elem1 = MovElem_to_number(retval.position[pos1]);
	unsigned char elem2 = MovElem_to_number(other.position[pos2]);

	char distance = elem2 - (char) elem1;

	// Increase distance by 1, because the random number generation is exclusive
	if(distance < 0) distance -= 1;
	else distance += 1;

	char delta = drandom_x() * distance;

	retval.position[pos1] = MovElem_from_number(elem1 + delta);
	retval.idle_iterations = 0;
	retval.fitness = FITNESS_MIN;

	return retval;
}



/******************************************/
/****** HIVE PROCEDURES            ********/
/******************************************/

static struct _HIVE {
	Solution *sols; // Set of solutions currently held by the forager bees
	int nSols;      // Number of such solutions

	int cycle;     // Keeps track of what cycle we are running

	Solution best;  // Best solution found so far
} HIVE;

// Initializes the global HIVE object
static
void HIVE_initialize(){
	HIVE.nSols = COLONY_SIZE * FORAGER_RATIO;
	HIVE.sols = malloc(sizeof(Solution) * HIVE.nSols);

	int i;
	for(i = 0; i < HIVE.nSols; i++){
		HIVE.sols[i].position = NULL; // In case HIVE is destroyed prematurely
		HIVE.sols[i].fitness = FITNESS_MIN;
	}

	HIVE.cycle = 0;

	HIVE.best.position = NULL;
	HIVE.best.fitness = FITNESS_MIN;
}

// Frees memory allocated in HIVE
// Does not free the best solution
static
void HIVE_destroy(){
	int i;
	for(i = 0; i < HIVE.nSols; i++){
		Solution_free(HIVE.sols[i]);
	}
	free(HIVE.sols);
}

// Tells the HIVE to increment one cycle in the cycle counter
static
void HIVE_increment_cycle(){
	HIVE.cycle++;
}

/* Adds solution 'sol' as the index-th solution of the HIVE.
 * No deep copy is made.
 *
 * Also checks the fitness of 'sol' and replaces the best solution in the HIVE if needed
 * This procedure does not free the current solution in that spot.
*/
static
void HIVE_add_solution(Solution sol, int index, int hpSize){
	HIVE.sols[index] = sol;

	// Replace best solution if needed
	if(sol.fitness > HIVE.best.fitness){
		// Change best solution
		Solution_free(HIVE.best);
		HIVE.best = Solution_copy(sol, hpSize);

		debug_print("Replaced best solution. Fitness: %lf\n", HIVE.best.fitness);
	}
}

/* Removes a solution from the HIVE, freeing the memory allocated for it.
 */
static
void HIVE_remove_solution(int index){
	free(HIVE.sols[index].position);
}

/* Increments the idle_iterations of the solution desired.
 */
static
void HIVE_increment_idle(int index){
	HIVE.sols[index].idle_iterations++;
}

/* Causes a minor variation in the solution at given index.
 *
 * The Solution with index 'index' is SOL1, we take a random SOL2 and a random
 *   spot SPOT in the Solutions' movement chain.
 * SOL1's movement at spot SPOT is made to approach the value in SOL2's movement
 *   at the same spot.
 */
static
Solution perturb_solution(int index, int hpSize){
	int other;

	do {
		other = urandom_max(HIVE.nSols);
	} while(other == index);

	return Solution_perturb_relative(HIVE.sols[index], HIVE.sols[other], hpSize);
}

/* The current Solution with index 'index' is SOL1.
 * Checks if 'alt' has a better fitness, and if that is so, replaces SOL1 with 'alt'.
 * Returns 'true' if replacement happened.
 *
 * If the original solution is replaced, 'alt' must not have its memory freed.
 * If it is not replaced, the user MUST call Solution_free() on 'alt' at some point.
 */
static
bool replace_solution(Solution alt, int index, int hpSize){
    if(alt.fitness > HIVE.sols[index].fitness){
        HIVE_remove_solution(index);
        HIVE_add_solution(alt, index, hpSize);
        debug_print("Replaced solution %d, fitness: %lf\n", index, alt.fitness);
        return true;
    } else return false;
}



