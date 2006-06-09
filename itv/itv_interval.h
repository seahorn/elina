/* ********************************************************************** */
/* itv_interval_h: abstract lattice of (unidimensional) intervals */
/* ********************************************************************** */

#ifndef _ITV_INTERVAL_H_
#define _ITV_INTERVAL_H_

#include <stdio.h>
#include "itv_config.h"

/* Be cautious: interval [a,b] is represented by [-a,b].  This is because
   bound quantities are always rounded toward +infty */
struct itv_interval_t {
  bound_t inf; /* negation of the inf bound */
  bound_t sup; /* sup bound */
};
typedef struct itv_interval_t itv_interval_t[1];
typedef struct itv_interval_t* itv_interval_ptr;

/* Internal datatype for quasilinear expressions */
typedef struct itv_linterm_t {
  num_t num;
  ap_dim_t dim;
} itv_linterm_t;

typedef struct itv_linexpr_t {
  itv_linterm_t* linterm;
  size_t size;
  itv_interval_t cst;
} itv_linexpr_t;

typedef struct itv_lincons_t {
  itv_linexpr_t itvlinexpr;
  ap_constyp_t constyp;
} itv_lincons_t;

typedef struct itv_lincons_array_t {
  itv_lincons_t* p;
  size_t size;
} itv_lincons_array_t;

#include "itv_internal.h"

/* ********************************************************************** */
/* itv_interval */
/* ********************************************************************** */

/* Initialization and clearing */
static inline void itv_interval_init(itv_interval_t a);
static inline void itv_interval_init_array(itv_interval_t* a, size_t size);
static inline void itv_interval_init_set(itv_interval_t a, const itv_interval_t b);
static inline void itv_interval_clear(itv_interval_t a);
static inline void itv_interval_clear_array(itv_interval_t* a, size_t size);
static inline void itv_interval_array_free(itv_interval_t* a, size_t size);

/* Assignement */
static inline void itv_interval_set(itv_interval_t a, const itv_interval_t b);
static inline void itv_interval_set_bottom(itv_interval_t a);
static inline void itv_interval_set_top(itv_interval_t a);
static inline void itv_interval_swap(itv_interval_t a, itv_interval_t b);

/* Normalization and tests */
bool itv_interval_canonicalize(itv_internal_t* intern,
			      itv_interval_t a, bool integer);
static inline bool itv_interval_is_top(const itv_interval_t a);
static inline bool itv_interval_is_bottom(itv_internal_t* intern,
					 const itv_interval_t a);
static inline bool itv_interval_is_leq(const itv_interval_t a, const itv_interval_t b);
static inline bool itv_interval_is_eq(const itv_interval_t a, const itv_interval_t b);

/* Lattice oprations */
static inline bool itv_interval_meet(itv_internal_t* intern,
				    itv_interval_t a, const itv_interval_t b, const itv_interval_t c);
static inline void itv_interval_join(itv_interval_t a, const itv_interval_t b, const itv_interval_t c);
static inline void itv_interval_widening(itv_interval_t a, const itv_interval_t b, const itv_interval_t c);

/* Arithmetic operations */
static inline void itv_interval_add(itv_interval_t a, const itv_interval_t b, const itv_interval_t c);
void itv_interval_mul_num(itv_internal_t* intern,
			 itv_interval_t a, const itv_interval_t b, const num_t c);
void itv_interval_div_num(itv_internal_t* intern,
			 itv_interval_t a, const itv_interval_t b, const num_t c);
void itv_interval_neg(itv_interval_t a, const itv_interval_t b);

/* Printing */
int itv_interval_snprint(char* s, size_t size, const itv_interval_t a);
void itv_interval_fprint(FILE* stream, const itv_interval_t a);

/* Conversions */
void num_set_scalar(num_t a, const ap_scalar_t* b, int round);
void bound_set_scalar(num_t a, const ap_scalar_t* b, int round);
void ap_scalar_set_num(ap_scalar_t* a, const num_t b, int round);
void ap_scalar_set_bound(ap_scalar_t* a, const bound_t b, int round);

void itv_interval_set_interval(itv_internal_t* intern,
			      itv_interval_t a, const ap_interval_t* b);
void ap_interval_set_itv_interval(ap_interval_t* a, const itv_interval_t b);

bool itv_interval_is_leq_interval(itv_internal_t* intern,
				 const itv_interval_t a, const ap_interval_t* b);

void itv_interval_mul_scalar(itv_internal_t* intern,
			     itv_interval_t a,
			     const itv_interval_t b, const ap_scalar_t* c);
void itv_interval_div_scalar(itv_internal_t* intern,
			     itv_interval_t a, 
			     const itv_interval_t b, const ap_scalar_t* c);

void itv_interval_mul_interval(itv_internal_t* intern,
			      itv_interval_t a,
			      const itv_interval_t b,
			      const ap_interval_t* c);

/* ********************************************************************** */
/* itvlinexpr, itvlincons, itv_lincons_array */
/* ********************************************************************** */

void itv_linexpr_init(itv_linexpr_t* expr, size_t size);
void itv_linexpr_reinit(itv_linexpr_t* expr, size_t size);
void itv_linexpr_clear(itv_linexpr_t* expr);
void itv_linexpr_set(itv_linexpr_t* a, const itv_linexpr_t* b);

void itv_linexpr_set_linexpr(itv_internal_t* intern,
			    itv_linexpr_t* expr, const ap_linexpr0_t* linexpr0);

void itv_lincons_set_lincons(itv_internal_t* intern,
			    itv_lincons_t* cons, const ap_lincons0_t* lincons0);
static inline
void itv_lincons_init(itv_lincons_t* cons);
void itv_lincons_clear(itv_lincons_t* cons);
void itv_lincons_set(itv_lincons_t* a, const itv_lincons_t* b);

itv_lincons_array_t itv_lincons_array_make(size_t size);
void itv_lincons_array_clear(itv_lincons_array_t* array);


/* Iterator (Macro): use:
   itv_linexpr_ForeachLinterm(itv_linexpr_t* e, size_t i, ap_dim_t d, num_t* pnum){
     ..
   }
   where
   - e is the inspected expression,
   - i is the internal iterator (of type size_t or int)
   - dim is the dimension of one linear term
   - pnum is a pointer to the corresponding coefficient

*/
#define itv_linexpr_ForeachLinterm(_p_e, _p_i, _p_d, _p_num) \
  for ((_p_i)=0; \
       (_p_i)<(_p_e)->size ? \
	  ((_p_d) = (_p_e)->linterm[i].dim, \
	   (_p_num) = &(_p_e)->linterm[i].num, \
	   true) : \
	 false; \
       (_p_i)++)

/* ********************************************************************** */
/* Definition of inline functions */
/* ********************************************************************** */

static inline void itv_interval_init(itv_interval_t a)
{
  bound_init(a->inf);
  bound_init(a->sup);
}
static inline void itv_interval_init_array(itv_interval_t* a, size_t size)
{
  int i;
  for (i=0; i<size; i++) itv_interval_init(a[i]);
}    
static inline void itv_interval_init_set(itv_interval_t a, const itv_interval_t b)
{
  bound_init_set(a->inf,b->inf);
  bound_init_set(a->sup,b->sup);
}
static inline void itv_interval_clear(itv_interval_t a)
{
  bound_clear(a->inf);
  bound_clear(a->sup);
}
static inline void itv_interval_clear_array(itv_interval_t* a, size_t size)
{
#if !defined(NUM_NATIVE)
  int i;
  for (i=0; i<size; i++) itv_interval_clear(a[i]);
#endif
}    
static inline void itv_interval_array_free(itv_interval_t* a, size_t size)
{
  itv_interval_clear_array(a,size);
  free(a);
}

static inline void itv_interval_set(itv_interval_t a, const itv_interval_t b)
{
  bound_set(a->inf,b->inf);
  bound_set(a->sup,b->sup);
}
static inline void itv_interval_set_bottom(itv_interval_t a)
{
  bound_set_int(a->inf,-1);
  bound_set_int(a->sup,-1);
}
static inline void itv_interval_set_top(itv_interval_t a)
{
  bound_set_infty(a->inf);
  bound_set_infty(a->sup);
}
static inline void itv_interval_swap(itv_interval_t a, itv_interval_t b)
{ itv_interval_t t; *t=*a;*a=*b;*b=*t; }

static inline bool itv_interval_is_top(const itv_interval_t a)
{ return bound_infty(a->inf) && bound_infty(a->sup); }
static inline bool itv_interval_is_bottom(itv_internal_t* intern,
					 const itv_interval_t a)
{
  return itv_interval_canonicalize(intern, *(itv_interval_t*)(&a), false);
}
static inline bool itv_interval_is_leq(const itv_interval_t a, const itv_interval_t b)
{
  return bound_cmp(a->sup,b->sup)<=0 && bound_cmp(a->inf,b->inf)<=0;
}
static inline bool itv_interval_is_eq(const itv_interval_t a, const itv_interval_t b)
{
  return bound_equal(a->sup,b->sup) && bound_equal(a->inf,b->inf);
}

static inline bool itv_interval_meet(itv_internal_t* intern,
				    itv_interval_t a, const itv_interval_t b, const itv_interval_t c)
{
  bound_min(a->sup,b->sup,c->sup);
  bound_min(a->inf,b->inf,c->inf);
  return itv_interval_canonicalize(intern,a,false);
}
static inline void itv_interval_join(itv_interval_t a, const itv_interval_t b, const itv_interval_t c)
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
static inline void itv_interval_widening(itv_interval_t a, 
					const itv_interval_t b, const itv_interval_t c)
{
  bound_widening(a->sup,b->sup,c->sup);
  bound_widening(a->inf,b->inf,c->inf);
}
static inline void itv_interval_add(itv_interval_t a, const itv_interval_t b, const itv_interval_t c)
{
  bound_add(a->sup,b->sup,c->sup);
  bound_add(a->inf,b->inf,c->inf);
}
static inline void itv_interval_add_num(itv_interval_t a, const itv_interval_t b, const num_t c)
{
  bound_add_num(a->sup,b->sup,c);
  bound_sub_num(a->inf,b->inf,c);
}
static inline
void itv_lincons_init(itv_lincons_t* cons){
  itv_linexpr_init(&cons->itvlinexpr,0);
}

#endif
