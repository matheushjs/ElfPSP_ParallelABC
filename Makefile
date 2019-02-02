# User defines
DEFS?= # e.g. -DN_HIVES=2
UFLAGS?= # e.g. -pg -g

CFLAGS=-Wall -O2 -I src
NVCCFLAGS=-O2 -I src
LIBS=-lm
CUDA_PRELIBS="-L/usr/local/cuda/lib64"
CUDA_LIBS=-lcuda -lcudart

MPI_LIBS=-pthread -Wl,-rpath -Wl,/usr/lib/openmpi/lib -Wl,--enable-new-dtags -L/usr/lib/openmpi/lib -lmpi
MPI_CFLAGS=-I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent -I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent/include \
		-I/usr/lib/openmpi/include -I/usr/lib/openmpi/include/openmpi

# We take as a rule that if any API changes, everything should be rebuilt.
# Same goes for the makefile itself
HARD_DEPS=movchain.h fitness/gyration.h fitness/CUDA_header.h fitness/fitness_private.h fitness/fitness.h \
          mtwist/mtwist.h abc_alg/hive.h abc_alg/abc_alg.h abc_alg/elf_tree_comm.h int3d.h config.h \
          abc_alg/solution.h movelem.h random.h hpchain.h Makefile

# This is a variable used by Makefile itself
VPATH=src/

all:
	make mpi seq

mpi:
	make mpi_lin mpi_quad mpi_threads mpi_lin_threads mpi_cuda

seq:
	make seq_lin seq_quad seq_threads seq_lin_threads seq_cuda

mpi_lin: main.o int3d.o measures_linear.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

mpi_quad: main.o int3d.o measures_quadratic.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

mpi_threads: main.o int3d.o measures_threads.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc -fopenmp $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

mpi_lin_threads: main.o int3d.o measures_linear_threads.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc -fopenmp $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

mpi_cuda: main.o int3d.o measures_cuda.o CUDA_collision_count.o CUDA_contact_count.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_parallel.o elf_tree_comm.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc $(CUDA_PRELIBS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS) $(CUDA_LIBS)

seq_lin: main.o int3d.o measures_linear.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_sequential.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS)

seq_quad: main.o int3d.o measures_quadratic.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_sequential.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS)

seq_threads: main.o int3d.o measures_threads.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_sequential.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc -fopenmp $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS)

seq_lin_threads: main.o int3d.o measures_linear_threads.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_sequential.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc -fopenmp $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS)

seq_cuda: main.o int3d.o measures_cuda.o CUDA_collision_count.o CUDA_contact_count.o hpchain.o movchain.o movelem.o mtwist.o abc_alg_sequential.o config.o hive.o gyration.o fitness.o random.o solution.o
	gcc $(CUDA_PRELIBS) $(CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(CUDA_LIBS)

clean:
	find -name "*~" -type f -exec rm -vf '{}' \;
	find -name "*.o" -type f -exec rm -vf '{}' \;
	rm -vf *~ gmon.out

clean_all: clean
	rm -vf mpi_lin mpi_quad mpi_threads mpi_lin_threads mpi_cuda seq_lin seq_quad seq_threads seq_lin_threads seq_cuda

docs:
	doxygen Doxyfile


# Explicit non-MPI object rules (WHEN ADDING NEW, MUST ADD TO $(BIN) TARGET TOO)
main.o:               main.c $(HARD_DEPS)
int3d.o:              int3d.c $(HARD_DEPS)
measures_quadratic.o: fitness/measures_quadratic.c $(HARD_DEPS)
measures_linear.o:    fitness/measures_linear.c $(HARD_DEPS)
hpchain.o:            hpchain.c $(HARD_DEPS)
movchain.o:           movchain.c $(HARD_DEPS)
movelem.o:            movelem.c $(HARD_DEPS)
mtwist.o:             mtwist/mtwist.c $(HARD_DEPS)
abc_alg_sequential.o: abc_alg/abc_alg_sequential.c $(HARD_DEPS)
config.o:             config.c $(HARD_DEPS)
hive.o:               abc_alg/hive.c $(HARD_DEPS)
gyration.o:           fitness/gyration.c $(HARD_DEPS)
fitness.o:            fitness/fitness.c $(HARD_DEPS)
random.o:             random.c $(HARD_DEPS)
solution.o:           abc_alg/solution.c $(HARD_DEPS)


# Explicit CUDA object rules
CUDA_collision_count.o: fitness/CUDA_collision_count.cu $(HARD_DEPS)
	nvcc $(NVCCFLAGS) -c $(DEFS) -o "$@" "$<" $(LIBS)

CUDA_contact_count.o: fitness/CUDA_contact_count.cu $(HARD_DEPS)
	nvcc $(NVCCFLAGS) -c $(DEFS) -o "$@" "$<" $(LIBS)

measures_cuda.o: fitness/measures_cuda.c $(HARD_DEPS)
	gcc $(CUDA_PRELIBS) $(CFLAGS) -c $(DEFS) -o "$@" "$<" $(LIBS) $(CUDA_LIBS)


# Explicit OpenMP object rules
measures_threads.o: fitness/measures_threads.c $(HARD_DEPS)
	gcc -c $(DEFS) $(CFLAGS) -fopenmp $(UFLAGS) -o "$@" "$<" $(LIBS)

measures_linear_threads.o: fitness/measures_linear_threads.c $(HARD_DEPS)
	gcc -c $(DEFS) $(CFLAGS) -fopenmp $(UFLAGS) -o "$@" "$<" $(LIBS)

# Explicit MPI object rules
abc_alg_parallel.o: abc_alg/abc_alg_parallel.c $(HARD_DEPS)
	gcc -c $(DEFS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS) $(MPI_LIBS)

elf_tree_comm.o: abc_alg/elf_tree_comm.c $(HARD_DEPS)
	gcc -c $(DEFS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS) $(MPI_LIBS)

# Implicit rule for building objects
%.o:
	gcc -c $(DEFS) $(CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS)
