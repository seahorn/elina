/* ********************************************************************** */
/* numint.h */
/* ********************************************************************** */

#ifndef _NUMINT_H_
#define _NUMINT_H_

#include <gmp.h>

#include "num_config.h"

#if defined(NUMINT_LONGINT)
#include "numint_longint.h"

#elif defined(NUMINT_LONGLONGINT)
#include "numint_longlongint.h"

#elif defined(NUMINT_MPZ)
#include "numint_mpz.h"

#else
#error "HERE"
#endif

/*
   NUMINT_INT and NUMINT_LONG also define

   - NUMINT_MAX: max positive value.
     It is assumed that (-NUMINT_MAX) is also repreentable

   - NUMINT_NATIVE: no heap allocated memory
*/

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void numint_set(numint_t a, const numint_t b);
static inline void numint_set_array(numint_t* a, const numint_t* b, size_t size);
static inline void numint_set_int(numint_t a, long int i);

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void numint_init(numint_t a);
static inline void numint_init_array(numint_t* a, size_t size);
static inline void numint_init_set(numint_t a, const numint_t b);
static inline void numint_init_set_int(numint_t a, long int i);

static inline void numint_clear(numint_t a);
static inline void numint_clear_array(numint_t* a, size_t size);

static inline void numint_swap(numint_t a, numint_t b)
{ numint_t t; *t=*a;*a=*b;*b=*t; }

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void numint_neg(numint_t a, const numint_t b);
static inline void numint_abs(numint_t a, const numint_t b);
static inline void numint_add(numint_t a, const numint_t b, const numint_t c);
static inline void numint_add_uint(numint_t a, const numint_t b, unsigned long int c);
static inline void numint_sub(numint_t a, const numint_t b, const numint_t c);
static inline void numint_sub_uint(numint_t a, const numint_t b, unsigned long int c);
static inline void numint_mul(numint_t a, const numint_t b, const numint_t c);
static inline void numint_mul_2(numint_t a, const numint_t b);
static inline void numint_fdiv_q(numint_t a, const numint_t b, const numint_t c);
static inline void numint_cdiv_q(numint_t a, const numint_t b, const numint_t c);
static inline void numint_cdiv_2(numint_t a, const numint_t b);
static inline void numint_min(numint_t a, const numint_t b, const numint_t c);
static inline void numint_max(numint_t a, const numint_t b, const numint_t c);

/* ====================================================================== */
/* Arithmetic Integer Operations */
/* ====================================================================== */

static inline void numint_divexact(numint_t a, const numint_t b, const numint_t c);
static inline void numint_mod(numint_t a, const numint_t b, const numint_t c);
static inline void numint_gcd(numint_t a, const numint_t b,  const numint_t c);

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int numint_sgn(const numint_t a);
static inline int numint_cmp(const numint_t a, const numint_t b);
static inline int numint_cmp_int(const numint_t a, long int b);
static inline int numint_equal(const numint_t a, const numint_t b);

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void numint_print(const numint_t a);
static inline void numint_fprint(FILE* stream, const numint_t a);
static inline int numint_snprint(char* s, size_t size, const numint_t a);

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

static inline void numint_set_int2(numint_t a, long int i, unsigned long int j);
  /* int2 -> numint */

static inline bool mpz_fits_numint(const mpz_t a);
static inline void numint_set_mpz(numint_t a, const mpz_t b);
  /* mpz -> numint */

static inline bool mpq_fits_numint(const mpq_t a);
static inline void numint_set_mpq(numint_t a, const mpq_t b);
  /* mpq -> numint */

static inline bool double_fits_numint(double a);
static inline void numint_set_double(numint_t a, double b);
  /* double -> numint */

static inline bool numint_fits_int(const numint_t a);
static inline long int numint_get_int(const numint_t a);
  /* numint -> int */

static inline void mpz_set_numint(mpz_t a, const numint_t b);
  /* numint -> mpz */

static inline void mpq_set_numint(mpq_t a, const numint_t b);
  /* numint -> mpq */

static inline bool numint_fits_double(const numint_t a);
static inline double numint_get_double(const numint_t a);
  /* numint -> double */

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

static inline unsigned char numint_serialize_id(void);
static inline size_t numint_serialize(void* dst, const numint_t src);
static inline size_t numint_deserialize(numint_t dst, const void* src);
static inline size_t numint_serialized_size(const numint_t a);

#endif
