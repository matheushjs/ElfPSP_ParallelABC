#ifndef CONFIG_H
#define CONFIG_H

extern int EPS_HH; // Epsilon for H-H side-chain contacts
extern int EPS_HP; // Epsilon for H-P side-chain contacts
extern int EPS_HB; // Epsilon for H-B side-chain contacts
extern int EPS_PP; // Epsilon for P-P side-chain contacts
extern int EPS_PB; // Epsilon for P-B side-chain contacts
extern int EPS_BB; // Epsilon for B-B side-chain contacts
extern int PENALTY_VALUE; // Penalty value for collisions

// Initializes configuration based on the configuration file.
void initialize_configuration();

/* config.h
 *
 * Header file for configuring the parameters of the program.
 * All parameters are given as macros.
 *
 * All the macros have been written in a way such that each parameter can be
 *   overriden by macros defined on command line (e.g. "-DEPS_HH=1" option on gcc)
 *
 */


/*************************/
/***  DEBUGGING        ***/
/*************************/

#ifndef DEBUG
	#define DEBUG 0   // 0 disables debug and allow the compiler to optimize them out.
#endif
#ifndef PROFILE
	#define PROFILE 0 // 0 disables some profiling messages
#endif


/*************************/
/***  ABC ALGORITHM    ***/
/*************************/

#ifndef COLONY_SIZE
	#define COLONY_SIZE 250 // Number of bees in the colony
#endif
#ifndef FORAGER_RATIO
	#define FORAGER_RATIO 0.5 // Proportion of foragers in the colony
#endif
#ifndef IDLE_LIMIT
	#define IDLE_LIMIT 100 // Maximum number of iterations through which the solution did not improve
#endif


/*************************/
/***  COMM. TOPOLOGY   ***/
/*************************/

#ifndef N_HIVES
	#define N_HIVES 1 // Number of hives in the system (each hive is a master-slave system)
#endif


#endif // CONFIG_H
