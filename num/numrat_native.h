/* ********************************************************************** */
/* rational_native.h */
/* ********************************************************************** */

#ifndef _NUMRAT_NATIVE_H_
#define _NUMRAT_NATIVE_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#include "gmp.h"
#include "mpfr.h"

#include "num_config.h"

/* We assume that the basic NUMINT on which rational is built is properly
   defined */
#include "numint.h"

#ifndef NUMINT_NATIVE
#error "HERE"
#endif

typedef struct _rat_t {
  numint_t n; /* numerator */
  numint_t d; /* denominator, >=0 */
} numrat_t[1];

/* ====================================================================== */
/* Rational operations */
/* ====================================================================== */

static inline void numrat_canonicalize(numrat_t r)
{
  if (r->d){
    numint_t pgcd;
    numint_gcd(pgcd,r->n,r->d);
    *r->n /= *pgcd;
    *r->d /= *pgcd;
  }
  else {
    numint_set_int(r->n,1);
  }
}
#define numrat_numref(a) a->n
#define numrat_denref(a) a->d

static inline void numrat_set_numint2(numrat_t a, const numint_t b, const numint_t c)
{
  numint_set(numrat_numref(a),b);
  numint_set(numrat_denref(a),c);
  numrat_canonicalize(a);
}

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */

static inline void numrat_set(numrat_t a, const numrat_t b)
{ *a = *b; }
static inline void numrat_set_array(numrat_t* a, const numrat_t* b, size_t size)
{ memcpy(a,b,size*sizeof(numrat_t)); }
static inline void numrat_set_int(numrat_t a, long int i)
{
  numint_set_int(a->n,i);
  numint_set_int(a->d,1);
}

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numrat_init(numrat_t a)
{ 
  numint_init(a->n);
  numint_init_set_int(a->d,1);
}
static inline void numrat_init_array(numrat_t* a, size_t size)
{
  size_t i;
  for (i=0; i<size; i++) numrat_init(a[i]);
}
static inline void numrat_init_set(numrat_t a, const numrat_t b)
{ *a = *b; }
static inline void numrat_init_set_int(numrat_t a, long int i)
{ numint_set_int(a->n,i); numint_set_int(a->d,1); }

static inline void numrat_clear(numrat_t a)
{}
static inline void numrat_clear_array(numrat_t* a, size_t size)
{}


/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numrat_neg(numrat_t a, const numrat_t b)
{ numint_neg(a->n,b->n); }
static inline void numrat_abs(numrat_t a, const numrat_t b)
{ numint_abs(a->n,b->n); }
static inline void numrat_add(numrat_t a, const numrat_t b, const numrat_t c)
{ 
  numint_t d;
  *d = *b->d * *c->d;
  *a->n = *b->n * *c->d + *b->d * *c->n; 
  *a->d = *d; 
  numrat_canonicalize(a); 
}
static inline void numrat_add_uint(numrat_t a, const numrat_t b, unsigned long int c)
{ 
  numint_add_uint(numrat_numref(a),numrat_numref(b),c);
  numint_set(numrat_denref(a),numrat_denref(b));
  numrat_canonicalize(a);
}
static inline void numrat_sub(numrat_t a, const numrat_t b, const numrat_t c)
{ 
  numint_t d;
  *d = *b->d * *c->d; 
  *a->n = *b->n * *c->d - *b->d * *c->n; 
  *a->d = *d; 
  numrat_canonicalize(a); 
}
static inline void numrat_sub_uint(numrat_t a, const numrat_t b, unsigned long int c)
{ 
  numint_sub_uint(numrat_numref(a),numrat_numref(b),c);
  numint_set(numrat_denref(a),numrat_denref(b));
  numrat_canonicalize(a);
}
static inline void numrat_mul(numrat_t a, const numrat_t b, const numrat_t c)
{
  *a->n = *b->n * *c->n;
  *a->d = *b->d * *c->d;
  numrat_canonicalize(a);   
}
static inline void numrat_mul_2(numrat_t a, const numrat_t b)
{
  if (*b->d % 2 == 0)
    *a->d = *b->d / 2;
  else
    *a->n = *b->n * 2;
}
static inline void numrat_div(numrat_t a, const numrat_t b, const numrat_t c)
{
  *a->n = *b->n * *c->d;
  *a->d = *b->d * *c->n;
  numrat_canonicalize(a);   
}
static inline void numrat_div_2(numrat_t a, const numrat_t b)
{
  if (*b->n % 2 == 0)
    *a->n = *b->n / 2;
  else
    *a->d = *b->d * 2;
}
static inline void numrat_floor(numrat_t a, const numrat_t b)
{
  numint_fdiv_q(a->n, b->n, b->d);
  numint_set_int(a->d,1);
}
static inline void numrat_ceil(numrat_t a, const numrat_t b)
{
  numint_cdiv_q(a->n, b->n, b->d);
  numint_set_int(a->d,1);
}
/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numrat_sgn(const numrat_t a)
{ return numint_sgn(a->n); }
static inline int numrat_cmp(const numrat_t a, const numrat_t b)
{ 
  numint_t aa,bb;
  *aa = *a->n * *b->d;
  *bb = *a->d * *b->n;
  return numint_cmp(aa,bb);
}
static inline int numrat_cmp_int(const numrat_t a, long int b)
{ 
  numint_t aa,bb;
  *aa = *a->n;
  *bb = *a->d * b;
  return numint_cmp(aa,bb);
}
static inline int numrat_equal(const numrat_t a, const numrat_t b)
{ return numint_equal(a->n,b->n) && numint_equal(a->d,b->d); }

static inline void numrat_min(numrat_t a, const numrat_t b, const numrat_t c)
{ numrat_set(a, numrat_cmp(b,c)<=0 ? b : c); }
static inline void numrat_max(numrat_t a, const numrat_t b, const numrat_t c)
{ numrat_set(a, numrat_cmp(b,c)>=0 ? b : c); }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numrat_fprint(FILE* stream, const numrat_t a)
{ 
  if (*a->n==0)
    fprintf(stream,"0");
  else if (*a->d==1)
    numint_fprint(stream,a->n);
  else {
    numint_fprint(stream,a->n);
    fprintf(stream,"/");
    numint_fprint(stream,a->d);
  }
}
static inline void numrat_print(const numrat_t a)
{ 
  numrat_fprint(stdout,a);
}
static inline int numrat_snprint(char* s, size_t size, const numrat_t a)
{
  int res;
  if (*a->n==0)
    res = snprintf(s,size, "0");
  else if (*a->d==1)
    res = numint_snprint(s,size,a->n);
  else {
    res = numint_snprint(s,size,a->n);
    res += snprintf(s+res,size-res,"/");
    res += numint_snprint(s+res,size-res,a->n);
  }
  return res;
}

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* int2 -> numrat */
static inline void numrat_set_int2(numrat_t a, long int i, unsigned long int j)
{ 
  numint_set_int(a->n,i);
  numint_set_int(a->d,(long int)j);
  numrat_canonicalize(a);
}

/* mpz -> numrat */
static inline bool mpz_fits_numrat(const mpz_t a)
{ return mpz_fits_numint(a); }
static inline bool numrat_set_mpz(numrat_t a, const mpz_t b)
{ 
  numint_set_mpz(a->n,b);
  numint_set_int(a->d,1);
  return true;
}

/* mpq -> numrat */
static inline bool mpq_fits_numrat(const mpq_t a)
{ return mpz_fits_numint(mpq_numref(a)) && mpz_fits_numint(mpq_denref(a)); }
static inline bool numrat_set_mpq(numrat_t a, const mpq_t b)
{ 
  numint_set_mpz(a->n,mpq_numref(b));
  numint_set_mpz(a->d,mpq_denref(b));
  return true;
}

/* double -> numrat */
static inline bool double_fits_numrat_tmp(double k, mpq_t mpq)
{
  mpq_set_d(mpq,k);
  bool res = mpq_fits_numrat(mpq);
  return res;
}

static inline bool double_fits_numrat(double k)
{
  mpq_t mpq;
  mpq_init(mpq);
  bool res = double_fits_numrat_tmp(k,mpq);
  mpq_clear(mpq);
  return res;
}
static inline bool numrat_set_double_tmp(numrat_t a, double k, mpq_t mpq)
{
  mpq_set_d(mpq,k);
  numrat_set_mpq(a,mpq);
  return true;
}
static inline bool numrat_set_double(numrat_t a, double k)
{
  mpq_t mpq;
  mpq_init(mpq);
  numrat_set_double_tmp(a,k,mpq);
  mpq_clear(mpq);
  return true;
}
/* numrat -> int */
static inline bool numrat_fits_int(const numrat_t a)
{
  numint_t b;
  numint_cdiv_q(b,a->n,a->d);
  return *b<=LONG_MAX && *b>=-LONG_MAX;
}
static inline bool int_set_numrat(long int* a, const numrat_t b)
{ 
  numint_t q,r;
  numint_cdiv_qr(q,r,b->n,b->d);
  *a = *q;
  return (*r==0);
}
static inline bool int_set_numrat_tmp(long int* a, const numrat_t b, 
				      mpz_t q, mpz_t r)
{ return int_set_numrat(a,b); }

/* numrat -> mpz */
static inline bool mpz_set_numrat(mpz_t a, const numrat_t b)
{
  numint_t q,r;
  numint_cdiv_qr(q,r,numrat_numref(b),numrat_denref(b));
  mpz_set_numint(a,q);
  return (numint_sgn(r)==0);
}
static inline bool mpz_set_numrat_tmp(mpz_t a, const numrat_t b, mpz_t mpz)
{ return mpz_set_numrat(a,b); }

/* numrat -> mpq */
static inline bool mpq_set_numrat(mpq_t a, const numrat_t b)
{
  mpz_set_numint(mpq_numref(a), numrat_numref(b));
  mpz_set_numint(mpq_denref(a), numrat_denref(b));
  return true;
}
/* numrat -> double */
static inline bool numrat_fits_double(const numrat_t a)
{ return true; }
/* mpfr should have exactly a precision of 53 bits */
static inline bool double_set_numrat_tmp(double* a, const numrat_t b, 
					 mpq_t mpq, mpfr_t mpfr)
{
  mpq_set_numrat(mpq,b);
  int res = mpfr_set_q(mpfr,mpq,+1);
  *a = mpfr_get_d(mpfr,+1); /* should be exact */
  return (res==0);
}
static inline bool double_set_numrat(double* a, const numrat_t b)
{
  mpq_t mpq;
  mpfr_t mpfr;
  mpq_init(mpq);
  mpfr_init2(mpfr,53);
  bool res = double_set_numrat_tmp(a,b,mpq,mpfr);
  mpq_clear(mpq);
  mpfr_clear(mpfr);
  return res;
}

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline unsigned char numrat_serialize_id(void)
{ return 0x10 + sizeof(numint_t)/4; }

static inline size_t numrat_serialize(void* dst, const numrat_t src)
{
  size_t x = numint_serialize(dst,numrat_numref(src));
  return x + numint_serialize((char*)dst+x,numrat_denref(src));
}

static inline size_t numrat_deserialize(numrat_t dst, const void* src)
{
  size_t x = numint_deserialize(numrat_numref(dst),src);
  return x + numint_deserialize(numrat_denref(dst),(const char*)src+x);
}

static inline size_t numrat_serialized_size(const numrat_t a)
{
  return numint_serialized_size(numrat_numref(a)) +
         numint_serialized_size(numrat_denref(a));
}

#endif
