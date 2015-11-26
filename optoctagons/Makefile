include ../Makefile.config

AFLAGS := -D_GNU_SOURCE -pthread -fno-tree-vectorize -m64 -march=native

#IS_VECTOR = -DVECTOR 
#IS_SSE = -DSSE
DFLAGS := -g -DNUM_DOUBLE $(AFLAGS) $(IS_VECTOR) $(IS_SSE) -DTHRESHOLD=0.75 -DTIMING 


PREFIX = $(APRON_PREFIX)
#PREFIX = /usr/local

LIBDIR = $(PREFIX)/lib

INCLDIR = $(PREFIX)/include
#SOBJS = $(LIBDIR)/liboptoct.so 

ifeq ($(IS_VECTOR),)
CLOSURE_OBJS = opt_oct_closure_comp_sparse.o  opt_oct_incr_closure_comp_sparse.o opt_oct_closure_dense_scalar.o opt_oct_incr_closure_dense_scalar.o
CLOSURE_C = opt_oct_closure_comp_sparse.c  opt_oct_incr_closure_comp_sparse.c opt_oct_closure_dense_scalar.c opt_oct_incr_closure_dense_scalar.c
CLOSUREH = opt_oct_closure_comp_sparse.h  opt_oct_incr_closure_comp_sparse.h opt_oct_closure_dense_scalar.h opt_oct_incr_closure_dense_scalar.h 
else
CLOSURE_OBJS = opt_oct_closure_comp_sparse.o  opt_oct_incr_closure_comp_sparse.o opt_oct_closure_dense.o opt_oct_incr_closure_dense.o
CLOSURE_C = opt_oct_closure_comp_sparse.c  opt_oct_incr_closure_comp_sparse.c opt_oct_closure_dense.c opt_oct_incr_closure_dense.c
CLOSUREH =  opt_oct_closure_comp_sparse.h  opt_oct_incr_closure_comp_sparse.h opt_oct_closure_dense.h opt_oct_incr_closure_dense.h vector_intrin.h
endif

OBJS = $(CLOSURE_OBJS) opt_oct_nary.o opt_oct_resize.o opt_oct_predicate.o opt_oct_representation.o opt_oct_transfer.o opt_oct_hmat.o

INCLUDES = \
-I$(MLGMPIDL_INCLUDE) \
-I../newpolka \
-I../apron \
-I../mlapronidl \
-I../num \
-I../itv \
-I../octagons \
-I$(MPFR_PREFIX)/include -I$(GMP_PREFIX)/include \
-I$(CAML_PREFIX)/lib/ocaml -I$(CAMLIDL_PREFIX)/lib/ocaml \
-I$(shell pwd)/LinkedListAPI

LIBS = -L$(shell pwd)/LinkedListAPI -llinkedlistapi -L../apron -L$(APRON_PREFIX) -lapron -L$(MPFR_PREFIX)/lib -lmpfr -L$(GMP_PREFIX)/lib -lgmp -lm 

INSTALL = install
INSTALLd = install -d

SOINST = liboptoct.so
AINST = liboptoct.a

OPTOCTH = opt_oct.h opt_oct_internal.h opt_oct_hmat.h $(CLOSUREH)



all : linkedlistapi liboptoct.so liboptoct.a 
	@echo "-- Compiled optoct with flags $(IS_VECTOR) $(IS_SSE) ... "

linkedlistapi:
	(cd LinkedListAPI; make all)

opt_oct_closure_comp_sparse.o : opt_oct_closure_comp_sparse.h opt_oct_closure_comp_sparse.c
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_closure_comp_sparse.o opt_oct_closure_comp_sparse.c 

opt_oct_incr_closure_comp_sparse.o : opt_oct_incr_closure_comp_sparse.h opt_oct_incr_closure_comp_sparse.c opt_oct_closure_comp_sparse.o
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_incr_closure_comp_sparse.o opt_oct_incr_closure_comp_sparse.c 

#ifneq ($(IS_VECTOR),)
opt_oct_closure_dense_scalar.o : opt_oct_closure_dense_scalar.h opt_oct_closure_dense_scalar.c
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_closure_dense_scalar.o opt_oct_closure_dense_scalar.c 

opt_oct_incr_closure_dense_scalar.o : opt_oct_incr_closure_dense_scalar.h opt_oct_incr_closure_dense_scalar.c opt_oct_closure_dense_scalar.o
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_incr_closure_dense_scalar.o opt_oct_incr_closure_dense_scalar.c 


#else
opt_oct_closure_dense.o : opt_oct_closure_dense.h opt_oct_closure_dense.c
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_closure_dense.o opt_oct_closure_dense.c 

opt_oct_incr_closure_dense.o : opt_oct_incr_closure_dense.h opt_oct_incr_closure_dense.c opt_oct_closure_dense.o
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_incr_closure_dense.o opt_oct_incr_closure_dense.c 

#endif

opt_oct_hmat.o : opt_oct_hmat.h opt_oct_hmat.c 
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_hmat.o opt_oct_hmat.c 

opt_oct_nary.o : opt_oct_nary.c opt_oct_hmat.o
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_nary.o opt_oct_nary.c 

opt_oct_resize.o : opt_oct_resize.c opt_oct_hmat.o
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_resize.o opt_oct_resize.c 

opt_oct_predicate.o : opt_oct_predicate.c opt_oct_hmat.o
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_predicate.o opt_oct_predicate.c 

opt_oct_representation.o : opt_oct_representation.c opt_oct_hmat.o
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_representation.o opt_oct_representation.c 

opt_oct_transfer.o : opt_oct_transfer.c opt_oct_hmat.o
	$(CC) -c $(CFLAGS) $(DFLAGS) $(INCLUDES) -o opt_oct_transfer.o opt_oct_transfer.c 

liboptoct.so : $(OBJS) $(OPTOCTH)
	$(CC) -shared $(DFLAGS) $(INCLUDES) -o $(SOINST) $(OBJS) $(LIBS)

liboptoct.a : $(OBJS) $(OPTOCTH)
	$(AR) rcs -o $(AINST) $(OBJS)
	$(RANLIB) $(AINST)

install:
	(cd LinkedListAPI; make install)
	$(INSTALLd) $(LIBDIR); \
	for i in $(SOINST); do \
		$(INSTALL) $$i $(LIBDIR); \
	done; 
	for i in $(AINST); do \
		$(INSTALL) $$i $(LIBDIR); \
	done; 
	$(INSTALLd) $(INCLDIR); \
	for i in $(OPTOCTH); do \
		$(INSTALL) $$i $(INCLDIR); \
	done; 

clean:
	(cd LinkedListAPI; make clean)
	-rm $(SOINST) 
	-rm $(AINST) 
	-rm *.o


uninstall:
	rm -f $(INCLDIR)/opt_oct*.h
	rm -f $(LIBDIR)/$(SOINST) $(LIBDIR)/$(AINST) $(LIBDIR)/liblinkedlistapi.so
