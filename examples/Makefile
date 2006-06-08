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
-L$(POLKA_PREFIX)/lib \
-std=c99

CFLAGS = $(ICFLAGS) -O3 -DNDEBUG
CFLAGS_DEBUG = $(ICFLAGS) -O0 -g -UNDEBUG
LDFLAGS = $(ICFLAGS) $(LCFLAGS)


all: C

C: example1g


%g: %g.o
	$(CC) $(LDFLAGS) -o $@  $< -lpolkag_debug -lapron_debug -lgmp

%g.o: %.c
	$(CC) $(CFLAGS_DEBUG) -c -o $@ $<
