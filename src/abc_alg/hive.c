#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include <movchain.h>
#include <hpchain.h>
#include <fitness/fitness.h>
#include <random.h>
#include <config.h>
#include <string.h>

#include "abc_alg.h"
#include "hive.h"
#include "solution.h"

/** Encapsulates a hive that develops a number of solutions using a number of bees. */
struct _HIVE {
	Solution *sols; /**< Set of solutions currently held by the forager bees */
	int nSols;      /**< Number of such solutions */
	int cycle;      /**< Keeps track of what cycle we are running */
	int hpSize;     /**< Stores size of the HP chain of the protein being predicted. */
	Solution best;  /**< Best solution found so far */
};

/** Our global HIVE */
struct _HIVE HIVE;

/******************************************/
/****** HIVE PROCEDURES            ********/
/******************************************/

// Documented in header file
void HIVE_initialize(){
	HIVE.nSols = COLONY_SIZE * FORAGER_RATIO;
	HIVE.sols = malloc(sizeof(Solution) * HIVE.nSols);
	HIVE.hpSize = strlen(HP_CHAIN);

	int i;
	for(i = 0; i < HIVE.nSols; i++)
		HIVE.sols[i] = Solution_random(HIVE.hpSize);

	HIVE.cycle = 0;
	HIVE.best = Solution_random(HIVE.hpSize);
}

// Documented in header file
void HIVE_destroy(){
	int i;
	for(i = 0; i < HIVE.nSols; i++){
		Solution_free(HIVE.sols[i]);
	}
	free(HIVE.sols);
}

int HIVE_nSols(){
	return HIVE.nSols;
}

int HIVE_cycle(){
	return HIVE.cycle;
}

Solution *HIVE_solutions(){
	return HIVE.sols;
}

Solution HIVE_solution(int idx){
	return HIVE.sols[idx];
}

Solution HIVE_best_sol(){
	return HIVE.best;
}

int HIVE_hp_size(){
	return HIVE.hpSize;
}

// Documented in header file
void HIVE_increment_cycle(){
	HIVE.cycle++;
}

// Documented in header file
void HIVE_add_solution(Solution sol, int index, int hpSize){
	HIVE.sols[index] = sol;

	// Replace best solution if needed
	if(sol.fitness > HIVE.best.fitness){
		// Change best solution
		Solution_free(HIVE.best);
		HIVE.best = Solution_copy(sol, hpSize);
	}
}

// Documented in header file
void HIVE_increment_idle(int index){
	Solution_inc_idle_iterations(&HIVE.sols[index]);
}

// Documented in header file
Solution HIVE_perturb_solution(int index, int hpSize){
	int other;

	do {
		other = urandom_max(HIVE.nSols);
	} while(other == index);

	return Solution_perturb_relative(HIVE.sols[index], HIVE.sols[other], hpSize);
}

bool HIVE_try_replace_solution(Solution alt, int index, int hpSize){
	double altFit = Solution_fitness(alt);
	double curFit = Solution_fitness(HIVE.sols[index]);

    if(altFit > curFit){
		Solution_free(HIVE.sols[index]);
		HIVE.sols[index] = alt;
        return true;
    } else return false;
}

void HIVE_force_replace_solution(Solution alt, int index){
	Solution_free(HIVE.sols[index]);
	HIVE.sols[index] = alt;
}

// Documented in header file
void HIVE_replace_best(Solution newBest){
	Solution_free(HIVE.best);
	HIVE.best = newBest;
}
