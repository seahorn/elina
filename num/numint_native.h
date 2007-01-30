/* ********************************************************************** */
/* numint_native.h */
/* ********************************************************************** */

#ifndef _NUMINT_NATIVE_H_
#define _NUMINT_NATIVE_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "gmp.h"

#include "num_config.h"
/* Require C99 compliant compiler */

#if defined(NUMINT_LONGINT)
typedef long int numint_native;
#define NUMINT_ZERO 0L
#define NUMINT_ONE 1L
#define NUMINT_MAX LONG_MAX

#elif defined(NUMINT_LONGLONGINT)
typedef long long int numint_native;
#define NUMINT_ZERO 0LL
#define NUMINT_ONE 1LL
#define NUMINT_MAX LLONG_MAX

#else
#error "HERE"
#endif

typedef numint_native numint_t[1];

#define NUMINT_NATIVE
#
/* ====================================================================== */
/* Assignement */
/* ====================================================================== */

static inline void numint_set(numint_t a, const numint_t b)
{ *a = *b; }
static inline void numint_set_array(numint_t* a, const numint_t* b, size_t size)
{ memcpy(a,b,size*sizeof(numint_t)); }
static inline void numint_set_int(numint_t a, long int i)
{ *a = (numint_native)i; }
 
/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numint_init(numint_t a)
{ *a = NUMINT_ZERO; }
static inline void numint_init_array(numint_t* a, size_t size)
{
  size_t i; 
  for (i=0; i<size; i++) *(a[i]) = NUMINT_ZERO; 
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
#if defined(NUMINT_LONGINT)
{ *a = labs(*b); }
#else
{ *a = llabs(*b); }
#endif
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

#if defined(NUMINT_LONGINT)
static inline void numint_fdiv_q(numint_t a, const numint_t b, const numint_t c)
{
  ldiv_t d = ldiv(*b,*c); /* rounding towards 0 */
  *a = d.quot;
  if (d.quot<NUMINT_ZERO && d.rem!=NUMINT_ZERO){
    *a = *a - 1; /* rounding towards minus infty */
  }
} 
static inline void numint_cdiv_q(numint_t q, const numint_t a, const numint_t b)
{
  ldiv_t d = ldiv(*a,*b); /* rounding towards 0 */
  if (d.quot>=NUMINT_ZERO && d.rem!=NUMINT_ZERO){
    d.quot++; /* rounding towards plus infty */
  }
  *q = d.quot;
} 
static inline void numint_cdiv_qr(numint_t q, numint_t r, const numint_t a, const numint_t b)
{
  ldiv_t d = ldiv(*a,*b); /* rounding towards 0 */
  if (d.quot>=NUMINT_ZERO && d.rem!=NUMINT_ZERO){
    d.quot++; /* rounding towards plus infty */
    d.rem -= *b;
  }
  *q = d.quot;
  *r = d.rem;
} 
#else
static inline void numint_fdiv_q(numint_t a, const numint_t b, const numint_t c)
{
  lldiv_t d = lldiv(*b,*c); /* rounding towards 0 */
  *a = d.quot;
  if (d.quot<0 && d.rem!=0){
    *a = *a - 1; /* rounding towards minus infty */
  }
} 
static inline void numint_cdiv_q(numint_t q, const numint_t a, const numint_t b)
{
  lldiv_t d = lldiv(*a,*b); /* rounding towards 0 */
  if (d.quot>=NUMINT_ZERO && d.rem!=NUMINT_ZERO){
    d.quot++; /* rounding towards plus infty */
  }
  *q = d.quot;
} 
static inline void numint_cdiv_qr(numint_t q, numint_t r, const numint_t a, const numint_t b)
{
  lldiv_t d = lldiv(*a,*b); /* rounding towards 0 */
  if (d.quot>=NUMINT_ZERO && d.rem!=NUMINT_ZERO){
    d.quot++; /* rounding towards plus infty */
    d.rem -= *b;
  }
  *q = d.quot;
  *r = d.rem;
} 
#endif

static inline void numint_cdiv_2(numint_t a, const numint_t b)
{ *a = (*b>=NUMINT_ZERO) ? (*b+1)/2 : *b/2; }
static inline void numint_cdiv_q_2exp(numint_t a, const numint_t b, unsigned long int c)
{ *a = (*b >> c)+(*b & ((NUMINT_ONE<<c)-NUMINT_ONE) ? 1 : 0); }
static inline void numint_fdiv_q_2exp(numint_t a, const numint_t b, unsigned long int c)
{ *a = (*b >> c); }
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
static inline numint_native _gcd_aux2(numint_native a, numint_native b)
{ /* a is supposed to be greater than b */
  numint_native t;
  while (b!=NUMINT_ZERO && a!=b) {
    t = b;
    b = a % b;
    a = t;
  }
  return a;
}
static inline numint_native _gcd_aux(numint_native a, numint_native b)
{
  numint_abs(&a,&a);
  numint_abs(&b,&b);
  return (a>=b) ? _gcd_aux2(a,b) : _gcd_aux2(b,a);
}
static inline void numint_gcd(numint_t a, const numint_t b,  const numint_t c)
{ *a = _gcd_aux(*b,*c); }

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numint_sgn(const numint_t a)
{ return (*a==NUMINT_ZERO ? 0 : (*a>NUMINT_ZERO ? 1 : -1)); }
static inline int numint_cmp(const numint_t a, const numint_t b)
{ return (*a==*b ? 0 : (*a>*b ? 1 : -1)); }
static inline int numint_cmp_int(const numint_t a, long int b)
{ return (*a==b ? 0 : (*a>b ? 1 : -1)); }
static inline bool numint_equal(const numint_t a, const numint_t b)
{ return *a==*b; }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

#if defined(NUMINT_LONGINT)
static inline void numint_print(const numint_t a)
{ printf("%li",*a); }
static inline void numint_fprint(FILE* stream, const numint_t a)
{ fprintf(stream,"%li",*a); }
static inline int numint_snprint(char* s, size_t size, const numint_t a)
{ return snprintf(s,size,"%li",*a); }
#else
static inline void numint_print(const numint_t a)
{ printf("%lli",*a); }
static inline void numint_fprint(FILE* stream, const numint_t a)
{ fprintf(stream,"%lli",*a); }
static inline int numint_snprint(char* s, size_t size, const numint_t a)
{ return snprintf(s,size,"%lli",*a); }
#endif

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* int2 -> numint */
static inline void numint_set_int2(numint_t a, long int i, unsigned long int j)
{ 
  numint_t b;
  numint_set_int(a,i);
  numint_set_int(b,(long int)j);
  numint_cdiv_q(a,a,b);
}

/* mpz -> numint */
static inline bool numint_set_mpz(numint_t a, const mpz_t b)
#if defined(NUMINT_LONGINT)
{ *a = mpz_get_si(b); return true; }
#else
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
  return true;
}
#endif

/* mpq -> numint */
static inline bool numint_set_mpq_tmp(numint_t a, const mpq_t b, 
				      mpz_t q, mpz_t r)
{ 
  mpz_cdiv_qr(q,r, mpq_numref(b),mpq_denref(b));
  numint_set_mpz(a,q);
  bool res = (mpz_sgn(r)==0);
  return res;
}
static inline bool numint_set_mpq(numint_t a, const mpq_t b)
{ 
  mpz_t q,r;
  mpz_init(q);mpz_init(r);
  bool res = numint_set_mpq_tmp(a,b,q,r);
  mpz_clear(q); mpz_clear(r);
  return res;
}
/* double -> numint */
static inline bool numint_set_double(numint_t a, double b)
{
  double c = ceil(b);
  *a = c;
  return (b==c);
}
/* numint -> int */
static inline bool int_set_numint(long int* a, const numint_t b)
{ *a = (long int)(*b); return true; }

/* numint -> mpz */
static inline bool mpz_set_numint(mpz_t a, const numint_t b)
#if defined(NUMINT_LONGINT)
{ mpz_set_si(a,*b); return true; }
#else
{
  unsigned long long int n;
  unsigned long int rep[2];
  
  n = llabs(*b);
  rep[1] = n && ULONG_MAX;
  rep[0] = n >> (sizeof(long int)*8);
  mpz_import(a,2,1,sizeof(unsigned long int),0,0,rep);
  if (*b<0)
    mpz_neg(a,a);
  return true;
}
#endif

/* numint -> mpq */
static inline bool mpq_set_numint(mpq_t a, const numint_t b)
#if defined(NUMINT_LONGINT)
{ mpq_set_si(a,*b,1); return true; }
#else
{ 
  mpz_set_numint(mpq_numref(a),b);
  mpz_set_ui(mpq_denref(a),1);
  return true;
}
#endif

/* numint -> double */
static inline bool double_set_numint(double* a, const numint_t b)
{ 
  *a = (double)(*b); 
  double aa = -((double)(-(*b)));
  return (*a==aa);
}


static inline bool mpz_fits_numint(const mpz_t a)
#if defined(NUMINT_LONGINT)
{ return mpz_fits_slong_p(a); }
#else
{
  size_t size = mpz_sizeinbase(a,2);
  return (size <= sizeof(numint_t)*8-1);
}
#endif

static inline bool mpq_fits_numint_tmp(const mpq_t a, mpz_t mpz)
{
  mpz_cdiv_q(mpz,mpq_numref(a),mpq_denref(a));
  bool res = mpz_fits_numint(mpz);
  return res;
}
static inline bool mpq_fits_numint(const mpq_t a)
{
  mpz_t mpz;
  mpz_init(mpz);
  bool res = mpq_fits_numint_tmp(a,mpz);
  mpz_clear(mpz);
  return res;
}
static inline bool double_fits_numint(double a)
{
  return a>=(double)(-NUMINT_MAX) && a<=(double)NUMINT_MAX;
}
static inline bool numint_fits_int(const numint_t a)
#if defined(NUMINT_LONGINT)
{ return true; }
#else
{ return (*a>=-LONG_MAX && *a<=LONG_MAX); }
#endif
static inline bool numint_fits_double(const numint_t a)
{ return true; }

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
