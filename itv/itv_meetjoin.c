/* ********************************************************************** */
/* itv_meetjoin.c */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "itv_config.h"
#include "itv_int.h"
#include "itv_internal.h"
#include "itv_representation.h"
#include "itv_constructor.h"
#include "itv_meetjoin.h"

/* ============================================================ */
/* III.1 Meet and Join */
/* ============================================================ */

itv_t* itv_meet(ap_manager_t* man, bool destructive, itv_t* a1, const itv_t* a2)
{
  size_t i;
  bool exc;
  size_t nbdims;
  itv_t* res;
  itv_internal_t* intern = (itv_internal_t*)man->internal;
 
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  res = destructive ? a1 : itv_alloc(a1->intdim,a1->realdim);
  if (a1->p==NULL || a2->p==NULL){
    itv_set_bottom(res);
    return res;
  }
  if (!destructive){
    itv_init(res);
  }
  nbdims = a1->intdim + a1->realdim;
  for (i=0; i<nbdims; i++){
    exc = itv_interval_meet(intern,res->p[i],a1->p[i],a2->p[i]);
    if (exc){
      itv_set_bottom(res);
      break;
    }
  }
  return res;
}

itv_t* itv_join(ap_manager_t* man, bool destructive, itv_t* a1, const itv_t* a2)
{
  size_t i;
  size_t nbdims;
  itv_t* res;
 
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_top;
  res = destructive ? a1 : itv_alloc(a1->intdim,a1->realdim);
  if (a1->p==NULL){
    if (a2->p!=NULL){
      man->result.flag_exact = tbool_true;
      itv_set(res,a2);
    }
    return res;
  }
  else if (a2->p==NULL){
    man->result.flag_exact = tbool_true;
    if (!destructive) itv_set(res,a1);
    return res;
  }
  man->result.flag_exact = tbool_top;
  if (!destructive){
    itv_init(res);
  }
  nbdims = a1->intdim + a2->realdim;
  for (i=0; i<nbdims; i++){
    itv_interval_join(res->p[i],a1->p[i],a2->p[i]);
  }
  return res;
}

itv_t* itv_meet_array(ap_manager_t* man, const itv_t*const* tab, size_t size)
{
  size_t i;
  itv_t* res;
  res = itv_copy(man,tab[0]);
  for (i=1;i<size;i++){
    itv_meet(man,true,res,tab[i]);
    if (res->p==NULL) break;
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  return res;
}
itv_t* itv_join_array(ap_manager_t* man, const itv_t*const * tab, size_t size)
{
  size_t i;
  itv_t* res;
  res = itv_copy(man,tab[0]);
  for (i=1;i<size;i++){
    itv_join(man,true,res,tab[i]);
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_top;
  return res;
}

/* Meet of an abstract value with a constraint */
bool itv_meet_itvlincons(itv_internal_t* intern,
			 itv_t* a,
			 itv_lincons_t* cons)
{
  size_t nbcoeffs,nbdims;
  ap_dim_t dim;
  int i,sgn;
  num_t* pnum;
  itv_linexpr_t* expr;
  bool change,globalchange;
  bool exc;

  if (a->p==NULL){
    itv_set_bottom(a);
    return false;
  }
  nbdims = a->intdim + a->realdim;
  expr = &cons->itvlinexpr;
  globalchange = false;
  nbcoeffs = 0;
  itv_linexpr_ForeachLinterm(expr,i,dim,pnum){
    sgn = num_sgn(*pnum);
    if (sgn!=0){
      nbcoeffs++;
      /* we decompose the constraint as: (a_ix_i + e) >= 0 */
      /* we compute the interval taken by -e/a */
      num_swap(intern->meet_itv_lincons_num,*pnum);
      itv_bound_itvlinexpr(intern,
			   intern->meet_itv_lincons_itvinterval, 
			   a, expr);
      num_swap(intern->meet_itv_lincons_num,*pnum);
      itv_interval_neg(intern->meet_itv_lincons_itvinterval,
		      intern->meet_itv_lincons_itvinterval);
      itv_interval_div_num(intern,
			  intern->meet_itv_lincons_itvinterval,
			  intern->meet_itv_lincons_itvinterval,
			  *pnum);
      /* we update the interval */
      change = false;
      if (sgn>0 || cons->constyp == AP_CONS_EQ){
	if (bound_cmp(intern->meet_itv_lincons_itvinterval->inf, a->p[dim]->inf) < 0){
	  bound_set(a->p[dim]->inf, intern->meet_itv_lincons_itvinterval->inf);
	  change = true;
	}
      }
      if (sgn<0 || cons->constyp == AP_CONS_EQ){
	if (bound_cmp(intern->meet_itv_lincons_itvinterval->sup, a->p[dim]->sup) < 0){
	  bound_set(a->p[dim]->sup, intern->meet_itv_lincons_itvinterval->sup);
	  change = true;
	}
      }
      if (change){
	globalchange = true;
	exc = itv_interval_canonicalize(intern,a->p[dim],dim<a->intdim);
	if (exc){
	  itv_set_bottom(a);
	  goto _itv_meet_itv_lincons_exit;
	}
      } 
    }
  }
  if (nbcoeffs==0){ /* Maybe an unsatisfiable constraint */
    bool unsat;
    sgn = bound_sgn(expr->cst[0].sup);
    switch(cons->constyp){
    case AP_CONS_EQ:
      unsat =
	sgn < 0 ||
	bound_sgn(expr->cst[0].inf)<0;
      break;
    case AP_CONS_SUPEQ:
      unsat = sgn<0;
      break;
    case AP_CONS_SUP:
      unsat = (sgn<=0);
      break;
    default:
      abort();
    }	
    if (unsat){
      itv_set_bottom(a);
      return true;
    }
    else
      return false;
  }
  else {
  _itv_meet_itv_lincons_exit:
    return globalchange;
  }
}

/* Meet of an abstract value with an array of constraint */
void itv_meet_itv_lincons_array(itv_internal_t* intern,
			       itv_t* a,
			       itv_lincons_array_t* array,
			       size_t kmax)
{
  size_t i,k;
  bool change;

  if (kmax<1) kmax=2;
  
  /* we possibly perform kmax passes */
  for (k=0;k<kmax;k++){
    change = false;
    for (i=0; i<array->size; i++){
      change = 
	itv_meet_itvlincons(intern,
			    a,
			    &array->p[i])
	||
	change;
    }
    if (!change || a->p==NULL) break;
  }
}


itv_t* itv_meet_lincons_array(ap_manager_t* man,
			      bool destructive,
			      itv_t* a,
			      const ap_lincons0_array_t* array)
{
  itv_t* res;
  int i,k,kmax;
  bool change;
  itv_internal_t* intern = (itv_internal_t*)man->internal;

  man->result.flag_best = array->size>1 ? tbool_top : tbool_true;
  man->result.flag_exact = tbool_top;
  res = destructive ? a : itv_copy(man,a);
  kmax = man->option.funopt[AP_FUNID_MEET_LINCONS_ARRAY].algorithm;
  if (kmax<1) kmax=2;
  
  /* we possibly perform kmax passes */
  for (k=0;k<kmax;k++){
    change = false;
    for (i=0; i<array->size; i++){
      itv_lincons_set_lincons(intern,
			     &intern->meet_lincons_array_itvlincons,
			     &array->p[i]);
      change = 
	itv_meet_itvlincons(intern,
			    res,
			    &intern->meet_lincons_array_itvlincons)
	||
	change;
    }
    if (!change || res->p==NULL) break;
  }
  return res;
}

/* Generalized time elapse operator */
void itv_add_ray(itv_internal_t* intern,
		 itv_t* a,
		 const ap_generator0_t* gen)
{
  int i,sgn;
  ap_coeff_t* coeff;
  ap_dim_t dim;
  ap_linexpr0_t* expr;
  
  if (a->p==NULL){
    itv_set_bottom(a);
    return;
  }
  expr = gen->linexpr0;
  ap_linexpr0_ForeachLinterm(expr,i,dim,coeff){
    assert(coeff->discr==AP_COEFF_SCALAR);
    ap_scalar_t* scalar = coeff->val.scalar;
    sgn = ap_scalar_sgn(scalar);
    if (sgn!=0){
      if (sgn>0 || gen->gentyp==AP_GEN_LINE){
	bound_set_infty(a->p[dim]->sup);
      }
      if (sgn<0 || gen->gentyp==AP_GEN_LINE){
	bound_set_infty(a->p[dim]->inf);
      }
    }
  } 
}

itv_t* itv_add_ray_array(ap_manager_t* man,
			 bool destructive,
			 itv_t* a,
			 const ap_generator0_array_t* array)
{
  int i;
  itv_t* res;
  itv_internal_t* intern = (itv_internal_t*)man->internal;
  
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_top;
  res = destructive ? a : itv_copy(man,a);
  if (a->p==NULL)
    return res;

  for (i=0;i<array->size; i++){
    itv_add_ray(intern,res,&array->p[i]);
  }
  return res;
}
