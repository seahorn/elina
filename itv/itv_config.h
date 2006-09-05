
#ifndef _ITV_CONFIG_H_
#define _ITV_CONFIG_H_

#include "num.h"
#include "bound.h"
#include "ap_global0.h"

#ifdef __cplusplus
#define HAS_BOOL
extern "C" {
#endif

int static inline num_size(const num_t a)
#if defined(NUM_MPQ)
{ return mpz_size(mpq_numref(a)) + mpz_size(mpq_denref(a)); }
#elif defined(NUM_LONGLONGRAT)
{ return 2; }
#elif defined(NUM_DOUBLE)
{ return 2; }
#else
#error "Here"
#endif

#ifndef HAS_BOOL
#define HAS_BOOL

typedef enum bool {
  false=0,
  true=1
} bool;
#endif

#ifdef __cplusplus
}
#endif

#endif
