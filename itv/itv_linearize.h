/* ********************************************************************** */
/* itv_linearize.c: */
/* ********************************************************************** */

#ifndef _ITV_LINEARIZE_H_
#define _ITV_LINEARIZE_H_

#include "itv.h"
#include "itv_linexpr.h"
#include "ap_expr0.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ********************************************************************** */
/* I. Evaluation of expressions  */
/* ********************************************************************** */

static inline void itv_eval_linexpr(itv_internal_t* intern,
				    itv_t itv,
				    itv_linexpr_t* expr,
				    itv_t* env);
  /* Evaluate the interval linear expression, using the array p[]
     associating intervals to dimensions */

static inline tbool_t itv_eval_cstlincons(itv_internal_t* intern, itv_lincons_t* lincons);
  /* Is the assumed constant constraint satisfied ? */

static inline bool itv_eval_ap_linexpr0(itv_internal_t* intern,
					itv_t itv,
					ap_linexpr0_t* expr,
					itv_t* env);
  /* Evaluate the interval linear expression, using the array p[]
     associating intervals to dimensions.

     Return true if all conversions were exact */

/* ********************************************************************** */
/* II. (Quasi)linearisation of interval linear expressions and constraints */
/* ********************************************************************** */

static inline bool itv_quasilinearize_linexpr(itv_internal_t* intern, itv_linexpr_t* linexpr, itv_t* env);
  /* Quasilinearize in-place linexpr using the bounding box itv */

static inline bool itv_quasilinearize_lincons(itv_internal_t* intern, itv_lincons_t* lincons, itv_t* env);
  /* Quasilinearize in-place lincons using the bounding box itv */

static inline bool itv_quasilinearize_lincons_array(itv_internal_t* intern, itv_lincons_array_t* tlincons, itv_t* env, bool linearize);
  /* (Quasi)linearize in-place tlincons using the bounding box itv */

static inline void itv_linearize_lincons_array(itv_internal_t* intern, itv_lincons_array_t* tlincons);
  /* Linearize in-place quasilinear tlincons using the bounding box itv */

/* ********************************************************************** */
/* III. Evaluation of tree expressions  */
/* ********************************************************************** */

static inline void itv_eval_ap_texpr0(itv_internal_t* intern, itv_t res, ap_texpr0_t* expr, itv_t* env);

/* ********************************************************************** */
/* IV. Linearisation of tree expressions. */
/* ********************************************************************** */

static inline void itv_intlinearize_texpr0(itv_internal_t* intern, itv_linexpr_t* res, ap_texpr0_t* expr, itv_t* env, size_t intdim, bool quasilinearize);
static inline void itv_intlinearize_tcons0(itv_internal_t* intern, itv_lincons_t* res, ap_tcons0_t* cons, itv_t* env, size_t intdim, bool quasilinearize);
static inline void itv_intlinearize_tcons0_array(itv_internal_t* intern, itv_lincons_array_t* res, ap_tcons0_array_t* cons, itv_t* env, size_t intdim, ap_linexpr_type_t linearize);

/* ********************************************************************** */
/* Definition of inline functions */
/* ********************************************************************** */

void ITVFUN(itv_eval_linexpr)(itv_internal_t* intern, itv_t itv, itv_linexpr_t* expr, itv_t* env);
tbool_t ITVFUN(itv_eval_cstlincons)(itv_internal_t* intern, itv_lincons_t* lincons);
bool ITVFUN(itv_eval_ap_linexpr0)(itv_internal_t* intern, itv_t itv, ap_linexpr0_t* expr, itv_t* env);

bool ITVFUN(itv_quasilinearize_linexpr)(itv_internal_t* intern, itv_linexpr_t* linexpr, itv_t* env);
bool ITVFUN(itv_quasilinearize_lincons)(itv_internal_t* intern, itv_lincons_t* lincons, itv_t* env);
bool ITVFUN(itv_quasilinearize_lincons_array)(itv_internal_t* intern, itv_lincons_array_t* tlincons, itv_t* env, bool linearize);
void ITVFUN(itv_linearize_lincons_array)(itv_internal_t* intern, itv_lincons_array_t* array);

void ITVFUN(itv_eval_ap_texpr0)(itv_internal_t* intern, itv_t itv, ap_texpr0_t* expr, itv_t* env);

void ITVFUN(itv_intlinearize_ap_texpr0)(itv_internal_t* intern,itv_linexpr_t* res,ap_texpr0_t* expr,itv_t* env, size_t intdim, bool quasilinearize);
void ITVFUN(itv_intlinearize_ap_tcons0)(itv_internal_t* intern,itv_lincons_t* res, ap_tcons0_t* cons, itv_t* env, size_t intdim, bool quasilinearize);
void ITVFUN(itv_intlinearize_ap_tcons0_array)(itv_internal_t* intern,itv_lincons_array_t* res, ap_tcons0_array_t* cons, itv_t* env, size_t intdim, ap_linexpr_type_t linearize);

static inline void itv_eval_linexpr(itv_internal_t* intern, itv_t itv, itv_linexpr_t* expr, itv_t* env)
{ ITVFUN(itv_eval_linexpr)(intern,itv,expr,env); }
static inline bool itv_eval_ap_linexpr0(itv_internal_t* intern, itv_t itv, ap_linexpr0_t* expr, itv_t* env)
{ return ITVFUN(itv_eval_ap_linexpr0)(intern,itv,expr,env); }
static inline tbool_t itv_eval_cstlincons(itv_internal_t* intern,itv_lincons_t* lincons)
{ return ITVFUN(itv_eval_cstlincons)(intern,lincons); }

static inline bool itv_quasilinearize_linexpr(itv_internal_t* intern, itv_linexpr_t* linexpr, itv_t* env)
{ return ITVFUN(itv_quasilinearize_linexpr)(intern,linexpr,env); }
static inline bool itv_quasilinearize_lincons(itv_internal_t* intern, itv_lincons_t* lincons, itv_t* env)
{ return ITVFUN(itv_quasilinearize_lincons)(intern,lincons,env); }
static inline bool itv_quasilinearize_lincons_array(itv_internal_t* intern, itv_lincons_array_t* tlincons, itv_t* env, bool linearize)
{ return ITVFUN(itv_quasilinearize_lincons_array)(intern,tlincons,env,linearize); }
static inline void itv_linearize_lincons_array(itv_internal_t* intern, itv_lincons_array_t* array)
{ ITVFUN(itv_linearize_lincons_array)(intern,array); }

static inline void itv_eval_ap_texpr0(itv_internal_t* intern, itv_t itv, ap_texpr0_t* expr, itv_t* env)
{ ITVFUN(itv_eval_ap_texpr0)(intern,itv,expr,env); }

static inline void itv_intlinearize_ap_texpr0(itv_internal_t* intern, itv_linexpr_t* linexpr, ap_texpr0_t* expr, itv_t* env, size_t intdim, bool quasilinearize)
{ ITVFUN(itv_intlinearize_ap_texpr0)(intern,linexpr,expr,env,intdim,quasilinearize); }
static inline void itv_intlinearize_ap_tcons0(itv_internal_t* intern, itv_lincons_t* lincons, ap_tcons0_t* cons, itv_t* env, size_t intdim, bool quasilinearize)
{ ITVFUN(itv_intlinearize_ap_tcons0)(intern,lincons,cons,env,intdim,quasilinearize); }
static inline void itv_intlinearize_ap_tcons0_array(itv_internal_t* intern, itv_lincons_array_t* tlincons, ap_tcons0_array_t* tcons, itv_t* env, size_t intdim, ap_linexpr_type_t linearize)
{ ITVFUN(itv_intlinearize_ap_tcons0_array)(intern,tlincons,tcons,env,intdim,linearize); }

#ifdef __cplusplus
}
#endif

#endif
