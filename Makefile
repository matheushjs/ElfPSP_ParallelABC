BIN=Parallel_ABC
HPSEQ=PPPPPHHPHPHPHPHPPHHPHHPHPPP
NP=1 # Number of nodes

# User defines
DEFS?= # e.g. -DN_HIVES=2
CYCLES?=1000
UFLAGS?= # e.g. -pg -g


CFLAGS=-Wall -Wpedantic -O3 -I src
LIBS=-lm

MPI_LIBS=-pthread -Wl,-rpath -Wl,/usr/lib/openmpi/lib -Wl,--enable-new-dtags -L/usr/lib/openmpi/lib -lmpi
MPI_CFLAGS=-I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent -I/usr/lib/openmpi/include/openmpi/opal/mca/event/libevent2021/libevent/include \
		-I/usr/lib/openmpi/include -I/usr/lib/openmpi/include/openmpi 

DIFF_CONTROL=$(subst ${} ${},,$(DEFS)).$(subst ${} ${},,$(UFLAGS))

VPATH= src/


all:
	make $(BIN) DEFS="-DCYCLES=$(CYCLES) -DCOUNTING_QUADRATIC=0 -DABC_PARALLEL=0 $(DEFS)"

quadratic:
	make $(BIN) DEFS="-DCYCLES=$(CYCLES) -DCOUNTING_QUADRATIC=1 -DABC_PARALLEL=0 $(DEFS)"

parallel:
	make $(BIN) DEFS="-DCYCLES=$(CYCLES) -DCOUNTING_QUADRATIC=0 -DABC_PARALLEL=1 $(DEFS)"

perturb_single:
	make parallel DEFS="-DPERTURB_SINGLE=1"

debug:
	make $(BIN) DEFS="-DCYCLES=$(CYCLES) -DDEBUG" UFLAGS="-pg -g"

run:
	# 273d.10 from the old article from the authors
	# ./$(BIN) $(HPSEQ)
	mpirun -n $(NP) ./$(BIN) $(HPSEQ)

run_sequential:
	./$(BIN) $(HPSEQ)

clean:
	find -name "*~" -type f -exec rm -vf '{}' \;
	rm -vf *~ gmon.out

clean_all: clean
	rm -vf $(BIN)
	find -name "*.o" -type f -exec rm -vf '{}' \;
	rm -vf DEFS.*


# Explicit non-MPI object rules (WHEN ADDING NEW, MUST ADD TO $(BIN) TARGET TOO)
main.o: main.c Makefile DEFS.$(DIFF_CONTROL)
int3d.o: int3d.c int3d.h Makefile DEFS.$(DIFF_CONTROL)
fitness.o: fitness/fitness.c fitness/fitness_collisions_linear.c.h  fitness/fitness_collisions_quadratic.c.h  fitness/fitness_contacts_linear.c.h  \
	fitness/fitness_contacts_quadratic.c.h  fitness/fitness_gyration.c.h  fitness/fitness.h  fitness/fitness_structures.c.h Makefile DEFS.$(DIFF_CONTROL)
hpchain.o: hpchain.c hpchain.h Makefile DEFS.$(DIFF_CONTROL)
movchain.o: movchain.c movchain.h Makefile DEFS.$(DIFF_CONTROL)
mtwist.o: mtwist/mtwist.c mtwist/mtwist.h Makefile DEFS.$(DIFF_CONTROL)

# Explicit MPI object rules
abc_alg.o: abc_alg/abc_alg.c  abc_alg/abc_alg.h  abc_alg/abc_alg_parallel.c.h  abc_alg/abc_alg_sequential.c.h 
	gcc -c $(DEFS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) -o "./build/$@" "$<" $(LIBS) $(MPI_LIBS)

elf_tree_comm.o: abc_alg/elf_tree_comm.c  abc_alg/elf_tree_comm.h
	gcc -c $(DEFS) $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) -o "./build/$@" "$<" $(LIBS) $(MPI_LIBS)

# Explicit rule for the binary
$(BIN): main.o int3d.o fitness.o hpchain.o movchain.o mtwist.o abc_alg.o elf_tree_comm.o
	gcc $(CFLAGS) $(MPI_CFLAGS) $(UFLAGS) $(DEFS) $(addprefix  build/, $^) -o $(BIN) $(LIBS) $(MPI_LIBS)

# Implicit rule for building objects
%.o: 
	gcc -c $(DEFS) $(CFLAGS) $(UFLAGS) -o "./build/$@" "$<" $(LIBS)

DEFS.$(DIFF_CONTROL):
	rm -f DEFS.*
	touch "$@"
