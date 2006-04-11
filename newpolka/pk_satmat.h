/* ********************************************************************** */
/* pk_satmat.h: operations on saturation matrices */
/* ********************************************************************** */

/* This header file define operations on saturation matrices. A
   saturation matrix is an array of bits (or an array of bitstrings) used to
   memorize wether a given generator saturate a given constraint. */

/* Saturation matrices are very similar to normal matrices as defined in
   matrix.h, where explanations can be found. */

#ifndef __PK_SATMAT_H__
#define __PK_SATMAT_H__

#include <stdlib.h>
#include "pk_bit.h"

typedef struct satmat_t {
  /* public part */
  bitstring_t** p;
  size_t nbrows;
  size_t nbcolumns;
  /* private part */
  size_t  _maxrows;   /* number of rows allocated */
} satmat_t;

satmat_t* satmat_alloc(size_t nbrows, size_t nbcols);
void satmat_realloc(satmat_t* sat, size_t nbrows);
void satmat_realloc2(satmat_t* sat, size_t nbcols);
void satmat_extend_columns(satmat_t* sat, size_t nbcols);
void satmat_free(satmat_t* sat);
void satmat_clear(satmat_t* sat);
satmat_t* satmat_copy(const satmat_t* sat);
satmat_t* satmat_copy_extend_columns(const satmat_t* sat, size_t nbcols);
void satmat_print(const satmat_t* sat);
void satmat_fprint(FILE* stream, const satmat_t* sat);

bitstring_t satmat_get(const satmat_t* sat, size_t i, bitindex_t jx);
void satmat_set(satmat_t* sat, size_t i, bitindex_t jx);
void satmat_clr(satmat_t* sat, size_t i, bitindex_t jx);

satmat_t* satmat_transpose(const satmat_t* org, size_t nbcols);

void satmat_exch_rows(satmat_t* sat, size_t l1, size_t l2);
void satmat_sort_rows(satmat_t* sat);
void satmat_move_rows(satmat_t* sat, size_t destrow, size_t orgrow, size_t size);
int satmat_index_in_sorted_rows(const bitstring_t* satline, const satmat_t* sat);

#endif
