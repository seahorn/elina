
include ../Makefile.config

PREFIX = $(POLKA_PREFIX)

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

OCAMLC = $(CAML_PREFIX)/bin/ocamlc.opt 
OCAMLOPT = $(CAML_PREFIX)/bin/ocamlopt.opt
OCAMLMKTOP = $(CAML_PREFIX)/bin/ocamlmktop
CAMLIDL = $(CAMLIDL_PREFIX)/bin/camlidl

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(GMP_PREFIX)/include \
-I../apron \
-I../num \
-I../mlgmpidl \
-I../mlapronidl \
-Wall -Wstrict-prototypes -Wimplicit-function-declaration \
-std=c99
# -Winline -Wconversion

CFLAGS =  $(ICFLAGS) $(OPTFLAGS) -DNDEBUG
# For debugging purpose
CFLAGS_DEBUG = $(ICFLAGS) -O0 -g -UNDEBUG
CFLAGS_PROF = -g -pg

# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl
OCAMLFLAGS = -g
OCAMLOPTFLAGS = -inline 20

#---------------------------------------
# Files
#---------------------------------------

CCMODULES = \
mf_qsort \
pk_user \
pk_internal pk_bit pk_satmat pk_vector pk_matrix pk_cherni \
pk_representation pk_constructor pk_test pk_extract \
pk_meetjoin pk_assign pk_project pk_resize pk_expandfold pk_widening pk_closure \
pkeq

CCSRC = pk_config.h pk_int.h pk.h $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = pk.h pkeq.h
CCBIN_TO_INSTALL = 
CCLIB_TO_INSTALL = libpolkag.a libpolkag_debug.a libpolka_caml.a libpolka_caml_debug.a 

CAML_TO_INSTALL = polka.idl polkaeq.idl \
polka.cma polka.cmxa polka.a \
polka.cmi polka.cmx polka.mli polka.ml \
polkaeq.cmi polkaeq.cmx polkaeq.mli polkaeq.ml 

#---------------------------------------
# Rules
#---------------------------------------

# Possible goals:
# depend doc install
# and the following one

all: allg

ml: $(CAML_TO_INSTALL) libpolka_caml.a libpolka_caml_debug.a

allg: libpolkag.a libpolkag_debug.a 

test0g: test0g_debug.o libpolkag_debug.a
	$(CC) $(ICFLAGS) $(XCFLAGS) -I$(GMP_PREFIX)/include -o $@ $< \
	-L. -lpolkag_debug -L$(APRON_PREFIX)/lib -lapron_debug -lgmp -lm

test1g: test1g_debug.o libpolkag_debug.a
	$(CC) $(ICFLAGS) $(XCFLAGS) -I$(GMP_PREFIX)/include -o $@ $< \
	-L. -lpolkag_debug -L$(APRON_PREFIX)/lib -lapron_debug -lgmp -lm

clean:
	/bin/rm -f *.[ao] 
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*
	/bin/rm -f test[01][ilg] test[ilg]_caml polkarun[ilg] polkatop[ilg]
	/bin/rm -fr *.cm[ioax] 
	/bin/rm -f manager.idl
	/bin/rm -fr tmp

mostlyclean: clean
	/bin/rm -f manager.idl
	/bin/rm -fr polka_caml.* polka.ml polka.mli

install:
	$(INSTALLd) $(PREFIX)/include $(PREFIX)/lib
	$(INSTALL) $(CCINC_TO_INSTALL) $(PREFIX)/include
	for i in $(CCLIB_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done
	for i in $(CCBIN_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/bin; fi; \
	done
	for i in $(CAML_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done

distclean:
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(PREFIX)/include/$$i; done
	for i in $(CCLIB_TO_INSTALL); do /bin/rm -f $(PREFIX)/lib/$$i; done
	for i in $(CCBIN_TO_INSTALL); do /bin/rm -f $(PREFIX)/bin/$$i; done
	for i in $(CAML_TO_INSTALL); do /bin/rm -f $(PREFIX)/lib/$$i; done
	/bin/rm -f Makefile.depend

dist: $(CCSRC) Makefile sedscript_caml newpolka.texi polka.idl polka.ml polka.mli polkaeq.idl polkaeq.ml polkaeq.mli polka_caml.c COPYING README
	(cd ..; tar zcvf newpolka.tgz $(^:%=newpolka/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

#-----------------------------------
# C part
#-----------------------------------

libpolkag.a: $(CCMODULES:%=%g.o)
	ar rcs $@ $^
libpolkag_debug.a: $(CCMODULES:%=%g_debug.o)
	ar rcs $@ $^

%i.o: %.c
	$(CC) $(CFLAGS) -DNUMINT_LONGINT -c -o $@ $<
%l.o: %.c
	$(CC) $(CFLAGS) -DNUMINT_LONGLONGINT -c -o $@ $<
%g.o: %.c
	$(CC) $(CFLAGS) -DNUMINT_MPZ -I$(GMP_PREFIX)/include -c -o $@ $<

%g_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) -DNUMINT_MPZ -I$(GMP_PREFIX)/include -c -o $@ $<

#-----------------------------------
# Caml part
#-----------------------------------

polkatopg: polka.cma libpolka_caml.a libpolkag_debug.a
	$(OCAMLMKTOP) $(OCAMLINC) $(OCAMLFLAGS) -verbose -o $@ \
	bigarray.cma gmp.cma apron.cma polka.cma \
	-ccopt "-L. -L../mlgmpidl -L../mlapronidl -L../apron" \
	-cclib "-lpolkag_debug" 

polkarung: polka.cma libpolka_caml_debug.a libpolkag_debug.a
	$(OCAMLC) $(OCAMLINC) $(OCAMLFLAGS) -verbose -o $@ \
	-make-runtime bigarray.cma gmp.cma apron.cma polka.cma \
	-ccopt "-L. -L../mlgmpidl -L../mlapronidl -L../apron" \
	-cclib "-lpolkag_debug"

testg_caml: test.cmo polkarung 
	$(OCAMLC) -g $(OCAMLINC) -o $@ -use-runtime polkarung \
	bigarray.cma gmp.cma apron.cma polka.cma test.cmo 

test.cmo: test.ml
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -I $(CAMLLIB_PREFIX)/lib -c $<

#---------------------------------------
# C rules
#---------------------------------------

libpolka_caml.a: polka_caml.o polkaeq_caml.o
	ar rcs $@ $^
libpolka_caml_debug.a: polka_caml_debug.o polkaeq_caml_debug.o
	ar rcs $@ $^

#---------------------------------------
# ML rules
#---------------------------------------

polka.cma: polka.cmo polkaeq.cmo libpolka_caml.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -custom -a -o $@ \
	polka.cmo polkaeq.cmo \
	-ccopt "-L$(POLKA_PREFIX)/lib" -cclib "-lpolka_caml"

polka.cmxa: polka.cmx polkaeq.cmx libpolka_caml.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ \
	polka.cmx polkaeq.cmx \
	-ccopt "-L$(POLKA_PREFIX)/lib" -cclib "-lpolka_caml"

#---------------------------------------
# IDL rules
#---------------------------------------

%_caml.c %.ml %.mli: %.idl sedscript_caml manager.idl
	mkdir -p tmp
	cp $*.idl tmp/$*.idl
	$(CAMLIDL) -no-include -nocpp -I $(MLAPRONIDL_PREFIX)/lib tmp/$*.idl
	cp tmp/$*_stubs.c $*_caml.c
	sed -f sedscript_caml tmp/$*.ml >$*.ml
	sed -f sedscript_caml tmp/$*.mli >$*.mli

manager.idl: ../mlapronidl/manager.idl
	cp $^ $@

#---------------------------------------
# C generic rules
#---------------------------------------

%.o: %.c
	$(CC) -I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml -I$(MLGMPIDL_PREFIX)/include -I$(MLAPRONIDL_PREFIX)/include  $(CFLAGS) -DNUMINT_MPZ -c -o $@ $<
%_debug.o: %.c
	$(CC) -I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml -I$(MLGMPIDL_PREFIX)/include -I$(MLAPRONIDL_PREFIX)/include  $(CFLAGS_DEBUG) -DNUMINT_MPZ -c -o $@ $<

#---------------------------------------
# ML generic rules
#---------------------------------------

%.cmi: %.mli
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<



#-----------------------------------
# DEPENDENCIES
#-----------------------------------
