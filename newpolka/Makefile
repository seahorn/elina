
include ../Makefile.config

PREFIX = $(POLKA_PREFIX)

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
CPP = gcc -E

# LATEX and others
LATEX = latex
TEXI2DVI = texi2dvi
MAKEINFO = makeinfo
DVIPS = dvips

OCAMLC = ocamlc.opt 
OCAMLOPT = ocamlopt.opt
OCAMLMKTOP = ocamlmktop
CAMLIDL = camlidl

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(GMP_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-I$(MLAPRONIDL_PREFIX)/include \
-I$(NUM_PREFIX)/include \
-I$(HOME)/dev/apron \
-Wall -Wconversion -Winline -Wimplicit-function-declaration \
-std=c99

CFLAGS =  $(ICFLAGS) -O3 -DNDEBUG
# For debugging purpose
CFLAGS_DEBUG = $(ICFLAGS) -O0 -g -UNDEBUG
CFLAGS_PROF = -g -pg

# Caml
OCAMLINC = -I $(MLGMPIDL_PREFIX)/lib -I $(MLAPRONIDL_PREFIX)/lib
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
pk_meetjoin pk_assign pk_project pk_resize pk_expandfold pk_widening pk_closure
CCSRC = pk_config.h pk_int.h pk.h $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = pk.h
CCBIN_TO_INSTALL = polkarung polkatopg
CCLIB_TO_INSTALL = libpolkag.a libpolkag_debug.a libpolka_caml.a libpolka_caml_debug.a 

CAML_TO_INSTALL = polka.mli polka.cmi polka.cmo polka.cmx polka.o

#---------------------------------------
# Rules
#---------------------------------------

# Possible goals:
# depend doc install
# and the following one

all: allg

ml: polka.mli polka.cmi polka.cmo polka.cmx libpolka_caml.a libpolka_caml_debug.a

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
	/bin/rm -fr *.cm[ioax] polka_caml.* polka.ml polka.mli
	/bin/rm -f manager.idl
	/bin/rm -fr tmp

install: $(CCINC_TO_INSTALL) $(CCLIB_TO_INSTALL)
	$(INSTALLd) $(INCDIR) $(LIBDIR)
	$(INSTALL) $(CCINC_TO_INSTALL) $(INCDIR)
	for i in $(CCLIB_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(LIBDIR); fi; \
	done
	for i in $(CCBIN_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(BINDIR); fi; \
	done
	for i in $(CAML_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(LIBDIR); fi; \
	done

distclean: clean
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(INCDIR)/$$i; done
	for i in $(CCLIB_TO_INSTALL); do /bin/rm -f $(LIBDIR)/$$i; done
	for i in $(CCBIN_TO_INSTALL); do /bin/rm -f $(BINDIR)/$$i; done
	for i in $(CAML_TO_INSTALL); do /bin/rm -f $(LIBDIR)/$$i; done
	/bin/rm -f Makefile.depend

dist: $(CCSRC) Makefile sedscript_caml newpolka.texi polka.ml polka.mli polka_caml.c COPYING README
	(cd ..; tar zcvf $(HOME)/newpolka.tgz $(^:%=newpolka/%))

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

polkatopg: polka.cmo libpolka_caml_debug.a libpolkag_debug.a
	$(OCAMLMKTOP) -verbose $(OCAMLINC) $(OCAMLFLAGS) -o $@ -custom bigarray.cma gmp.cma apron.cmo polka.cmo \
	-ccopt -L$(MLGMPIDL_PREFIX)/lib -cclib -lgmp_caml_debug \
	-ccopt -L$(GMP_PREFIX)/lib -cclib -lgmp \
	-ccopt -L$(MLCOMMONINTERFACIDL_PREFIX)/lib -cclib -lapron_caml_debug \
	-ccopt -L$(APRON_PREFIX)/lib -cclib -lapron_debug \
	-ccopt -L. -cclib -lpolka_caml_debug -cclib -lpolkag_debug \
	-ccopt -L$(CAMLIDL_PREFIX)/lib/ocaml -cclib -lcamlidl

polkarung: polka.cmo libpolka_caml_debug.a libpolkag_debug.a
	$(OCAMLC) -verbose $(OCAMLINC) $(OCAMLFLAGS) -o $@ -make-runtime bigarray.cma gmp.cma apron.cmo polka.cmo \
	-ccopt -L$(MLGMPIDL_PREFIX)/lib -cclib -lgmp_caml_debug \
	-ccopt -L$(GMP_PREFIX)/lib -cclib -lgmp \
	-ccopt -L$(MLCOMMONINTERFACIDL_PREFIX)/lib -cclib -lapron_caml_debug \
	-ccopt -L$(APRON_PREFIX)/lib -cclib -lapron_debug \
	-ccopt -L. -cclib -lpolka_caml_debug -cclib -lpolkag_debug \
	-ccopt -L$(CAMLIDL_PREFIX)/lib/ocaml -cclib -lcamlidl

testg_caml: test.cmo polkarung 
	$(OCAMLC) -g $(OCAMLINC) -I $(MLAPRONIDL_PREFIX)/lib -o $@ -use-runtime polkarung bigarray.cma gmp.cma apron.cmo polka.cmo test.cmo 

test.cmo: test.ml
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -I $(CAMLLIB_PREFIX)/lib -c $<


libpolka_caml.a: polka_caml.o
	ar rcs $@ $^
libpolka_caml_debug.a: polka_caml_debug.o
	ar rcs $@ $^

%.o: %.c
	$(CC) -I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml -I$(MLGMPIDL_PREFIX)/include -I$(MLAPRONIDL_PREFIX)/include  $(CFLAGS) -DNUMINT_MPZ -c -o $@ $<
%_debug.o: %.c
	$(CC) -I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml -I$(MLGMPIDL_PREFIX)/include -I$(MLAPRONIDL_PREFIX)/include  $(CFLAGS_DEBUG) -DNUMINT_MPZ -c -o $@ $<

%_caml.c %.ml %.mli: %.idl sedscript_caml manager.idl
	mkdir -p tmp
	cp $*.idl tmp/$*.idl
	$(CAMLIDL) -no-include -nocpp -I $(MLAPRONIDL_PREFIX)/lib tmp/$*.idl
	cp tmp/$*_stubs.c $*_caml.c
	sed -f sedscript_caml tmp/$*.ml | grep --extended-regexp '^(.)+$$' >$*.ml
	sed -f sedscript_caml tmp/$*.mli | grep --extended-regexp '^(.)+$$' >$*.mli

manager.idl: $(MLAPRONIDL_PREFIX)/lib/manager.idl
	ln -fs $^ $@

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
