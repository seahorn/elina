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
-I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include \
-I$(PPL_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml

ICXXFLAGS = \
-I../newpolka \
-I../mlgmpidl \
-I../apron \
-I../mlapronidl \
-I../num \
-I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include \
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
	/bin/rm -f *.cm[ioax] *.cmxa pplrun ppltop
	/bin/rm -fr *~ \#*\# tmp

mostlyclean:
	/bin/rm -f ppl_caml.c ppl.ml ppl.mli

install:
	$(INSTALLd) $(INCDIR) $(LIBDIR) $(BINDIR) $(CAMLDIR)
	$(INSTALL) apron_ppl.h $(INCDIR)
	$(INSTALL) libapron_ppl.a $(LIBDIR)
	for i in apron_ppl_test pplrun ppltop; do \
		if test -f $$i; then $(INSTALL) $$i $(BINDIR); fi; \
	done
	for i in libppl_caml.a ppl.idl ppl.cmi ppl.cma ppl.cmxa ppl.a; do \
		if test -f $$i; then $(INSTALL) $$i $(CAMLDIR); fi; \
	done

uninstall:
	/bin/rm -f $(INCDIR)/*apron_ppl*
	/bin/rm -f $(LIBDIR)/*apron_ppl* $(LIBDIR)/ppl.a $(LIBDIR)/ppl.idl $(LIBDIR)/ppl.cm*
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
	-L$(GMP_PREFIX)/lib -L$(MPFR_PREFIX)/lib -lgmpxx -lmpfr -lgmp -lm

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

ml: ppl.cma ppl.cmxa ppltop pplrun

ppltop: ppl.cma libppl_caml.a libapron_ppl.a
	$(OCAMLMKTOP) $(OCAMLINC) $(OCAMLFLAGS) -o $@ -cc $(CXX) -custom \
	bigarray.cma gmp.cma apron.cma ppl.cma \
	-ccopt "-L. -L../apron -L../mlgmpidl -L../mlapronidl"

pplrun: ppl.cma libppl_caml.a
	$(OCAMLC) $(OCAMLINC) $(OCAMLFLAGS) -o $@ -make-runtime -cc $(CXX) \
	bigarray.cma gmp.cma apron.cma ppl.cma \
	-ccopt "-L.  -L../apron -L../mlgmpidl -L../mlapronidl"

ppl.cma: ppl.cmi ppl.cmo libppl_caml.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -a -o $@ \
	ppl.cmo \
	-ccopt "-L$(PPL_PREFIX)/lib" \
	-cclib "-lppl_caml -lapron_ppl -lppl -lgmpxx"

ppl.cmxa: ppl.cmi ppl.cmx libppl_caml.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ \
	ppl.cmx \
	-ccopt "-L$(PPL_PREFIX)/lib" \
	-cclib "-lppl_caml -lapron_ppl -lppl -lgmpxx"

libppl_caml.a: ppl_caml.o
	$(AR) rcs $@ $^
	$(RANLIB) $@

libppl_caml.so: ppl_caml.o
	$(SHARED) -o $@ $^

manager.idl: ../mlapronidl/manager.idl
	ln -s $< $@

rebuild: manager.idl ppl.idl
	mkdir -p tmp
	cp ppl.idl tmp/
	$(CAMLIDL) -no-include -nocpp tmp/ppl.idl
	cp tmp/ppl_stubs.c ppl_caml.c
	sed -f sedscript_caml tmp/ppl.ml >ppl.ml
	sed -f sedscript_caml tmp/ppl.mli >ppl.mli

.PRECIOUS: %_caml.c %.ml %.mli %.cmi libppl_caml.a libppl_caml.so ppl.cmx ppl.cmo


#---------------------------------------
# ML generic rules
#---------------------------------------

%.cmi: %.mli  $(DEPS)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi  $(DEPS)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi  $(DEPS)
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<
