/* ********************************************************************** */
/* numflt.h */
/* ********************************************************************** */

#ifndef _NUMFLT_H_
#define _NUMFLT_H_

#include "gmp.h"
#include "mpfr.h"
#include "ap_scalar.h"
#include "num_config.h"

#if defined(NUMFLT_DOUBLE) || defined(NUMFLT_LONGDOUBLE)
#include "numflt_native.h"
#else
#error "HERE"
#endif

#ifdef __cplusplus
extern "C" {
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
static inline void numflt_set(numflt_t a, numflt_t b);
static inline void numflt_set_array(numflt_t* a, numflt_t* b, size_t size);
static inline void numflt_set_int(numflt_t a, long int i);

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numflt_init(numflt_t a);
static inline void numflt_init_array(numflt_t* a, size_t size);
static inline void numflt_init_set(numflt_t a, numflt_t b);
static inline void numflt_init_set_int(numflt_t a, long int i);

static inline void numflt_clear(numflt_t a);
static inline void numflt_clear_array(numflt_t* a, size_t size);

static inline void numflt_swap(numflt_t a, numflt_t b)
{ numflt_t t; *t=*a;*a=*b;*b=*t; }

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numflt_neg(numflt_t a, numflt_t b);
static inline void numflt_abs(numflt_t a, numflt_t b);
static inline void numflt_add(numflt_t a, numflt_t b, numflt_t c);
static inline void numflt_add_uint(numflt_t a, numflt_t b, unsigned long int c);
static inline void numflt_sub(numflt_t a, numflt_t b, numflt_t c);
static inline void numflt_sub_uint(numflt_t a, numflt_t b, unsigned long int c);
static inline void numflt_mul(numflt_t a, numflt_t b, numflt_t c);
static inline void numflt_mul_2(numflt_t a, numflt_t b);
static inline void numflt_div(numflt_t a, numflt_t b, numflt_t c);
static inline void numflt_div_2(numflt_t a, numflt_t b);
static inline void numflt_min(numflt_t a, numflt_t b, numflt_t c);
static inline void numflt_max(numflt_t a, numflt_t b, numflt_t c);
static inline void numflt_floor(numflt_t a, numflt_t b);
static inline void numflt_ceil(numflt_t a, numflt_t b);

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numflt_sgn(numflt_t a);
static inline int numflt_cmp(numflt_t a, numflt_t b);
static inline int numflt_cmp_int(numflt_t a, long int b);
static inline bool numflt_equal(numflt_t a, numflt_t b);

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numflt_print(numflt_t a);
static inline void numflt_fprint(FILE* stream, numflt_t a);
static inline int  numflt_snprint(char* s, size_t size, numflt_t a);

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

static inline void numflt_set_int2(numflt_t a, long int i, unsigned long int j);
  /* int2 -> numflt */
static inline bool numflt_set_mpz(numflt_t a, mpz_t b);
  /* mpz -> numflt */
static inline bool numflt_set_mpq(numflt_t a, mpq_t b);
  /* mpq -> numflt */
static inline bool numflt_set_double(numflt_t a, double b);
  /* double -> numflt */
static inline bool numflt_set_ap_scalar(numflt_t a, ap_scalar_t* b);
  /* (finite) ap_scalar -> numflt */

static inline bool int_set_numflt(long int* a, numflt_t b);
  /* numflt -> int */
static inline bool mpz_set_numflt(mpz_t a, numflt_t b);
  /* numflt -> mpz */
static inline bool mpq_set_numflt(mpq_t a, numflt_t b);
  /* numflt -> mpq */
static inline bool double_set_numflt(double* a, numflt_t b);
  /* numflt -> double */
static inline bool ap_scalar_set_numflt(ap_scalar_t* a, numflt_t b);
  /* numflt -> ap_scalar */

static inline bool mpz_fits_numflt(mpz_t a);
static inline bool mpq_fits_numflt(mpq_t a);
static inline bool double_fits_numflt(double a);
static inline bool numflt_fits_int(numflt_t a);
static inline bool numflt_fits_double(numflt_t a);

/* Optimized versions */
/* mpfr should have exactly the precision NUMFLT_MANT_DIG */
static inline bool numflt_set_mpz_tmp(numflt_t a, mpz_t b, mpfr_t mpfr);
static inline bool numflt_set_mpq_tmp(numflt_t a, mpq_t b, mpfr_t mpfr);

/* ====================================================================== */
/* Only for floating point */
/* ====================================================================== */

static inline bool numflt_infty(numflt_t a);
static inline void numflt_set_infty(numflt_t a);

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline unsigned char numflt_serialize_id(void)
{ return 0x20 + sizeof(numflt_t)/4; }

static inline size_t numflt_serialize(void* dst, numflt_t src);
static inline size_t numflt_deserialize(numflt_t dst, const void* src);
static inline size_t numflt_serialized_size(numflt_t a);


/* */
static inline bool numflt_set_ap_scalar(numflt_t a, ap_scalar_t* b)
{
  assert (ap_scalar_infty(b)==0);
  switch (b->discr){
  case AP_SCALAR_MPQ:
    return numflt_set_mpq(a,b->val.mpq);
  case AP_SCALAR_DOUBLE:
    return numflt_set_double(a,b->val.dbl);
  default: abort();
  }
}
static inline bool ap_scalar_set_numflt(ap_scalar_t* a, numflt_t b)
{
  ap_scalar_reinit(a,AP_SCALAR_DOUBLE);
  return double_set_numflt(&a->val.dbl,b);
}

#ifdef __cplusplus
}
#endif

#endif
