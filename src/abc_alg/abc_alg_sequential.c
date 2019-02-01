#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <movchain.h>
#include <hpchain.h>
#include <fitness/fitness.h>
#include <random.h>

#include "abc_alg.h"
#include "hive.h"


/******************************************/
/****** OTHER PROCEDURES           ********/
/******************************************/

/* Performs the forager phase of the searching cycle
 * Procedure idea:
 *   For each solution, generate a new one in the neighborhood
 *   replace the varied solution if it was improved
 */
static
void forager_phase(int hpSize){
	int i;

	for(i = 0; i < HIVE_nSols(); i++){
		// Change a random element of the solution
		Solution alt = HIVE_perturb_solution(i, hpSize);
        alt.fitness = FitnessCalc_run2(alt.position);
		bool replaced = HIVE_replace_solution(alt, i, hpSize);

		// If 'alt' wasn't a better solution
		if(replaced == false){
			// Free memory resources
			Solution_free(alt);

			// Increment idle iterations on the original solution
			HIVE_increment_idle(i);
		}
	}
}

/* Performs the onlooker phase of the searching cycle
 * Procedure idea;
 *   Calculate the SUM of fitnesses for all solutions
 *   Fitness can be negative, so we add a BASE that is the lowest fitness found
 *   For each solution SOL, (SOL.fitness/SUM) is its probability PROB of being perturbed
 *   (PROB * nOnlookers) is the number of perturbations that should be generated
 */
static
void onlooker_phase(int hpSize){
	int i, j;
	int nOnlookers = COLONY_SIZE - (COLONY_SIZE * FORAGER_RATIO);

	// Find the minimum (If no negative numbers, min should be 0)
	double min = 0;
	for(i = 0; i < HIVE_nSols(); i++){
		if(HIVE_solution(i)->fitness < min)
			min = HIVE_solution(i)->fitness;
	}

	// Sum the 'normalized' fitnesses
	double sum = 0;
	for(i = 0; i < HIVE_nSols(); i++){
		sum += HIVE_solution(i)->fitness - min;
	}

	// For each solution, count the number of onlooker bees that should perturb it
	//   then perturb it.
	for(i = 0; i < HIVE_nSols(); i++){
		double norm = HIVE_solution(i)->fitness - min;
		double prob = norm / sum; // The probability of perturbing such solution

		// Count number of onlookers that should perturb such solution
		int nIter = round(prob * nOnlookers);

		for(j = 0; j < nIter; j++){
			// Change a random element of the solution
			Solution alt = HIVE_perturb_solution(i, hpSize);
            alt.fitness = FitnessCalc_run2(alt.position);
			bool replaced = HIVE_replace_solution(alt, i, hpSize);

			// If 'alt' wasn't a better solution
			if(replaced == false){
				// Free memory resources
				Solution_free(alt);

				// Increment idle iterations on the original solution
				HIVE_increment_idle(i);
			}
		}
	}
}

/* Performs the scout phase of the searching cycle
 * Procedure idea:
 *   Find all the solutions whose idle_iterations exceeded the limit
 *   Replace such solutions by random solutions
 */
static
void scout_phase(int hpSize){
	int i;

	for(i = 0; i < HIVE_nSols(); i++){
		if(HIVE_solution(i)->idle_iterations > IDLE_LIMIT){
			HIVE_remove_solution(i);
            Solution sol = Solution_random(hpSize);
            sol.fitness = FitnessCalc_run2(sol.position);
            HIVE_add_solution(sol, i, hpSize);
		}
	}
}

MovElem *ABC_predict_structure(const HPElem * hpChain, int hpSize, int nCycles, PredResults *results){
	HIVE_initialize();
	FitnessCalc_initialize(hpChain, hpSize);

	int i;

	// Generate initial random solutions
	for(i = 0; i < HIVE_nSols(); i++)
		HIVE_add_solution(Solution_random(hpSize), i, hpSize);

	for(i = 0; i < nCycles; i++){
		forager_phase(hpSize);

		onlooker_phase(hpSize);

		/* For each solution, check its idle_iterations
		 * If it exceeded the limit, replace it with a new random solution
		 */
		scout_phase(hpSize);
	}

	MovElem * retval = HIVE_best_sol()->position;
	HIVE_nullify_best();

	if(results){
		results->fitness = HIVE_best_sol()->fitness;
		FitnessCalc_measures(retval, &results->contactsH, &results->collisions, &results->bbGyration);
	}

	FitnessCalc_cleanup();
	HIVE_destroy();

	return retval;
}

/*
 * DEBUG PROCEDURES
 *

void print_3d_coords(FILE *fp, int3d *bbCo, int3d *scCo, int size){
	int i;
	for(i = 0; i < size; i++){
		fprintf(fp, "%d,%d,%d\n%d,%d,%d\n",
			   bbCo[i].x, bbCo[i].y, bbCo[i].z,
			   scCo[i].x, scCo[i].y, scCo[i].z);
	}
}

void print_to_file(char *filename, Solution sol, HPElem *chain){
	FILE *fp = fopen(filename, "w+");

	int size = strlen(chain);
	int3d *bbCo, *scCo;

	MovChain_build_3d(sol.position, size-1, &bbCo, &scCo);
	print_3d_coords(fp, bbCo, scCo, size);
	fprintf(fp, "\n%s\n", chain);

	free(bbCo);
	free(scCo);
	fclose(fp);
}

void print_chain(Solution sol, int hpSize){
	int i;
	for(i = 0; i < hpSize; i++){
		MovElem_print(sol.position[i], stdout);
		printf(" ");
	}
	printf("\n");
}

int main(int argc, char *argv[]){
//	   I'd like to perform the following tasks:
//
//	   1) Initialize 2 solutions SOL1 and SOL2 and perturbate_relative on
//	      both of them repeatedly. If we always choose the same positions
//	      in SOL1 and SOL2, say position POS, if we always take SOL1's POS-th
//	      element to near SOL2's POS-th element, I expect the solutions to
//	      eventually become equal.
//
//	   2) Initialize a solution SOL1 and N other solutions, perturb_relative on them
//	      and make the final conformation available in multiple files, so
//	      that we can visualize whether the multiple perturbed solutions are
//	      coherent and actually random as desired.
//

	char *hpChain = "HHHPHHH";
	int size = 7;
	char filename[256];

	FitnessCalc_initialize(hpChain, size);
	random_seed();

//	TEST 1
//	Inside the Solution_perturb_relative, you must make pos2 = pos1
//	Solution sol1 = Solution_random(size);
//	Solution sol2 = Solution_random(size);

//	print_chain(sol1, size);
//	print_chain(sol2, size);
//	int i;
//	for(i = 0; i < 10000; i++){
//		sol1 = Solution_perturb_relative(sol1, sol2, size);
//	}
//	print_chain(sol1, size);

//  TEST 2
	Solution sol1 = Solution_random(size);
	print_to_file("test2_baseSol.txt", sol1, hpChain);

	int i;
	for(i = 0; i < 10; i++){
		Solution sol2 = Solution_random(size);
		Solution res = Solution_perturb_relative(sol1, sol2, size);

		sprintf(filename, "test2_sol%d.txt", i);
		print_to_file(filename, res, hpChain);
		printf("Fitness for %d: %lf\n", i, res.fitness);
	}
}
 *
 */
