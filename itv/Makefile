
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
-I../num \
-I../apron \
-Wall -Wconversion -Winline -Wimplicit-function-declaration \
-O3 -g -DNDEBUG -std=c99

# Use XCFLAGS to specify machine-dependent compilation flags.
XCFLAGS =

# For debugging purpose
CFLAGS_DEBUG = -O0 -g -UNDEBUG
CFLAGS_PROF = -g -pg

# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl
OCAMLFLAGS = -g
OCAMLOPTFLAGS = -inline 20

#---------------------------------------
# Files
#---------------------------------------

CCMODULES = itv itv_linexpr
CCSRC = itv_config.h $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = itv.h itv_config.h
CCBIN_TO_INSTALL = 
CCLIB_TO_INSTALL = \
libitvmpq.a libitvmpq_debug.a \
libitvllr.a libitvllr_debug.a \
libitvdbl.a libitvdbl_debug.a

#---------------------------------------
# Rules
#---------------------------------------

# Possible goals:
# depend doc install
# and the following one

all: allmpq allllr alldbl 

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
	for i in $(CCBIN_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/bin; fi; \
	done

distclean:
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(PREFIX)/include/$$i; done
	for i in $(CCLIB_TO_INSTALL); do /bin/rm -f $(PREFIX)/lib/$$i; done
	for i in $(CCBIN_TO_INSTALL); do /bin/rm -f $(PREFIX)/bin/$$i; done
	/bin/rm -f Makefile.depend

dist: $(CCSRC) Makefile itv.texi COPYING README
	(cd ..; tar zcvf itv.tgz $(^:%=itv/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

#-----------------------------------
# C part
#-----------------------------------

libitvmpq.a: $(CCMODULES:%=%mpq.o)
	ar rcs $@ $^
libitvmpq_debug.a: $(CCMODULES:%=%mpq_debug.o)
	ar rcs $@ $^

%mpq.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) -DNUM_MPQ -c -o $@ $<
%mpq_debug.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) $(CFLAGS_DEBUG) -DNUM_MPQ -c -o $@ $<

libitvllr.a: $(CCMODULES:%=%llr.o)
	ar rcs $@ $^
libitvllr_debug.a: $(CCMODULES:%=%llr_debug.o)
	ar rcs $@ $^

%llr.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%llr_debug.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) $(CFLAGS_DEBUG) -DNUM_LONGLONGRAT -c -o $@ $<

libitvdbl.a: $(CCMODULES:%=%dbl.o)
	ar rcs $@ $^
libitvdbl_debug.a: $(CCMODULES:%=%dbl_debug.o)
	ar rcs $@ $^

%dbl.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) -DNUM_DOUBLE -c -o $@ $<
%dbl_debug.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) $(CFLAGS_DEBUG) -DNUM_DOUBLE -c -o $@ $<

testmpq: testmpq_debug.o libitvmpq_debug.a
	$(CC) -L. -L$(GMP_PREFIX)/lib -L../apron -o $@ $< -litvmpq_debug -lapron_debug -lgmp -lmpfr -lm

testllr: testllr_debug.o libitvllr_debug.a
	$(CC) -L. -L$(GMP_PREFIX)/lib -L../apron -o $@ $< -litvllr_debug -lapron_debug -lgmp -lmpfr -lm

#-----------------------------------
# DEPENDENCIES
#-----------------------------------

itv%.o: itv.c itv.h
itv_linexpr%.o: itv_linexpr.c itv_linexpr.h itv.h
