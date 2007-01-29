/* ********************************************************************** */
/* itv_linexpr.c: */
/* ********************************************************************** */

#ifndef _ITV_LINEXPR_H_
#define _ITV_LINEXPR_H_

#include "itv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Interval linear term */
typedef struct itv_linterm_t {
  itv_t itv;
  bool equality;
    /* indicate that the interval is reduced to a single value */
  ap_dim_t dim;
} itv_linterm_t;

/* Interval linear expression */
typedef struct itv_linexpr_t {
  itv_linterm_t* linterm;
  size_t size;
  itv_t cst;
  bool equality;
    /* indicate that the interval is reduced to a single value */
} itv_linexpr_t;

/* Interval linear constraint */
typedef struct itv_lincons_t {
  itv_linexpr_t linexpr;
  ap_constyp_t constyp;
} itv_lincons_t;

/* Iterator (Macro): use:
   itv_linexpr_ForeachLinterm(itv_linexpr_t* e, size_t i, ap_dim_t d, itv_ptr pitv, bool* pequality){
     ..
   }
   where
   - e is the inspected expression,
   - i is the internal iterator (of type size_t or int)
   - dim is the dimension of one linear term
   - pitv is a pointer to the corresponding coefficient
   - pequality indicates if the interval is actually a point

*/
#define itv_linexpr_ForeachLinterm(_p_e, _p_i, _p_d, _p_itv, _p_equality)	\
  for ((_p_i)=0; \
       (_p_i)<(_p_e)->size ? \
	  ((_p_d) = (_p_e)->linterm[i].dim, \
	   (_p_itv) = (_p_e)->linterm[i].itv, \
	   (_p_equality) = &(_p_e)->linterm[i].equality, \
	   true) : \
	 false; \
       (_p_i)++)

void itv_linexpr_init(itv_linexpr_t* expr, size_t size);
void itv_linexpr_reinit(itv_linexpr_t* expr, size_t size);
void itv_linexpr_clear(itv_linexpr_t* expr);

static inline void itv_lincons_init(itv_lincons_t* cons);
static inline void itv_lincons_clear(itv_lincons_t* cons);

bool itv_linexpr_set_ap_linexpr0(itv_internal_t* intern,
				 itv_linexpr_t* expr, 
				 const ap_linexpr0_t* linexpr0);
  /* Convert a ap_linexpr0_t into a itv_linexpr_t

     Return true if the conversion is exact */

bool itv_lincons_set_ap_lincons0(itv_internal_t* intern,
				 itv_lincons_t* cons, 
				 const ap_lincons0_t* lincons0);
  /* Convert a ap_lincons0_t into a itv_lincons_t 

     Return true if the conversion is exact */

void itv_eval_itv_linexpr(itv_internal_t* intern,
			  itv_t itv,
			  const itv_t* p,
			  const itv_linexpr_t* expr);
  /* Evaluate the interval linear expression, using the array p[]
     associating intervals to dimensions */

bool itv_eval_ap_linexpr0(itv_internal_t* intern,
			  itv_t itv,
			  const itv_t* p,
			  const ap_linexpr0_t* expr);
  /* Evaluate the interval linear expression, using the array p[]
     associating intervals to dimensions.

     Return true if all conversions were exact */


/* ********************************************************************** */
/* Definition of inline functions */
/* ********************************************************************** */

static inline
void itv_lincons_init(itv_lincons_t* cons)
{
  itv_linexpr_init(&cons->linexpr,0);
}
static inline
void itv_lincons_clear(itv_lincons_t* cons)
{
  itv_linexpr_clear(&cons->linexpr);
}

#ifdef __cplusplus
}
#endif

#endif
