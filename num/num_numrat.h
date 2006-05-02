/* ********************************************************************** */
/* num_numrat.h */
/* ********************************************************************** */

#ifndef _NUM_NUMRAT_H_
#define _NUM_NUMRAT_H_

#ifndef _NUM_H_
#error "File num_numrat.h should not be included directly, only via num.h"
#endif

#if defined(NUM_LONGRAT)
#define NUMINT_LONGINT

#elif defined(NUM_LONGLONGRAT)
#define NUMINT_LONGLONGINT

#elif defined(NUM_MPQ)
#define NUMINT_MPZ

#else
#error "HERE"
#endif

#include "numrat.h"
typedef numrat_t num_t;

#define NUM_NUMRAT

#ifdef NUMINT_NATIVE
#define NUM_NATIVE
#endif

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */

static inline void num_set(num_t a, const num_t b)
              { numrat_set(a,b); }
static inline void num_set_array(num_t* a, const num_t* b, size_t size)
              { numrat_set_array(a,b,size); }
static inline void num_set_int(num_t a, long int i)
              { numrat_set_int(a,i); }

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void num_init(num_t a)
              { numrat_init(a); }
static inline void num_init_array(num_t* a, size_t size)
              { numrat_init_array(a,size); }
static inline void num_init_set(num_t a, const num_t b)
              { numrat_init_set(a,b); }
static inline void num_init_set_int(num_t a, long int i)
              { numrat_init_set_int(a,i); }

static inline void num_clear(num_t a)
              { numrat_clear(a); }
static inline void num_clear_array(num_t* a, size_t size)
              { numrat_clear_array(a,size); }

static inline void num_swap(num_t a, num_t b)
              { numrat_swap(a,b); }

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void num_neg(num_t a, const num_t b)
              { numrat_neg(a,b); }
static inline void num_abs(num_t a, const num_t b)
              { numrat_abs(a,b); }
static inline void num_add(num_t a, const num_t b, const num_t c)
              { numrat_add(a,b,c); }
static inline void num_add_uint(num_t a, const num_t b, unsigned long int c)
              { numrat_add_uint(a,b,c); }
static inline void num_sub(num_t a, const num_t b, const num_t c)
              { numrat_sub(a,b,c); }
static inline void num_sub_uint(num_t a, const num_t b, unsigned long int c)
              { numrat_sub_uint(a,b,c); }
static inline void num_mul(num_t a, const num_t b, const num_t c)
              { numrat_mul(a,b,c); }
static inline void num_mul_2(num_t a, const num_t b)
              { numrat_mul_2(a,b); }
static inline void num_div(num_t a, const num_t b, const num_t c)
              { numrat_div(a,b,c); }
static inline void num_div_2(num_t a, const num_t b)
              { numrat_div_2(a,b); }
static inline void num_min(num_t a, const num_t b, const num_t c)
              { numrat_min(a,b,c); }
static inline void num_max(num_t a, const num_t b, const num_t c)
              { numrat_max(a,b,c); }
static inline void num_floor(num_t a, const num_t b)
              { numrat_floor(a,b); }
static inline void num_ceil(num_t a, const num_t b)
              { numrat_ceil(a,b); }
  /* Approximate to the nearest integer toward resp. -infty, +infty */

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int num_sgn(const num_t a)
         { return numrat_sgn(a); }
static inline int num_cmp(const num_t a, const num_t b)
      { return numrat_cmp(a,b); }
static inline int num_cmp_int(const num_t a, long int b)
      { return numrat_cmp_int(a,b); }
static inline int num_equal(const num_t a, const num_t b)
      { return numrat_equal(a,b); }

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void num_print(const num_t a)
              { numrat_print(a); }
static inline void num_fprint(FILE* stream, const num_t a)
              { numrat_fprint(stream, a); }
static inline int num_snprint(char* s, size_t size, const num_t a)
      { return numrat_snprint(s,size,a); }

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

static inline void num_set_int2(num_t a, long int i, unsigned long int j)
              { numrat_set_int2(a,i,j); }
  /* int2 -> num */

static inline bool mpz_fits_num(const mpz_t a)
          { return mpz_fits_numrat(a); }
static inline void num_set_mpz(num_t a, const mpz_t b)
              { numrat_set_mpz(a,b); }
  /* mpz -> num */

static inline bool mpq_fits_num(const mpq_t a)
          { return mpq_fits_numrat(a); }
static inline void num_set_mpq(num_t a, const mpq_t b)
              { numrat_set_mpq(a,b); }
  /* mpq -> num */

static inline bool double_fits_num(double a)
          { return double_fits_numrat(a); }
static inline void num_set_double(num_t a, double k)
              { numrat_set_double(a,k); }
  /* double -> num */

static inline bool num_fits_int(const num_t a)
       { return numrat_fits_int(a); }
static inline long int num_get_int(const num_t a)
           { return numrat_get_int(a); }
  /* num -> int */
static inline void mpz_set_num(mpz_t a, const num_t b)
                 { mpz_set_numrat(a,b); }
  /* num -> mpz */
static inline void mpq_set_num(mpq_t a, const num_t b)
                 { mpq_set_numrat(a,b); }
  /* num -> mpq */
static inline bool num_fits_double(const num_t a)
       { return numrat_fits_double(a); }
static inline double num_get_double(const num_t a)
         { return numrat_get_double(a); }
  /* num -> double */


/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline unsigned char num_serialize_id(void)
{ return numrat_serialize_id(); }

static inline size_t num_serialize(void* dst, const num_t src)
{ return numrat_serialize(dst,src); }

static inline size_t num_deserialize(num_t dst, const void* src)
{ return numrat_deserialize(dst,src); }

static inline size_t num_serialized_size(const num_t a)
{ return numrat_serialized_size(a); }

#endif
