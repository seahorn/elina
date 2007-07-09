/* ************************************************************************* */
/* ap_linearize_aux.c: auxiliary functions for (quasi)linearisation */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

/* Auxiliary module to ap_linearize, which contains functions depending of the
   number representation */

#include "ap_linearize.h"
#include "ap_linearize_aux.h"

#include "num.h"
#include "bound.h"
#include "itv.h"
#include "itv_linexpr.h"
#include "itv_linearize.h"

/* ********************************************************************** */
/* I. Quasilinearization of interval linear expressions */
/* ********************************************************************** */

static
bool quasilinearize_alloc(ap_manager_t* man, ap_abstract0_t* abs,
			  itv_internal_t** pintern,
			  itv_lincons_t* plincons,
			  itv_t** penv, ap_dimension_t* pdim)
{
  bool exact,exact2;
  ap_interval_t** tinterval;
  size_t size;

  assert(ap_abstract0_is_bottom(man,abs)!=tbool_true);
  exact = true;

  tinterval = ap_abstract0_to_box(man,abs);
  exact = (man->result.flag_exact == tbool_true) && exact;

  *pdim = ap_abstract0_dimension(man,abs);
  size = pdim->intdim+pdim->realdim;
  *pintern = itv_internal_alloc();
  exact = itv_array_set_ap_interval_array(*pintern,penv,tinterval,size)
    && exact;
  ap_interval_array_free(tinterval,size);
  itv_lincons_init(plincons);
  return exact;
}

static inline
void quasilinearize_free(itv_internal_t* intern,
			 itv_lincons_t* lincons,
			 itv_t* env, ap_dimension_t dim)
{
  itv_lincons_clear(lincons);
  itv_array_free(env,dim.intdim+dim.realdim);
  itv_internal_free(intern);
}

/* Evaluate a interval linear expression on the abstract
   value such as to transform it into a quasilinear expression.

   discr allows to choose the type of scalars used for computations and for the
   result.  pexact is a pointer to a Boolean, which is set to true if all
   the conversions and computations were exact.
*/

ap_linexpr0_t*
ITVFUN(ap_quasilinearize_linexpr0)(ap_manager_t* man,
				   ap_abstract0_t* abs,
				   ap_linexpr0_t* linexpr0,
				   bool* pexact)
{
  ap_dimension_t dim;
  ap_linexpr0_t* rlinexpr0;
  itv_internal_t* intern;
  itv_lincons_t lincons;
  itv_t* env;
  bool exact,exact2;

  exact = quasilinearize_alloc(man,abs,&intern,&lincons,&env,&dim);
  exact = itv_linexpr_set_ap_linexpr0(intern,&lincons.linexpr,linexpr0)
    && exact;
  exact = itv_quasilinearize_linexpr(intern,&lincons.linexpr,env)
    && exact;
  rlinexpr0 = NULL;
  ap_linexpr0_set_itv_linexpr(intern,&rlinexpr0,&lincons.linexpr);
  quasilinearize_free(intern,&lincons,env,dim);
  *pexact = exact;
  return rlinexpr0;
}

/* Same for ap_lincons0_t */
ap_lincons0_t
ITVFUN(ap_quasilinearize_lincons0)(ap_manager_t* man,
				   ap_abstract0_t* abs, ap_lincons0_t* lincons0,
				   bool* pexact)
{
  ap_linexpr0_t* rlinexpr0 = ITVFUN(ap_quasilinearize_linexpr0)(man,abs,
								lincons0->linexpr0,
								pexact);
  return ap_lincons0_make(lincons0->constyp,
			  rlinexpr0,
			  (lincons0->scalar ?
			   ap_scalar_alloc_set(lincons0->scalar) :
			   NULL)
			  );
}

/* Same for arrays of ap_linexpr0_t */
ap_linexpr0_t**
ITVFUN(ap_quasilinearize_linexpr0_array)(ap_manager_t* man,
					 ap_abstract0_t* abs, ap_linexpr0_t** texpr, size_t size,
					 bool* pexact)
{
  ap_dimension_t dim;
  ap_linexpr0_t** tab;
  itv_internal_t* intern;
  itv_lincons_t lincons;
  itv_t* env;
  bool exact,exact2;
  size_t i;

  exact = quasilinearize_alloc(man,abs,&intern,&lincons,&env,&dim);
  tab = (ap_linexpr0_t**)malloc(size*sizeof(ap_linexpr0_t*));
  for (i=0; i<size; i++){
    exact = itv_linexpr_set_ap_linexpr0(intern,&lincons.linexpr,texpr[i])
      && exact;
    exact = itv_quasilinearize_linexpr(intern,&lincons.linexpr,env)
      && exact;
    tab[i] = NULL;
    ap_linexpr0_set_itv_linexpr(intern,&tab[i],&lincons.linexpr);
  }
  quasilinearize_free(intern,&lincons,env,dim);
  *pexact = exact;
  return tab;
}

/* Same for ap_lincons0_array_t */
ap_lincons0_array_t
ITVFUN(ap_quasilinearize_lincons0_array)(ap_manager_t* man,
					 ap_abstract0_t* abs, ap_lincons0_array_t* array,
					 bool* pexact, bool linearize)
{
  itv_internal_t* intern;
  itv_lincons_t lincons;
  itv_t* env;
  ap_dimension_t dim;
  bool exact;
  itv_lincons_array_t tlincons;
  ap_lincons0_array_t res;
  size_t i;

  exact = quasilinearize_alloc(man,abs,&intern,&lincons,&env,&dim);
  itv_lincons_array_init(&tlincons,array->size);
  exact = itv_lincons_array_set_ap_lincons0_array(intern,&tlincons,array) && exact;
  exact = itv_quasilinearize_lincons_array(intern,&tlincons,env,linearize);
  
  res = ap_lincons0_array_make(tlincons.size);
  for (i=0;i<tlincons.size;i++){
    ap_lincons0_set_itv_lincons(intern,&res.p[i],&tlincons.p[i]);
  }
  itv_lincons_array_clear(&tlincons);
  quasilinearize_free(intern,&lincons,env,dim);
  return res;
}

/* evaluation */
ap_interval_t*
ITVFUN(ap_eval_linexpr0)(ap_manager_t* man,
			 ap_abstract0_t* abs, ap_linexpr0_t* expr,
			 bool* pexact)
{
  bool exact;
  itv_internal_t* intern;
  ap_dimension_t dim;
  ap_interval_t** aenv;
  itv_t* env;
  itv_t res;
  ap_interval_t* r = ap_interval_alloc();
  if (pexact) *pexact = true;
  aenv = ap_abstract0_to_box(man,abs);
  if (!aenv) {
    ap_interval_set_bottom(r);
    return r;
  }
  dim = ap_abstract0_dimension(man,abs);
  intern = itv_internal_alloc();
  itv_init(res);
  itv_array_set_ap_interval_array(intern,&env,aenv,dim.intdim+dim.realdim);
  exact = itv_eval_ap_linexpr0(intern,res,expr,env);
  if (pexact) *pexact = exact;
  ap_interval_set_itv(intern,r,res);
  itv_internal_free(intern);
  ap_interval_array_free(aenv,dim.intdim+dim.realdim);
  itv_array_free(env,dim.intdim+dim.realdim);
  itv_clear(res);
  return r;
}

/* ********************************************************************** */
/* Interval Linearization of tree expressions */
/* ********************************************************************** */

/* ====================================================================== */
/* Main */
/* ====================================================================== */

static
bool intlinearize_alloc(ap_manager_t* man, ap_abstract0_t* abs,
			itv_internal_t** pintern,
			itv_t** penv, ap_dimension_t* pdim)
{
  bool exact,exact2;
  ap_interval_t** tinterval;
  size_t size;

  assert(ap_abstract0_is_bottom(man,abs)!=tbool_true);
  exact = true;

  tinterval = ap_abstract0_to_box(man,abs);
  exact = (man->result.flag_exact == tbool_true) && exact;

  *pdim = ap_abstract0_dimension(man,abs);
  size = pdim->intdim+pdim->realdim;
  *pintern = itv_internal_alloc();
  exact = itv_array_set_ap_interval_array(*pintern,penv,tinterval,size)
    && exact;
  ap_interval_array_free(tinterval,size);
  return exact;
}

static inline
void intlinearize_free(itv_internal_t* intern,
		       itv_t* env, ap_dimension_t dim)
{
  itv_array_free(env,dim.intdim+dim.realdim);
  itv_internal_free(intern);
}

ap_interval_t*
ITVFUN(ap_eval_texpr0)(ap_manager_t* man,
		       ap_abstract0_t* abs, ap_texpr0_t* expr,
		       bool* pexact)
{
  itv_internal_t* intern;
  ap_dimension_t dim;
  itv_t* env;
  itv_t res;
  ap_interval_t* r = ap_interval_alloc();

  if (pexact) *pexact = false;

  intlinearize_alloc(man,abs,&intern,&env,&dim);
  itv_init(res);
  itv_eval_ap_texpr0(intern,res,expr,env);
  ap_interval_set_itv(intern,r,res);
  itv_clear(res);
  intlinearize_free(intern,env,dim);
  return r;
}

ap_linexpr0_t*
ITVFUN(ap_intlinearize_texpr0)(ap_manager_t* man,
			       ap_abstract0_t* abs, ap_texpr0_t* expr,
			       bool* pexact, bool quasilinearize)
{
  itv_internal_t* intern;
  ap_dimension_t dim;
  itv_t* env;
  itv_linexpr_t linexpr;

  ap_linexpr0_t* res = NULL;

  if (pexact) *pexact = false;
  intlinearize_alloc(man,abs,&intern,&env,&dim);
  itv_linexpr_init(&linexpr,0);
  itv_intlinearize_ap_texpr0(intern, &linexpr, expr, env, 
			     dim.intdim, quasilinearize);
  ap_linexpr0_set_itv_linexpr(intern,&res,&linexpr);
  itv_linexpr_clear(&linexpr);
  intlinearize_free(intern,env,dim);
  return res;
}

ap_linexpr0_t**
ITVFUN(ap_intlinearize_texpr0_array)(ap_manager_t* man,
				     ap_abstract0_t* abs, ap_texpr0_t** texpr0, size_t size,
				     bool* pexact, bool quasilinearize)
{
  itv_internal_t* intern;
  ap_dimension_t dim;
  itv_t* env;
  itv_linexpr_t linexpr;
  ap_linexpr0_t** res;
  size_t i;

  if (pexact) *pexact = false;
  intlinearize_alloc(man,abs,&intern,&env,&dim);
  itv_linexpr_init(&linexpr,0);
  res = malloc(size*sizeof(ap_linexpr0_t*));
  for (i=0; i<size; i++){
    itv_intlinearize_ap_texpr0(intern, &linexpr, texpr0[i], env, 
			       dim.intdim, quasilinearize);
    res[i] = NULL;
    ap_linexpr0_set_itv_linexpr(intern,&res[i],&linexpr);
  }
  itv_linexpr_clear(&linexpr);
  intlinearize_free(intern,env,dim);
  return res;
}

ap_lincons0_t
ITVFUN(ap_intlinearize_tcons0)(ap_manager_t* man,
			       ap_abstract0_t* abs, ap_tcons0_t* cons,
			       bool* pexact, bool quasilinearize)
{
  itv_internal_t* intern;
  ap_dimension_t dim;
  itv_t* env;
  itv_lincons_t lincons;
  ap_lincons0_t res;
  if (pexact) *pexact = false;

  intlinearize_alloc(man,abs,&intern,&env,&dim);
  itv_lincons_init(&lincons);
  itv_intlinearize_ap_tcons0(intern,
			     &lincons, cons, env,
			     dim.intdim,quasilinearize);
  res.linexpr0 = NULL;
  res.scalar = NULL;
  ap_lincons0_set_itv_lincons(intern,&res,&lincons);
  itv_lincons_clear(&lincons);
  intlinearize_free(intern,env,dim);
  return res;
}

ap_lincons0_array_t
ITVFUN(ap_intlinearize_tcons0_array)(ap_manager_t* man,
				     ap_abstract0_t* abs, ap_tcons0_array_t* array,
				     bool* pexact, ap_linexpr_type_t linearize)
{
  itv_internal_t* intern;
  ap_dimension_t dim;
  itv_t* env;
  itv_lincons_array_t tlincons;
  ap_lincons0_array_t res;
  size_t i;

  if (pexact) *pexact = false;
  intlinearize_alloc(man,abs,&intern,&env,&dim);
  itv_lincons_array_init(&tlincons,array->size);
  itv_intlinearize_ap_tcons0_array(intern,&tlincons,
				   array,env, dim.intdim,
				   linearize);
  res = ap_lincons0_array_make(tlincons.size);
  for (i=0;i<tlincons.size;i++){
    ap_lincons0_set_itv_lincons(intern,&res.p[i],&tlincons.p[i]);
  }
  intlinearize_free(intern,env,dim);
  return res;
}
