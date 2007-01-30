include ../Makefile.config

H_FILES = \
num_config.h \
num.h \
num_numint.h \
num_numrat.h \
num_numflt.h \
numint.h \
numint_native.h \
numint_mpz.h \
numrat.h \
numrat_native.h \
numrat_mpq.h \
numflt.h \
numflt_native.h \
bound.h \
bound_def.h

all:

install:
	mkdir -p $(NUM_PREFIX)/include
	cp $(H_FILES) $(NUM_PREFIX)/include

distclean:
	(cd $(NUM_PREFIX)/include; /bin/rm -f $(H_FILES))

dist: $(H_FILES) Makefile README
	(cd ..; tar zcvf num.tgz $(^:%=num/%))

clean:
	rm -f testl* testmp* testdbl testldbl

FLAGS = -Wswitch-default -Werror-implicit-function-declaration -Wall -std=c99 -g -I$(APRON_PREFIX)/include -I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include -L$(APRON_PREFIX)/lib -L$(GMP_PREFIX)/lib -L$(MPFR_PREFIX)/lib
LIBS = -lapron -lmpfr -lgmp -lm

test: testli testlli testlr testllr testmpz testmpq testdbl testldbl

testli: test.c
	gcc $(FLAGS) -DNUM_LONGINT -o $@ test.c $(LIBS)

testlli: test.c
	gcc $(FLAGS) -DNUM_LONGLONGINT -o $@ test.c $(LIBS)

testlr: test.c
	gcc $(FLAGS) -DNUM_LONGRAT -o $@ test.c $(LIBS)

testllr: test.c
	gcc $(FLAGS) -DNUM_LONGLONGRAT -o $@ test.c $(LIBS)

testmpz: test.c
	gcc $(FLAGS) -DNUM_MPZ -o $@ test.c $(LIBS)

testmpq: test.c
	gcc $(FLAGS) -DNUM_MPQ -o $@ test.c $(LIBS)

testdbl: test.c
	gcc $(FLAGS) -DNUM_DOUBLE -o $@ test.c $(LIBS)

testldbl: test.c
	gcc $(FLAGS) -DNUM_LONGDOUBLE -o $@ test.c $(LIBS)
