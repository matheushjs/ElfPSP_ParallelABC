#ifndef CONFIG_H
#define CONFIG_H

/* Check configuration.yml for documentation of the variables below */
extern char *HP_CHAIN;
extern int EPS_HH;
extern int EPS_HP;
extern int EPS_HB;
extern int EPS_PP;
extern int EPS_PB;
extern int EPS_BB;
extern int PENALTY_VALUE;
extern int N_CYCLES;
extern int COLONY_SIZE;
extern double FORAGER_RATIO;
extern int IDLE_LIMIT;
extern int N_HIVES;
extern int RANDOM_SEED;

// Initializes configuration based on the configuration file.
void initialize_configuration();

/* All the macros below have been written in a way such that each parameter can be
 *   overriden by macros defined on command line (e.g. "-DEBUG=1" option on gcc)
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

#endif // CONFIG_H
