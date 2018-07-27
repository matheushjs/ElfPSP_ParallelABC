#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include "config.h"

#define error_print(fmt, ...) \
	fprintf(stderr, "ERROR:%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__);

#define SUPPRESS_WARNING(func) (void)(func)

/**********************************/
/*****   DEBUG MACROS     *********/
/**********************************/

#if DEBUG == 1
	#define debug_print(fmt, ...) \
		do { if(1) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)
#else
	#define debug_print(fmt, ...) \
		do { if(0) fprintf(stderr, "%s:%d:%s(): " fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); } while (0)
#endif /* DEBUG */

/**********************************/
/*****   PROFILING MACROS    ******/
/**********************************/

#if PROFILE == 1
	#include <time.h>

	#define profile_clock(TIMEVAR) \
		do { if (1) TIMEVAR = clock(); } while (0)

	#define profile_print(TIMEVAR, NAME) \
		do { if (1) fprintf(stderr, "Time spent on %s: %lf\n", \
							   NAME, (clock() - TIMEVAR) / (double) CLOCKS_PER_SEC ); } while (0)
#else
	#define profile_clock(TIMEVAR) do { } while (0)
	#define profile_print(TIMEVAR, NAME) do { } while (0)
#endif /* PROFILE */

#endif // UTILS_H

