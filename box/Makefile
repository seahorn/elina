include ../Makefile.config

PREFIX = $(BOX_PREFIX)

SRCDIR = $(shell pwd)

#---------------------------------------
# Programs
#---------------------------------------

#---------------------------------------
# Flags
#---------------------------------------

# Use ICFLAGS to specify machine-independent compilation flags.
ICFLAGS = \
-I$(GMP_PREFIX)/include  \
-I$(MPFR_PREFIX)/include  \
-I../num \
-I../itv \
-I../apron \
-I../mlgmpidl \
-I../mlapronidl \
-I $(CAMLIDL_PREFIX)/lib/ocaml -I $(CAML_PREFIX)/lib/ocaml

# Caml
OCAMLINC = -I ../mlgmpidl -I ../mlapronidl

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

libboxmpq.a: $(CCMODULES:%=%mpq.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

libboxmpq_debug.a: $(CCMODULES:%=%mpq_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%mpq.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<
%mpq_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_MPQ -c -o $@ $<

libboxllr.a: $(CCMODULES:%=%llr.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@
libboxllr_debug.a: $(CCMODULES:%=%llr_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%llr.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<
%llr_debug.o: %.c
	$(CC) $(CFLAGS_DEBIG) $(ICFLAGS) -DNUM_LONGLONGRAT -c -o $@ $<

libboxdbl.a: $(CCMODULES:%=%dbl.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@
libboxdbl_debug.a: $(CCMODULES:%=%dbl_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

%dbl.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -DNUM_DOUBLE -c -o $@ $<
%dbl_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -DNUM_DOUBLE -c -o $@ $<

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

box.cma: box.cmo libbox_caml.a
	$(OCAMLC) $(OCAMLFLAGS) $(OCAMLINC) -a -o $@ box.cmo \
	-ccopt "-L$(BOX_PREFIX)/lib" -cclib "-lbox_caml"

box.cmxa: box.cmx libbox_caml.a
	$(OCAMLOPT) $(OCAMLOPTFLAGS) $(OCAMLINC) -a -o $@ box.cmx \
	-ccopt "-L$(BOX_PREFIX)/lib" -cclib "-lbox_caml"
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
