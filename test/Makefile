include ../Makefile.config

ICFLAGS = \
-I$(GMP_PREFIX)/include \
-I$(MPFR_PREFIX)/include \
-I$(APRON_PREFIX)/include \
-Wall -Wstrict-prototypes -Wimplicit-function-declaration \
-std=c99

LCFLAGS = \
-L$(GMP_PREFIX)/lib \
-L$(MPFR_PREFIX)/lib \
-L$(APRON_PREFIX)/lib \
-L$(PPL_PREFIX)/lib \
-L$(CAMLIDL_PREFIX)/lib/ocaml

all: C

# C examples

C: ctest1

%: %.o
	$(CXX) $(CFLAGS) $(ICFLAGS) $(LCFLAGS) -o $@  $< \
	-lap_pkgrid -lap_ppl -lppl -lgmpxx -lpolkaMPQ -loctMPQ -lboxMPQ -lapron -lmpfr -lgmp

%_debug: %_debug.o
	$(CXX) $(CFLAGS_DEBUG) $(ICFLAGS) $(LCFLAGS) -o $@  $< \
	-lap_pkgrid_debug -lap_ppl_debug -lppl -lgmpxx -lpolkaMPQ_debug -loctMPQ_debug -lboxMPQ_debug -lapron_debug -lmpfr -lgmp

%.o : %.c
	$(CC) $(ICFLAGS) $(CFLAGS) -c -o $@ $<
%_debug.o: %.c
	$(CC) $(ICFLAGS) $(CFLAGS_DEBUG) -c -o $@ $<

# OCaml examples

#

clean:
	rm -f ctest1 *_debug *.o

distclean: clean

dist: ctest1.c Makefile README
	(cd ..; tar zcvf test.tgz $(^:%=test/%))
