/* ********************************************************************** */
/* itv.h: (unidimensional) intervals */
/* ********************************************************************** */

#ifndef _ITV_H_
#define _ITV_H_

#include <stdio.h>
#include "num.h"
#include "bound.h"
#include "itv_config.h"
#include "ap_global0.h"

#if defined(NUM_LONGINT)
#define NUM_SUFFIX li
#elif defined(NUM_LONGLONGINT)
#define NUM_SUFFIX lli
#elif defined(NUM_MPZ)
#define NUM_SUFFIX mpz
#elif defined(NUM_LONGRAT)
#define NUM_SUFFIX lr
#elif defined(NUM_LONGLONGRAT)
#define NUM_SUFFIX llr
#elif defined(NUM_MPQ)
#define NUM_SUFFIX mpq
#elif defined(NUM_DOUBLE)
#define NUM_SUFFIX dbl
#elif defined(NUM_LONGDOUBLE)
#define NUM_SUFFIX ldbl
#else
#error "HERE"
#endif

/* ITVFUN(add) expands to itvmpq_add, or itvli_add, ... */
#define ITVFUN3(SUFFIX,NAME) itv ## SUFFIX ## _ ## NAME
#define ITVFUN2(x,y) ITVFUN3(x,y)
#define ITVFUN(x) ITVFUN2(NUM_SUFFIX,x)
/* ITVAPFUN(ap_coeff_set_itv) expands to ap_coeff_set_itvmpq */
#define ITVAPFUN3(SUFFIX,NAME) NAME ## SUFFIX
#define ITVAPFUN2(x,y) ITVAPFUN3(x,y)
#define ITVAPFUN(x) ITVAPFUN2(NUM_SUFFIX,x)

#ifdef __cplusplus
extern "C" {
#endif

/* Be cautious: interval [a,b] is represented by [-a,b].  This is because
   bound quantities are always rounded toward +infty */

struct itv_t {
  bound_t inf; /* negation of the inf bound */
  bound_t sup; /* sup bound */
};
typedef struct itv_t itv_t[1];
typedef struct itv_t* itv_ptr;


/* Workspace to avoid temporary allocation and deallocation when num_t and
   bound_t are multiprecision numbers */
typedef struct itv_internal_t {
  num_t canonicalize_num;
  bound_t muldiv_bound;
  bound_t mul_bound;
  itv_t mul_itv;
  itv_t mul_itv2;
  ap_scalar_t* ap_conversion_scalar;
  bound_t ap_conversion_bound;
  itv_t eval_itv;
  itv_t eval_itv2;
} itv_internal_t;


static inline itv_internal_t* itv_internal_alloc();
  /* Allocate and initialize internal workspace */
static inline void itv_internal_free(itv_internal_t* intern);
  /* Clear and free internal workspace */

static inline void itv_internal_init(itv_internal_t* intern);
static inline void itv_internal_clear(itv_internal_t* intern);
  /* GMP Semantics */

/* ********************************************************************** */
/* itv */
/* ********************************************************************** */

/* Initialization and clearing */
static inline void itv_init(itv_t a);
static inline void itv_init_array(itv_t* a, size_t size);
static inline void itv_init_set(itv_t a, const itv_t b);
static inline void itv_clear(itv_t a);
static inline void itv_clear_array(itv_t* a, size_t size);
static inline void itv_array_free(itv_t* a, size_t size);

/* Assignement */
static inline void itv_set(itv_t a, const itv_t b);
static inline void itv_set_bottom(itv_t a);
static inline void itv_set_top(itv_t a);
static inline void itv_swap(itv_t a, itv_t b);

/* Normalization and tests */
static inline bool itv_canonicalize(itv_internal_t* intern,
				    itv_t a, bool integer);
  /* Canonicalize an interval:
     - if integer is true, narrows bound to integers
     - return true if the interval is bottom
     - return false otherwise
  */
static inline bool itv_is_top(const itv_t a);
static inline bool itv_is_bottom(itv_internal_t* intern, itv_t a);
  /* Return true iff the interval is resp. [-oo,+oo] or empty */
static inline bool itv_is_point(itv_internal_t* intern, const itv_t a);
  /* Return true iff the interval is a single point */
static inline bool itv_is_leq(const itv_t a, const itv_t b);
  /* Inclusion test */
static inline bool itv_is_eq(const itv_t a, const itv_t b);
  /* Equality test */

/* Lattice operations */
static inline bool itv_meet(itv_internal_t* intern,
			    itv_t a, const itv_t b, const itv_t c);
  /* Assign a with the intersection of b and c */
static inline void itv_join(itv_t a, const itv_t b, const itv_t c);
  /* Assign a with the union of b and c */
static inline void itv_widening(itv_t a, const itv_t b, const itv_t c);
  /* Assign a with the standard interval widening of b by c */

/* Arithmetic operations */
static inline void itv_add(itv_t a, const itv_t b, const itv_t c);
static inline void itv_sub(itv_t a, const itv_t b, const itv_t c);
static inline void itv_neg(itv_t a, const itv_t b);
static inline void itv_mul(itv_internal_t* intern,
			   itv_t a, const itv_t b, const itv_t c);
static inline void itv_add_bound(itv_t a, const itv_t b, const bound_t c);
static inline void itv_mul_bound(itv_internal_t* intern,
				 itv_t a, const itv_t b, const bound_t c);
static inline void itv_div_bound(itv_internal_t* intern,
				 itv_t a, const itv_t b, const bound_t c);
  

/* Printing */
static inline int itv_snprint(char* s, size_t size, const itv_t a);
static inline void itv_fprint(FILE* stream, const itv_t a);
static inline void itv_print(const itv_t a);

/* All these functions return true if the conversion is exact */
static inline bool itv_set_ap_scalar(itv_internal_t* intern,
				     itv_t a, const ap_scalar_t* b);
  /* Convert a ap_scalar_t into a itv_t.
     Assumes the scalar is finite.
     If it returns true, the interval is a single point */
static inline bool itv_set_ap_interval(itv_internal_t* intern,
				       itv_t a, const ap_interval_t* b);
  /* Convert a ap_interval_t into a itv_t */
static inline bool itv_set_ap_coeff(itv_internal_t* intern,
				    itv_t a, const ap_coeff_t* b);
  /* Convert a ap_coeff_t into a itv_t. */

static inline bool ap_interval_set_itv(itv_internal_t* intern,
				       ap_interval_t* a, const itv_t b);
  /* Convert a itv_t into a ap_interval_t */
static inline bool ap_coeff_set_itv(itv_internal_t* intern,
				    ap_coeff_t* a, const itv_t b);
  /* Convert a itv_t into a ap_coeff_t */

/* ********************************************************************** */
/* Definition of inline functions */
/* ********************************************************************** */

void ITVFUN(internal_init)(itv_internal_t* intern);
static inline void itv_internal_init(itv_internal_t* intern)
{ ITVFUN(internal_init)(intern); }

void ITVFUN(internal_clear)(itv_internal_t* intern);
static inline void itv_internal_clear(itv_internal_t* intern)
{ ITVFUN(internal_clear)(intern); }

itv_internal_t* ITVFUN(internal_alloc)();
static inline itv_internal_t* itv_internal_alloc()
{ return ITVFUN(internal_alloc)(); }

void ITVFUN(internal_free)(itv_internal_t* intern);
static inline void itv_internal_free(itv_internal_t* intern)
{ ITVFUN(internal_free)(intern); }

bool ITVFUN(canonicalize)(itv_internal_t* intern,
			  itv_t a, bool integer);
static inline bool itv_canonicalize(itv_internal_t* intern,
			       itv_t a, bool integer)
{ return ITVFUN(canonicalize)(intern,a,integer); }

void ITVFUN(mul_bound)(itv_internal_t* intern,
		       itv_t a, const itv_t b, const bound_t c);
static inline void itv_mul_bound(itv_internal_t* intern,
		       itv_t a, const itv_t b, const bound_t c)
{ ITVFUN(mul_bound)(intern,a,b,c); }

void ITVFUN(div_bound)(itv_internal_t* intern,
		       itv_t a, const itv_t b, const bound_t c);
static inline void itv_div_bound(itv_internal_t* intern,
			    itv_t a, const itv_t b, const bound_t c)
{ ITVFUN(div_bound)(intern,a,b,c); }

void ITVFUN(sub)(itv_t a, const itv_t b, const itv_t c);
static inline void itv_sub(itv_t a, const itv_t b, const itv_t c)
{ ITVFUN(sub)(a,b,c); }

void ITVFUN(neg)(itv_t a, const itv_t b);
static inline void itv_neg(itv_t a, const itv_t b)
{ ITVFUN(neg)(a,b); }

void ITVFUN(mul)(itv_internal_t* intern, itv_t a, const itv_t b, const itv_t c);
static inline void itv_mul(itv_internal_t* intern, itv_t a, const itv_t b, const itv_t c)
{ ITVFUN(mul)(intern,a,b,c); }

void ITVFUN(fprint)(FILE* stream, const itv_t a);
static inline void itv_fprint(FILE* stream, const itv_t a)
{ ITVFUN(fprint)(stream,a); }

int ITVFUN(snprint)(char* s, size_t size, const itv_t a);
static inline int itv_snprint(char* s, size_t size, const itv_t a)
{ return ITVFUN(snprint)(s,size,a); }

bool ITVFUN(set_ap_scalar)(itv_internal_t* intern,
			   itv_t a, const ap_scalar_t* b);
static inline bool itv_set_ap_scalar(itv_internal_t* intern,
				itv_t a, const ap_scalar_t* b)
{ return ITVFUN(set_ap_scalar)(intern,a,b); }

bool ITVFUN(set_ap_interval)(itv_internal_t* intern,
			     itv_t a, const ap_interval_t* b);
static inline bool itv_set_ap_interval(itv_internal_t* intern,
				  itv_t a, const ap_interval_t* b)
{ return ITVFUN(set_ap_interval)(intern,a,b); }

bool ITVFUN(set_ap_coeff)(itv_internal_t* intern,
			  itv_t itv, const ap_coeff_t* coeff);
static inline bool itv_set_ap_coeff(itv_internal_t* intern,
			       itv_t itv, const ap_coeff_t* coeff)
{ return ITVFUN(set_ap_coeff)(intern,itv,coeff); }

bool ITVAPFUN(ap_interval_set_itv)(itv_internal_t* intern,
				   ap_interval_t* a, const itv_t b);
static inline bool ap_interval_set_itv(itv_internal_t* intern,
				       ap_interval_t* a, const itv_t b)
{ return ITVAPFUN(ap_interval_set_itv)(intern,a,b); }

bool ITVAPFUN(ap_coeff_set_itv)(itv_internal_t* intern,
				ap_coeff_t* a, const itv_t b);
static inline bool ap_coeff_set_itv(itv_internal_t* intern,
				    ap_coeff_t* a, const itv_t b)
{ return ITVAPFUN(ap_coeff_set_itv)(intern,a,b); }


static inline void itv_init(itv_t a)
{
  bound_init(a->inf);
  bound_init(a->sup);
}
static inline void itv_init_array(itv_t* a, size_t size)
{
  int i;
  for (i=0; i<size; i++) itv_init(a[i]);
}    
static inline void itv_init_set(itv_t a, const itv_t b)
{
  bound_init_set(a->inf,b->inf);
  bound_init_set(a->sup,b->sup);
}
static inline void itv_clear(itv_t a)
{
  bound_clear(a->inf);
  bound_clear(a->sup);
}
static inline void itv_clear_array(itv_t* a, size_t size)
{
#if !defined(NUM_NATIVE)
  int i;
  for (i=0; i<size; i++) itv_clear(a[i]);
#endif
}    
static inline void itv_array_free(itv_t* a, size_t size)
{
  itv_clear_array(a,size);
  free(a);
}

static inline void itv_set(itv_t a, const itv_t b)
{
  bound_set(a->inf,b->inf);
  bound_set(a->sup,b->sup);
}
static inline void itv_set_bottom(itv_t a)
{
  bound_set_int(a->inf,-1);
  bound_set_int(a->sup,-1);
}
static inline void itv_set_top(itv_t a)
{
  bound_set_infty(a->inf);
  bound_set_infty(a->sup);
}
static inline void itv_swap(itv_t a, itv_t b)
{ itv_t t; *t=*a;*a=*b;*b=*t; }

static inline bool itv_is_top(const itv_t a)
{ 
  return bound_infty(a->inf) && bound_infty(a->sup); 
}
static inline bool itv_is_bottom(itv_internal_t* intern, itv_t a)
{
  return itv_canonicalize(intern, a, false);
}
static inline bool itv_is_point(itv_internal_t* intern, const itv_t a)
{
  if (!bound_infty(a->inf) && !bound_infty(a->sup)){
    num_neg(intern->canonicalize_num,bound_numref(a->inf));
    return num_equal(intern->canonicalize_num,bound_numref(a->sup));
  }
  else
    return false;
}
static inline bool itv_is_leq(const itv_t a, const itv_t b)
{
  return bound_cmp(a->sup,b->sup)<=0 && bound_cmp(a->inf,b->inf)<=0;
}
static inline bool itv_is_eq(const itv_t a, const itv_t b)
{
  return bound_equal(a->sup,b->sup) && bound_equal(a->inf,b->inf);
}

static inline bool itv_meet(itv_internal_t* intern,
			    itv_t a, const itv_t b, const itv_t c)
{
  bound_min(a->sup,b->sup,c->sup);
  bound_min(a->inf,b->inf,c->inf);
  return itv_canonicalize(intern,a,false);
}
static inline void itv_join(itv_t a, const itv_t b, const itv_t c)
{
  bound_max(a->sup,b->sup,c->sup);
  bound_max(a->inf,b->inf,c->inf);
}
static inline void bound_widening(bound_t a, const bound_t b, const bound_t c)
{
  if (bound_infty(c) ||
      bound_cmp(b,c)<0){
    bound_set_infty(a);
  } else {
    bound_set(a,b);
  }
}
static inline void itv_widening(itv_t a, 
				const itv_t b, const itv_t c)
{
  bound_widening(a->sup,b->sup,c->sup);
  bound_widening(a->inf,b->inf,c->inf);
}
static inline void itv_add(itv_t a, const itv_t b, const itv_t c)
{
  bound_add(a->sup,b->sup,c->sup);
  bound_add(a->inf,b->inf,c->inf);
}
static inline void itv_add_bound(itv_t a, const itv_t b, const bound_t c)
{
  bound_add(a->sup,b->sup,c);
  bound_sub(a->inf,b->inf,c);
}
static inline void itv_sub_bound(itv_t a, const itv_t b, const bound_t c)
{
  bound_sub(a->sup,b->sup,c);
  bound_add(a->inf,b->inf,c);
}
static inline void itv_print(const itv_t itv)
{ itv_fprint(stdout,itv); }

#ifdef __cplusplus
}
#endif

#endif
