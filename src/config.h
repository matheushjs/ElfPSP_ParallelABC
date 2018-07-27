#ifndef CONFIG_H
#define CONFIG_H

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
/*** VERSION SWITCHING ***/
/*************************/

#ifndef COUNTING_QUADRATIC    // Use quadratic procedures for counting contacts/collisions?
	#define COUNTING_QUADRATIC 0
#endif

#ifndef OPTIMIZATION_PARALLEL // Use parallel procedures for the ABC algorithm?
	#define OPTIMIZATION_PARALLEL 0
#endif

#ifndef PERTURB_SINGLE        // Perturb solutions isolatedly, not relative to another?
	#define PERTURB_SINGLE 0
#endif

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
/***  FITNESS          ***/
/*************************/

/* Disable any of the EPS_* below by setting them to 0 */

#ifndef EPS_HH
        #define EPS_HH 10 // Epsilon for H-H side-chain contacts
#endif
#ifndef EPS_HP
        #define EPS_HP -3 // Epsilon for H-P side-chain contacts
#endif
#ifndef EPS_HB
        #define EPS_HB -3 // Epsilon for H-B side-chain contacts
#endif
#ifndef EPS_PP
        #define EPS_PP 1 // Epsilon for P-P side-chain contacts
#endif
#ifndef EPS_PB
        #define EPS_PB 1 // Epsilon for P-B side-chain contacts
#endif
#ifndef EPS_BB
        #define EPS_BB 1 // Epsilon for B-B contacts
#endif

#ifndef PENALTY_VALUE
        #define PENALTY_VALUE 10 // Penalty value for collisions
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
#ifndef CYCLES
	#define CYCLES 600 // Number of forager/onlooker/scout iterations
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
