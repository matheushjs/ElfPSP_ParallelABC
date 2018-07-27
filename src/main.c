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

int main(int argc, char *argv[]){
	int i;

	if(argc == 1){
		fprintf(stderr, "Usage: %s HP_Sequence [output file]\n", argv[0]);
		return 1;
	}

	HPElem *hpChain = argv[1];
	int hpSize = strlen(hpChain);

	// Check hpChain validity
	char good = 0;
	for(i = 0; hpChain[i] != '\0'; i++)
		if(hpChain[i] == 'H') good = 1;
	if(!good) return 1;

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

		char *filename = "output.txt";
		if(argc == 3)
			filename = argv[2];

		FILE *fp = fopen(filename, "w+");
		print_3d(chain, hpChain, hpSize, fp);

		fclose(fp);
		free(chain);
	}

	return 0;
}
