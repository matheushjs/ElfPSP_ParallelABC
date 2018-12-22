#include <stdio.h>
#include <stdlib.h>

#include "config.h"

int EPS_HH = 10;
int EPS_HP = -3;
int EPS_HB = -3;
int EPS_PP = 1;
int EPS_PB = 1;
int EPS_BB = 1;
int PENALTY_VALUE = 10;

static const char filename[] = "configuration.yml";

void initialize_configuration(){
	FILE *fp = fopen(filename, "r");
	if(!fp) return;

	int errSum = 0;
	errSum += fscanf(fp, " EPSILON_HYDROPHOBIC_HYDROPHOBIC: %d", &EPS_HH);
	errSum += fscanf(fp, " EPSILON_HYDROPHOBIC_POLAR: %d", &EPS_HP);
	errSum += fscanf(fp, " EPSILON_HYDROPHOBIC_BACKBONE: %d", &EPS_HB);
	errSum += fscanf(fp, " EPSILON_POLAR_POLAR: %d", &EPS_PP);
	errSum += fscanf(fp, " EPSILON_POLAR_BACKBONE: %d", &EPS_PB);
	errSum += fscanf(fp, " EPSILON_BACKBONE_BACKBONE: %d", &EPS_BB);
	errSum += fscanf(fp, " PENALTY_VALUE: %d", &PENALTY_VALUE);

	if(errSum != 7){
		fprintf(stderr, "Something went wrong while reading the configuration file '%s'.\n"
				"Make the file is in the correct format.\n", filename);
		exit(EXIT_FAILURE);
	}

	fclose(fp);
}
