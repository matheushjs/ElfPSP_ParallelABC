#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hpchain.h"
#include "movchain.h"
#include "fitness/fitness.h"
#include "abc_alg/abc_alg.h"

void print_3d(const MovElem * movchain, const HPElem * hpChain, int hpSize, FILE *fp){
	int3d *coordsBB, *coordsSC;
	MovChain_build_3d(movchain, hpSize-1, &coordsBB, &coordsSC);

	int i;
	for(i = 0; i < hpSize; i++){
		int3d_print(coordsBB[i], fp);
		fprintf(fp, "\n");
		int3d_print(coordsSC[i], fp);
		fprintf(fp,"\n");
	}

	fprintf(fp, "\n%s", hpChain);

	free(coordsBB);
	free(coordsSC);
}

char validateHPChain(HPElem *hpChain){
	int i;
	char bad = 1;

	// Verify existence of at least 1 hydrophobic bead
	for(i = 0; hpChain[i] != '\0'; i++){
		if(hpChain[i] == 'H')
			bad = 0;
	}
	if(bad){
		// No H beads in the string.
		return 1;
	}

	// Verify if all characters are either H or P
	int nH = 0;
	int nP = 0;
	int n  = 0;
	for(i = 0; hpChain[i] != '\0'; i++){
		n++;
		if(hpChain[i] == 'H') nH++;
		if(hpChain[i] == 'P') nP++;
	}
	if(nH + nP == n){
		// Success
		return 0;
	} else {
		// Weird characters in the string.
		return 2;
	}
}

int main(int argc, char *argv[]){
	if(argc == 1){
		fprintf(stderr, "Usage: %s HP_Sequence [num_cycles] [output file]\n", argv[0]);
		return 1;
	}

	HPElem *hpChain = argv[1];
	int     hpSize  = strlen(hpChain);
	int   nCycles  = argc >= 3 ? atoi(argv[2]) : CYCLES;
	char *outFile  = argc >= 4 ? argv[3]       : "output.txt";

	// Validate HP Chain
	if(validateHPChain(hpChain) != 0){
		fprintf(stderr, "Invalid HP Chain given: %s.\n"
		                "Chain must consist only of 'H' and 'P' characters.\n"
		                "Chain must also have at least 1 'H' bead.\n", argv[1]);
		return 1;
	}

	clock_t beg = clock();

	PredResults results;
	MovElem * chain = ABC_predict_structure(hpChain, hpSize, &results);

	double time = (clock() - beg) / (double) CLOCKS_PER_SEC;

	if(results.contactsH >= 0){
		printf("Fitness: %lf\n", results.fitness);
		printf("Hcontacts: %d\n", results.contactsH);
		printf("Collisions: %d\n", results.collisions);
		printf("BBGyration: %lf\n", results.bbGyration);
		printf("Time: %lf\n", time);

		FILE *fp = fopen(outFile, "w+");
		print_3d(chain, hpChain, hpSize, fp);

		fclose(fp);
		free(chain);
	}

	return 0;
}
