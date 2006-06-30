
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
CPP = gcc -E

# Library creation
AR = ar
SHARED = gcc -shared

# LATEX and others
LATEX = latex
TEXI2DVI = texi2dvi
MAKEINFO = makeinfo
DVIPS = dvips

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

CCINC_TO_INSTALL = $(CCINC)
CCBIN_TO_INSTALL =
CCLIB_TO_INSTALL =

CAML_TO_INSTALL = 


#---------------------------------------
# Rules
#---------------------------------------

# Possible goals:
# depend doc install
# and the following one

all: liboct test ml

liboct: liboctZi.a liboctZl.a liboctZg.a liboctQi.a liboctQl.a liboctQg.a liboctFd.a liboctFl.a

test: testZi testZl testZg testQi testQl testQg testFd testFl

ml: mlZi mlZl mlZg mlQi mlQl mlQg mlFd mlFl


clean:
	/bin/rm -f *.[ao] *.so test*
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*
	/bin/rm -fr *.cm[ioax] *.cmxa
	/bin/rm -fr oct_caml.c oct.ml oct.mli octtop* octrun* manager.idl tmp
	/bin/rm -fr *~ \#*\#

install: $(CCINC_TO_INSTALL) $(CCLIB_TO_INSTALL)
	$(INSTALLd) $(INCDIR)/oct $(LIBDIR)
	$(INSTALL) $(CCINC_TO_INSTALL) $(INCDIR)/oct
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
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(INCDIR)/oct/$$i; done
	for i in $(CCLIB_TO_INSTALL); do /bin/rm -f $(LIBDIR)/$$i; done
	for i in $(CCBIN_TO_INSTALL); do /bin/rm -f $(BINDIR)/$$i; done
	for i in $(CAML_TO_INSTALL); do /bin/rm -f $(LIBDIR)/$$i; done
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

test%: liboct%.a oct_test%.o
	$(CC) -o $@ oct_test$*.o \
	-L$(GMP_PREFIX)/lib -lgmp \
	-L$(POLKA_PREFIX)/lib \
	-L$(APRON_PREFIX)/lib -lpolkag_debug -lapron \
	-L. -loct$* -lm

%Zi.o: %.c $(CCINC)
	$(CC) $(CFLAGS) -DNUM_LONGINT -c -o $@ $<
%Zl.o: %.c $(CCINC)
	$(CC) $(CFLAGS) -DNUM_LONGLONGINT -c -o $@ $<
%Zg.o: %.c $(CCINC)
	$(CC) $(CFLAGS) -DNUM_MPZ -I$(GMP_PREFIX)/include -c -o $@ $<

%Qi.o: %.c $(CCINC)
	$(CC) $(CFLAGS) -DNUM_LONGRAT -c -o $@ $<
%Ql.o: %.c $(CCINC)
	$(CC) $(CFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%Qg.o: %.c $(CCINC)
	$(CC) $(CFLAGS) -DNUM_MPQ -I$(GMP_PREFIX)/include -c -o $@ $<

%Fd.o: %.c $(CCINC)
	$(CC) $(CFLAGS) -DNUM_DOUBLE -c -o $@ $<
%Fl.o: %.c $(CCINC)
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
	apron.cmo oct.cmo \
        -L$(MLGMPIDL_PREFIX)/lib -lgmp_caml \
	-L$(GMP_PREFIX)/lib -lgmp \
	-L$(MLCOMMONINTERFACIDL_PREFIX)/lib -lapron_caml \
	-L$(APRON_PREFIX)/lib -lapron \
	-L. -loct$*_caml -loct$* \
	-L$(CAMLIDL_PREFIX)/lib/ocaml -lcamlidl

oct%.cmxa: oct.cmx liboct%_caml.a liboct%.a
	$(OCAMLMKLIB) -o oct$* -linkall -verbose $(OCAMLINC) \
	apron.cmx oct.cmx \
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

%_caml.c %.ml %.mli: %.idl sedscript_caml manager.idl
	mkdir -p tmp
	cp $*.idl tmp/$*.idl
	$(CAMLIDL) -no-include -nocpp -I $(MLAPRONIDL_PREFIX)/lib tmp/$*.idl
	cp tmp/$*_stubs.c $*_caml.c
	sed -f sedscript_caml tmp/$*.ml | grep --extended-regexp '^(.)+$$' >$*.ml
	sed -f sedscript_caml tmp/$*.mli | grep --extended-regexp '^(.)+$$' >$*.mli

manager.idl: $(MLAPRONIDL_PREFIX)/lib/manager.idl
	ln -fs $^ $@

.PRECIOUS: %_caml.c %.ml %.mli liboct%_caml.a liboct%_caml.so

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

