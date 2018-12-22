# User defines
DEFS?= # e.g. -DN_HIVES=2
UFLAGS?= # e.g. -pg -g

CFLAGS=-Wall -Wpedantic -O3 -I src
NVCCFLAGS=-O3 -I src
LIBS=-lm
CUDA_PRELIBS="-L/usr/local/cuda-9.0/lib64"
CUDA_LIBS=-lcuda -lcudart

MPI_LIBS=-pthread -Wl,-rpath -Wl,/usr/lib/openmpi/lib -Wl,--enable-new-dtags -L/usr/lib/openmpi/lib -lmpi
MPI_CFLAGS=-I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent -I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent/include \
		-I/usr/lib/openmpi/include -I/usr/lib/openmpi/include/openmpi

# This is a variable used by Makefile itself
VPATH=src/

all:
	make mpi_lin mpi_quad mpi_threads mpi_cuda seq_lin seq_quad seq_threads seq_cuda

mpi_lin: main.o int3d.o fitness_linear.o hpchain.o movchain.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o abc_alg_common.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

mpi_quad: main.o int3d.o fitness_quadratic.o hpchain.o movchain.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o abc_alg_common.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

mpi_threads: main.o int3d.o fitness_threads.o hpchain.o movchain.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o abc_alg_common.o
	gcc -fopenmp $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

mpi_cuda: main.o int3d.o fitness_cuda.o CUDA_collision_count.o CUDA_contact_count.o hpchain.o movchain.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o abc_alg_common.o
	gcc $(CUDA_PRELIBS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS) $(CUDA_LIBS)

seq_lin: main.o int3d.o fitness_linear.o hpchain.o movchain.o mtwist.o abc_alg_sequential.o config.o abc_alg_common.o
	gcc $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS)

seq_quad: main.o int3d.o fitness_quadratic.o hpchain.o movchain.o mtwist.o abc_alg_sequential.o config.o abc_alg_common.o
	gcc $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS)

seq_threads: main.o int3d.o fitness_threads.o hpchain.o movchain.o mtwist.o abc_alg_sequential.o config.o abc_alg_common.o
	gcc -fopenmp $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS)

seq_cuda: main.o int3d.o fitness_cuda.o CUDA_collision_count.o CUDA_contact_count.o hpchain.o movchain.o mtwist.o abc_alg_sequential.o config.o abc_alg_common.o
	gcc $(CUDA_PRELIBS) $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(CUDA_LIBS)

clean:
	find -name "*~" -type f -exec rm -vf '{}' \;
	find -name "*.o" -type f -exec rm -vf '{}' \;
	rm -vf *~ gmon.out

clean_all: clean
	rm -vf mpi_lin mpi_quad mpi_threads mpi_cuda seq_lin seq_quad seq_threads seq_cuda


# Explicit non-MPI object rules (WHEN ADDING NEW, MUST ADD TO $(BIN) TARGET TOO)
main.o:               main.c  Makefile
int3d.o:              int3d.c  int3d.h  Makefile
fitness_quadratic.o:  fitness/fitness_quadratic.c fitness/fitness_run.c.h fitness/fitness_gyration.c.h  \
                        fitness/fitness.h  fitness/fitness_private.h Makefile
fitness_linear.o:     fitness/fitness_linear.c fitness/fitness_run.c.h fitness/fitness_gyration.c.h  \
                        fitness/fitness.h  fitness/fitness_private.h Makefile
hpchain.o:            hpchain.c  hpchain.h  Makefile
movchain.o:           movchain.c  movchain.h  Makefile
mtwist.o:             mtwist/mtwist.c  mtwist/mtwist.h  Makefile
abc_alg_sequential.o: abc_alg/abc_alg_sequential.c abc_alg/abc_alg.h Makefile
config.o:             config.c config.h Makefile
abc_alg_common.o:     abc_alg/abc_alg_common.c abc_alg/abc_alg.h Makefile


# Explicit CUDA object rules
CUDA_collision_count.o: fitness/CUDA_collision_count.cu fitness/CUDA_collision_count.h \
                          fitness/cuda_utils.h Makefile
	nvcc $(NVCCFLAGS) -c $(DEFS) -o "$@" "$<" $(LIBS)

CUDA_contact_count.o: fitness/CUDA_contact_count.cu fitness/CUDA_contact_count.h \
                        fitness/cuda_utils.h Makefile
	nvcc $(NVCCFLAGS) -c $(DEFS) -o "$@" "$<" $(LIBS)

fitness_cuda.o: fitness/fitness_cuda.c fitness/fitness_run.c.h fitness/fitness_gyration.c.h  \
                  fitness/fitness.h  fitness/fitness_private.h fitness/CUDA_collision_count.h \
                  fitness/CUDA_contact_count.h Makefile
	gcc $(CUDA_PRELIBS) $(CFLAGS) -c $(DEFS) -o "$@" "$<" $(LIBS) $(CUDA_LIBS)


# Explicit OpenMP object rules
fitness_threads.o: fitness/fitness_threads.c fitness/fitness_run.c.h fitness/fitness_gyration.c.h  \
                   fitness/fitness.h fitness/fitness_private.h Makefile
	gcc -c $(DEFS) $(CFLAGS) -fopenmp $(UFLAGS) -o "$@" "$<" $(LIBS)

# Explicit MPI object rules
abc_alg_parallel.o: abc_alg/abc_alg_parallel.c  abc_alg/abc_alg.h  abc_alg/abc_alg_common.h
	gcc -c $(DEFS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS) $(MPI_LIBS)

elf_tree_comm.o: abc_alg/elf_tree_comm.c  abc_alg/elf_tree_comm.h
	gcc -c $(DEFS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS) $(MPI_LIBS)

# Implicit rule for building objects
%.o:
	gcc -c $(DEFS) $(CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS)
