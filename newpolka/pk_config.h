
#ifndef _PK_CONFIG_H_
#define _PK_CONFIG_H_

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include "numint.h"
#include "numrat.h"
#include <gmp.h>

#include "ap_coeff.h"
#include "ap_dimension.h"
#include "ap_expr0.h"
#include "ap_manager.h"

/* Extension to the num package */
static inline size_t numint_size(numint_t a)
#if defined (NUMINT_NATIVE)
{ return 1; }
#elif defined(NUMINT_MPZ)
{ return mpz_size(a); }
#else
#error "Here"
#endif

/* Do not change ! */
static const int polka_cst = 1;
static const int polka_eps = 2;





#ifndef HAS_BOOL
#define HAS_BOOL

typedef enum bool {
  false=0,
  true=1
} bool;
#endif



#endif

