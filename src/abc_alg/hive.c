#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

#include <movchain.h>
#include <hpchain.h>
#include <fitness/fitness.h>
#include <random.h>
#include <config.h>

#include "abc_alg.h"
#include "hive.h"
#include "solution.h"

/** Encapsulates a hive that develops a number of solutions using a number of bees. */
struct _HIVE {
	Solution *sols; /**< Set of solutions currently held by the forager bees */
	int nSols;      /**< Number of such solutions */
	int cycle;      /**< Keeps track of what cycle we are running */
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

	int i;
	for(i = 0; i < HIVE.nSols; i++){
		HIVE.sols[i].position = NULL; // In case HIVE is destroyed prematurely
		HIVE.sols[i].fitness = FITNESS_MIN;
	}

	HIVE.cycle = 0;

	HIVE.best.position = NULL;
	HIVE.best.fitness = FITNESS_MIN;
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

Solution *HIVE_solution(int idx){
	return &HIVE.sols[idx];
}

Solution *HIVE_best_sol(){
	return &HIVE.best;
}

void HIVE_nullify_best(){
	HIVE.best.position = NULL;
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
void HIVE_remove_solution(int index){
	free(HIVE.sols[index].position);
}

// Documented in header file
void HIVE_increment_idle(int index){
	HIVE.sols[index].idle_iterations++;
}

// Documented in header file
Solution HIVE_perturb_solution(int index, int hpSize){
	int other;

	do {
		other = urandom_max(HIVE.nSols);
	} while(other == index);

	return Solution_perturb_relative(HIVE.sols[index], HIVE.sols[other], hpSize);
}

// Documented in header file
bool HIVE_replace_solution(Solution alt, int index, int hpSize){
    if(alt.fitness > HIVE.sols[index].fitness){
        HIVE_remove_solution(index);
        HIVE_add_solution(alt, index, hpSize);
        return true;
    } else return false;
}
