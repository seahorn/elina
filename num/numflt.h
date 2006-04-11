/* ********************************************************************** */
/* numflt.h */
/* ********************************************************************** */

#ifndef _NUMFLT_H_
#define _NUMFLT_H_

#include <gmp.h>
#include "num_config.h"

#if defined(NUMFLT_DOUBLE)
#include "numflt_double.h"
#elif defined(NUMFLT_LONGDOUBLE)
#include "numflt_longdouble.h"
#else
#error "HERE"
#endif

/* 
   NUMINT_DOUBLE and NUMFLT_LONGDOUBLE also define 
   
   - NUMFLT_MAX: positive infinity value
     It is assumed that (-NUMINT_MAX) is also repreentable
     
   - NUMFLT_NATIVE: no heap allocated memory
*/


/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void numflt_set(numflt_t a, const numflt_t b);
static inline void numflt_set_array(numflt_t* a, const numflt_t* b, size_t size);
static inline void numflt_set_int(numflt_t a, long int i);

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numflt_init(numflt_t a);
static inline void numflt_init_array(numflt_t* a, size_t size);
static inline void numflt_init_set(numflt_t a, const numflt_t b);
static inline void numflt_init_set_int(numflt_t a, long int i);

static inline void numflt_clear(numflt_t a);
static inline void numflt_clear_array(numflt_t* a, size_t size);

static inline void numflt_swap(numflt_t* a, numflt_t* b)
{ numflt_t t; *t=*a;*a=*b;*b=*t; }

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numflt_neg(numflt_t a, const numflt_t b);
static inline void numflt_abs(numflt_t a, const numflt_t b);
static inline void numflt_add(numflt_t a, const numflt_t b, const numflt_t c);
static inline void numflt_add_uint(numflt_t a, const numflt_t b, unsigned long int c);
static inline void numflt_sub(numflt_t a, const numflt_t b, const numflt_t c);
static inline void numflt_sub_uint(numflt_t a, const numflt_t b, unsigned long int c);
static inline void numflt_mul(numflt_t a, const numflt_t b, const numflt_t c);
static inline void numflt_mul_2(numflt_t a, const numflt_t b);
static inline void numflt_div(numflt_t a, const numflt_t b, const numflt_t c);
static inline void numflt_div_2(numflt_t a, const numflt_t b);
static inline void numflt_min(numflt_t a, const numflt_t b, const numflt_t c);
static inline void numflt_max(numflt_t a, const numflt_t b, const numflt_t c);
static inline void numflt_floor(numflt_t a, const numflt_t b);
static inline void numflt_ceil(numflt_t a, const numflt_t b);

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numflt_sgn(const numflt_t a);
static inline int numflt_cmp(const numflt_t a, const numflt_t b);
static inline int numflt_cmp_int(const numflt_t a, long int b);
static inline int numflt_equal(const numflt_t a, const numflt_t b);

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numflt_print(const numflt_t a);
static inline void numflt_print(FILE* stream, const numflt_t a);
static inline int numflt_snprint(char* s, size_t size, const numflt_t a);

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

static inline void numflt_set_int2(numflt_t a, long int i, unsigned long int j);
  /* int2 -> numflt */

static inline bool mpz_fits_numflt(const mpz_t a);
static inline void numflt_set_mpz(numflt_t a, const mpz_t b);
  /* mpz -> numflt */

static inline bool mpq_fits_numflt(const mpq_t a);
static inline void numflt_set_mpq(numflt_t a, const mpq_t b);
  /* mpq -> numflt */

static inline bool double_fits_numflt(double a);
static inline void numflt_set_double(numflt_t a, double k);
  /* double -> numflt */

static inline bool numflt_fits_int(const numflt_t a);
static inline long int numflt_get_int(const numflt_t a);
  /* numflt -> int */
static inline void mpz_set_numflt(mpz_t a, const numflt_t b);
  /* numflt -> mpz */
static inline void mpq_set_numflt(mpq_t a, const numflt_t b);
  /* numflt -> mpq */
static inline bool numflt_fits_double(const numflt_t a);
static inline double numflt_get_double(const numflt_t a);
  /* numflt -> double */

/* ====================================================================== */
/* Only for floating point */
/* ====================================================================== */

static inline bool numflt_infty(const numflt_t a);
static inline void numflt_set_infty(numflt_t a);

#endif
