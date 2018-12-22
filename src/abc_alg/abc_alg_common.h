#ifndef _ABC_ALG_COMMON_
#define _ABC_ALG_COMMON_

typedef struct _Solution {
	MovElem * position;   // Position of such solution
	double fitness;      // Fitness of such solution
	int idle_iterations; // Number of iterations through which the food didn't improve
} Solution;

// Returns a Solution whose fields are all uninitialized, but with due memory allocated.
Solution Solution_blank(int hpSize);

// Returns a deep copy (all memory recursively duplicated) of the given solution.
Solution Solution_copy(Solution sol, int hpSize);

// Frees memory allocated for given solution
void Solution_free(Solution sol);

/* Returns a Solution whose movement chain is uniformly random
 * The returned Solution has its idle_positions set to 0.
 * The returned Solution won't have its fitness calculated.
 */
Solution Solution_random(int hpSize);

/* Chooses a random element ELEM1 in 'perturb'.
 * Then chooses a random element ELEM2 in 'other'.
 * Takes the distance DIST between ELEM1 and ELEM2
 * Changes 'perturb' so that its ELEM1 approaches ELEM2 by a random amount, from 0 to 100%.
 * The solution 'perturb' is returned.
 *
 * The returned Solution has its idle_positions set to 0.
 * The returned Solution won't have its fitness calculated.
 */
Solution Solution_perturb_relative(Solution perturb, Solution other, int hpSize);


struct _HIVE {
	Solution *sols; // Set of solutions currently held by the forager bees
	int nSols;      // Number of such solutions

	int cycle;      // Keeps track of what cycle we are running

	Solution best;  // Best solution found so far
};
extern struct _HIVE HIVE;

// Initializes the global HIVE object
void HIVE_initialize();

// Frees memory allocated in HIVE
// Does not free the best solution
void HIVE_destroy();

// Tells the HIVE to increment one cycle in the cycle counter
void HIVE_increment_cycle();

/* Adds solution 'sol' as the index-th solution of the HIVE.
 * No deep copy is made.
 *
 * Also checks the fitness of 'sol' and replaces the best solution in the HIVE if needed
 * This procedure does not free the current solution in that spot.
*/
void HIVE_add_solution(Solution sol, int index, int hpSize);

// Removes a solution from the HIVE, freeing the memory allocated for it.
void HIVE_remove_solution(int index);

// Increments the idle_iterations of the solution desired.
void HIVE_increment_idle(int index);

/* Causes a minor variation in the solution at given index.
 *
 * The Solution with index 'index' is SOL1, we take a random SOL2 and a random
 *   spot SPOT in the Solutions' movement chain.
 * SOL1's movement at spot SPOT is made to approach the value in SOL2's movement
 *   at the same spot.
 */
Solution perturb_solution(int index, int hpSize);

/* The current Solution with index 'index' is SOL1.
 * Checks if 'alt' has a better fitness, and if that is so, replaces SOL1 with 'alt'.
 * Returns 'true' if replacement happened.
 *
 * If the original solution is replaced, 'alt' must not have its memory freed.
 * If it is not replaced, the user MUST call Solution_free() on 'alt' at some point.
 */
bool replace_solution(Solution alt, int index, int hpSize);

#endif
