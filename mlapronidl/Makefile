include ../Makefile.config

#---------------------------------------
# Directories
#---------------------------------------

SRCDIR = $(shell pwd)
#
# Installation directory prefix
MLAPRONIDL_INSTALL = $(HOME)/pkg/mlapronidl/$(ARCH)
#
PREFIX = $(APRON_INSTALL)
#
# C include and lib directories
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
BINDIR = $(PREFIX)/bin

#---------------------------------------
# CAML part
#---------------------------------------
OCAMLC = ocamlc.opt
OCAMLOPT = ocamlopt.opt
OCAMLDEP = ocamldep
OCAMLLEX = ocamllex.opt
OCAMLYACC = ocamlyacc
OCAMLINC = -I $(MLGMPIDL_PREFIX)/lib
OCAMLFLAGS = -g
OCAMLOPTFLAGS = -for-pack Apron -inline 20

CAMLIDL = camlidl
M4 = m4

#---------------------------------------
# C part
#---------------------------------------
CC = gcc
ICFLAGS = \
-I$(GMP_PREFIX)/include -I$(MLGMPIDL_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml \
-I$(APRON_PREFIX)/include \
-Wall -Winline -Wimplicit-function-declaration -std=c99

CFLAGS = $(ICFLAGS) -O3 -DNDEBUG
CFLAGS_DEBUG = $(ICFLAGS) -O0 -g -UNDEBUG
CFLAGS_PROF = $(CFLAGS) -g -pg

#---------------------------------------
# Latex part
#---------------------------------------
LATEX=latex
DVIPS=dvips

#---------------------------------------
# Files
#---------------------------------------

IDLMODULES0 = scalar interval coeff dim linexpr0 lincons0 generator0 manager abstract0
IDLMODULES1 = environment linexpr1 lincons1 generator1 abstract1
IDLMODULES = $(IDLMODULES0) $(IDLMODULES1)
MLMODULES0 = $(IDLMODULES0)
MLMODULES1 = var $(IDLMODULES1)
MLMODULES = $(MLMODULES0) $(MLMODULES1)

MLSRC = $(MLMODULES:%=%.mli) $(MLMODULES:%=%.ml)
MLINT = $(MLMODULES:%=%.cmi)
MLOBJ = $(MLMODULES:%=%.cmo)
MLOBJx = $(MLMODULES:%=%.cmx)
MLLIB_TOINSTALL = $(MLMODULES:%=%.mli) $(MLINT) apron.cmi apron.cmo
MLLIB_TOINSTALLx = apron.cmx apron.o
CCMODULES = apron_caml $(IDLMODULES:%=%_caml)
CCSRC = $(CCMODULES:%=%.c)
CCLIB_TOINSTALL = libapron_caml.a libapron_caml_debug.a
CCINC_TOINSTALL = apron_caml.h

#---------------------------------------
# Rules
#---------------------------------------

all: $(MLSRC) $(MLINT) $(MLOBJ) $(MLOBJx) apron.cmi apron.cmx libapron_caml.a libapron_caml_debug.a

mlsrc: $(MLSRC)

mldep: $(MLSRC)
	ocamldep $(OCAMLINC) $(MLSRC)

#---------------------------------------
# Misc rules
#---------------------------------------

tar: $(IDLMODULES:%=%.idl) apron_caml.c apron_caml.h Makefile README mlapronidl.tex sedscript_caml
	(cd ..; tar zcvf $(HOME)/mlapronidl.tgz $(^:%=mlapronidl/%))

dist: $(IDLMODULES:%=%.idl) $(IDLMODULES:%=%.ml) $(IDLMODULES:%=%.mli) $(IDLMODULES:%=%_caml.c) apron_caml.c apron_caml.h Makefile README mlapronidl.tex sedscript_caml mlapronidl.dvi html
	(cd ..; tar zcvf $(HOME)/mlapronidl.tgz $(^:%=mlapronidl/%))

clean:
	/bin/rm -f $(IDLMODULES:%=%.ml) $(IDLMODULES:%=%.mli) $(IDLMODULES:%=%_caml.c)
	/bin/rm -f mpz.idl mpq.idl
	/bin/rm -f mlapronidl.out mlapronidl.aux mlapronidl.idx mlapronidl.ilg mlapronidl.ind mlapronidl.bbl mlapronidl.blg mlapronidl.dvi mlapronidl.log mlapronidl.toc mlapronidl.ps
	/bin/rm -f *.o *.a *.cmi *.cmo *.cmx *.cmxa *.cma
	/bin/rm -fr tmp html
	/bin/rm -f ocamldoc.[cefkimoptv]*

distclean:
	/bin/rm -f $(MLLIB_TOINSTALL:%=$(MLAPRONIDL_PREFIX)/lib/%)
	/bin/rm -f $(MLLIB_TOINSTALLx:%=$(MLAPRONIDL_PREFIX)/lib/%)
	/bin/rm -f $(CCLIB_TOINSTALL:%=$(MLAPRONIDL_PREFIX)/lib/%)
	/bin/rm -f $(IDLMODULES:%=$(MLAPRONIDL_PREFIX)/lib/%.idl)
	/bin/rm -f $(CCINC_TOINSTALL:%=$(MLAPRONIDL_PREFIX)/include/%)

install: $(MLLIB_TOINSTALL) $(MLLIB_TOINSTALLx) $(CCLIB_TOINSTALL) $(IDLMODULES:%=%.idl) $(CCINC_TOINSTALL)
	mkdir -p $(MLAPRONIDL_PREFIX)/lib
	mkdir -p $(MLAPRONIDL_PREFIX)/include
	cp $(MLLIB_TOINSTALL) $(MLLIB_TOINSTALLx) $(CCLIB_TOINSTALL) $(IDLMODULES:%=%.idl) $(MLAPRONIDL_PREFIX)/lib
	cp $(CCINC_TOINSTALL) $(MLAPRONIDL_PREFIX)/include

#---------------------------------------
# C rules
#---------------------------------------

libapron_caml.a: $(CCMODULES:%=%.o)
	ar rcs $@ $^
libapron_caml_debug.a: $(CCMODULES:%=%_debug.o)
	ar rcs $@ $^
libapron_caml_prof.a: $(CCMODULES:%=%_prof.o)
	ar rcs $@ $^

#---------------------------------------
# ML rules
#---------------------------------------

apron.cmo apron.cmi: $(MLOBJ)
	$(OCAMLC) -pack -o $@ $(MLOBJ)

apron.cmx apron.o: $(MLOBJx)
	$(OCAMLOPT) -pack -o $@ $(MLOBJx)

mpz.idl: $(MLGMPIDL_PREFIX)/lib/mpz.idl
	ln -s $^ $@
mpq.idl: $(MLGMPIDL_PREFIX)/lib/mpq.idl
	ln -s $^ $@

#---------------------------------------
# TEX and HTML rules
#---------------------------------------

.PHONY: mlapronidl.dvi html

doc: mlapronidl.dvi html

mlapronidl.dvi: $(MLINT) $(MLSRC)
	ocamldoc $(OCAMLINC) \
	-latextitle 1,chapter -latextitle 2,section -latextitle 3,subsection -latextitle 4,subsubsection -latextitle 5,paragraph -latextitle 6,subparagraph -noheader -notrailer -latex -o ocamldoc.tex $(MLMODULES:%=%.mli)
	latex mlapronidl
	makeindex mlapronidl
	latex mlapronidl
	latex mlapronidl

html: $(MLINT) $(MLSRC)
	mkdir -p html
	ocamldoc $(OCAMLINC) -html -d html -colorize-code $(MLMODULES:%=%.mli)

#--------------------------------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#--------------------------------------------------------------

.SUFFIXES: .tex .fig .c .h .o .ml .mli .cmi .cmo .cmx .idl _debug.o _prof.o _caml.c .nw

#---------------------------------------
# C generic rules
#---------------------------------------

%.o: %.c apron_caml.h
	$(CC) $(CFLAGS) -c $<
%_debug.o: %.c apron_caml.h
	$(CC) $(CFLAGS_DEBUG) -c -o $@ $<
%_prof.o: %.c apron_caml.h
	$(CC) $(CFLAGS_PROF) -c -o $@ $<

#---------------------------------------
# IDL generic rules
#---------------------------------------

#abstract%_caml.c abstract%.ml abstract%.mli: abstract%.idl sedscript_caml macros.m4
#	cp abstract$*.idl tmp/abstract$*.idl
#	$(CAMLIDL) -no-include  -prepro "$(M4) macros.m4" -I $(SRCDIR) tmp/abstract$*.idl
#	cp tmp/abstract$*_stubs.c abstract$*_caml.c
#	sed -f sedscript_caml tmp/abstract$*.mli >abstract$*.mli
#	sed -f sedscript_caml tmp/abstract$*.ml >abstract$*.ml

%_caml.c %.ml %.mli: %.idl sedscript_caml
	mkdir -p tmp
	cp $*.idl tmp/$*.idl
	$(CAMLIDL) -no-include -prepro "$(M4) --prefix-builtins macros.m4" -I $(SRCDIR) tmp/$*.idl
	sed -e '1 a\\n/* This file is part of the APRON Library, released under LGPL license.\n  Please read the COPYING file packaged in the distribution  */' tmp/$*_stubs.c >$*_caml.c
	sed -f sedscript_caml tmp/$*.ml >$*.ml
	sed -f sedscript_caml tmp/$*.mli >$*.mli

#---------------------------------------
# ML generic rules
#---------------------------------------

%.cmi: %.mli
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<

#---------------------------------------
# Dependencies
#---------------------------------------
interval.cmi: scalar.cmi
coeff.cmi: scalar.cmi interval.cmi
linexpr0.cmi: dim.cmi coeff.cmi
lincons0.cmi: linexpr0.cmi dim.cmi
generator0.cmi: linexpr0.cmi dim.cmi
abstract0.cmi: manager.cmi linexpr0.cmi lincons0.cmi interval.cmi \
    generator0.cmi dim.cmi
environment.cmi: dim.cmi var.cmi
linexpr1.cmi: linexpr0.cmi environment.cmi coeff.cmi
lincons1.cmi: linexpr1.cmi lincons0.cmi environment.cmi coeff.cmi
generator1.cmi: linexpr1.cmi generator0.cmi environment.cmi coeff.cmi
abstract1.cmi: manager.cmi linexpr1.cmi lincons1.cmi interval.cmi \
    generator1.cmi environment.cmi abstract0.cmi
var.cmo: var.cmi
var.cmx: var.cmi
scalar.cmo: scalar.cmi
scalar.cmx: scalar.cmi
interval.cmo: scalar.cmi interval.cmi
interval.cmx: scalar.cmx interval.cmi
coeff.cmo: scalar.cmi interval.cmi coeff.cmi
coeff.cmx: scalar.cmx interval.cmx coeff.cmi
dim.cmo: dim.cmi
dim.cmx: dim.cmi
linexpr0.cmo: scalar.cmi interval.cmi dim.cmi coeff.cmi linexpr0.cmi
linexpr0.cmx: scalar.cmx interval.cmx dim.cmx coeff.cmx linexpr0.cmi
lincons0.cmo: linexpr0.cmi lincons0.cmi
lincons0.cmx: linexpr0.cmx lincons0.cmi
generator0.cmo: linexpr0.cmi generator0.cmi
generator0.cmx: linexpr0.cmx generator0.cmi
manager.cmo: manager.cmi
manager.cmx: manager.cmi
abstract0.cmo: manager.cmi linexpr0.cmi lincons0.cmi interval.cmi \
    generator0.cmi dim.cmi abstract0.cmi
abstract0.cmx: manager.cmx linexpr0.cmx lincons0.cmx interval.cmx \
    generator0.cmx dim.cmx abstract0.cmi
environment.cmo: dim.cmi var.cmi environment.cmi
environment.cmx: dim.cmx var.cmx environment.cmi
linexpr1.cmo: linexpr0.cmi environment.cmi coeff.cmi linexpr1.cmi
linexpr1.cmx: linexpr0.cmx environment.cmx coeff.cmx linexpr1.cmi
lincons1.cmo: linexpr1.cmi linexpr0.cmi lincons0.cmi environment.cmi \
    coeff.cmi lincons1.cmi
lincons1.cmx: linexpr1.cmx linexpr0.cmx lincons0.cmx environment.cmx \
    coeff.cmx lincons1.cmi
generator1.cmo: linexpr1.cmi linexpr0.cmi generator0.cmi environment.cmi \
    coeff.cmi generator1.cmi
generator1.cmx: linexpr1.cmx linexpr0.cmx generator0.cmx environment.cmx \
    coeff.cmx generator1.cmi
abstract1.cmo: manager.cmi linexpr1.cmi lincons1.cmi interval.cmi \
    generator1.cmi environment.cmi abstract0.cmi abstract1.cmi
abstract1.cmx: manager.cmx linexpr1.cmx lincons1.cmx interval.cmx \
    generator1.cmx environment.cmx abstract0.cmx abstract1.cmi
