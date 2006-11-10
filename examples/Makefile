include ../Makefile.config

CC = gcc

ICFLAGS = \
-I$(GMP_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-I$(NUM_PREFIX)/include \
-I$(POLKA_PREFIX)/include \
-Wall -Wconversion -Winline -Wimplicit-function-declaration \
-std=c99

LCFLAGS = \
-L$(GMP_PREFIX)/lib \
-L$(APRON_PREFIX)/lib \
-L$(POLKA_PREFIX)/lib

CFLAGS = $(ICFLAGS) -O3 -DNDEBUG
CFLAGS_DEBUG = $(ICFLAGS) -O0 -g -UNDEBUG
LDFLAGS = $(ICFLAGS) $(LCFLAGS)

OCAMLC = ocamlc.opt
OCAMLOPT = ocamlopt.opt

OCAMLINC = \
-I $(MLGMPIDL_PREFIX)/lib \
-I $(MLAPRONIDL_PREFIX)/lib \
-I $(POLKA_PREFIX)/lib \
-I $(ITV_PREFIX)/lib

OCAMLFLAGS = $(OCAMLINC)

OCAMLLDFLAGS = \
bigarray.cma gmp.cma apron.cma itv.cma polka.cma \
-cclib "$(LCFLAGS) -L$(ITV_PREFIX)/lib -litvmpq_debug" \
-cclib "-L$(POLKA_PREFIX)/lib -lpolkag_debug" 

OCAMLOPTLDFLAGS = \
bigarray.cmxa gmp.cmxa apron.cmxa itv.cmxa polka.cmxa \
-cclib "$(LCFLAGS) -L$(ITV_PREFIX)/lib -litvmpq_debug" \
-cclib "-L$(POLKA_PREFIX)/lib -lpolkag_debug" 

all: C ml

# C examples

C: example1g

%g: %g.o
	$(CC) $(LDFLAGS) -o $@  $< -lpolkag_debug -lapron_debug -lgmp

%g.o: %.c
	$(CC) $(CFLAGS_DEBUG) -c -o $@ $<

# OCaml examples

ml: mlexample1g 

mlexample%g.opt: mlexample%.cmx
	$(OCAMLOPT) $(OCAMLINC) $(OCAMLOPTLDFLAGS) -o $@ $<

%g: %.cmo itvpolkarung
	$(OCAMLC) $(OCAMLINC) $(OCAMLLDFLAGS) -g -use-runtime itvpolkarung -o $@ $<

itvpolkarung:
	$(OCAMLC) $(OCAMLINC) -verbose -make-runtime -o $@ \
	bigarray.cma gmp.cma apron.cma itv.cma polka.cma \
	-cclib "$(LCFLAGS) -L$(ITV_PREFIX)/lib -litvmpq_debug" \
	-cclib "-L$(POLKA_PREFIX)/lib -lpolkag_debug" 

itvpolkatopg:
	ocamlmktop $(OCAMLINC) -verbose -custom -o $@ \
	bigarray.cma gmp.cma apron.cma itv.cma polka.cma \
	-cclib "$(LCFLAGS) -L$(ITV_PREFIX)/lib -litvmpq_debug" \
	-cclib "-L$(POLKA_PREFIX)/lib -lpolkag_debug" 

%.cmo: %.ml
	$(OCAMLC) $(OCAMLFLAGS) -c -o $@ $<

%.cmx: %.ml
	$(OCAMLOPT) $(OCAMLFLAGS) -c -o $@ $<

#

clean:
	rm -f example1g itvpolkarung itvpolkatopg *.cm[ioxa] *.o mlexample1g mlexample2g

distclean: clean

dist: example1.c mlexample1.ml mlexample2.ml Makefile README
	(cd ..; tar zcvf examples.tgz $(^:%=examples/%))
