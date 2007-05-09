
include ../Makefile.config

#---------------------------------------
# Directories
#---------------------------------------

SRCDIR = $(shell pwd)
#
PREFIX = $(APRON_PREFIX)
#
# C include and lib directories
INCDIR = $(PREFIX)/include
LIBDIR = $(PREFIX)/lib
BINDIR = $(PREFIX)/bin
#

#---------------------------------------
# C part
#---------------------------------------
ICFLAGS = -I$(GMP_PREFIX)/include -I$(MPFR_PREFIX)/include

#---------------------------------------
# Files
#---------------------------------------

H_FILES = \
ap_config.h \
ap_scalar.h ap_interval.h ap_coeff.h ap_dimension.h \
ap_linexpr0.h ap_lincons0.h ap_generator0.h ap_expr0.h \
ap_manager.h ap_abstract0.h ap_generic.h \
ap_var.h ap_environment.h \
ap_linexpr1.h ap_lincons1.h ap_generator1.h ap_expr1.h \
ap_abstract1.h \
ap_global0.h ap_global1.h \
ap_linearize.h

C_FILES = \
ap_scalar.c ap_interval.c ap_coeff.c ap_dimension.c \
ap_linexpr0.c ap_lincons0.c ap_generator0.c \
ap_manager.c ap_abstract0.c ap_generic.c \
ap_var.c ap_environment.c \
ap_linexpr1.c ap_lincons1.c ap_generator1.c \
ap_abstract1.c \
ap_linearize.c

#---------------------------------------
# Rules
#---------------------------------------

all: libapron.a libapron_debug.a

#---------------------------------------
# Misc rules
#---------------------------------------

tar: $(H_FILES) $(C_FILES) apron.texi Makefile COPYING README
	(cd ..; tar zcvf apron.tgz $(^:%=apron/%))

dist: $(H_FILES) $(C_FILES) apron.texi Makefile COPYING README apron.info apron.dvi html
	(cd ..; tar zcvf apron.tgz $(^:%=apron/%))

clean:
	/bin/rm -f *.aux *.bbl *.blg *.dvi *.log *.toc *.ps apron.cps apron.fns apron.info apron.fn apron.ky apron.pg apron.cp apron.tp apron.vr apron.kys apron.pgs apron.tps apron.vrs newpolka.texi box.texi apron_ppl.texi
	/bin/rm -f *.o *.a *.cmi *.cmo *.cmx *.cmxa *.cma
	/bin/rm -fr html

install: libapron.a libapron_debug.a
	mkdir -p $(APRON_PREFIX)/include
	cp $(H_FILES) $(APRON_PREFIX)/include
	mkdir -p $(APRON_PREFIX)/lib
	cp libapron.a libapron_debug.a $(APRON_PREFIX)/lib

distclean:
	/bin/rm -f $(H_FILES:%=$(APRON_PREFIX)/include/%)
	/bin/rm -f $(APRON_PREFIX)/lib/libapron.a
	/bin/rm -f $(APRON_PREFIX)/lib/libapron_debug.a

#---------------------------------------
# Latex rules
#---------------------------------------

.PHONY : html

apron_ppl.texi: ../ppl/apron_ppl.texi
	ln -s $< $@
newpolka.texi: ../newpolka/newpolka.texi
	ln -s $< $@
box.texi: ../box/box.texi
	ln -s $< $@

apron.pdf: apron.dvi
	$(DVIPDF) apron.dvi $@

apron.dvi: apron.texi rationale.texi apron_ppl.texi newpolka.texi box.texi
	$(TEXI2DVI) -o $@ $<

apron.info: apron.texi apron_ppl.texi newpolka.texi box.texi
	$(MAKEINFO) -o $@ $<


html: apron.texi apron_ppl.texi newpolka.texi box.texi
	$(TEXI2HTML) -split=section -nonumber -menu -subdir=html $<
	cp -f ../octagons/oct_doc.html html

#---------------------------------------
# C rules
#---------------------------------------

libapron.a: $(C_FILES:%.c=%.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@
libapron_debug.a: $(C_FILES:%.c=%_debug.o)
	$(AR) rcs $@ $^
	$(RANLIB) $@

.PHONY : dep

dep: $(H_FILES)
	for i in $(C_FILES); do			\
		cpp $(ICFLAGS) -MM $$i;			\
	done

#--------------------------------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#--------------------------------------------------------------

.SUFFIXES: .c .h .o

#---------------------------------------
# C generic rules
#---------------------------------------

%.o: %.c %.h 
	$(CC) $(CFLAGS) $(ICFLAGS) -c $<

%_debug.o: %.c %.h
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -c -o $@ $<

#---------------------------------------
# dependencies (generated with make dep)
#---------------------------------------

ap_scalar.o: ap_scalar.c ap_scalar.h ap_config.h
ap_interval.o: ap_interval.c ap_interval.h \
  ap_config.h \
  ap_scalar.h /scratch/home/bjeannet/pkg/mpfr/linux386/include/mpfr.h
ap_coeff.o: ap_coeff.c ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h
ap_dimension.o: ap_dimension.c ap_dimension.h
ap_linexpr0.o: ap_linexpr0.c ap_linexpr0.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_dimension.h
ap_lincons0.o: ap_lincons0.c ap_lincons0.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_linexpr0.h ap_dimension.h
ap_generator0.o: ap_generator0.c ap_generator0.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_linexpr0.h ap_dimension.h
ap_manager.o: ap_manager.c ap_manager.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h
ap_abstract0.o: ap_abstract0.c ap_abstract0.h ap_manager.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_expr0.h ap_linexpr0.h ap_dimension.h ap_lincons0.h \
  ap_generator0.h
ap_generic.o: ap_generic.c ap_generic.h ap_manager.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_expr0.h ap_linexpr0.h ap_dimension.h ap_lincons0.h \
  ap_generator0.h ap_abstract0.h
ap_var.o: ap_var.c ap_manager.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_var.h
ap_environment.o: ap_environment.c ap_manager.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_environment.h ap_dimension.h ap_var.h
ap_linexpr1.o: ap_linexpr1.c ap_linexpr1.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_environment.h ap_dimension.h ap_var.h ap_linexpr0.h
ap_lincons1.o: ap_lincons1.c ap_lincons1.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_environment.h ap_dimension.h ap_var.h ap_lincons0.h \
  ap_linexpr0.h ap_linexpr1.h
ap_generator1.o: ap_generator1.c ap_generator1.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_environment.h ap_dimension.h ap_var.h ap_generator0.h \
  ap_linexpr0.h ap_linexpr1.h
ap_abstract1.o: ap_abstract1.c ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_manager.h ap_abstract0.h ap_expr0.h ap_linexpr0.h \
  ap_dimension.h ap_lincons0.h ap_generator0.h ap_expr1.h ap_linexpr1.h \
  ap_environment.h ap_var.h ap_lincons1.h ap_generator1.h ap_abstract1.h
ap_linearize.o: ap_linearize.c ap_linearize.h ap_manager.h ap_coeff.h \
  ap_config.h \
  ap_scalar.h \
  ap_interval.h ap_expr0.h ap_linexpr0.h ap_dimension.h ap_lincons0.h \
  ap_generator0.h ap_abstract0.h
