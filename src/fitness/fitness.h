#ifndef FITNESS_H
#define FITNESS_H

#include <int3d.h>
#include <hpchain.h>
#include <movchain.h>
#include <config.h>

/* Initialize the resources needed for calling the functions in this library.
 *   of this library can be called in parallel among threads.
 */
void FitnessCalc_initialize(const HPElem * hpChain, int hpSize);

/* Cleans up resources allocated.
 */
void FitnessCalc_cleanup();


/* Returns the fitness for a protein already registered with FitnessCalc_initialize,
 *   considering that the protein has its 3d coordinates in coordsBB and coordsSC.
 */
double FitnessCalc_run(const int3d *coordsBB, const int3d *coordsSC);

/* Returns the fitness for a protein already registered with FitnessCalc_initialize,
 *   considering that the protein has movement chain 'chain'.
 */
double FitnessCalc_run2(const MovElem * chain);

/* Returns measures for a given movement chain.
 * chain    - the movement chain from which to extract measures
 *
 * For all the other arguments, if they are NULL they are ignored, if they aren't null, the memory pointed to receives the desired value.
 * Hcontacts_p  - the number of hidrophobic contacts
 * collisions_p - the number of collisions among beads
 * bbGyration_p - the gyration radius for the backbone beads
 */
void FitnessCalc_measures(const MovElem *chain, int *Hcontacts_p, int *collisions_p, double *bbGyration_p);

#endif // FITNESS_H

