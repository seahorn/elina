include ../Makefile.config

ICFLAGS = \
-I$(GMP_PREFIX)/include \
-I$(MPFR_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-I$(PPL_PREFIX)/include 

LCFLAGS = \
-L$(GMP_PREFIX)/lib \
-L$(MPFR_PREFIX)/lib \
-L$(APRON_PREFIX)/lib \
-L$(PPL_PREFIX)/lib \
-L$(CAMLIDL_PREFIX)/lib/ocaml

OCAMLINC = \
-I $(MLGMPIDL_PREFIX)/lib \
-I $(APRON_PREFIX)/lib 

OCAMLOPTLDFLAGS = \
-noautolink -verbose -ccopt "$(LCFLAGS)" \
bigarray.cmxa gmp.cmxa apron.cmxa box.cmxa polka.cmxa ppl.cmxa \
-cclib "-lpolkaGrid_caml -lap_pkgrid_debug -lap_ppl_caml -lap_ppl_debug -lppl -lpolka_caml -lpolka_debug -loct_caml -loctMPQ -lbox_caml -lbox_debug -litv_debug -lapron_caml_debug -lapron_debug -lgmp_caml -lmpfr -lgmpxx -lgmp -lbigarray -lcamlidl"

all: C

# C examples

C: ctest1

%: %.o
	$(CXX) $(ICFLAGS) $(LCFLAGS) -o $@  $< \
	-lap_pkgrid -lap_ppl -lppl -lgmpxx -lpolkaMPQ -loctMPQ -lboxMPQ -lapron -lmpfr -lgmp

%_debug: %_debug.o
	$(CXX) -g $(ICFLAGS) $(LCFLAGS) -o $@  $< \
	-lap_pkgrid_debug -lap_ppl_debug -lppl -lgmpxx -lpolkaMPQ_debug -loctMPQ_debug -lboxMPQ_debug -lapron_debug -lmpfr -lgmp

%.o : %.c
	$(CC) $(CFLAGS) $(ICFLAGS) $(LCFLAGS) -c -o $@ $<
%_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) $(LCFLAGS) -c -o $@ $<

# OCaml examples
%.opt: %.ml
	$(OCAMLOPT) -cc "g++" $(OCAMLOPTFLAGS) $(OCAMLINC) $(OCAMLOPTLDFLAGS) -o $@ $<

clean:
	rm -f ctest1 ctest1_debug *.o test_00.cm[xo] test_00.o test_00.opt

distclean: clean

mostlyclean: clean

dist: ctest1.c Makefile README
	(cd ..; tar zcvf test.tgz $(^:%=test/%))
