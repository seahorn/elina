/* ********************************************************************** */
/* numflt_double.h */
/* ********************************************************************** */

#ifndef _NUMFLT_DOUBLE_H_
#define _NUMFLT_DOUBLE_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <gmp.h>
#include <mpfr.h>

#include "num_config.h"

typedef double numflt_t[1];

#define NUMFLT_MAX (double)1.0/(double)0.0
#define NUMFLT_NATIVE

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void numflt_set(numflt_t a, const numflt_t b)
{ *a = *b; }
static inline void numflt_set_array(numflt_t* a, const numflt_t* b, size_t size)
{ memcpy(a,b,size*sizeof(numflt_t)); }
static inline void numflt_set_int(numflt_t a, long int i)
{ *a = (double)i; }
 
/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numflt_init(numflt_t a)
{ *a = 0.0; }
static inline void numflt_init_array(numflt_t* a, size_t size)
{
  size_t i; 
  for (i=0; i<size; i++) *(a[i]) = 0.0; 
}
static inline void numflt_init_set(numflt_t a, const numflt_t b)
{ numflt_set(a,b); }
static inline void numflt_init_set_int(numflt_t a, long int i)
{ numflt_set_int(a,i); }

static inline void numflt_clear(numflt_t a)
{}
static inline void numflt_clear_array(numflt_t* a, size_t size)
{}

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numflt_neg(numflt_t a, const numflt_t b)
{ *a = -(*b); }
static inline void numflt_abs(numflt_t a, const numflt_t b)
{ *a = fabs(*b); }
static inline void numflt_add(numflt_t a, const numflt_t b, const numflt_t c)
{ *a = *b + *c; }
static inline void numflt_add_uint(numflt_t a, const numflt_t b, unsigned long int c)
{ *a = *b + (double)c; }
static inline void numflt_sub(numflt_t a, const numflt_t b, const numflt_t c)
{ *a = *b - *c; }
static inline void numflt_sub_uint(numflt_t a, const numflt_t b, unsigned long int c)
{ *a = *b - (double)c; }
static inline void numflt_mul(numflt_t a, const numflt_t b, const numflt_t c)
{ *a = *b * *c; }
static inline void numflt_mul_2(numflt_t a, const numflt_t b)
{ *a = ldexp(*b,1); }
static inline void numflt_div(numflt_t a, const numflt_t b, const numflt_t c)
{ *a = *b / *c; }
static inline void numflt_div_2(numflt_t a, const numflt_t b)
{ *a = ldexp(*b,-1); }
static inline void numflt_min(numflt_t a, const numflt_t b, const numflt_t c)
{ *a = fmin(*b,*c); }
static inline void numflt_max(numflt_t a, const numflt_t b, const numflt_t c)
{ *a = fmax(*b,*c); }
static inline void numflt_floor(numflt_t a, const numflt_t b)
{ *a = floor(*b); }
static inline void numflt_ceil(numflt_t a, const numflt_t b)
{ *a = ceil(*b); }

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numflt_sgn(const numflt_t a)
{ return (*a==0.0 ? 0 : (*a>0.0 ? 1 : -1)); }
static inline int numflt_cmp(const numflt_t a, const numflt_t b)
{ return (*a==*b ? 0 : (*a>*b ? 1 : -1)); }
static inline int numflt_cmp_int(const numflt_t a, long int b)
{ 
  double bb = (double)b;
  return (*a==bb ? 0 : (*a>bb ? 1 : -1)); 
}
static inline int numflt_equal(const numflt_t a, const numflt_t b)
{ return *a==*b; }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numflt_print(const numflt_t a)
{ printf("%.20g",*a+0.0); }
static inline void numflt_fprint(FILE* stream, const numflt_t a)
{ fprintf(stream,"%.20g",*a+0.0); }
static inline int numflt_snprint(char* s, size_t size, const numflt_t a)
{ return snprintf(s,size,"%.20g",*a+0.0); }

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* int2 -> numflt */
static inline void numflt_set_int2(numflt_t a, long int i, unsigned long int j)
{ *a = (double)i/(-((double)(-j))); }

/* mpz -> numflt */
static inline bool mpz_fits_numflt(const mpz_t a)
{
  double k = mpz_get_d(a);
  return (fabs(k)+1.0) != NUMFLT_MAX;
}
/* mpfr is supposed to have exactly the IEEE754 double precision of 53 bits */
static inline bool numflt_set_mpz_tmp(numflt_t a, const mpz_t b, mpfr_t mpfr)
{ 
  int res = mpfr_set_z(mpfr,b,+1);
  *a = mpfr_get_d(mpfr,+1);/* Normally, exact conversion here (unless overfloww) */
  return (res==0);
}
static inline bool numflt_set_mpz(numflt_t a, const mpz_t b)
{
  mpfr_t mpfr;
  
  mpfr_init2(mpfr,53);
  bool res = numflt_set_mpz_tmp(a,b,mpfr);
  mpfr_clear(mpfr);
  return res;
}
/* mpq -> numflt */
static inline bool mpq_fits_numflt(const mpq_t a)
{
  double k = mpq_get_d(a);
  return (fabs(k)+1.0) != NUMFLT_MAX;
}
/* mpfr is supposed to have exactly the IEEE754 double precision of 53 bits */
static inline bool numflt_set_mpq_tmp(numflt_t a, const mpq_t b, mpfr_t mpfr)
{
  int res = mpfr_set_q(mpfr,b,+1);
  *a = mpfr_get_d(mpfr,+1);/* Normally, exact conversion here (unless overfloww) */
  return (res==0);
}  
static inline bool numflt_set_mpq(numflt_t a, const mpq_t b)
{
  mpfr_t mpfr;
  
  mpfr_init2(mpfr,53);
  bool res = numflt_set_mpq_tmp(a,b,mpfr);
  mpfr_clear(mpfr);
  return res;
}
/* double -> numflt */
static inline bool double_fits_numflt(double a)
{ return true; }
static inline bool numflt_set_double(numflt_t a, double k)
{ *a = k; return true; }

/* numflt -> int */
static inline bool numflt_fits_int(const numflt_t a)
{ 
  double d = ceil(*a);
  return d >= (double)(-LONG_MAX) && d<= (double)LONG_MAX;
}
static inline bool int_set_numflt(long int*a, const numflt_t b)
{
  double c = ceil(*b);
  *a = (long int)c;
  return (*b==c);
}

/* numflt -> mpz */
static inline bool mpz_set_numflt(mpz_t a, const numflt_t b)
{ 
  double c = ceil(*b);
  mpz_set_d(a,c);
  return (*b==c);
}
/* numflt -> mpq */
static inline bool mpq_set_numflt(mpq_t a, const numflt_t b)
{ mpq_set_d(a,*b); return true; }

/* numflt -> double */
static inline bool numflt_fits_double(const numflt_t a)
{ return true; }
static inline bool double_set_numflt(double* a, const numflt_t b)
{ *a = *b; return true; }

/* ====================================================================== */
/* Only for floating point */
/* ====================================================================== */

static inline bool numflt_infty(const numflt_t a)
{ return fabs(*a) == NUMFLT_MAX; }
static inline void numflt_set_infty(numflt_t a)
{ *a = NUMFLT_MAX; }

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline size_t numflt_serialize(void* dst, const numflt_t src)
{
  num_store_words8(dst,src,sizeof(numflt_t));
  return sizeof(numflt_t);
}

static inline size_t numflt_deserialize(numflt_t dst, const void* src)
{
  num_store_words8(dst,src,sizeof(numflt_t));
  return sizeof(numflt_t);
}

static inline size_t numflt_serialized_size(const numflt_t a)
{ return sizeof(numflt_t); }

#endif

