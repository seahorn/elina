
include ../Makefile.config

PREFIX = $(ITV_PREFIX)

SRCDIR = $(shell pwd)

#---------------------------------------
# Programs
#---------------------------------------

# Installation program
INSTALL = install
INSTALLd = install -d
# C compiler and C preprocessor
CC = gcc
CPP = gcc -E

# LATEX and others
LATEX = latex
TEXI2DVI = texi2dvi
MAKEINFO = makeinfo
DVIPS = dvips

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(GMP_PREFIX)/include  \
-I$(MPFR_PREFIX)/include  \
-I../num \
-I../apron 

#---------------------------------------
# Files
#---------------------------------------

CCMODULES = itv itv_linexpr
CCSRC = itv_config.h $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = itv.h itv_config.h
CCBIN_TO_INSTALL = 
CCLIB_TO_INSTALL = \
libitvli.a libitvli_debug.a \
libitvlli.a libitvlli_debug.a \
libitvlr.a libitvlr_debug.a \
libitvllr.a libitvllr_debug.a \
libitvmpz.a libitvmpz_debug.a \
libitvmpq.a libitvmpq_debug.a \
libitvmpq.a libitvmpq_debug.a \
libitvdbl.a libitvdbl_debug.a \
libitvldbl.a libitvldbl_debug.a \
libitv.a libitv_debug.a

#---------------------------------------
# Rules
#---------------------------------------

# Possible goals:
# depend doc install
# and the following one

all: allmpq allllr alldbl libitv.a libitv_debug.a

allmpq: libitvmpq.a libitvmpq_debug.a

allllr: libitvllr.a libitvllr_debug.a

alldbl: libitvdbl.a libitvdbl_debug.a

clean:
	/bin/rm -f *.[ao] 
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*

mostlyclean: clean

install: $(CCINC_TO_INSTALL) $(CCLIB_TO_INSTALL)
	$(INSTALLd) $(PREFIX)/include $(PREFIX)/lib
	$(INSTALL) $(CCINC_TO_INSTALL) $(PREFIX)/include
	for i in $(CCLIB_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done
#	for i in $(CCBIN_TO_INSTALL); do \
#		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/bin; fi; \
#	done

distclean:
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(PREFIX)/include/$$i; done
	for i in $(CCLIB_TO_INSTALL); do /bin/rm -f $(PREFIX)/lib/$$i; done
	for i in $(CCBIN_TO_INSTALL); do /bin/rm -f $(PREFIX)/bin/$$i; done
	/bin/rm -f Makefile.depend

dist: $(CCSRC) Makefile COPYING README
	(cd ..; tar zcvf itv.tgz $(^:%=itv/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

#-----------------------------------
# C part
#-----------------------------------

libitv.a: $(CCMODULES:%=%li.o) $(CCMODULES:%=%lli.o) $(CCMODULES:%=%lr.o) $(CCMODULES:%=%llr.o) $(CCMODULES:%=%mpz.o) $(CCMODULES:%=%mpq.o) $(CCMODULES:%=%dbl.o) $(CCMODULES:%=%ldbl.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

libitv_debug.a: $(CCMODULES:%=%li_debug.o) $(CCMODULES:%=%lli_debug.o) $(CCMODULES:%=%lr_debug.o) $(CCMODULES:%=%llr_debug.o) $(CCMODULES:%=%mpz_debug.o) $(CCMODULES:%=%mpq_debug.o) $(CCMODULES:%=%dbl_debug.o) $(CCMODULES:%=%ldbl_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

libitv%.a: $(subst .c,%.o,$(CCMODULES:%=%.c))
	$(AR) rcs $@ $^
	$(RANLIB) $@
libitv%_debug.a: $(subst .c,%_debug.o,$(CCMODULES:%=%.c))
	$(AR) rcs $@ $^
	$(RANLIB) $@

%li.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGINT -c -o $@ $<
%li_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_LONGINT -c -o $@ $<

%lli.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGLONGINT -c -o $@ $<
%lli_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_LONGLONGINT -c -o $@ $<

%lr.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGRAT -c -o $@ $<
%lr_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_LONGRAT -c -o $@ $<

%llr.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%llr_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<

%mpz.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPZ -c -o $@ $<
%mpz_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPZ -c -o $@ $<

%mpq.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
%mpq_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<


%dbl.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_DOUBLE -c -o $@ $<
%dbl_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_DOUBLE -c -o $@ $<

%ldbl.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGDOUBLE -c -o $@ $<
%ldbl_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_LONGDOUBLE -c -o $@ $<

testmpq: testmpq_debug.o libitvmpq_debug.a
	$(CC) -L. -L$(GMP_PREFIX)/lib -L../apron -o $@ $< -litvmpq_debug -lapron_debug -lgmp -lmpfr -lm

testllr: testllr_debug.o libitvllr_debug.a
	$(CC) -L. -L$(GMP_PREFIX)/lib -L../apron -o $@ $< -litvllr_debug -lapron_debug -lgmp -lmpfr -lm

#-----------------------------------
# DEPENDENCIES
#-----------------------------------

itv%.o: itv.c itv.h
itv_linexpr%.o: itv_linexpr.c itv_linexpr.h itv.h
