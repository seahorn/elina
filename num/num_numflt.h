/* ********************************************************************** */
/* num_numflt.h */
/* ********************************************************************** */

#ifndef _NUM_NUMFLT_H_
#define _NUM_NUMFLT_H_

#ifndef _NUM_H_
#error "File num_numflt.h should not be included directly, only via num.h"
#endif

#if defined(NUM_DOUBLE)
#define NUMFLT_DOUBLE

#elif defined(NUM_LONGDOUBLE)
#define NUMFLT_LONGDOUBLE

#else
#error "HERE"
#endif

#include "numflt.h"

typedef numflt_t num_t;

#define NUM_NUMFLT

#ifdef NUMFLT_MAX
#define NUM_MAX NUMFLT_MAX
#endif

#ifdef NUMFLT_NATIVE
#define NUM_NATIVE
#endif

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */

static inline void num_set(num_t a, const num_t b)
              { numflt_set(a,b); }
static inline void num_set_array(num_t* a, const num_t* b, size_t size)
              { numflt_set_array(a,b,size); }
static inline void num_set_int(num_t a, long int i)
              { numflt_set_int(a,i); }

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void num_init(num_t a)
              { numflt_init(a); }
static inline void num_init_array(num_t* a, size_t size)
              { numflt_init_array(a,size); }
static inline void num_init_set(num_t a, const num_t b)
              { numflt_init_set(a,b); }
static inline void num_init_set_int(num_t a, long int i)
              { numflt_init_set_int(a,i); }

static inline void num_clear(num_t a)
              { numflt_clear(a); }
static inline void num_clear_array(num_t* a, size_t size)
              { numflt_clear_array(a,size); }

static inline void num_swap(num_t a, num_t b)
              { numflt_swap(a,b); }

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void num_neg(num_t a, const num_t b)
              { numflt_neg(a,b); }
static inline void num_abs(num_t a, const num_t b)
              { numflt_abs(a,b); }
static inline void num_add(num_t a, const num_t b, const num_t c)
              { numflt_add(a,b,c); }
static inline void num_add_uint(num_t a, const num_t b, unsigned long int c)
              { numflt_add_uint(a,b,c); }
static inline void num_sub(num_t a, const num_t b, const num_t c)
              { numflt_sub(a,b,c); }
static inline void num_sub_uint(num_t a, const num_t b, unsigned long int c)
              { numflt_sub_uint(a,b,c); }
static inline void num_mul(num_t a, const num_t b, const num_t c)
              { numflt_mul(a,b,c); }
static inline void num_mul_2(num_t a, const num_t b)
              { numflt_mul_2(a,b); }
static inline void num_div(num_t a, const num_t b, const num_t c)
              { numflt_div(a,b,c); }
static inline void num_div_2(num_t a, const num_t b)
              { numflt_div_2(a,b); }
static inline void num_min(num_t a, const num_t b, const num_t c)
              { numflt_min(a,b,c); }
static inline void num_max(num_t a, const num_t b, const num_t c)
              { numflt_max(a,b,c); }
static inline void num_floor(num_t a, const num_t b)
              { numflt_floor(a,b); }
static inline void num_ceil(num_t a, const num_t b)
              { numflt_ceil(a,b); }
  /* Approximate to the nearest integer toward resp. -infty, +infty */

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int num_sgn(const num_t a)
         { return numflt_sgn(a); }
static inline int num_cmp(const num_t a, const num_t b)
      { return numflt_cmp(a,b); }
static inline int num_cmp_int(const num_t a, long int b)
      { return numflt_cmp_int(a,b); }
static inline bool num_equal(const num_t a, const num_t b)
      { return numflt_equal(a,b); }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void num_print(const num_t a)
              { numflt_print(a); }
static inline void num_fprint(FILE* stream, const num_t a)
              { numflt_fprint(stream, a); }
static inline int num_snprint(char* s, size_t size, const num_t a)
      { return numflt_snprint(s,size,a); }

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

static inline void num_set_int2(num_t a, long int i, unsigned long int j)
              { numflt_set_int2(a,i,j); }
  /* int2 -> num */

static inline bool mpz_fits_num(const mpz_t a)
          { return mpz_fits_numflt(a); }
static inline bool num_set_mpz(num_t a, const mpz_t b)
          { return numflt_set_mpz(a,b); }
  /* mpz -> num */

static inline bool mpq_fits_num(const mpq_t a)
          { return mpq_fits_numflt(a); }
static inline bool num_set_mpq(num_t a, const mpq_t b)
          { return numflt_set_mpq(a,b); }
  /* mpq -> num */

static inline bool double_fits_num(double a)
          { return double_fits_numflt(a); }
static inline bool num_set_double(num_t a, double k)
          { return numflt_set_double(a,k); }
  /* double -> num */

static inline bool num_fits_int(const num_t a)
          { return numflt_fits_int(a); }
static inline bool int_set_num(long int*a, const num_t b)
          { return int_set_numflt(a,b); }
  /* num -> int */
static inline bool mpz_set_num(mpz_t a, const num_t b)
          { return mpz_set_numflt(a,b); }
  /* num -> mpz */
static inline bool mpq_set_num(mpq_t a, const num_t b)
          { return mpq_set_numflt(a,b); }
  /* num -> mpq */
static inline bool num_fits_double(const num_t a)
          { return numflt_fits_double(a); }
static inline bool double_set_num(double* a, const num_t b)
          { return double_set_numflt(a,b); }
  /* num -> double */

/* Optimized versions */
static inline bool num_set_mpz_tmp(numflt_t a, const mpz_t b, mpfr_t mpfr)
{ return numflt_set_mpz_tmp(a,b,mpfr); }
static inline bool num_set_mpq_tmp(numflt_t a, const mpq_t b, 
				   mpz_t q, mpz_t r, mpfr_t mpfr)
{ return numflt_set_mpq_tmp(a,b,mpfr); }


static inline bool mpq_fits_num_tmp(const mpq_t a, mpz_t mpz)
{ return mpq_fits_numflt(a); }
static inline bool num_set_double_tmp(num_t a, double k, mpq_t mpq)
{ return numflt_set_double(a,k); }
static inline bool int_set_num_tmp(long int* a, const num_t b, 
				   mpz_t q, mpz_t r)
{ return int_set_numflt(a,b); }
static inline bool mpz_set_num_tmp(mpz_t a, const num_t b, mpz_t mpz)
{ return mpz_set_numflt(a,b); }
static inline bool double_set_num_tmp(double* a, const num_t b, 
				      mpq_t mpq, mpfr_t mpfr)
{ return double_set_numflt(a,b); }
static inline bool double_fits_num_tmp(double k, mpq_t mpq)
{ return double_fits_numflt(k); }


/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline unsigned char num_serialize_id(void)
{ return numflt_serialize_id(); }

static inline size_t num_serialize(void* dst, const num_t src)
{ return numflt_serialize(dst,src); }

static inline size_t num_deserialize(num_t dst, const void* src)
{ return numflt_deserialize(dst,src); }

static inline size_t num_serialized_size(const num_t a)
{ return numflt_serialized_size(a); }

#endif
