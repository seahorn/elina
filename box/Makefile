
include ../Makefile.config

PREFIX = $(BOX_PREFIX)

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

# For the OCAML interface
OCAMLDEP = $(CAML_PREFIX)/bin/ocamldep
OCAMLC = $(CAML_PREFIX)/bin/ocamlc.opt
OCAMLOPT = $(CAML_PREFIX)/bin/ocamlopt.opt
OCAMLLEX = $(CAML_PREFIX)/bin/ocamllex.opt
OCAMLYACC = $(CAML_PREFIX)/bin/ocamlyacc
OCAMLMKTOP = $(CAML_PREFIX)/bin/ocamlmktop
CAMLIDL = $(CAMLIDL_PREFIX)/bin/camlidl


#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(GMP_PREFIX)/include  \
-I../num \
-I../itv \
-I../apron \
-I../mlgmpidl \
-I../mlapronidl \
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

CCMODULES = box_internal box_representation box_constructor box_meetjoin box_assign box_resize box_otherops 
CCSRC = box_config.h box.h $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = box.h
CCBIN_TO_INSTALL = 
CCLIB_TO_INSTALL = libboxmpq.a libboxmpq_debug.a
CAML_TO_INSTALL = box.idl box.ml box.mli box.cmi box.cmx box.cma box.cmxa box.a libbox_caml.a libbox_caml_debug.a

#---------------------------------------
# Rules
#---------------------------------------

# Possible goals:
# depend doc install
# and the following one

all: allmpq

allmpq: libboxmpq.a libboxmpq_debug.a

ml: box.mli box.ml box.cmi box.cmx box.cma box.cmxa libbox_caml.a libbox_caml_debug.a

clean:
	/bin/rm -f *.[ao] *.cm[ioax] *.cmxa testg test*_caml* code2latex
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*
	/bin/rm -fr boxpolkarung boxpolkatopg tmp

mostlyclean: clean
	/bin/rm -fr manager.idl
	/bin/rm -f box.ml box.mli

install: $(CCINC_TO_INSTALL) $(CCLIB_TO_INSTALL)
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

dist: $(CCSRC) Makefile sedscript_caml box.texi box.idl box.ml box.mli box_caml.c COPYING README
	(cd ..; tar zcvf box.tgz $(^:%=box/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

#-----------------------------------
# C part
#-----------------------------------

libboxmpq.a: $(CCMODULES:%=%mpq.o)
	ar rcs $@ $^ ../itv/libitvmpq.a
libboxmpq_debug.a: $(CCMODULES:%=%mpq_debug.o)
	ar rcs $@ $^

%mpq.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) -DNUM_MPQ -c -o $@ $<
%mpq_debug.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) $(CFLAGS_DEBUG) -DNUM_MPQ -c -o $@ $<

libboxllr.a: $(CCMODULES:%=%llr.o)
	ar rcs $@ $^
libboxllr_debug.a: $(CCMODULES:%=%llr_debug.o)
	ar rcs $@ $^

%llr.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%llr_debug.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) $(CFLAGS_DEBUG) -DNUM_LONGLONGRAT -c -o $@ $<

libboxdbl.a: $(CCMODULES:%=%dbl.o)
	ar rcs $@ $^
libboxdbl_debug.a: $(CCMODULES:%=%dbl_debug.o)
	ar rcs $@ $^

%dbl.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) -DNUM_DOUBLE -c -o $@ $<
%dbl_debug.o: %.c
	$(CC) $(ICFLAGS) $(XCFLAGS) $(CFLAGS_DEBUG) -DNUM_DOUBLE -c -o $@ $<

#---------------------------------------
# C rules
#---------------------------------------

libpolka_caml.a: polka_caml.o
	ar rcs $@ $^
libpolka_caml_debug.a: polka_caml_debug.o
	ar rcs $@ $^

libbox_caml.a: box_caml.o
	ar rcs $@ $^
libbox_caml_debug.a: box_caml_debug.o
	ar rcs $@ $^

#---------------------------------------
# ML rules
#---------------------------------------

box.cma: box.cmo libbox_caml.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -a -o $@ box.cmo \
	-ccopt "-L$(BOX_PREFIX)/lib" -cclib "-lbox_caml"

box.cmxa: box.cmx libbox_caml.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ box.cmx \
	-ccopt "-L$(BOX_PREFIX)/lib" -cclib "-lbox_caml"


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
# ML generic rules
#---------------------------------------

%.cmi: %.mli
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmo: %.ml %.cmi
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -c $<

%.cmx: %.ml %.cmi
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -c $<

#---------------------------------------
# C generic rules
#---------------------------------------

%.o: %.c
	$(CC) -I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml -I$(MLGMPIDL_PREFIX)/include -I$(MLAPRONIDL_PREFIX)/include $(ICFLAGS) $(XCFLAGS) -DNUM_MPQ -c -o $@ $<
%_debug.o: %.c
	$(CC) -I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml -I$(MLGMPIDL_PREFIX)/include -I$(MLAPRONIDL_PREFIX)/include $(ICFLAGS) $(XCFLAGS) $(CFLAGS_DEBUG) -DNUM_MPQ -c -o $@ $<

#-----------------------------------
# DEPENDENCIES
#-----------------------------------
