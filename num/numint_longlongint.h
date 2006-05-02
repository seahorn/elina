/* ********************************************************************** */
/* numint_longlongint.h */
/* ********************************************************************** */

#ifndef _NUMINT_LONGLONGINT_H_
#define _NUMINT_LONGLONGINT_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <gmp.h>

#include "num_config.h"
/* Require C99 compliant compiler */

typedef long long int numint_t[1];

#define NUMINT_MAX LLONG_MAX
#define NUMINT_NATIVE

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */

static inline void numint_set(numint_t a, const numint_t b)
{ *a = *b; }
static inline void numint_set_array(numint_t* a, const numint_t* b, size_t size)
{ memcpy(a,b,size*sizeof(numint_t)); }
static inline void numint_set_int(numint_t a, long int i)
{ *a = (long long int)i; }
 
/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numint_init(numint_t a)
{ *a = 0LL; }
static inline void numint_init_array(numint_t* a, size_t size)
{
  size_t i; 
  for (i=0; i<size; i++) *(a[i]) = 0LL; 
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
{ *a = llabs(*b); }
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
  lldiv_t d = lldiv(*b,*c); /* rounding towards 0 */
  *a = d.quot;
  if (d.quot<0 && d.rem!=0){
    *a = *a - 1; /* rounding towards minus infty */
  }
} 
static inline void numint_cdiv_q(numint_t a, const numint_t b, const numint_t c)
{
  lldiv_t d = lldiv(*b,*c); /* rounding towards 0 */
  *a = d.quot;
  if (d.quot>=0LL && d.rem!=0LL){
    *a = *a + 1; /* rounding towards plus infty */
  }
} 
static inline void numint_cdiv_2(numint_t a, const numint_t b)
{ *a = (*b>=0) ? (*b+1)/2 : *b/2; }
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
static inline long long int _gcd_aux2(long long int a, long long int b)
{ /* a is supposed to be greater than b */
  long long int t;
  while (b!=0 && a!=b) {
    t = b;
    b = a % b;
    a = t;
  }
  return a;
}
static inline long long int _gcd_aux(long long int a, long long int b)
{
  a = llabs(a);
  b = llabs(b);
  return (a>=b) ? _gcd_aux2(a,b) : _gcd_aux2(b,a);
}
static inline void numint_gcd(numint_t a, const numint_t b,  const numint_t c)
{ *a = _gcd_aux(*b,*c); }

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numint_sgn(const numint_t a)
{ return (*a==0LL ? 0 : (*a>0LL ? 1 : -1)); }
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
{ printf("%lli",*a); }
static inline void numint_fprint(FILE* stream, const numint_t a)
{ fprintf(stream,"%lli",*a); }
static inline int numint_snprint(char* s, size_t size, const numint_t a)
{ return snprintf(s,size,"%lli",*a); }

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* int2 -> numint */
static inline void numint_set_int2(numint_t a, long int i, unsigned long int j)
{ 
  numint_t b;
  numint_set_int(a,i);
  numint_init_set_int(b,(long int)j);
  numint_cdiv_q(a,a,b);
  numint_clear(b);
}

/* mpz -> numint */
static inline bool mpz_fits_numint(const mpz_t a)
{
  size_t size = mpz_sizeinbase(a,2);
  return (size <= sizeof(numint_t)*8-1);
}
static inline void numint_set_mpz(numint_t a, const mpz_t b)
{
  int sgn;
  size_t count; 
  unsigned long int tab[2];
  
  sgn = mpz_sgn(b);
  mpz_export(&tab,&count,1,sizeof(long int),0,0,b); 
  if (count==0){
    *a = 0;
  }
  else {
    *a = tab[0];
    if (count==2){
      *a = *a << (sizeof(long int)*8);
      *a = *a + (long long int)(tab[1]);
      if (*a<0){
	assert(0);
      }
    }
    if (sgn<0) *a = -(*a);
  }
}

/* mpq -> numint */
static inline bool mpq_fits_numint(const mpq_t a)
{
  mpz_t d;
  bool res;
  mpz_init(d);
  mpz_cdiv_q(d,mpq_numref(a),mpq_denref(a));
  res = mpz_fits_numint(d);
  mpz_clear(d);
  return res;
}
static inline void numint_set_mpq(numint_t a, const mpq_t b)
{ 
  mpz_t d;
  mpz_init(d);
  mpz_cdiv_q(d,mpq_numref(b),mpq_denref(b));
  numint_set_mpz(a,d);
  mpz_clear(d);
}

/* double -> numint */
static inline bool double_fits_numint(double a)
{
  return a>=(double)(-LLONG_MAX) && a<=(double)LLONG_MAX;
}
static inline void numint_set_double(numint_t a, double b)
{ *a = ceil(b); }

/* numint -> int */
static inline bool numint_fits_int(const numint_t a)
{ return (*a>=LONG_MIN && *a<=LONG_MAX); }
static inline long int numint_get_int(const numint_t a)
{ return (long int)*a; }

/* numint -> mpz */
static inline void mpz_set_numint(mpz_t a, const numint_t b)
{
  unsigned long long int n;
  unsigned long int rep[2];
  
  n = llabs(*b);
  rep[1] = n && ULONG_MAX;
  rep[0] = n >> (sizeof(long int)*8);
  mpz_import(a,2,1,sizeof(unsigned long int),0,0,rep);
  if (*b<0)
    mpz_neg(a,a);
}
/* numint -> mpq */
static inline void mpq_set_numint(mpq_t a, const numint_t b)
{ 
  mpz_set_numint(mpq_numref(a),b);
  mpz_set_ui(mpq_denref(a),1);
}
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
