/* ********************************************************************** */
/* numint_mpz.h */
/* ********************************************************************** */

#ifndef _NUMINT_MPZ_H_
#define _NUMINT_MPZ_H_

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include "gmp.h"
#include "mpfr.h"

#include "num_config.h"

/* Require C99 compliant compiler */

typedef mpz_t numint_t;

#undef NUMINT_MAX
#undef NUMINT_NATIVE

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void numint_set(numint_t a, const numint_t b)
{ mpz_set(a,b); }
static inline void numint_set_array(numint_t* a, const numint_t* b, size_t size)
{ 
  size_t i;
  for (i=0; i<size; i++) mpz_set(a[i],b[i]);
}
static inline void numint_set_int(numint_t a, long int i)
{ mpz_set_si(a,i); }
 
/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numint_init(numint_t a)
{ mpz_init(a); }
static inline void numint_init_array(numint_t* a, size_t size)
{
  size_t i; 
  for (i=0; i<size; i++) mpz_init(a[i]); 
}
static inline void numint_init_set(numint_t a, const numint_t b)
{ mpz_init_set(a,b); }
static inline void numint_init_set_int(numint_t a, long int i)
{ mpz_init_set_si(a,i); }

static inline void numint_clear(numint_t a)
{ mpz_clear(a); }
static inline void numint_clear_array(numint_t* a, size_t size)
{ 
  size_t i; 
  for (i=0; i<size; i++) mpz_clear(a[i]); 
}

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numint_neg(numint_t a, const numint_t b)
{ mpz_neg(a,b); }
static inline void numint_abs(numint_t a, const numint_t b)
{ mpz_abs(a,b); }
static inline void numint_add(numint_t a, const numint_t b, const numint_t c)
{ mpz_add(a,b,c); }
static inline void numint_add_uint(numint_t a, const numint_t b, unsigned long int c)
{ mpz_add_ui(a,b,c); }
static inline void numint_sub(numint_t a, const numint_t b, const numint_t c)
{ mpz_sub(a,b,c); }
static inline void numint_sub_uint(numint_t a, const numint_t b, unsigned long int c)
{ mpz_sub_ui(a,b,c); }
static inline void numint_mul(numint_t a, const numint_t b, const numint_t c)
{ mpz_mul(a,b,c); }
static inline void numint_mul_2(numint_t a, const numint_t b)
{ mpz_mul_2exp(a,b,1); }
static inline void numint_fdiv_q(numint_t a, const numint_t b, const numint_t c)
{ mpz_fdiv_q(a,b,c); }
static inline void numint_cdiv_q(numint_t a, const numint_t b, const numint_t c)
{ mpz_cdiv_q(a,b,c); }
static inline void numint_cdiv_qr(numint_t a, numint_t b, const numint_t c, const numint_t d)
{ mpz_cdiv_qr(a,b,c,d); }
static inline void numint_cdiv_2(numint_t a, const numint_t b)
{ mpz_cdiv_q_2exp(a,b,1); }
static inline void numint_cdiv_q_2exp(numint_t a, const numint_t b, unsigned long int c)
{ mpz_cdiv_q_2exp(a,b,c); }
static inline void numint_fdiv_q_2exp(numint_t a, const numint_t b, unsigned long int c)
{ mpz_fdiv_q_2exp(a,b,c); }
static inline void numint_min(numint_t a, const numint_t b, const numint_t c)
{ mpz_set(a, mpz_cmp(b,c)<=0 ? b : c); }
static inline void numint_max(numint_t a, const numint_t b, const numint_t c)
{ mpz_set(a, mpz_cmp(b,c)>=0 ? b : c); }

/* ====================================================================== */
/* Arithmetic Integer Operations */
/* ====================================================================== */

static inline void numint_divexact(numint_t a, const numint_t b, const numint_t c)
{ mpz_divexact(a,b,c); }
static inline void numint_mod(numint_t a, const numint_t b, const numint_t c)
{ mpz_mod(a,b,c); }
static inline void numint_gcd(numint_t a, const numint_t b,  const numint_t c)
{ mpz_gcd(a,b,c); }

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numint_sgn(const numint_t a)
{ return mpz_sgn(a); }
static inline int numint_cmp(const numint_t a, const numint_t b)
{ return mpz_cmp(a,b); }
static inline int numint_cmp_int(const numint_t a, long int b)
{ return mpz_cmp_si(a,b); }
static inline bool numint_equal(const numint_t a, const numint_t b)
{ return mpz_cmp(a,b)==0; }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numint_print(const numint_t a)
{ mpz_out_str(stdout,10,a); }
static inline void numint_fprint(FILE* stream, const numint_t a)
{ mpz_out_str(stream,10,a); }
static inline int numint_snprint(char* s, size_t size, const numint_t a)
{ 
  int res;
  if (mpz_sizeinbase(a,10)+2>size) 
    res = snprintf(s,size, mpz_sgn(a)>0 ? "+BIG" : "-BIG");
  else {
    mpz_get_str(s,10,a);
    res = strlen(s);
  }
  return res;
}

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
}

/* mpz -> numint */
static inline bool numint_set_mpz(numint_t a, const mpz_t b)
{ mpz_set(a,b); return true; }

/* mpq -> numint */
static inline bool numint_set_mpq_tmp(numint_t a, const mpq_t b, 
				      mpz_t q, mpz_t r)
{
  mpz_cdiv_qr(a, r, mpq_numref(b),mpq_denref(b));
  bool res = (mpz_sgn(r)==0);
  return res;
}
static inline bool numint_set_mpq(numint_t a, const mpq_t b)
{ 
  mpz_t r;
  mpz_init(r);
  bool res = numint_set_mpq_tmp(a,b,r,r);
  mpz_clear(r);
  return res;
}

/* double -> numint */
static inline bool numint_set_double(numint_t a, double b)
{
  double c = ceil(b);
  mpz_set_d(a,c); 
  return (b==c);
}

/* numint -> int */
static inline bool int_set_numint(long int* a, const numint_t b)
{ *a = mpz_get_si(b); return true; }

/* numint -> mpz */
static inline bool mpz_set_numint(mpz_t a, const numint_t b)
{ mpz_set(a,b); return true; }
/* numint -> mpq */
static inline bool mpq_set_numint(mpq_t a, const numint_t b)
{ 
  mpz_set(mpq_numref(a),b);
  mpz_set_ui(mpq_denref(a),1);
  return true;
}
/* numint -> double */
/* mpfr is supposed to have exactly the IEEE754 double precision of 53 bits */
static inline bool double_set_numint_tmp(double* a, const numint_t b, 
					 mpfr_t mpfr)
{
  int res = mpfr_set_z(mpfr,b,+1);
  *a = mpfr_get_d(mpfr,+1);/* Normally, exact conversion here (unless overfloww) */
  return (res==0);
}
static inline bool double_set_numint(double* a, const numint_t b)
{
  mpfr_t mpfr;
  
  mpfr_init2(mpfr,53);
  bool res = double_set_numint_tmp(a,b,mpfr);
  mpfr_clear(mpfr);
  return res;
}

static inline bool mpz_fits_numint(const mpz_t a)
{ return true; }
static inline bool mpq_fits_numint_tmp(const mpq_t a, mpz_t mpz)
{ return true; }
static inline bool mpq_fits_numint(const mpq_t a)
{ return true; }
static inline bool double_fits_numint(double a)
{ return true; }
static inline bool numint_fits_int(const numint_t a)
{ return mpz_fits_slong_p(a); }
static inline bool numint_fits_double(const numint_t a)
{  
  double d = mpz_get_d(a);
  return fabs(d) != (double)1.0/(double)0.0;
}

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline unsigned char numint_serialize_id(void)
{ return 0xf; }

static inline size_t numint_serialize(void* dst, const numint_t src)
{ 
  size_t count = 0;
  *((char*)dst) = mpz_sgn(src);
  mpz_export((char*)dst+5,&count,1,1,1,0,src);
  num_dump_word32((char*)dst+1,count);
  return count+5;
}

static inline size_t numint_deserialize(numint_t dst, const void* src) 
{
  size_t count = num_undump_word32((const char*)src+1);
  mpz_import(dst,count,1,1,1,0,(const char*)src+5);
  if (*(const char*)src<0)
    mpz_neg(dst,dst);
  return count+5;
}

/* not the exact size of serialized data, but a sound overapproximation */
static inline size_t numint_serialized_size(const numint_t a) 
{ return mpz_sizeinbase(a,2)/8+5+sizeof(mp_limb_t); }

#endif
