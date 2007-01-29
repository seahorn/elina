# Makefile
#
# APRON Library / Octagonal Domain
#
# Copyright (C) Antoine Mine' 2006

# This file is part of the APRON Library, released under LGPL license.
# Please read the COPYING file packaged in the distribution.

include ../Makefile.config

PREFIX = $(OCT_PREFIX)

# C include and lib directories
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
BINDIR = $(PREFIX)/bin

SRCDIR = $(shell pwd)

#---------------------------------------
# Programs
#---------------------------------------

# Installation program
INSTALL = install
INSTALLd = install -d

# C compiler and C preprocessor
CC = gcc

# Library creation
AR = ar
SHARED = gcc -shared

OCAMLC = $(CAML_PREFIX)/bin/ocamlc.opt
OCAMLOPT = $(CAML_PREFIX)/bin/ocamlopt.opt
OCAMLMKTOP = $(CAML_PREFIX)/bin/ocamlmktop
OCAMLMKLIB = $(CAML_PREFIX)/bin/ocamlmklib
CAMLIDL = $(CAMLIDL_PREFIX)/bin/camlidl

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I../newpolka \
-I../mlgmpidl \
-I$(MPFR_PREFIX)/include \
-I$(GMP_PREFIX)/include \
-I../apron \
-I../mlapronidl \
-I../num \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml \
-Wall -W -Wundef -Wbad-function-cast -Wcast-align \
-Wstrict-prototypes -Wdisabled-optimization \
-Wno-unused \
-std=c99
# -Winline

CFLAGS = $(OPTFLAGS) $(ICFLAGS) -DNDEBUG

# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl
OCAMLFLAGS = -g
OCAMLOPTFLAGS = -inline 20

#---------------------------------------
# Files
#---------------------------------------

CCSOURCES = oct_hmat.c oct_print.c oct_transfer.c oct_closure.c oct_nary.c \
	    oct_representation.c oct_predicate.c oct_resize.c setround.c

CCINC = oct_internal.h oct_fun.h setround.h

# trigers a whole recompilation
#DEPS = $(APRON_PREFIX)/include/ap_abstract0.h

#---------------------------------------
# Rules
#---------------------------------------

root:
	@echo
	@echo "Please choose a target from:"
	@echo
	@echo " single C library  : Zi Zl Zg Qi Ql Qg Fd Fl"
	@echo " C & OCaml library : allZi allZl allZg allQi allQl allQg allFd allFl "
	@echo " all               : to compile everything "
	@echo " install           : to install what has been compiled"
	@echo " uninstall         : to uninstall everything"
	@echo

all: allZi allZl allZg allQi allQl allQg allFd allFl

ml: mlZi mlZl mlZg mlQi mlQl mlQg mlFd mlFl


allZi: Zi octtestZi
allZl: Zl octtestZl
allZg: Zg octtestZg
allQi: Qi octtestQi
allQl: Ql octtestQl
allQg: Qg octtestQg
allFd: Fd octtestFd
allFl: Fl octtestFl

Zi: liboctZi.a
Zl: liboctZl.a
Zg: liboctZg.a
Qi: liboctQi.a
Ql: liboctQl.a
Qg: liboctQg.a
Fd: liboctFd.a
Fl: liboctFl.a

clean:
	/bin/rm -f *.[ao] *.so octtest*
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*
	/bin/rm -fr *.cm[ioax] *.cmxa
	/bin/rm -fr oct_caml.c oct.ml oct.mli octtop* octrun* manager.idl tmp
	/bin/rm -fr *~ \#*\#

install:
	$(INSTALLd) $(INCDIR) $(INCDIR)/oct $(LIBDIR) $(BINDIR)
	$(INSTALL) oct.h $(INCDIR)
	$(INSTALL) $(CCINC) $(INCDIR)/oct
	for i in liboct??.* liboct??_caml.* oct.idl; do \
		if test -f $$i; then $(INSTALL) $$i $(LIBDIR); fi; \
	done
	for i in octtest?? octtop?? octrun??; do \
		if test -f $$i; then $(INSTALL) $$i $(BINDIR); fi; \
	done
	for i in oct.mli oct.cmi oct??.cma oct??.cmxa oct??.a; do \
		if test -f $$i; then $(INSTALL) $$i $(LIBDIR); fi; \
	done

uninstall:
	/bin/rm -fr $(INCDIR)/oct
	/bin/rm -f $(BINDIR)/octtest?? $(BINDIR)/octtop?? $(BINDIR)/octrun??
	/bin/rm -f $(LIBDIR)/liboct??.* $(LIBDIR)/liboct??_caml.*
	/bin/rm -f $(LIBDIR)/oct.mli $(LIBDIR)/oct.cmi $(LIBDIR)/oct.idl $(LIBDIR)/oct??.cma $(LIBDIR)/oct??.cmxa $(LIBDIR)/oct??.a
	/bin/rm -f Makefile.depend

distclean: uninstall
	/bin/rm -f Makefile.depend

dist: Makefile COPYING README oct_doc.html sedscript_caml $(CCSOURCES) $(CCINC) oct.h oct_test.c oct.idl
	(cd ..; tar zcvf octagons.tgz $(^:%=octagons/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o .so

#-----------------------------------
# C part
#-----------------------------------

liboct%.a: $(subst .c,%.o,$(CCSOURCES))
	$(AR) rcs $@ $^

liboct%.so: $(subst .c,%.o,$(CCSOURCES))
	$(SHARED) -o $@ $^

octtest%: liboct%.a oct_test%.o
	$(CC) -o $@ oct_test$*.o \
	-L. -loct$* \
	-L../newpolka -lpolkag_debug \
	-L../apron -lapron \
	-L$(MPFR_PREFIX)/lib -lmpfr \
	-L$(GMP_PREFIX)/lib -lgmp \
	-lm

%_caml.o: %_caml.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_LONGINT -c -o $@ $<
%Zi.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_LONGINT -c -o $@ $<
%Zl.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_LONGLONGINT -c -o $@ $<
%Zg.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_MPZ -I$(GMP_PREFIX)/include -c -o $@ $<

%Qi.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_LONGRAT -c -o $@ $<
%Ql.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%Qg.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_MPQ -I$(GMP_PREFIX)/include -c -o $@ $<

%Fd.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_DOUBLE -c -o $@ $<
%Fl.o: %.c $(CCINC) $(DEPS)
	$(CC) $(CFLAGS) -DNUM_LONGDOUBLE -c -o $@ $<

# TODO: mpfr ?

.PRECIOUS: liboct%.a liboct%.so %Zi.o %Zl.o %Zg.o %Qi.o %Ql.o %Qg.o %Fd.o %Fl.o


#-----------------------------------
# Caml part
#-----------------------------------

.INTERMEDIATE: mlZi

mlZi: oct.mli oct.cmi oct.cma oct.cmxa octtopZi octrunZi
mlZl: oct.mli oct.cmi oct.cma oct.cmxa octtopZl octrunZl
mlZg: oct.mli oct.cmi oct.cma oct.cmxa octtopZg octrunZg
mlQi: oct.mli oct.cmi oct.cma oct.cmxa octtopQi octrunQi
mlQl: oct.mli oct.cmi oct.cma oct.cmxa octtopQl octrunQl
mlQg: oct.mli oct.cmi oct.cma oct.cmxa octtopQg octrunQg
mlFd: oct.mli oct.cmi oct.cma oct.cmxa octtopFd octrunFd
mlFl: oct.mli oct.cmi oct.cma oct.cmxa octtopFl octrunFl

octtop%: oct.cma liboct_caml.a liboct%.a
	$(OCAMLC) -verbose $(OCAMLINC) $(OCAMLFLAGS) -o $@ -custom bigarray.cma gmp.cma apron.cma oct.cma -ccopt -L. -cclib -loct$* -ccopt -L../mlgmpidl -ccopt -L../mlapronidl -ccopt -L../apron

octrun%: oct.cma liboct_caml.a
	$(OCAMLC) -verbose $(OCAMLINC) $(OCAMLFLAGS) -o $@ -make-runtime bigarray.cma gmp.cma apron.cma oct.cma -ccopt -L. -cclib -loct$* -ccopt -L../mlgmpidl -ccopt -L../mlapronidl -ccopt -L../apron

oct.cma: oct.cmo liboct_caml.a
	$(OCAMLC) -verbose $(OCAMLINC) -a -o $@ oct.cmo -ccopt -L. -cclib -loct_caml

oct.cmxa oct.a: oct.cmx liboct_caml.a
	$(OCAMLOPT) -verbose $(OCAMLINC) -a -o $@ oct.cmx -ccopt -L. -cclib -loct_caml

liboct_caml.a: oct_caml.o
	$(AR) rcs $@ $^

liboct_caml.so: oct_caml.o
	$(SHARED) -o $@ $^

%_caml.c %.ml %.mli: %.idl sedscript_caml $(DEPS)
	mkdir -p tmp
	cp ../mlapronidl/*.idl tmp/
	cp $*.idl tmp/
	cd tmp && $(CAMLIDL) -no-include -nocpp -I $(MLAPRONIDL_PREFIX)/lib $*.idl
	cp tmp/$*_stubs.c $*_caml.c
	sed -f sedscript_caml tmp/$*.ml | grep --extended-regexp '^(.)+$$' >$*.ml
	sed -f sedscript_caml tmp/$*.mli | grep --extended-regexp '^(.)+$$' >$*.mli

manager.idl: ../mlapronidl/manager.idl
	cp $^ $@

.PRECIOUS: %_caml.c %.ml %.mli liboct%_caml.a liboct%_caml.so oct.cmx oct.cmo

#---------------------------------------
# ML generic rules
#---------------------------------------

%.cmi: %.mli  $(DEPS)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi  $(DEPS)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi  $(DEPS)
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<



#-----------------------------------
# DEPENDENCIES
#-----------------------------------
