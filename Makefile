HPSEQ=PPPPPHHPHPHPHPHPPHHPHHPHPPP
NP=1 # Number of nodes

# User defines
DEFS?= # e.g. -DN_HIVES=2
UFLAGS?= # e.g. -pg -g


CFLAGS=-Wall -Wpedantic -O3 -I src
LIBS=-lm

MPI_LIBS=-pthread -Wl,-rpath -Wl,/usr/lib/openmpi/lib -Wl,--enable-new-dtags -L/usr/lib/openmpi/lib -lmpi
MPI_CFLAGS=-I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent -I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent/include \
		-I/usr/lib/openmpi/include -I/usr/lib/openmpi/include/openmpi

VPATH= src/


all:
	make parallel_lin
	make parallel_quad
	make seq_lin
	make seq_quad

parallel_lin: main.o int3d.o fitness_linear.o hpchain.o movchain.o mtwist.o abc_alg_parallel.o elf_tree_comm.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

parallel_quad: main.o int3d.o fitness_quadratic.o hpchain.o movchain.o mtwist.o abc_alg_parallel.o elf_tree_comm.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

seq_lin: main.o int3d.o fitness_linear.o hpchain.o movchain.o mtwist.o abc_alg_sequential.o elf_tree_comm.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)

seq_quad: main.o int3d.o fitness_quadratic.o hpchain.o movchain.o mtwist.o abc_alg_sequential.o elf_tree_comm.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $^ -o $@ $(LIBS) $(MPI_LIBS)


debug:
	make $(BIN) DEFS="-DDEBUG" UFLAGS="-pg -g"

run:
	# 273d.10 from the old article from the authors
	# ./$(BIN) $(HPSEQ)
	mpirun -n $(NP) ./$(BIN) $(HPSEQ)

run_sequential:
	./$(BIN) $(HPSEQ)

clean:
	find -name "*~" -type f -exec rm -vf '{}' \;
	find -name "*.o" -type f -exec rm -vf '{}' \;
	rm -vf *~ gmon.out

clean_all: clean
	rm -vf parallel_lin parallel_quad seq_lin seq_quad


# Explicit non-MPI object rules (WHEN ADDING NEW, MUST ADD TO $(BIN) TARGET TOO)
main.o:               main.c  Makefile
int3d.o:              int3d.c  int3d.h  Makefile
fitness_quadratic.o:  fitness/fitness_quadratic.c fitness/fitness_run.c.h fitness/fitness_gyration.c.h  \
                        fitness/fitness.h  fitness/fitness_structures.c.h Makefile
fitness_linear.o:     fitness/fitness_linear.c fitness/fitness_run.c.h fitness/fitness_gyration.c.h  \
                        fitness/fitness.h  fitness/fitness_structures.c.h Makefile
hpchain.o:            hpchain.c  hpchain.h  Makefile
movchain.o:           movchain.c  movchain.h  Makefile
mtwist.o:             mtwist/mtwist.c  mtwist/mtwist.h  Makefile
abc_alg_sequential.o: abc_alg/abc_alg_sequential.c abc_alg/abc_alg.h  abc_alg/abc_alg_common.c.h Makefile

# Explicit MPI object rules
abc_alg_parallel.o: abc_alg/abc_alg_parallel.c  abc_alg/abc_alg.h  abc_alg/abc_alg_common.c.h
	gcc -c $(DEFS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS) $(MPI_LIBS)

elf_tree_comm.o: abc_alg/elf_tree_comm.c  abc_alg/elf_tree_comm.h
	gcc -c $(DEFS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS) $(MPI_LIBS)

# Implicit rule for building objects
%.o:
	gcc -c $(DEFS) $(CFLAGS) $(UFLAGS) -o "$@" "$<" $(LIBS)
