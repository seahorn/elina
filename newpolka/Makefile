include ../Makefile.config

PREFIX = $(POLKA_PREFIX)

SRCDIR = $(shell pwd)

#---------------------------------------
# Programs
#---------------------------------------

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
-I $(CAMLIDL_PREFIX)/lib/ocaml -I $(CAML_PREFIX)/lib/ocaml

# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl

#---------------------------------------
# Files
#---------------------------------------

CCMODULES = \
mf_qsort \
pk_user pk_internal pk_bit pk_satmat pk_vector pk_matrix pk_cherni \
pk_representation pk_constructor pk_test pk_extract \
pk_meetjoin pk_assign pk_project pk_resize pk_expandfold \
pk_widening pk_closure \
pkeq

CCSRC = pk_config.h pk_int.h pk.h $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = pk.h pkeq.h
CCBIN_TO_INSTALL =
CCLIB_TO_INSTALL = libpolkag.a libpolkag_debug.a libpolka_caml.a libpolka_caml_debug.a

CAML_TO_INSTALL = polka.idl \
polka.cma polka.cmxa polka.a \
polka.cmi polka.cmx polka.mli polka.ml

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
#	for i in $(CCBIN_TO_INSTALL); do \
#		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/bin; fi; \
#	done
	for i in $(CAML_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done

distclean:
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(PREFIX)/include/$$i; done
	for i in $(CCLIB_TO_INSTALL); do /bin/rm -f $(PREFIX)/lib/$$i; done
#	for i in $(CCBIN_TO_INSTALL); do /bin/rm -f $(PREFIX)/bin/$$i; done
	for i in $(CAML_TO_INSTALL); do /bin/rm -f $(PREFIX)/lib/$$i; done
	/bin/rm -f Makefile.depend

dist: $(CCSRC) Makefile sedscript_caml newpolka.texi polka.idl polka.ml polka.mli polka_caml.c COPYING README
	(cd ..; tar zcvf newpolka.tgz $(^:%=newpolka/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

#-----------------------------------
# C part
#-----------------------------------

libpolkag.a: $(CCMODULES:%=%g.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

libpolkag_debug.a: $(CCMODULES:%=%g_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%i.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUMINT_LONGINT -c -o $@ $<
%l.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUMINT_LONGLONGINT -c -o $@ $<
%g.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUMINT_MPZ -I$(GMP_PREFIX)/include -c -o $@ $<

%g_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUMINT_MPZ -I$(GMP_PREFIX)/include -c -o $@ $<

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

libpolka_caml.a: polka_caml.o 
	$(AR) rcs $@ $^
	$(RANLIB) $@

libpolka_caml_debug.a: polka_caml_debug.o 
	$(AR) rcs $@ $^
	$(RANLIB) $@

#---------------------------------------
# ML rules
#---------------------------------------

polka.cma: polka.cmo libpolka_caml.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -custom -a -o $@ \
	polka.cmo \
	-ccopt "-L$(POLKA_PREFIX)/lib" -cclib "-lpolka_caml"

polka.cmxa: polka.cmx libpolka_caml.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ \
	polka.cmx \
	-ccopt "-L$(POLKA_PREFIX)/lib" -cclib "-lpolka_caml"
	$(RANLIB) polka.a

#---------------------------------------
# IDL rules
#---------------------------------------

manager.idl: ../mlapronidl/manager.idl
	ln -s $< $@

# generates X.ml, X.mli, X_caml.c for X = polka
rebuild: manager.idl polka.idl 
	mkdir -p tmp
	for i in polka; do \
		echo "module $$i"; \
		cp $${i}.idl tmp/$${i}.idl; \
		$(CAMLIDL) -no-include -nocpp tmp/$${i}.idl; \
		cp tmp/$${i}_stubs.c $${i}_caml.c; \
		$(SED) -f sedscript_caml tmp/$${i}.ml >$${i}.ml; \
		$(SED) -f sedscript_caml tmp/$${i}.mli >$${i}.mli; \
	done

#---------------------------------------
# C generic rules
#---------------------------------------

%.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUMINT_MPZ -c -o $@ $<
%_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUMINT_MPZ -c -o $@ $<

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
