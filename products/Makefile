include ../Makefile.config

PREFIX = $(APRON_PREFIX)

SRCDIR = $(shell pwd)

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I../apron \
-I../num -I../itv \
-I../newpolka \
-I../ppl \
-I../mlgmpidl \
-I../mlapronidl \
-I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include \
-I$(CAMLIDL_PREFIX)/lib/ocaml -I$(CAML_PREFIX)/lib/ocaml

# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl -I ../newpolka

#---------------------------------------
# Files
#---------------------------------------

CCMODULES = ap_pkgrid
CCSRC = $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = ap_pkgrid.h
CCBIN_TO_INSTALL =
CCLIB_TO_INSTALL = \
libap_pkgrid.a libap_pkgrid_debug.a \
polkaGrid.mli polkaGrid.ml polkaGrid.cmi polkaGrid.cmx polkaGrid.cma polkaGrid.cmxa polkaGrid.a libpolkaGrid_caml.a libpolkaGrid_caml_debug.a

#---------------------------------------
# Rules
#---------------------------------------

all: libap_pkgrid.a libap_pkgrid_debug.a

ml: polkaGrid.mli polkaGrid.ml polkaGrid.cmi polkaGrid.cmx polkaGrid.cma polkaGrid.cmxa libpolkaGrid_caml.a libpolkaGrid_caml_debug.a

clean:
	/bin/rm -f *.[ao] *.cm[xiao] *.cmxa
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*

mostlyclean: clean
	/bin/rm -fr manager.idl
	/bin/rm -f polkaGrid.ml polkaGrid.mli polkaGrid_caml.c

install:
	$(INSTALLd) $(PREFIX)/include $(PREFIX)/lib
	$(INSTALL) $(CCINC_TO_INSTALL) $(PREFIX)/include
	for i in $(CCLIB_TO_INSTALL); do \
		if test -f $$i; then $(INSTALL) $$i $(PREFIX)/lib; fi; \
	done

distclean:
	for i in $(CCINC_TO_INSTALL); do /bin/rm -f $(PREFIX)/include/$$i; done
	for i in $(CCLIB_TO_INSTALL); do /bin/rm -f $(PREFIX)/lib/$$i; done
	for i in $(CCBIN_TO_INSTALL); do /bin/rm -f $(PREFIX)/bin/$$i; done
	/bin/rm -f Makefile.depend

dist: $(CCSRC) ap_pkgrid.texi sedscript_caml polkaGrid.idl polkaGrid.ml polkaGrid.mli polkaGrid_caml.c Makefile COPYING README
	(cd ..; tar zcvf products.tgz $(^:%=products/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

#-----------------------------------
# C part
#-----------------------------------

libap_pkgrid.a: ap_pkgrid.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libap_pkgrid_debug.a: ap_pkgrid_debug.o
	$(AR) rcs $@ $^
	$(RANLIB) $@

#---------------------------------------
# C rules
#---------------------------------------

libpolkaGrid_caml.a: polkaGrid_caml.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libpolkaGrid_caml_debug.a: polkaGrid_caml_debug.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
#---------------------------------------
# ML rules
#---------------------------------------

polkaGrid.cma: polkaGrid.cmo libpolkaGrid_caml.a libap_pkgrid.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -a -o $@ polkaGrid.cmo \
	-ccopt "-L$(APRON_PREFIX)/lib -L$(POLKA_PREFIX)/lib -L$(AP_PPL_PREFIX)/lib -L$(PPL_PREFIX)/lib" -cclib "-lpolkaGrid_caml -lap_pkgrid -lap_ppl -lppl -lpolka"

polkaGrid.cmxa: polkaGrid.cmx libpolkaGrid_caml.a libap_pkgrid.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ polkaGrid.cmx \
	-ccopt "-L$(APRON_PREFIX)/lib -L$(POLKA_PREFIX)/lib -L$(AP_PPL_PREFIX)/lib -L$(PPL_PREFIX)/lib" -cclib "-lpolkaGrid_caml -lap_pkgrid -lap_ppl -lppl -lpolka"
	$(RANLIB) polkaGrid.a

#---------------------------------------
# IDL rules
#---------------------------------------

manager.idl: ../mlapronidl/manager.idl
	ln -s $< $@

# generates polkaGrid.ml, polkaGrid.mli, polkaGrid_caml.c from polkaGrid.idl
rebuild: manager.idl polkaGrid.idl
	mkdir -p tmp
	cp polkaGrid.idl tmp/polkaGrid.idl
	$(CAMLIDL) -no-include -nocpp tmp/polkaGrid.idl
	cp tmp/polkaGrid_stubs.c polkaGrid_caml.c
	$(SED) -f sedscript_caml tmp/polkaGrid.ml >polkaGrid.ml
	$(SED) -f sedscript_caml tmp/polkaGrid.mli >polkaGrid.mli

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
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
%_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
