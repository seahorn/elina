include ../Makefile.config

PREFIX = $(APRON_PREFIX)

SRCDIR = $(shell pwd)

#---------------------------------------
# Programs
#---------------------------------------

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I../num \
-I../itv \
-I../apron \
-I../mlgmpidl \
-I../mlapronidl \
-I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include \
-I$(CAMLIDL_PREFIX)/lib/ocaml -I$(CAML_PREFIX)/lib/ocaml

# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl

#---------------------------------------
# Files
#---------------------------------------

CCMODULES = box_internal box_representation box_constructor box_meetjoin box_assign box_resize box_otherops
CCSRC = box_config.h box.h $(CCMODULES:%=%.h) $(CCMODULES:%=%.c)

CCINC_TO_INSTALL = box.h
CCBIN_TO_INSTALL =
CCLIB_TO_INSTALL = libbox.a libbox_debug.a libboxMPQ.a libboxMPQ_debug.a libboxD.a libboxD_debug.a libboxMPFR.a
CAML_TO_INSTALL = box.idl box.ml box.mli box.cmi box.cmx box.cma box.cmxa box.a libbox_caml.a libbox_caml_debug.a

#---------------------------------------
# Rules
#---------------------------------------

# Possible goals:
# depend doc install
# and the following one

all: libbox.a libbox_debug.a allMPQ allD allMPFR

allMPQ: libboxMPQ.a libboxMPQ_debug.a
allD: libboxD.a libboxD_debug.a
allMPFR: libboxMPFR.a libboxMPFR_debug.a

ml: box.mli box.ml box.cmi box.cmx box.cma box.cmxa libbox_caml.a libbox_caml_debug.a

clean:
	/bin/rm -f *.[ao] *.cm[ioax] *.cmxa testg test*_caml* code2latex
	/bin/rm -f *.?.tex *.log *.aux *.bbl *.blg *.toc *.dvi *.ps *.pstex*
	/bin/rm -fr boxpolkarung boxpolkatopg tmp

mostlyclean: clean
	/bin/rm -fr manager.idl
	/bin/rm -f box.ml box.mli box_caml.c

install: $(CCINC_TO_INSTALL) $(CCLIB_TO_INSTALL)
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

dist: $(CCSRC) Makefile sedscript_caml box.texi box.idl box.ml box.mli box_caml.c COPYING README
	(cd ..; tar zcvf box.tgz $(^:%=box/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .c .h .a .o

#-----------------------------------
# C part
#-----------------------------------

libbox.a: libboxMPQ.a
	cp $^ $@
libbox_debug.a: libboxMPQ_debug.a
	cp $^ $@

libboxMPQ.a: $(CCMODULES:%=%MPQ.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

libboxMPQ_debug.a: $(CCMODULES:%=%MPQ_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%MPQ.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
%MPQ_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<

libboxRll.a: $(CCMODULES:%=%Rll.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@
libboxRll_debug.a: $(CCMODULES:%=%Rll_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%Rll.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%Rll_debug.o: %.c
	$(CC) $(CFLAGS_DEBIG) $(ICFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<

libboxD.a: $(CCMODULES:%=%D.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@
libboxD_debug.a: $(CCMODULES:%=%D_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%D.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_DOUBLE -c -o $@ $<
%D_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_DOUBLE -c -o $@ $<

libboxMPFR.a: $(CCMODULES:%=%MPFR.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@
libboxMPFR_debug.a: $(CCMODULES:%=%MPFR_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%MPFR.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPFR -c -o $@ $<
%MPFR_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPFR -c -o $@ $<

#---------------------------------------
# C rules
#---------------------------------------

libbox_caml.a: box_caml.o
	$(AR) rcs $@ $^
	$(RANLIB) $@
libbox_caml_debug.a: box_caml_debug.o
	$(AR) rcs $@ $^
	$(RANLIB) $@

#---------------------------------------
# ML rules
#---------------------------------------

box.cma: box.cmo libbox_caml.a libbox.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -a -o $@ box.cmo \
	-ccopt "-L$(APRON_PREFIX)/lib" -cclib "-lbox_caml -lbox"

box.cmxa: box.cmx libbox_caml.a libbox.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ box.cmx \
	-ccopt "-L$(APRON_PREFIX)/lib" -cclib "-lbox_caml -lbox"
	$(RANLIB) box.a


#---------------------------------------
# IDL rules
#---------------------------------------

manager.idl: ../mlapronidl/manager.idl
	ln -s $< $@

# generates box.ml, box.mli, box_caml.c from box.idl
rebuild: manager.idl box.idl 
	mkdir -p tmp
	cp box.idl tmp/box.idl
	$(CAMLIDL) -no-include -nocpp tmp/box.idl
	cp tmp/box_stubs.c box_caml.c
	$(SED) -f sedscript_caml tmp/box.ml >box.ml
	$(SED) -f sedscript_caml tmp/box.mli >box.mli

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

#-----------------------------------
# DEPENDENCIES
#-----------------------------------
