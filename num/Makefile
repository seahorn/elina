include ../Makefile.config

H_FILES = \
num_config.h \
num.h \
num_numint.h \
num_numrat.h \
num_numflt.h \
numint.h \
numint_longint.h \
numint_longlongint.h \
numint_mpz.h \
numrat.h \
numrat_native.h \
numrat_mpq.h \
numflt.h \
numflt_double.h \
numflt_longdouble.h \
bound.h \
bound_def.h

all:

install:
	mkdir -p $(NUM_PREFIX)/include
	cp $(H_FILES) $(NUM_PREFIX)/include

clean:

distclean:
	(cd $(NUM_PREFIX)/include; /bin/rm -f $(H_FILES))

dist: $(H_FILES) Makefile README
	(cd ..; tar zcvf num.tgz $(^:%=num/%))

