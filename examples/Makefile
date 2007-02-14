include ../Makefile.config

CC = gcc

ICFLAGS = \
-I$(GMP_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-Wall -Wstrict-prototypes -Wimplicit-function-declaration \
-std=c99
# -Winline -Wconversion

LCFLAGS = \
-L$(GMP_PREFIX)/lib \
-L$(MPFR_PREFIX)/lib \
-L$(APRON_PREFIX)/lib \
-L$(CAMLIDL_PREFIX)/lib/ocaml

CFLAGS = $(ICFLAGS) -O3 -DNDEBUG
CFLAGS_DEBUG = $(ICFLAGS) -O0 -g -UNDEBUG
LDFLAGS = $(ICFLAGS) $(LCFLAGS)

OCAMLC = ocamlc.opt
OCAMLOPT = ocamlopt.opt

OCAMLINC = \
-I $(MLGMPIDL_PREFIX)/lib \
-I $(MLAPRONIDL_PREFIX)/lib 

OCAMLFLAGS = $(OCAMLINC)

OCAMLLDFLAGS = \
-noautolink -ccopt "$(LCFLAGS)" \
bigarray.cma gmp.cma apron.cma \
box.cma polka.cma \
-cclib "-lpolka_caml -lpolkag_debug -lbox_caml -lboxmpq_debug -litvmpq_debug -lapron_caml_debug -lapron_debug -lgmp_caml -lgmp -lmpfr -lbigarray -lcamlidl"

OCAMLOPTLDFLAGS = \
-ccopt "$(LCFLAGS)" \
bigarray.cmxa gmp.cmxa apron.cmxa \
box.cmxa -cclib "-lboxmpq_debug -litvmpq_debug -lapron_debug" \
polka.cmxa -cclib "-lpolkag_debug"

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

%g: %.cmo boxpolkarung
	$(OCAMLC) $(OCAMLINC) $(OCAMLLDFLAGS) -g -use-runtime boxpolkarung -o $@ $<

boxpolkarung:
	$(OCAMLC) $(OCAMLINC) -verbose -make-runtime -o $@ $(OCAMLLDFLAGS)

boxpolkatopg:
	ocamlmktop $(OCAMLINC) -verbose -custom -o $@ $(OCAMLLDFLAGS)

%.cmo: %.ml
	$(OCAMLC) $(OCAMLFLAGS) -c -o $@ $<

%.cmx: %.ml
	$(OCAMLOPT) $(OCAMLFLAGS) -c -o $@ $<

#

clean:
	rm -f example1g boxpolkarung boxpolkatopg *.cm[ioxa] *.o mlexample1g mlexample2g mlexample3g *.opt

distclean: clean

dist: example1.c mlexample1.ml mlexample2.ml Makefile README
	(cd ..; tar zcvf examples.tgz $(^:%=examples/%))
