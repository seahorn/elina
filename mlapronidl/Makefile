include ../Makefile.config

#---------------------------------------
# Directories
#---------------------------------------

SRCDIR = $(shell pwd)
#
#
PREFIX = $(APRON_INSTALL)
#

#---------------------------------------
# CAML part
#---------------------------------------
OCAMLC = $(CAML_PREFIX)/bin/ocamlc.opt
OCAMLOPT = $(CAML_PREFIX)/bin/ocamlopt.opt
OCAMLDEP = $(CAML_PREFIX)/bin/ocamldep
OCAMLLEX = $(CAML_PREFIX)/bin/ocamllex.opt
OCAMLYACC = $(CAML_PREFIX)/bin/ocamlyacc
OCAMLDOC = $(CAML_PREFIX)/bin/ocamldoc.opt

OCAMLINC = -I ../mlgmpidl
OCAMLFLAGS = -g
OCAMLOPTFLAGS = -for-pack Apron -inline 20

CAMLIDL = $(CAMLIDL_PREFIX)/bin/camlidl
M4 = m4

#---------------------------------------
# C part
#---------------------------------------
CC = gcc
ICFLAGS = \
-I../mlgmpidl -I../apron \
-I$(GMP_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml \
-Wall -Winline -Wimplicit-function-declaration -std=c99

CFLAGS = $(ICFLAGS) $(OPTFLAGS) -DNDEBUG
CFLAGS_DEBUG = $(ICFLAGS) -O0 -g -UNDEBUG
CFLAGS_PROF = $(CFLAGS) -g -pg

#---------------------------------------
# Latex part
#---------------------------------------
LATEX=latex
DVIPDF=dvipdf

#---------------------------------------
# Files
#---------------------------------------

IDLMODULES = \
scalar interval coeff \
dim linexpr0 lincons0 generator0 manager abstract0 \
var environment linexpr1 lincons1 generator1 abstract1 

IDLMODULEStex = ocamldoc.tex \
Scalar.tex Interval.tex Coeff.tex \
Dim.tex Linexpr0.tex Lincons0.tex Generator0.tex Manager.tex Abstract0.tex \
Var.tex Environment.tex Linexpr1.tex Lincons1.tex Generator1.tex Abstract1.tex \
Mpz.tex Mpq.tex Mpzf.tex Mpqf.tex Gmp_random.tex Itv.tex Polka.tex Oct.tex

MLMODULES = $(IDLMODULES) apron_parser apron_lexer parser

MLSRC = $(MLMODULES:%=%.mli) $(MLMODULES:%=%.ml) apron_lexer.mll apron_parser.mly
MLINT = $(MLMODULES:%=%.cmi)
MLOBJ = $(MLMODULES:%=%.cmo)
MLOBJx = $(MLMODULES:%=%.cmx)
MLLIB_TOINSTALL = $(IDLMODULES:%=%.idl) $(MLMODULES:%=%.mli) apron.cmi apron.cma
MLLIB_TOINSTALLx = apron.cmxa apron.a
CCMODULES = apron_caml $(IDLMODULES:%=%_caml)
CCSRC = apron_caml.h $(CCMODULES:%=%.c)
CCLIB_TOINSTALL = libapron_caml.a libapron_caml_debug.a
CCINC_TOINSTALL = apron_caml.h

#---------------------------------------
# Rules
#---------------------------------------

all: apron.cmi apron.cmx apron.cma apron.cmxa apron.a libapron_caml.a libapron_caml_debug.a

mldep: $(MLSRC)
	ocamldep $(MLSRC)

#---------------------------------------
# Misc rules
#---------------------------------------

tar: $(IDLMODULES:%=%.idl) apron_lexer.mll apron_lexer.mli apron_parser.mly parser.ml parser.mli apron_caml.c apron_caml.h macros.m4 Makefile COPYING README mlapronidl.tex sedscript_caml
	(cd ..; tar zcvf mlapronidl.tgz $(^:%=mlapronidl/%))

dist: $(IDLMODULES:%=%.idl) $(MLSRC) $(CCSRC) macros.m4 apron_caml.c apron_caml.h Makefile COPYING README mlapronidl.tex sedscript_caml mlapronidl.pdf html
	(cd ..; tar zcvf mlapronidl.tgz $(^:%=mlapronidl/%))

clean:
	/bin/rm -f $(IDLMODULEStex)
	/bin/rm -f mpz.idl mpq.idl
	/bin/rm -f mlapronidl.out mlapronidl.aux mlapronidl.idx mlapronidl.ilg mlapronidl.ind mlapronidl.bbl mlapronidl.blg mlapronidl.dvi mlapronidl.log mlapronidl.toc mlapronidl.ps
	/bin/rm -f *.o *.a *.cmi *.cmo *.cmx *.cmxa *.cma
	/bin/rm -fr tmp html
	/bin/rm -f ocamldoc.[cefkimoptv]*

mostlyclean: clean
	/bin/rm -f $(IDLMODULES:%=%.ml) $(IDLMODULES:%=%.mli) $(IDLMODULES:%=%_caml.c)

distclean:
	/bin/rm -f $(MLLIB_TOINSTALL:%=$(MLAPRONIDL_PREFIX)/lib/%)
	/bin/rm -f $(MLLIB_TOINSTALLx:%=$(MLAPRONIDL_PREFIX)/lib/%)
	/bin/rm -f $(CCLIB_TOINSTALL:%=$(MLAPRONIDL_PREFIX)/lib/%)
	/bin/rm -f $(CCINC_TOINSTALL:%=$(MLAPRONIDL_PREFIX)/include/%)

install: $(MLLIB_TOINSTALL) $(MLLIB_TOINSTALLx) $(CCLIB_TOINSTALL) $(CCINC_TOINSTALL)
	mkdir -p $(MLAPRONIDL_PREFIX)/lib
	mkdir -p $(MLAPRONIDL_PREFIX)/include
	cp $(MLLIB_TOINSTALL) $(MLLIB_TOINSTALLx) $(CCLIB_TOINSTALL) $(MLAPRONIDL_PREFIX)/lib
	cp $(CCINC_TOINSTALL) $(MLAPRONIDL_PREFIX)/include

#---------------------------------------
# C rules
#---------------------------------------

libapron_caml.a: $(CCMODULES:%=%.o)
	ar rcs $@ $^
libapron_caml_debug.a: $(CCMODULES:%=%_debug.o)
	ar rcs $@ $^

#---------------------------------------
# ML rules
#---------------------------------------

apron.cma: apron.cmo libapron_caml.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -a -o $@ apron.cmo \
	-cclib "-L$(MLAPRONIDL_PREFIX)/lib -lapron_caml" \
	-cclib "-L$(APRON_PREFIX)/lib -lapron"

apron.cmxa: apron.cmx libapron_caml.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ apron.cmx \
	-cclib "-L$(MLAPRONIDL_PREFIX)/lib -lapron_caml" \
	-cclib "-L$(APRON_PREFIX)/lib -lapron"

apron.cmo apron.cmi: $(MLOBJ) $(MLINT)
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -pack -o $@ $(MLOBJ)

apron.cmx apron.o: $(MLOBJx)
	$(OCAMLOPT) -pack -o $@ $(MLOBJx)

mpz.idl: ../mlgmpidl/mpz.idl
	cp $^ $@
mpq.idl: ../mlgmpidl/mpq.idl
	cp $^ $@

apron_lexer.ml: apron_lexer.mll
	$(OCAMLLEX) $^
apron_parser.ml apron_parser.mli: apron_parser.mly
	$(OCAMLYACC) $^

#---------------------------------------
# TEX and HTML rules
#---------------------------------------

.PHONY: html

mlapronidl.pdf: mlapronidl.dvi
	$(DVIPDF) mlapronidl.dvi

mlapronidl.dvi: mlapronidl.tex $(MLMODULES:%=%.mli)
	$(OCAMLDOC) $(OCAMLINC) \
	-latextitle 1,chapter -latextitle 2,section -latextitle 3,subsection -latextitle 4,subsubsection -latextitle 5,paragraph -latextitle 6,subparagraph -noheader -notrailer -latex -sepfiles -o ocamldoc.tex ../mlgmpidl/mpzf.mli ../mlgmpidl/mpqf.mli $(MLMODULES:%=%.mli) ../itv/itv.mli ../octagons/oct.mli ../newpolka/polka.mli  ../mlgmpidl/mpz.mli ../mlgmpidl/mpq.mli ../mlgmpidl/gmp_random.mli
	latex mlapronidl
	makeindex mlapronidl
	latex mlapronidl
	latex mlapronidl



html: $(IDLMODULES:%=%.mli) mlapronidl.odoc
	mkdir -p html
	$(OCAMLDOC) $(OCAMLINC) -html -d html -colorize-code ../mlgmpidl/mpzf.mli ../mlgmpidl/mpqf.mli $(IDLMODULES:%=%.mli) ../itv/itv.mli ../octagons/oct.mli ../newpolka/polka.mli ../mlgmpidl/mpz.mli ../mlgmpidl/mpq.mli ../mlgmpidl/gmp_random.mli 
	cp -f index.html html

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

%.mli %_caml.c %.ml: %.idl sedscript_caml mpz.idl mpq.idl
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
environment.cmi: var.cmi dim.cmi
linexpr1.cmi: var.cmi linexpr0.cmi environment.cmi coeff.cmi
lincons1.cmi: var.cmi linexpr1.cmi lincons0.cmi environment.cmi coeff.cmi
generator1.cmi: var.cmi linexpr1.cmi generator0.cmi environment.cmi coeff.cmi
abstract1.cmi: var.cmi manager.cmi linexpr1.cmi lincons1.cmi interval.cmi \
    generator1.cmi environment.cmi abstract0.cmi
apron_parser.cmi: lincons0.cmi generator0.cmi coeff.cmi
apron_lexer.cmi: apron_parser.cmi
parser.cmi: manager.cmi linexpr1.cmi lincons1.cmi generator1.cmi \
    environment.cmi abstract1.cmi
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
var.cmo: var.cmi
var.cmx: var.cmi
environment.cmo: var.cmi dim.cmi environment.cmi
environment.cmx: var.cmx dim.cmx environment.cmi
linexpr1.cmo: var.cmi linexpr0.cmi environment.cmi coeff.cmi linexpr1.cmi
linexpr1.cmx: var.cmx linexpr0.cmx environment.cmx coeff.cmx linexpr1.cmi
lincons1.cmo: var.cmi linexpr1.cmi linexpr0.cmi lincons0.cmi environment.cmi \
    coeff.cmi abstract0.cmi lincons1.cmi
lincons1.cmx: var.cmx linexpr1.cmx linexpr0.cmx lincons0.cmx environment.cmx \
    coeff.cmx abstract0.cmx lincons1.cmi
generator1.cmo: var.cmi linexpr1.cmi linexpr0.cmi generator0.cmi \
    environment.cmi coeff.cmi abstract0.cmi generator1.cmi
generator1.cmx: var.cmx linexpr1.cmx linexpr0.cmx generator0.cmx \
    environment.cmx coeff.cmx abstract0.cmx generator1.cmi
abstract1.cmo: var.cmi manager.cmi linexpr1.cmi lincons1.cmi interval.cmi \
    generator1.cmi environment.cmi abstract0.cmi abstract1.cmi
abstract1.cmx: var.cmx manager.cmx linexpr1.cmx lincons1.cmx interval.cmx \
    generator1.cmx environment.cmx abstract0.cmx abstract1.cmi
apron_parser.cmo: scalar.cmi lincons0.cmi interval.cmi generator0.cmi \
    coeff.cmi apron_parser.cmi
apron_parser.cmx: scalar.cmx lincons0.cmx interval.cmx generator0.cmx \
    coeff.cmx apron_parser.cmi
apron_lexer.cmo: apron_parser.cmi apron_lexer.cmi
apron_lexer.cmx: apron_parser.cmx apron_lexer.cmi
parser.cmo: var.cmi manager.cmi linexpr1.cmi lincons1.cmi lincons0.cmi \
    generator1.cmi generator0.cmi environment.cmi coeff.cmi apron_parser.cmi \
    apron_lexer.cmi abstract1.cmi parser.cmi
parser.cmx: var.cmx manager.cmx linexpr1.cmx lincons1.cmx lincons0.cmx \
    generator1.cmx generator0.cmx environment.cmx coeff.cmx apron_parser.cmx \
    apron_lexer.cmx abstract1.cmx parser.cmi
