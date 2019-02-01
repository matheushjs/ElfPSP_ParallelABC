#ifndef SOLUTION_H_
#define SOLUTION_H_

#define FITNESS_MIN -1E9

/** Encapsulates a solution, which is a protein conformation that is developed by a bee. */
typedef struct Solution_ {
	MovElem *position;    /**< Position of such solution */
	double fitness;       /**< Fitness of such solution */
	int idle_iterations;  /**< Number of iterations through which the food didn't improve */
} Solution;

/** Returns a Solution whose fields are all uninitialized, but with due memory allocated. */
Solution Solution_blank(int hpSize);

/** Returns a deep copy (all memory recursively duplicated) of the given solution. */
Solution Solution_copy(Solution sol, int hpSize);

/** Frees memory allocated for given solution */
void Solution_free(Solution sol);

/** Returns a Solution whose movement chain is uniformly random
 * The returned Solution has its idle_positions set to 0.
 * The returned Solution won't have its fitness calculated.
 */
Solution Solution_random(int hpSize);

/** Chooses a random element ELEM1 in 'perturb'.
 * Then chooses a random element ELEM2 in 'other'.
 * Takes the distance DIST between ELEM1 and ELEM2
 * Changes 'perturb' so that its ELEM1 approaches ELEM2 by a random amount, from 0 to 100%.
 * The solution 'perturb' is returned.
 *
 * The returned Solution has its idle_positions set to 0.
 * The returned Solution won't have its fitness calculated.
 */
Solution Solution_perturb_relative(Solution perturb, Solution other, int hpSize);

#endif
