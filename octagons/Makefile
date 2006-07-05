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

OCAMLC = ocamlc.opt 
OCAMLOPT = ocamlopt.opt
OCAMLMKTOP = ocamlmktop
OCAMLMKLIB = ocamlmklib
CAMLIDL = camlidl

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(POLKA_PREFIX)/include \
-I$(MLGMPIDL_PREFIX)/include \
-I$(GMP_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-I$(MLAPRONIDL_PREFIX)/include \
-I$(NUM_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml \
-Wall -W -Wundef -Wbad-function-cast -Wcast-align \
-Wstrict-prototypes -Wdisabled-optimization \
-Wno-unused \
-std=c99
# -Winline

CFLAGS =   -g $(ICFLAGS)

# Caml
OCAMLINC = -I $(MLGMPIDL_PREFIX)/lib -I $(MLAPRONIDL_PREFIX)/lib
OCAMLFLAGS = -g
OCAMLOPTFLAGS = -inline 20

#---------------------------------------
# Files
#---------------------------------------

CCSOURCES = oct_hmat.c oct_print.c oct_transfer.c oct_closure.c oct_nary.c \
            oct_representation.c oct_predicate.c oct_resize.c setround.c

CCINC = oct.h oct_internal.h setround.h

# trigers a whole recompilation
DEPS = $(APRON_PREFIX)/include/ap_abstract0.h

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

allZi: Zi octtestZi mlZi
allZl: Zl octtestZl mlZl
allZg: Zg octtestZg mlZg
allQi: Qi octtestQi mlQi
allQl: Ql octtestQl mlQl
allQg: Qg octtestQg mlQg
allFd: Fd octtestFd mlFd
allFl: Fl octtestFl mlFl

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
	$(INSTALLd) $(INCDIR)/oct $(LIBDIR)
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

uninstall: clean
	/bin/rm -fr $(INCDIR)/oct
	/bin/rm -f $(BINDIR)/octtest?? $(BINDIR)/octtop?? $(BINDIR)/octrun??
	/bin/rm -f $(LIBDIR)/liboct??.* $(LIBDIR)/liboct??_caml.*
	/bin/rm -f $(LIBDIR)/oct.mli $(LIBDIR)/oct.cmi $(LIBDIR)/oct.idl $(LIBDIR)/oct??.cma $(LIBDIR)/oct??.cmxa $(LIBDIR)/oct??.a
	/bin/rm -f Makefile.depend

distclean: clean uninstall
	/bin/rm -f Makefile.depend

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
	-L$(GMP_PREFIX)/lib -lgmp \
	-L$(POLKA_PREFIX)/lib \
	-L$(APRON_PREFIX)/lib -lpolkag_debug -lapron \
	-L. -loct$* -lm

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

mlZi: oct.mli oct.cmi octZi.cma octZi.cmxa octtopZi octrunZi 
mlZl: oct.mli oct.cmi octZl.cma octZl.cmxa octtopZl octrunZl 
mlZg: oct.mli oct.cmi octZg.cma octZg.cmxa octtopZg octrunZg 
mlQi: oct.mli oct.cmi octQi.cma octQi.cmxa octtopQi octrunQi 
mlQl: oct.mli oct.cmi octQl.cma octQl.cmxa octtopQl octrunQl 
mlQg: oct.mli oct.cmi octQg.cma octQg.cmxa octtopQg octrunQg 
mlFd: oct.mli oct.cmi octFd.cma octFd.cmxa octtopFd octrunFd 
mlFl: oct.mli oct.cmi octFl.cma octFl.cmxa octtopFl octrunFl 

octtop%: oct.cmo liboct%_caml.a liboct%.a
	$(OCAMLMKTOP) -verbose $(OCAMLINC) $(OCAMLFLAGS) -o $@ -custom bigarray.cma gmp.cma apron.cmo oct.cmo \
	-ccopt -L$(MLGMPIDL_PREFIX)/lib -cclib -lgmp_caml \
	-ccopt -L$(GMP_PREFIX)/lib -cclib -lgmp \
	-ccopt -L$(MLCOMMONINTERFACIDL_PREFIX)/lib -cclib -lapron_caml \
	-ccopt -L$(APRON_PREFIX)/lib -cclib -lapron \
	-ccopt -L. -cclib -loct$*_caml -cclib -loct$* \
	-ccopt -L$(CAMLIDL_PREFIX)/lib/ocaml -cclib -lcamlidl

octrun%: oct.cmo liboct%_caml.a liboct%.a
	$(OCAMLC) -verbose $(OCAMLINC) $(OCAMLFLAGS) -o $@ -make-runtime bigarray.cma gmp.cma apron.cmo oct.cmo \
	-ccopt -L$(MLGMPIDL_PREFIX)/lib -cclib -lgmp_caml \
	-ccopt -L$(GMP_PREFIX)/lib -cclib -lgmp \
	-ccopt -L$(MLCOMMONINTERFACIDL_PREFIX)/lib -cclib -lapron_caml \
	-ccopt -L$(APRON_PREFIX)/lib -cclib -lapron \
	-ccopt -L. -cclib -loct$*_caml -cclib -loct$* \
	-ccopt -L$(CAMLIDL_PREFIX)/lib/ocaml -cclib -lcamlidl

oct%.cma: oct.cmo liboct%_caml.a liboct%.a
	$(OCAMLMKLIB) -o oct$* -custom -linkall -verbose $(OCAMLINC) \
	oct.cmo \
        -L$(MLGMPIDL_PREFIX)/lib -lgmp_caml \
	-L$(GMP_PREFIX)/lib -lgmp \
	-L$(MLCOMMONINTERFACIDL_PREFIX)/lib -lapron_caml \
	-L$(APRON_PREFIX)/lib -lapron \
	-L. -loct$*_caml -loct$* \
	-L$(CAMLIDL_PREFIX)/lib/ocaml -lcamlidl

oct%.cmxa oct%.a: oct.cmx liboct%_caml.a liboct%.a
	$(OCAMLMKLIB) -o oct$* -linkall -verbose $(OCAMLINC) \
	oct.cmx \
        -L$(MLGMPIDL_PREFIX)/lib -lgmp_caml \
	-L$(GMP_PREFIX)/lib -lgmp \
	-L$(MLCOMMONINTERFACIDL_PREFIX)/lib -lapron_caml \
	-L$(APRON_PREFIX)/lib -lapron \
	-L. -loct$*_caml -loct$* \
	-L$(CAMLIDL_PREFIX)/lib/ocaml -lcamlidl

liboct%_caml.a: oct_caml%.o
	$(AR) rcs $@ $^

liboct%_caml.so: oct_caml%.o
	$(SHARED) -o $@ $^

%_caml.c %.ml %.mli: %.idl sedscript_caml $(DEPS)
	mkdir -p tmp
	cp $(MLAPRONIDL_PREFIX)/lib/*.idl tmp/
	cp $*.idl tmp/
	cd tmp && $(CAMLIDL) -no-include -nocpp -I $(MLAPRONIDL_PREFIX)/lib $*.idl
	cp tmp/$*_stubs.c $*_caml.c
	sed -f sedscript_caml tmp/$*.ml | grep --extended-regexp '^(.)+$$' >$*.ml
	sed -f sedscript_caml tmp/$*.mli | grep --extended-regexp '^(.)+$$' >$*.mli

manager.idl: $(MLAPRONIDL_PREFIX)/lib/manager.idl
	ln -fs $^ $@

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

