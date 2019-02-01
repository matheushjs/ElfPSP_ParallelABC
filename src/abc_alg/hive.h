#ifndef _HIVE_H_
#define _HIVE_H_

#include "solution.h"

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
Solution HIVE_perturb_solution(int index, int hpSize);

/* The current Solution with index 'index' is SOL1.
 * Checks if 'alt' has a better fitness, and if that is so, replaces SOL1 with 'alt'.
 * Returns 'true' if replacement happened.
 *
 * If the original solution is replaced, 'alt' must not have its memory freed.
 * If it is not replaced, the user MUST call Solution_free() on 'alt' at some point.
 */
bool HIVE_replace_solution(Solution alt, int index, int hpSize);

#endif
