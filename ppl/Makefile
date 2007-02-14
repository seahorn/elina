# Makefile
#
# APRON Library / PPL library wrapper
#
# Copyright (C) Antoine Mine' 2006

# This file is part of the APRON Library, released under LGPL license.
# Please read the COPYING file packaged in the distribution.

include ../Makefile.config

PREFIX = $(POLKA_PREFIX)

# C include and lib directories
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
BINDIR = $(PREFIX)/bin
CAMLDIR = $(PREFIX)/lib

SRCDIR = $(shell pwd)

#---------------------------------------
# Programs
#---------------------------------------

# Installation program
INSTALL = install
INSTALLd = install -d

# C compiler and C preprocessor
CC = gcc

# C++ compiler
CXX = g++

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
-I../newpolka \
-I../mlgmpidl \
-I../apron \
-I../mlapronidl \
-I../num \
-I$(POLKA_PREFIX)/include \
-I$(MLGMPIDL_PREFIX)/include \
-I$(GMP_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-I$(MLAPRONIDL_PREFIX)/include \
-I$(NUM_PREFIX)/include \
-I$(PPL_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml \
-Wall -W -Wundef -Wcast-align \
-Wdisabled-optimization \
-Wno-unused
# -Winline
CCFLAGS = $(OPTFLAGS) $(ICFLAGS) -DNDEBUG
CXXFLAGS = $(OPTFLAGS) $(ICFLAGS) -DNDEBUG

# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl
OCAMLFLAGS = -g
OCAMLOPTFLAGS = -inline 20


#---------------------------------------
# Files
#---------------------------------------

CXXSOURCES = ppl_user.cc ppl_poly.cc
CSOURCES = ppl_test.c
CCINC = ppl_user.h apron_ppl.h

# trigers a whole recompilation
# DEPS = $(APRON_PREFIX)/include/ap_abstract0.h

#---------------------------------------
# Rules
#---------------------------------------

all: libapron_ppl.a apron_ppl_test

clean:
	/bin/rm -f *.[ao] *.so apron_ppl_test
	/bin/rm -f *.cm[ioax] *.cmxa apron_ppl_run apron_ppl_top
	/bin/rm -f apron_ppl_caml.c apron_ppl.ml apron_ppl.mli
	/bin/rm -fr *~ \#*\# tmp

install:
	$(INSTALLd) $(INCDIR) $(LIBDIR) $(BINDIR) $(CAMLDIR)
	$(INSTALL) apron_ppl.h $(INCDIR)
	$(INSTALL) libapron_ppl.a $(LIBDIR)
	for i in apron_ppl_test apron_ppl_run apron_ppl_top; do \
		if test -f $$i; then $(INSTALL) $$i $(BINDIR); fi; \
	done
	for i in libapron_ppl_caml.a apron_ppl.idl apron_ppl.cmi apron_ppl.cma apron_ppl.cmxa apron_ppl.a; do \
		if test -f $$i; then $(INSTALL) $$i $(CAMLDIR); fi; \
	done

uninstall:
	/bin/rm -f $(INCDIR)/*apron_ppl*
	/bin/rm -f $(LIBDIR)/*apron_ppl*
	/bin/rm -f $(BINDIR)/*apron_ppl*
	/bin/rm -f $(CAMLDIR)/*apron_ppl*

distclean: clean uninstall

dist: Makefile COPYING README $(CXXSOURCES) $(CSOURCES) $(CCINC) apron_ppl.idl sedscript_caml
	(cd ..; tar zcvf ppl.tgz $(^:%=ppl/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .cc .h .a .o .so



#-----------------------------------
# C / C++ part
#-----------------------------------

libapron_ppl.a: $(subst .cc,.o,$(CXXSOURCES))
	$(AR) rcs $@ $^

apron_ppl_test: libapron_ppl.a ppl_test.o
	$(CXX) $(CXXFLAGS) -o $@ ppl_test.o \
	-L. -lapron_ppl \
	-L../newpolka -L$(POLKA_PREFIX)/lib -lpolkag_debug \
	-L../apron -L$(APRON_PREFIX)/lib -lapron \
	-L$(PPL_PREFIX)/lib -lppl \
	-L$(GMP_PREFIX)/lib -lgmpxx -lmpfr -lgmp -lm

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<


#-----------------------------------
# Caml part
#-----------------------------------

ml: apron_ppl.mli apron_ppl.cmi apron_ppl.cma apron_ppl.cmxa apron_ppl_top apron_ppl_run

apron_ppl_top: apron_ppl.cma libapron_ppl_caml.a libapron_ppl.a
	$(OCAMLMKTOP) $(OCAMLINC) $(OCAMLFLAGS) -o $@ -cc $(CXX) -custom bigarray.cma gmp.cma apron.cma apron_ppl.cma -ccopt -L. -cclib -lapron_ppl -ccopt -L$(PPL_PREFIX)/lib -cclib -lppl -ccopt -L../mlgmpidl -ccopt -L../mlapronidl -ccopt -L../apron -ccopt -L$(GMP_PREFIX)/lib -cclib -lgmpxx

apron_ppl_run: apron_ppl.cma libapron_ppl_caml.a
	$(OCAMLC) $(OCAMLINC) $(OCAMLFLAGS) -o $@ -make-runtime -cc $(CXX) bigarray.cma gmp.cma apron.cma apron_ppl.cma -ccopt -L. -cclib -lapron_ppl -ccopt -L$(PPL_PREFIX)/lib -cclib -lppl -ccopt -L../mlgmpidl -ccopt -L../mlapronidl -ccopt -L../apron -ccopt -L$(GMP_PREFIX)/lib -cclib -lgmpxx

apron_ppl.cma: apron_ppl.cmi apron_ppl.cmo libapron_ppl_caml.a
	$(OCAMLMKLIB) $(OCAMLINC) -o apron_ppl apron_ppl.cmo -lapron_ppl_caml

apron_ppl.cmxa apron_ppl.a: apron_ppl.cmi apron_ppl.cmx libapron_ppl_caml.a
	$(OCAMLMKLIB) $(OCAMLINC) -o apron_ppl apron_ppl.cmx -lapron_ppl_caml

libapron_ppl_caml.a: apron_ppl_caml.o
	$(AR) rcs $@ $^

libapron_ppl_caml.so: apron_ppl_caml.o
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

.PRECIOUS: %_caml.c %.ml %.mli %.cmi libapron_ppl_caml.a libapron_ppl_caml.so apron_ppl.cmx apron_ppl.cmo


#---------------------------------------
# ML generic rules
#---------------------------------------

%.cmi: %.mli  $(DEPS)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi  $(DEPS)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi  $(DEPS)
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<
