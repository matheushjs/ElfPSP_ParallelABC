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
#include "abc_alg_common.h"

#define FITNESS_MIN -1E9

// Our global HIVE
struct _HIVE HIVE;

/******************************************/
/****** SOLUTION PROCEDURES        ********/
/******************************************/

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

		debug_print("Replaced best solution. Fitness: %lf\n", HIVE.best.fitness);
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
Solution perturb_solution(int index, int hpSize){
	int other;

	do {
		other = urandom_max(HIVE.nSols);
	} while(other == index);

	return Solution_perturb_relative(HIVE.sols[index], HIVE.sols[other], hpSize);
}

// Documented in header file
bool replace_solution(Solution alt, int index, int hpSize){
    if(alt.fitness > HIVE.sols[index].fitness){
        HIVE_remove_solution(index);
        HIVE_add_solution(alt, index, hpSize);
        debug_print("Replaced solution %d, fitness: %lf\n", index, alt.fitness);
        return true;
    } else return false;
}
