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

OCAMLC = ocamlc
OCAMLOPT = ocamlopt

OCAMLINC = \
-I $(MLGMPIDL_PREFIX)/lib \
-I $(MLAPRONIDL_PREFIX)/lib \
-I $(POLKA_PREFIX)/lib \
-I $(ITV_PREFIX)/lib

OCAMLFLAGS = $(OCAMLINC)

all: C

# C examples

C: example1g

%g: %g.o
	$(CC) $(LDFLAGS) -o $@  $< -lpolkag_debug -lapron_debug -lgmp

%g.o: %.c
	$(CC) $(CFLAGS_DEBUG) -c -o $@ $<

# OCaml examples

ml: mlexample1g 

mlexample1g.opt: mlexample1.cmx
	$(OCAMLOPT) $(OCAMLINC) -o $@ \
	bigarray.cmxa gmp.cmxa apron.cmx itv.cmx polka.cmx $< \
	-cclib "$(LCFLAGS) -L$(ITV_PREFIX)/lib" \
	-cclib "-lgmp_caml_debug -lgmp" \
	-cclib " -lapron_caml_debug -lapron_debug" \
	-cclib " -litv_caml_debug -litvmpq_debug" \
	-cclib " -lpolka_caml_debug -lpolkag_debug" \
	-cclib "-L$(CAMLIDL_PREFIX)/lib/ocaml -lcamlidl"

mlexample1g: mlexample1.cmo itvpolkarung
	$(OCAMLC) $(OCAMLINC) -g -use-runtime itvpolkarung -o $@ \
	bigarray.cma gmp.cma apron.cmo itv.cmo polka.cmo $<

itvpolkarung:
	$(OCAMLC) $(OCAMLINC) -verbose -make-runtime -o $@ \
	bigarray.cma gmp.cma apron.cmo itv.cmo polka.cmo \
	-cclib "$(LCFLAGS) -L$(ITV_PREFIX)/lib" \
	-cclib "-lgmp_caml_debug -lgmp" \
	-cclib " -lapron_caml_debug -lapron_debug" \
	-cclib " -litv_caml_debug -litvmpq_debug" \
	-cclib " -lpolka_caml_debug -lpolkag_debug" \
	-cclib "-L$(CAMLIDL_PREFIX)/lib/ocaml -lcamlidl"

%.cmo: %.ml
	$(OCAMLC) $(OCAMLFLAGS) -c -o $@ $<

%.cmx: %.ml
	$(OCAMLOPT) $(OCAMLFLAGS) -c -o $@ $<

#

clean:
	rm -f example1g itvpolkarung *.cm[ioxa] *.o mlexample1g

dist: example1.c mlexample1.ml Makefile README
	(cd ..; tar zcvf $(HOME)/examples.tgz $(^:%=examples/%))





