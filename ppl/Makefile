# Makefile
#
# APRON Library / PPL library wrapper
#
# Copyright (C) Antoine Mine' 2006

# This file is part of the APRON Library, released under LGPL license.
# Please read the COPYING file packaged in the distribution.

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

# C++ compiler
CXX = g++

# Library creation
AR = ar
SHARED = gcc -shared

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
-Wall -W -Wundef -Wcast-align \
-Wdisabled-optimization \
-Wno-unused
# -Winline
CCFLAGS = $(OPTFLAGS) $(ICFLAGS) -DNDEBUG
CXXFLAGS = $(OPTFLAGS) $(ICFLAGS) -DNDEBUG

# Caml
OCAMLINC = -I $(MLGMPIDL_PREFIX)/lib -I $(MLAPRONIDL_PREFIX)/lib
OCAMLFLAGS = -g
OCAMLOPTFLAGS = -inline 20


#---------------------------------------
# Files
#---------------------------------------

CXXSOURCES = ppl_user.cc ppl_poly.cc
CSOURCES = ppl_test.c
CCINC = ppl_user.h apron_ppl.h

# trigers a whole recompilation
DEPS = $(APRON_PREFIX)/include/ap_abstract0.h

#---------------------------------------
# Rules
#---------------------------------------

all: libapronppl.a ppl_test


clean:
	/bin/rm -f *.[ao] *.so ppl_test
	/bin/rm -fr *~ \#*\#

install:
	$(INSTALLd) $(INCDIR) $(LIBDIR)
	$(INSTALL) apron_ppl.h $(INCDIR)
	$(INSTALL) libapronppl.a $(LIBDIR)
	$(INSTALL) ppl_test $(BINDIR)

uninstall:
	/bin/rm -f $(INCDIR)/apron_ppl.h
	/bin/rm -f $(LIBDIR)/libapronppl.a
	/bin/rm -f $(BINDIR)/ppl_test

distclean: clean uninstall

dist: Makefile COPYING README $(CXXSOURCES) $(CSOURCES) $(CCINC) 
	(cd ..; tar zcvf ppl.tgz $(^:%=ppl/%))

#---------------------------------------
# IMPLICIT RULES AND DEPENDENCIES
#---------------------------------------

.SUFFIXES: .tex .cc .h .a .o .so



#-----------------------------------
# C / C++ part
#-----------------------------------

libapronppl.a: $(subst .cc,.o,$(CXXSOURCES))
	$(AR) rcs $@ $^

ppl_test: libapronppl.a ppl_test.o
	$(CXX) $(CXXFLAGS) -o $@ ppl_test.o \
	-L. -lapronppl \
	-L$(POLKA_PREFIX)/lib -lpolkag_debug \
	-L$(APRON_PREFIX)/lib -lapron \
	`ppl-config --ldflags` \
	-L$(GMP_PREFIX)/lib -lmpfr -lgmp -lm

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CCFLAGS) -c -o $@ $<


#-----------------------------------
# Caml part
#-----------------------------------
