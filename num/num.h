/* ********************************************************************** */
/* num.h */
/* ********************************************************************** */

#ifndef _NUM_H_
#define _NUM_H_

#include <stdio.h>

#include "gmp.h"
#include "mpfr.h"

/* Be aware: 
   we erase the (obsolete) GMP function mpq_set_num */
#undef mpq_set_num

#include "num_config.h"

#if defined(NUM_LONGINT) || defined(NUM_LONGLONGINT) || defined(NUM_MPZ)
#include "num_numint.h"
#elif defined(NUM_LONGRAT) || defined(NUM_LONGLONGRAT) || defined(NUM_MPQ)
#include "num_numrat.h"
#elif defined(NUM_DOUBLE) || defined(NUM_LONGDOUBLE)
#include "num_numflt.h"
#else
#error "HERE"
#endif

/* Parameter: one of the following macros should be defined:
   NUM_LONGINT,
   NUM_LONGLONGINT,
   NUM_MPZ,

   NUM_LONGRAT,
   NUM_LONGLONGRAT,
   NUM_MPQ,

   NUM_DOUBLE,
   NUM_LONGDOUBLE

   Each parameter induces the definition of the following macros:
   NUM_LONGINT:     NUM_NUMINT, NUMINT_LONGINT,     NUM_MAX, NUMINT_MAX, NUM_NATIVE, NUMINT_NATIVE
   NUM_LONGLONGINT: NUM_NUMINT, NUMINT_LONGLONGINT, NUM_MAX, NUMINT_MAX, NUM_NATIVE, NUMINT_NATIVE
   NUM_MPZ:         NUM_NUMINT, NUMINT_MPZ 

   NUM_LONGRAT:     NUM_NUMRAT, NUMINT_LONGINT,     NUM_NATIVE, NUMINT_NATIVE
   NUM_LONGLONGRAT: NUM_NUMRAT, NUMINT_LONGLONGINT, NUM_NATIVE, NUMINT_NATIVE
   NUM_MPQ:         NUM_NUMRAT, NUMINT_MPZ 

   NUM_DOUBLE:      NUM_NUMFLT, NUMFLT_DOUBLE,      NUM_MAX, NUMFLT_MAX, NUM_NATIVE, NUMFLT_NATIVE
   NUM_LONGDOUBLE:  NUM_NUMFLT, NUMFLT_LONGDOUBLE,  NUM_MAX, NUMFLT_MAX, NUM_NATIVE, NUMFLT_NATIVE
*/

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void num_set(num_t a, const num_t b);
static inline void num_set_array(num_t* a, const num_t* b, size_t size);
static inline void num_set_int(num_t a, long int i);

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void num_init(num_t a);
static inline void num_init_array(num_t* a, size_t size);
static inline void num_init_set(num_t a, const num_t b);
static inline void num_init_set_int(num_t a, long int i);

static inline void num_clear(num_t a);
static inline void num_clear_array(num_t* a, size_t size);

static inline void num_swap(num_t a, num_t b);

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void num_neg(num_t a, const num_t b);
static inline void num_abs(num_t a, const num_t b);
static inline void num_add(num_t a, const num_t b, const num_t c);
static inline void num_add_uint(num_t a, const num_t b, unsigned long int c);
static inline void num_sub(num_t a, const num_t b, const num_t c);
static inline void num_sub_uint(num_t a, const num_t b, unsigned long int c);
static inline void num_mul(num_t a, const num_t b, const num_t c);
static inline void num_mul_2(num_t a, const num_t b);
static inline void num_div(num_t a, const num_t b, const num_t c);
static inline void num_div_2(num_t a, const num_t b);
static inline void num_min(num_t a, const num_t b, const num_t c);
static inline void num_max(num_t a, const num_t b, const num_t c);
static inline void num_floor(num_t a, const num_t b);
static inline void num_ceil(num_t a, const num_t b);
  /* Approximate to the nearest integer toward resp. -infty, +infty */

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int num_sgn(const num_t a);
static inline int num_cmp(const num_t a, const num_t b);
static inline int num_cmp_int(const num_t a, long int b);
static inline bool num_equal(const num_t a, const num_t b);

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void num_print(const num_t a);
static inline void num_fprint(FILE* stream, const num_t a);
static inline int num_snprint(char* s, size_t size, const num_t a);

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

static inline void num_set_int2(num_t a, long int i, unsigned long int j);
  /* int2 -> num */
static inline bool num_set_mpz(num_t a, const mpz_t b);
  /* mpz -> num */
static inline bool num_set_mpq(num_t a, const mpq_t b);
  /* mpq -> num */
static inline bool num_set_double(num_t a, double b);
  /* double -> num */

static inline bool int_set_num(long int* a, const num_t b);
  /* num -> int */
static inline bool mpz_set_num(mpz_t a, const num_t b);
  /* num -> mpz */
static inline bool mpq_set_num(mpq_t a, const num_t b);
  /* num -> mpq */
static inline bool double_set_num(double* a, const num_t b);
  /* num -> double */

static inline bool mpz_fits_num(const mpz_t a);
static inline bool mpq_fits_num(const mpq_t a);
static inline bool double_fits_num(double a);
static inline bool num_fits_int(const num_t a);
static inline bool num_fits_double(const num_t a);

/* Optimized versions */
/* mpfr should have exactly the precision NUMFLT_MANT_DIG */
static inline bool num_set_mpz_tmp(num_t a, const mpz_t b, mpfr_t mpfr);
static inline bool num_set_mpq_tmp(num_t a, const mpq_t b, 
				   mpz_t q, mpz_t r, mpfr_t mpfr);
static inline bool num_set_double_tmp(num_t a, double k, mpq_t mpq);

static inline bool int_set_num_tmp(long int* a, const num_t b, 
				   mpz_t q, mpz_t r);
static inline bool mpz_set_num_tmp(mpz_t a, const num_t b, mpz_t mpz);
/* mpfr should have exactly the precision DBL_MANT_DIG */
static inline bool double_set_num_tmp(double* a, const num_t b, 
				      mpq_t mpq, mpfr_t mpfr);

static inline bool mpq_fits_num_tmp(const mpq_t a, mpz_t mpz);
static inline bool double_fits_num_tmp(double k, mpq_t mpq);

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

/* Notes:
   - call _serialized_size to get the minimal size of the buffer to allocate
   - _serialize then returns the actual size of the serialized data (may be 
   less than _serialized_size)
   - deserialze assumes that the num destination has already been initialized
   - when deserializing an array, you must supply the number of elements in the
   array (it is not stored with the array by serialize)
   - serialization is not type-safe (it depends on the chosen numerical type
   and, for types using native integers, on whether the machine is 32 or 64 
   bits). You can only deserialize data if the num_serialize_id function 
   returns the same value as when the data was serialized. (This id is not
   stored systematically with each serialized number to save state, so, the
   checking is up to you.)
*/

static inline unsigned char num_serialize_id(void);

static inline size_t num_serialize(void* dst, const num_t src);
static inline size_t num_deserialize(num_t dst, const void* src);
static inline size_t num_serialized_size(const num_t a);

static inline size_t num_serialize_array(void* dst, const num_t* src, size_t size)
{
  size_t i,n=0;
  for (i=0;i<size;i++)
    n += num_serialize((char*)dst+n,src[i]);
  return n;
}

static inline size_t num_deserialize_array(num_t* dst, const void* src, size_t size)
{
  size_t i,n=0;
  for (i=0;i<size;i++)
    n += num_deserialize(dst[i],(const char*)src+n);
  return n;
}

static inline size_t num_serialized_size_array(const num_t* src, size_t size)
{
  size_t i,n=0;
  for (i=0;i<size;i++)
    n += num_serialized_size(src[i]);
  return n;
}

#endif
