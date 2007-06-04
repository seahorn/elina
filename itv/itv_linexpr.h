/* ********************************************************************** */
/* itv_linexpr.c: */
/* ********************************************************************** */

#ifndef _ITV_LINEXPR_H_
#define _ITV_LINEXPR_H_

#include "itv.h"
#include "ap_expr0.h"

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
  num_t num;
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
#define itv_linexpr_ForeachLinterm(_p_e, _p_i, _p_d, _p_itv, _p_equality) \
  for ((_p_i)=0;							\
       (_p_i)<(_p_e)->size ?						\
	 (((_p_d) = (_p_e)->linterm[i].dim),				\
	  ((_p_itv) = (_p_e)->linterm[i].itv),				\
	  ((_p_equality) = &((_p_e)->linterm[i].equality)),		\
	  ((_p_d)!=AP_DIM_MAX)) :					\
	 false;								\
       (_p_i)++)

/* ********************************************************************** */
/* I. Constructor and Destructor */
/* ********************************************************************** */

static inline void itv_linexpr_init(itv_linexpr_t* expr, size_t size);
static inline void itv_linexpr_reinit(itv_linexpr_t* expr, size_t size);
static inline void itv_linexpr_clear(itv_linexpr_t* expr);

static inline void itv_lincons_init(itv_lincons_t* cons);
static inline void itv_lincons_clear(itv_lincons_t* cons);

/* ********************************************************************** */
/* II. Conversions from and to APRON datatypes */
/* ********************************************************************** */

static inline bool itv_linexpr_set_ap_linexpr0(itv_internal_t* intern,
					       itv_linexpr_t* expr, 
					       ap_linexpr0_t* linexpr0);
  /* Convert an ap_linexpr0_t into an itv_linexpr_t.
     expr points to an initialized object. 
     Return true if the conversion is exact
  */
static inline bool itv_lincons_set_ap_lincons0(itv_internal_t* intern,
					       itv_lincons_t* cons,
					       ap_lincons0_t* lincons0);
  /* Convert an ap_lincons0_t into an itv_lincons_t
     cons points to an initialized object. 
     Return true if the conversion is exact
  */

static inline void itv_linexpr0_set_itv_linexpr(itv_internal_t* intern,
					       ap_linexpr0_t** plinexpr0,
					       itv_linexpr_t* linexpr);
  /* Convert an itv_linexpr_t into an ap_linexpr0_t.
     If the result *plinexpr0 is not NULL,
     possibly reinitialize *plinexpr0, otherwise allocate it.
  */
static inline void ap_lincons0_set_itv_lincons(itv_internal_t* intern,
					       ap_lincons0_t* plincons0,
					       itv_lincons_t* lincons);
  /* Convert an itv_linexpr_t into an ap_linexpr0_t.
     The result plincons0 is supposed initialized.
  */

/* ********************************************************************** */
/* III. Evaluation of expressions  */
/* ********************************************************************** */

static inline void itv_eval_linexpr(itv_internal_t* intern,
				    itv_t itv,
				    itv_t* p,
				    itv_linexpr_t* expr);
  /* Evaluate the interval linear expression, using the array p[]
     associating intervals to dimensions */

static inline bool itv_eval_ap_linexpr0(itv_internal_t* intern,
					itv_t itv,
					itv_t* p,
					ap_linexpr0_t* expr);
  /* Evaluate the interval linear expression, using the array p[]
     associating intervals to dimensions.

     Return true if all conversions were exact */

/* ********************************************************************** */
/* IV. (Quasi)linearisation */
/* ********************************************************************** */

static inline bool itv_linexpr_quasilinearize(itv_internal_t* intern, itv_linexpr_t* linexpr, itv_t* titv);
  /* Quasilinearize in-place linexpr using the bounding box itv */

static inline bool itv_lincons_quasilinearize(itv_internal_t* intern, itv_lincons_t* lincons, itv_t* titv);
  /* Quasilinearize in-place linexpr using the bounding box itv */


/* ********************************************************************** */
/* V. Arithmetic */
/* ********************************************************************** */

static inline void itv_linexpr_neg(itv_linexpr_t* expr);
  /* Negate in-place an expression */
static inline void itv_linexpr_scale(itv_internal_t* intern,
				     itv_linexpr_t* expr, itv_t coeff);
  /* Scale in-place an expression by an interval */

static inline itv_linexpr_t itv_linexpr_add(itv_internal_t* intern,
					    itv_linexpr_t* exprA,
					    itv_linexpr_t* exprB);
static inline itv_linexpr_t itv_linexpr_sub(itv_internal_t* intern,
					    itv_linexpr_t* exprA,
					    itv_linexpr_t* exprB);
  /* Resp. add and substract two linear epxression.
     (Substraction temporarily negates exprB,a nd then restores it */

/* ********************************************************************** */
/* Definition of inline functions */
/* ********************************************************************** */

void ITVFUN(itv_linexpr_init)(itv_linexpr_t* expr, size_t size);
void ITVFUN(itv_linexpr_reinit)(itv_linexpr_t* expr, size_t size);
void ITVFUN(itv_linexpr_clear)(itv_linexpr_t* expr);
bool ITVFUN(itv_linexpr_set_ap_linexpr0)(itv_internal_t* intern,
					 itv_linexpr_t* expr, 
					 ap_linexpr0_t* linexpr0);
bool ITVFUN(itv_lincons_set_ap_lincons0)(itv_internal_t* intern,
					 itv_lincons_t* cons, 
					 ap_lincons0_t* lincons0);
void ITVFUN(ap_linexpr0_set_itv_linexpr)(itv_internal_t* intern,
					 ap_linexpr0_t** linexpr0, 
					 itv_linexpr_t* linexpr);
void ITVFUN(ap_lincons0_set_itv_lincons)(itv_internal_t* intern,
					 ap_lincons0_t* lincons0, 
					 itv_lincons_t* lincons);

void ITVFUN(itv_eval_linexpr)(itv_internal_t* intern,
			      itv_t itv,
			      itv_t* p,
			      itv_linexpr_t* expr);
bool ITVFUN(itv_eval_ap_linexpr0)(itv_internal_t* intern,
			      itv_t itv,
			      itv_t* p,
			      ap_linexpr0_t* expr);
bool ITVFUN(itv_linexpr_quasilinearize)(itv_internal_t* intern, 
					itv_linexpr_t* linexpr, itv_t* titv);
bool ITVFUN(itv_lincons_quasilinearize)(itv_internal_t* intern, 
					itv_lincons_t* lincons, itv_t* titv);



void ITVFUN(itv_linexpr_neg)(itv_linexpr_t* expr);
void ITVFUN(itv_linexpr_scale)(itv_internal_t* intern,
			       itv_linexpr_t* expr, itv_t coeff);
void ITVFUN(itv_linexpr_div)(itv_internal_t* intern,
			     itv_linexpr_t* expr, itv_t coeff);
itv_linexpr_t ITVFUN(itv_linexpr_add)(itv_internal_t* intern,
				      itv_linexpr_t* exprA,
				      itv_linexpr_t* exprB);
itv_linexpr_t ITVFUN(itv_linexpr_sub)(itv_internal_t* intern,
				      itv_linexpr_t* exprA,
				      itv_linexpr_t* exprB);


static inline void itv_linexpr_init(itv_linexpr_t* expr, size_t size)
{ ITVFUN(itv_linexpr_init)(expr,size); }

static inline void itv_linexpr_reinit(itv_linexpr_t* expr, size_t size)
{ ITVFUN(itv_linexpr_reinit)(expr,size); }

static inline void itv_linexpr_clear(itv_linexpr_t* expr)
{ ITVFUN(itv_linexpr_clear)(expr); }

static inline
void itv_lincons_init(itv_lincons_t* cons)
{
  itv_linexpr_init(&cons->linexpr,0);
  num_init(cons->num);
}
static inline
void itv_lincons_clear(itv_lincons_t* cons)
{
  itv_linexpr_clear(&cons->linexpr);
  num_clear(cons->num);
}

static inline bool itv_linexpr_set_ap_linexpr0(itv_internal_t* intern,
					       itv_linexpr_t* expr, 
					       ap_linexpr0_t* linexpr0)
{ return ITVFUN(itv_linexpr_set_ap_linexpr0)(intern,expr,linexpr0); }

static inline bool itv_lincons_set_ap_lincons0(itv_internal_t* intern,
					       itv_lincons_t* cons, 
					       ap_lincons0_t* lincons0)
{ return ITVFUN(itv_lincons_set_ap_lincons0)(intern,cons,lincons0); }

static inline void ap_linexpr0_set_itv_linexpr(itv_internal_t* intern,
					       ap_linexpr0_t** expr, 
					       itv_linexpr_t* linexpr0)
{ ITVFUN(ap_linexpr0_set_itv_linexpr)(intern,expr,linexpr0); }

static inline void ap_lincons0_set_itv_lincons(itv_internal_t* intern,
					       ap_lincons0_t* cons, 
					       itv_lincons_t* lincons0)
{ ITVFUN(ap_lincons0_set_itv_lincons)(intern,cons,lincons0); }

static inline void itv_eval_linexpr(itv_internal_t* intern,
				    itv_t itv,
				    itv_t* p,
				    itv_linexpr_t* expr)
{ ITVFUN(itv_eval_linexpr)(intern,itv,p,expr); }

static inline bool itv_eval_ap_linexpr0(itv_internal_t* intern,
					itv_t itv,
					itv_t* p,
					ap_linexpr0_t* expr)
{ return ITVFUN(itv_eval_ap_linexpr0)(intern,itv,p,expr); }

static inline bool itv_linexpr_quasilinearize(itv_internal_t* intern, itv_linexpr_t* linexpr, itv_t* titv)
{ return ITVFUN(itv_linexpr_quasilinearize)(intern,linexpr,titv); }
static inline bool itv_lincons_quasilinearize(itv_internal_t* intern, itv_lincons_t* lincons, itv_t* titv)
{ return ITVFUN(itv_lincons_quasilinearize)(intern,lincons,titv); }


static inline void itv_linexpr_neg(itv_linexpr_t* expr)
{ ITVFUN(itv_linexpr_neg)(expr); }

static inline void itv_linexpr_scale(itv_internal_t* intern,
				     itv_linexpr_t* expr, itv_t coeff)
{ ITVFUN(itv_linexpr_scale)(intern,expr,coeff); }

static inline void itv_linexpr_div(itv_internal_t* intern,
				     itv_linexpr_t* expr, itv_t coeff)
{ ITVFUN(itv_linexpr_div)(intern,expr,coeff); }

static inline itv_linexpr_t itv_linexpr_add(itv_internal_t* intern,
					    itv_linexpr_t* exprA,
					    itv_linexpr_t* exprB)
{ return ITVFUN(itv_linexpr_add)(intern,exprA,exprB); }

static inline itv_linexpr_t itv_linexpr_sub(itv_internal_t* intern,
					    itv_linexpr_t* exprA,
					    itv_linexpr_t* exprB)
{ return ITVFUN(itv_linexpr_sub)(intern,exprA,exprB); }

#ifdef __cplusplus
}
#endif

#endif
