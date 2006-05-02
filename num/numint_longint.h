/* ********************************************************************** */
/* numint_longint.h */
/* ********************************************************************** */

#ifndef _NUMINT_LONGINT_H_
#define _NUMINT_LONGINT_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <assert.h>
#include <gmp.h>
#include "num_config.h"

typedef long int numint_t[1];

#define NUMINT_MAX LONG_MAX
#define NUMINT_NATIVE

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void numint_set(numint_t a, const numint_t b)
{ *a = *b; }
static inline void numint_set_array(numint_t* a, const numint_t* b, size_t size)
{ memcpy(a,b,size*sizeof(numint_t)); }
static inline void numint_set_int(numint_t a, long int i)
{ *a = i; }
 
/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numint_init(numint_t a)
{ *a = 0L; }
static inline void numint_init_array(numint_t* a, size_t size)
{
  size_t i; 
  for (i=0; i<size; i++) *(a[i]) = 0L; 
}
static inline void numint_init_set(numint_t a, const numint_t b)
{ numint_set(a,b); }
static inline void numint_init_set_int(numint_t a, long int i)
{ numint_set_int(a,i); }

static inline void numint_clear(numint_t a)
{}
static inline void numint_clear_array(numint_t* a, size_t size)
{}

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numint_neg(numint_t a, const numint_t b)
{ *a = -(*b); }
static inline void numint_abs(numint_t a, const numint_t b)
{ *a = labs(*b); }
static inline void numint_add(numint_t a, const numint_t b, const numint_t c)
{ *a = *b + *c; }
static inline void numint_add_uint(numint_t a, const numint_t b, unsigned long int c)
{ *a = *b + c; }
static inline void numint_sub(numint_t a, const numint_t b, const numint_t c)
{ *a = *b - *c; }
static inline void numint_sub_uint(numint_t a, const numint_t b, unsigned long int c)
{ *a = *b - c; }
static inline void numint_mul(numint_t a, const numint_t b, const numint_t c)
{ *a = *b * *c; }
static inline void numint_mul_2(numint_t a, const numint_t b)
{ *a = *b << 1; }
static inline void numint_fdiv_q(numint_t a, const numint_t b, const numint_t c)
{
  ldiv_t d = ldiv(*b,*c); /* rounding towards 0 */
  *a = d.quot;
  if (d.quot<0L && d.rem!=0L){
    *a = *a - 1; /* rounding towards minus infty */
  }
} 
static inline void numint_cdiv_q(numint_t a, const numint_t b, const numint_t c)
{
  ldiv_t d = ldiv(*b,*c); /* rounding towards 0 */
  *a = d.quot;
  if (d.quot>=0L && d.rem!=0L){
    *a = *a + 1; /* rounding towards plus infty */
  }
} 
static inline void numint_cdiv_2(numint_t a, const numint_t b)
{ *a = (*b>=0L) ? (*b+1)/2 : *b/2; }
static inline void numint_min(numint_t a, const numint_t b, const numint_t c)
{ *a = (*b<=*c) ? *b : *c; }
static inline void numint_max(numint_t a, const numint_t b, const numint_t c)
{ *a = (*b>=*c) ? *b : *c; }

/* ====================================================================== */
/* Arithmetic Integer Operations */
/* ====================================================================== */

static inline void numint_divexact(numint_t a, const numint_t b, const numint_t c)
{ *a = *b / *c; }
static inline void numint_mod(numint_t a, const numint_t b, const numint_t c)
{ *a = *b % *c; }
static inline long int _gcd_aux2(long int a, long int b)
{ /* a is supposed to be greater than b */
  long int t;
  while (b!=0L && a!=b) {
    t = b;
    b = a % b;
    a = t;
  }
  return a;
}
static inline long int _gcd_aux(long int a, long int b)
{
  a = labs(a);
  b = labs(b);
  return (a>=b) ? _gcd_aux2(a,b) : _gcd_aux2(b,a);
}
static inline void numint_gcd(numint_t a, const numint_t b,  const numint_t c)
{ *a = _gcd_aux(*b,*c); }

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numint_sgn(const numint_t a)
{ return (*a==0L ? 0 : (*a>0L ? 1 : -1)); }
static inline int numint_cmp(const numint_t a, const numint_t b)
{ return (*a==*b ? 0 : (*a>*b ? 1 : -1)); }
static inline int numint_cmp_int(const numint_t a, long int b)
{ return (*a==b ? 0 : (*a>b ? 1 : -1)); }
static inline int numint_equal(const numint_t a, const numint_t b)
{ return *a==*b; }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numint_print(const numint_t a)
{ printf("%li",*a); }
static inline void numint_fprint(FILE* stream, const numint_t a)
{ fprintf(stream,"%li",*a); }
static inline int numint_snprint(char* s, size_t size, const numint_t a)
{ return snprintf(s,size,"%li",*a); }

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* int2 -> numint */
static inline void numint_set_int2(numint_t a, long int i, unsigned long int j)
{ long int k = j; numint_cdiv_q(a,&i,&k); }

/* mpz -> numint */
static inline bool mpz_fits_numint(const mpz_t a)
{ return mpz_fits_slong_p(a); }
static inline void numint_set_mpz(numint_t a, const mpz_t b)
{ *a = mpz_get_si(b); }

/* mpq -> numint */
static inline bool mpq_fits_numint(const mpq_t a)
{
  mpz_t d;
  bool res;
  mpz_init(d);
  mpz_cdiv_q(d,mpq_numref(a),mpq_denref(a));
  res = mpz_fits_slong_p(d);
  mpz_clear(d);
  return res;
}
static inline void numint_set_mpq(numint_t a, const mpq_t b)
{ 
  mpz_t d;
  mpz_init(d);
  mpz_cdiv_q(d,mpq_numref(b),mpq_denref(b));
  *a = mpz_get_si(d);
  mpz_clear(d);
}

/* double -> numint */
static inline bool double_fits_numint(double a)
{
  return a>=(double)(-LONG_MAX) && a<=(double)LONG_MAX;
}
static inline void numint_set_double(numint_t a, double b)
{ *a = ceil(b); }
 
/* numint -> int */
static inline bool numint_fits_int(const numint_t a)
{ return true; }
static inline long int numint_get_int(const numint_t a)
{ return *a; }

/* numint -> mpz */
static inline void mpz_set_numint(mpz_t a, const numint_t b)
{ mpz_set_si(a,*b); }

/* numint -> mpq */
static inline void mpq_set_numint(mpq_t a, const numint_t b)
{ mpq_set_si(a,*b,1); }

/* numint -> double */
static inline bool numint_fits_double(const numint_t a)
{ return true; }
static inline double numint_get_double(const numint_t a)
{ return (double)(*a); }

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline unsigned char numint_serialize_id(void)
{ return sizeof(numint_t)/4; }

static inline size_t numint_serialize(void* dst, const numint_t src)
{
  num_store_words8(dst,src,sizeof(numint_t));
  return sizeof(numint_t);
}

static inline size_t numint_deserialize(numint_t dst, const void* src)
{
  num_store_words8(dst,src,sizeof(numint_t));
  return sizeof(numint_t);
}

static inline size_t numint_serialized_size(const numint_t a)
{ return sizeof(numint_t); }

#endif
