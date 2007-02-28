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

# Library creation
SHARED = gcc -shared

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
-I$(GMP_PREFIX)/include \
-I$(PPL_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml

ICXXFLAGS = \
-I../newpolka \
-I../mlgmpidl \
-I../apron \
-I../mlapronidl \
-I../num \
-I$(GMP_PREFIX)/include \
-I$(PPL_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml


# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl

#---------------------------------------
# Files
#---------------------------------------

CXXSOURCES = ppl_user.cc ppl_poly.cc ppl_grid.cc
CSOURCES = ppl_test.c
CCINC = ppl_user.h apron_ppl.h

#---------------------------------------
# Rules
#---------------------------------------

all: libapron_ppl.a apron_ppl_test

clean:
	/bin/rm -f *.[ao] *.so apron_ppl_test
	/bin/rm -f *.cm[ioax] *.cmxa apron_ppl_run apron_ppl_top
	/bin/rm -fr *~ \#*\# tmp

mostlyclean:
	/bin/rm -f apron_ppl_caml.c apron_ppl.ml apron_ppl.mli

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
	$(RANLIB) $@
libapron_ppl_debug.a: $(subst .cc,_debug.o,$(CXXSOURCES))
	$(AR) rcs $@ $^
	$(RANLIB) $@

apron_ppl_test: libapron_ppl_debug.a ppl_test_debug.o
	$(CXX) $(CXXFLAGS) -o $@ ppl_test_debug.o \
	-L. -lapron_ppl_debug \
	-L../newpolka -L$(POLKA_PREFIX)/lib -lpolkag_debug \
	-L../apron -L$(APRON_PREFIX)/lib -lapron \
	-L$(PPL_PREFIX)/lib -lppl \
	-L$(GMP_PREFIX)/lib -lgmpxx -lmpfr -lgmp -lm

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(ICXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -c -o $@ $<

%_debug.o: %.cc
	$(CXX) $(CXXFLAGS_DEBUG) $(ICXXFLAGS) -c -o $@ $<

%_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -c -o $@ $<

#-----------------------------------
# Caml part
#-----------------------------------

ml: apron_ppl.cma apron_ppl.cmxa apron_ppl_top apron_ppl_run

apron_ppl_top: apron_ppl.cma libapron_ppl_caml.a libapron_ppl.a
	$(OCAMLMKTOP) $(OCAMLINC) $(OCAMLFLAGS) -o $@ -cc $(CXX) -custom bigarray.cma gmp.cma apron.cma apron_ppl.cma -ccopt -L. -cclib -lapron_ppl -ccopt -L$(PPL_PREFIX)/lib -cclib -lppl -ccopt -L../mlgmpidl -ccopt -L../mlapronidl -ccopt -L../apron -ccopt -L$(GMP_PREFIX)/lib -cclib -lgmpxx

apron_ppl_run: apron_ppl.cma libapron_ppl_caml.a
	$(OCAMLC) $(OCAMLINC) $(OCAMLFLAGS) -o $@ -make-runtime -cc $(CXX) bigarray.cma gmp.cma apron.cma apron_ppl.cma -ccopt -L. -cclib -lapron_ppl -ccopt -L$(PPL_PREFIX)/lib -cclib -lppl -ccopt -L../mlgmpidl -ccopt -L../mlapronidl -ccopt -L../apron -ccopt -L$(GMP_PREFIX)/lib -cclib -lgmpxx

apron_ppl.cma: apron_ppl.cmi apron_ppl.cmo libapron_ppl_caml.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -a -o $@ apron_ppl.cmo -ccopt -L$(PPL_PREFIX)/lib -cclib -lapron_ppl_caml

apron_ppl.cmxa: apron_ppl.cmi apron_ppl.cmx libapron_ppl_caml.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ apron_ppl.cmx -ccopt -L$(PPL_PREFIX)/lib -cclib -lapron_ppl_caml

libapron_ppl_caml.a: apron_ppl_caml.o
	$(AR) rcs $@ $^
	$(RANLIB) $@

libapron_ppl_caml.so: apron_ppl_caml.o
	$(SHARED) -o $@ $^

manager.idl: ../mlapronidl/manager.idl
	ln -s $< $@

rebuild: manager.idl apron_ppl.idl
	mkdir -p tmp
	cp apron_ppl.idl tmp/
	$(CAMLIDL) -no-include -nocpp tmp/apron_ppl.idl
	cp tmp/apron_ppl_stubs.c apron_ppl_caml.c
	sed -f sedscript_caml tmp/apron_ppl.ml | grep --extended-regexp '^(.)+$$' >apron_ppl.ml
	sed -f sedscript_caml tmp/apron_ppl.mli | grep --extended-regexp '^(.)+$$' >apron_ppl.mli

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
