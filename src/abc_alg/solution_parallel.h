#ifndef SOLUTION_PARALLEL_H
#define SOLUTION_PARALLEL_H

#include <mpi/mpi.h>
#include "solution.h"

/** Packs a Solution in the given buffer. */
void Solution_pack(Solution sol, int hpSize, void *buf, int maxSize, int *position, MPI_Comm comm){
	MPI_Pack(&sol.fitness, 1, MPI_DOUBLE, buf, maxSize, position, comm);
	MPI_Pack(sol.chain, hpSize-1, MPI_CHAR, buf, maxSize, position, comm);
}

/** Unpacks a Solution and returns it. */
Solution Solution_unpack(int hpSize, void *buf, int maxSize, int *position, MPI_Comm comm){
	Solution sol = Solution_blank(hpSize);
	MPI_Unpack(buf, maxSize, position, &sol.fitness, 1, MPI_DOUBLE, comm);
	MPI_Unpack(buf, maxSize, position, sol.chain, hpSize-1, MPI_CHAR, comm);
	return sol;
}

#endif
