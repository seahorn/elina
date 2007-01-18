/* ********************************************************************** */
/* itv_h: abstract lattice of (unidimensional) intervals */
/* ********************************************************************** */

#ifndef _ITV_H_
#define _ITV_H_

#include <stdio.h>
#include "box_config.h"

/* Be cautious: interval [a,b] is represented by [-a,b].  This is because
   bound quantities are always rounded toward +infty */
struct itv_t {
  bound_t inf; /* negation of the inf bound */
  bound_t sup; /* sup bound */
};
typedef struct itv_t itv_t[1];
typedef struct itv_t* itv_ptr;

/* Internal datatype for quasilinear expressions */
typedef struct box_linterm_t {
  num_t num;
  ap_dim_t dim;
} box_linterm_t;

typedef struct box_linexpr_t {
  box_linterm_t* linterm;
  size_t size;
  itv_t cst;
} box_linexpr_t;

typedef struct box_lincons_t {
  box_linexpr_t itvlinexpr;
  ap_constyp_t constyp;
} box_lincons_t;

typedef struct box_lincons_array_t {
  box_lincons_t* p;
  size_t size;
} box_lincons_array_t;

#include "box_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

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
bool itv_canonicalize(box_internal_t* intern,
		      itv_t a, bool integer);
static inline bool itv_is_top(const itv_t a);
static inline bool itv_is_bottom(box_internal_t* intern,
					 const itv_t a);
static inline bool itv_is_leq(const itv_t a, const itv_t b);
static inline bool itv_is_eq(const itv_t a, const itv_t b);

/* Lattice operations */
static inline bool itv_meet(box_internal_t* intern,
				    itv_t a, const itv_t b, const itv_t c);
static inline void itv_join(itv_t a, const itv_t b, const itv_t c);
static inline void itv_widening(itv_t a, const itv_t b, const itv_t c);

/* Arithmetic operations */
static inline 
void itv_add(itv_t a, const itv_t b, const itv_t c);
void itv_mul_num(box_internal_t* intern,
		 itv_t a, const itv_t b, const num_t c);
void itv_div_num(box_internal_t* intern,
		 itv_t a, const itv_t b, const num_t c);
void itv_neg(itv_t a, const itv_t b);

/* Printing */
int itv_snprint(char* s, size_t size, const itv_t a);
void itv_fprint(FILE* stream, const itv_t a);

/* Conversions */
void num_set_scalar(num_t a, const ap_scalar_t* b, int round);
void bound_set_scalar(num_t a, const ap_scalar_t* b, int round);
void ap_scalar_set_num(ap_scalar_t* a, const num_t b, int round);
void ap_scalar_set_bound(ap_scalar_t* a, const bound_t b, int round);

void itv_set_interval(box_internal_t* intern,
		      itv_t a, const ap_interval_t* b);
void ap_interval_set_itv(ap_interval_t* a, const itv_t b);

bool itv_is_leq_interval(box_internal_t* intern,
			 const itv_t a, const ap_interval_t* b);
  
void itv_mul_scalar(box_internal_t* intern,
		    itv_t a,
		    const itv_t b, const ap_scalar_t* c);
void itv_div_scalar(box_internal_t* intern,
		    itv_t a, 
		    const itv_t b, const ap_scalar_t* c);

void itv_mul_interval(box_internal_t* intern,
		      itv_t a,
		      const itv_t b,
		      const ap_interval_t* c);

/* ********************************************************************** */
/* itvlinexpr, itvlincons, box_lincons_array */
/* ********************************************************************** */

void box_linexpr_init(box_linexpr_t* expr, size_t size);
void box_linexpr_reinit(box_linexpr_t* expr, size_t size);
void box_linexpr_clear(box_linexpr_t* expr);
void box_linexpr_set(box_linexpr_t* a, const box_linexpr_t* b);

void box_linexpr_set_linexpr(box_internal_t* intern,
			    box_linexpr_t* expr, const ap_linexpr0_t* linexpr0);

void box_lincons_set_lincons(box_internal_t* intern,
			    box_lincons_t* cons, const ap_lincons0_t* lincons0);
static inline
void box_lincons_init(box_lincons_t* cons);
void box_lincons_clear(box_lincons_t* cons);
void box_lincons_set(box_lincons_t* a, const box_lincons_t* b);

box_lincons_array_t box_lincons_array_make(size_t size);
void box_lincons_array_clear(box_lincons_array_t* array);


/* Iterator (Macro): use:
   box_linexpr_ForeachLinterm(box_linexpr_t* e, size_t i, ap_dim_t d, num_t* pnum){
     ..
   }
   where
   - e is the inspected expression,
   - i is the internal iterator (of type size_t or int)
   - dim is the dimension of one linear term
   - pnum is a pointer to the corresponding coefficient

*/
#define box_linexpr_ForeachLinterm(_p_e, _p_i, _p_d, _p_num) \
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
{ return bound_infty(a->inf) && bound_infty(a->sup); }
static inline bool itv_is_bottom(box_internal_t* intern,
					 const itv_t a)
{
  return itv_canonicalize(intern, *(itv_t*)(&a), false);
}
static inline bool itv_is_leq(const itv_t a, const itv_t b)
{
  return bound_cmp(a->sup,b->sup)<=0 && bound_cmp(a->inf,b->inf)<=0;
}
static inline bool itv_is_eq(const itv_t a, const itv_t b)
{
  return bound_equal(a->sup,b->sup) && bound_equal(a->inf,b->inf);
}

static inline bool itv_meet(box_internal_t* intern,
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
static inline void itv_add_num(itv_t a, const itv_t b, const num_t c)
{
  bound_add_num(a->sup,b->sup,c);
  bound_sub_num(a->inf,b->inf,c);
}
static inline
void box_lincons_init(box_lincons_t* cons){
  box_linexpr_init(&cons->itvlinexpr,0);
}

#ifdef __cplusplus
}
#endif

#endif
