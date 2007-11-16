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
	mkdir -p $(APRON_PREFIX)/include
	cp $(H_FILES) $(APRON_PREFIX)/include

distclean:
	(cd $(APRON_PREFIX)/include; /bin/rm -f $(H_FILES))

dist: $(H_FILES) Makefile README
	(cd ..; tar zcvf num.tgz $(^:%=num/%))

clean:
	rm -f testl* testmp* testdbl testldbl

ICFLAGS = -I$(APRON_PREFIX)/include -I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include -L$(APRON_PREFIX)/lib -L$(GMP_PREFIX)/lib -L$(MPFR_PREFIX)/lib

CLIBS = -lapron -lmpfr -lgmp -lm

test: testli testlli testlr testllr testmpz testmpq testdbl testldbl

testli: test.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGINT -o $@ test.c $(CLIBS)

testlli: test.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGLONGINT -o $@ test.c $(CLIBS)

testlr: test.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGRAT -o $@ test.c $(CLIBS)

testllr: test.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGLONGRAT -o $@ test.c $(CLIBS)

testmpz: test.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPZ -o $@ test.c $(CLIBS)

testmpq: test.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPQ -o $@ test.c $(CLIBS)

testdbl: test.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_DOUBLE -o $@ test.c $(CLIBS)

testldbl: test.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGDOUBLE -o $@ test.c $(CLIBS)
