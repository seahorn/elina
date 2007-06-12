include ../Makefile.config

ICFLAGS = \
-I$(GMP_PREFIX)/include \
-I$(MPFR_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-Wall -Wstrict-prototypes -Wimplicit-function-declaration \
-std=c99
# -Winline -Wconversion

LCFLAGS = \
-L$(GMP_PREFIX)/lib \
-L$(MPFR_PREFIX)/lib \
-L$(APRON_PREFIX)/lib \
-L$(PPL_PREFIX)/lib \
-L$(CAMLIDL_PREFIX)/lib/ocaml

OCAMLINC = \
-I $(MLGMPIDL_PREFIX)/lib \
-I $(MLAPRONIDL_PREFIX)/lib

OCAMLLDFLAGS = \
-noautolink -ccopt "$(LCFLAGS)" \
bigarray.cma gmp.cma apron.cma \
box.cma oct.cma polka.cma ppl.cma \
-cclib "-lppl_caml -lapron_ppl -lppl -lpolka_caml -lpolkag_debug -loct_caml -loctQg -lbox_caml -lboxmpq_debug -litvmpq_debug -lapron_caml_debug -lapron_debug -lgmp_caml -lmpfr -lgmpxx -lgmp -lbigarray -lcamlidl"

OCAMLOPTLDFLAGS = \
-noautolink -verbose -ccopt "$(LCFLAGS)" \
bigarray.cmxa gmp.cmxa apron.cmxa box.cmxa polka.cmxa ppl.cmxa \
-cclib "-lppl_caml -lapron_ppl -lppl -lpolka_caml -lpolkag_debug -loct_caml -loctQg -lbox_caml -lboxmpq_debug -litvmpq_debug -lapron_caml_debug -lapron_debug -lgmp_caml -lmpfr -lgmpxx -lgmp -lbigarray -lcamlidl"

all: C ml

# C examples

C: ctest1q

%q: %.o
	$(CXX) $(CFLAGS) $(ICFLAGS) $(LCFLAGS) -o $@  $< \
	-lap_pkgrid -lapron_ppl -lppl -lgmpxx -lpolkag -loctQg -lboxmpq -lapron -litvmpq -litvdbl -lmpfr -lgmp

%q_debug: %_debug.o
	$(CXX) $(CFLAGS_DEBUG) $(ICFLAGS) $(LCFLAGS) -o $@  $< \
	-lap_pkgrid_debug -lapron_ppl_debug -lppl -lgmpxx -lpolkag_debug -loctQg_debug -lboxmpq_debug -lapron_debug -litvmpq_debug -litvdbl_debug -lmpfr -lgmp

%.o: %.c
	$(CC) $(CFLAGS) $(ICFLAGS) -c -o $@ $<
%_debug.o: %.c
	$(CC) $(CFLAGS_DEBUG) $(ICFLAGS) -c -o $@ $<

# OCaml examples

#

clean:
	rm -f ctest1q *_debug *.o

distclean: clean

dist: ctest1.c Makefile README
	(cd ..; tar zcvf test.tgz $(^:%=test/%))
