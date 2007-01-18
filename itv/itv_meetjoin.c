/* ********************************************************************** */
/* box_meetjoin.c */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "box_config.h"
#include "box_int.h"
#include "box_internal.h"
#include "box_representation.h"
#include "box_constructor.h"
#include "box_meetjoin.h"

/* ============================================================ */
/* III.1 Meet and Join */
/* ============================================================ */

box_t* box_meet(ap_manager_t* man, bool destructive, box_t* a1, const box_t* a2)
{
  size_t i;
  bool exc;
  size_t nbdims;
  box_t* res;
  box_internal_t* intern = (box_internal_t*)man->internal;
 
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  res = destructive ? a1 : box_alloc(a1->intdim,a1->realdim);
  if (a1->p==NULL || a2->p==NULL){
    box_set_bottom(res);
    return res;
  }
  if (!destructive){
    box_init(res);
  }
  nbdims = a1->intdim + a1->realdim;
  for (i=0; i<nbdims; i++){
    exc = itv_meet(intern,res->p[i],a1->p[i],a2->p[i]);
    if (exc){
      box_set_bottom(res);
      break;
    }
  }
  return res;
}

box_t* box_join(ap_manager_t* man, bool destructive, box_t* a1, const box_t* a2)
{
  size_t i;
  size_t nbdims;
  box_t* res;
 
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_top;
  res = destructive ? a1 : box_alloc(a1->intdim,a1->realdim);
  if (a1->p==NULL){
    if (a2->p!=NULL){
      man->result.flag_exact = tbool_true;
      box_set(res,a2);
    }
    return res;
  }
  else if (a2->p==NULL){
    man->result.flag_exact = tbool_true;
    if (!destructive) box_set(res,a1);
    return res;
  }
  man->result.flag_exact = tbool_top;
  if (!destructive){
    box_init(res);
  }
  nbdims = a1->intdim + a2->realdim;
  for (i=0; i<nbdims; i++){
    itv_join(res->p[i],a1->p[i],a2->p[i]);
  }
  return res;
}

box_t* box_meet_array(ap_manager_t* man, const box_t*const* tab, size_t size)
{
  size_t i;
  box_t* res;
  res = box_copy(man,tab[0]);
  for (i=1;i<size;i++){
    box_meet(man,true,res,tab[i]);
    if (res->p==NULL) break;
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  return res;
}
box_t* box_join_array(ap_manager_t* man, const box_t*const * tab, size_t size)
{
  size_t i;
  box_t* res;
  res = box_copy(man,tab[0]);
  for (i=1;i<size;i++){
    box_join(man,true,res,tab[i]);
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_top;
  return res;
}

/* Meet of an abstract value with a constraint */
bool box_meet_itvlincons(box_internal_t* intern,
			 box_t* a,
			 box_lincons_t* cons)
{
  size_t nbcoeffs,nbdims;
  ap_dim_t dim;
  int i,sgn;
  num_t* pnum;
  box_linexpr_t* expr;
  bool change,globalchange;
  bool exc;

  if (a->p==NULL){
    box_set_bottom(a);
    return false;
  }
  nbdims = a->intdim + a->realdim;
  expr = &cons->itvlinexpr;
  globalchange = false;
  nbcoeffs = 0;
  box_linexpr_ForeachLinterm(expr,i,dim,pnum){
    sgn = num_sgn(*pnum);
    if (sgn!=0){
      nbcoeffs++;
      /* we decompose the constraint as: (a_ix_i + e) >= 0 */
      /* we compute the interval taken by -e/a */
      num_swap(intern->meet_box_lincons_num,*pnum);
      box_bound_itvlinexpr(intern,
			   intern->meet_box_lincons_itvinterval, 
			   a, expr);
      num_swap(intern->meet_box_lincons_num,*pnum);
      itv_neg(intern->meet_box_lincons_itvinterval,
		      intern->meet_box_lincons_itvinterval);
      itv_div_num(intern,
			  intern->meet_box_lincons_itvinterval,
			  intern->meet_box_lincons_itvinterval,
			  *pnum);
      /* we update the interval */
      change = false;
      if (sgn>0 || cons->constyp == AP_CONS_EQ){
	if (bound_cmp(intern->meet_box_lincons_itvinterval->inf, a->p[dim]->inf) < 0){
	  bound_set(a->p[dim]->inf, intern->meet_box_lincons_itvinterval->inf);
	  change = true;
	}
      }
      if (sgn<0 || cons->constyp == AP_CONS_EQ){
	if (bound_cmp(intern->meet_box_lincons_itvinterval->sup, a->p[dim]->sup) < 0){
	  bound_set(a->p[dim]->sup, intern->meet_box_lincons_itvinterval->sup);
	  change = true;
	}
      }
      if (change){
	globalchange = true;
	exc = itv_canonicalize(intern,a->p[dim],dim<a->intdim);
	if (exc){
	  box_set_bottom(a);
	  goto _box_meet_box_lincons_exit;
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
      box_set_bottom(a);
      return true;
    }
    else
      return false;
  }
  else {
  _box_meet_box_lincons_exit:
    return globalchange;
  }
}

/* Meet of an abstract value with an array of constraint */
void box_meet_box_lincons_array(box_internal_t* intern,
			       box_t* a,
			       box_lincons_array_t* array,
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
	box_meet_itvlincons(intern,
			    a,
			    &array->p[i])
	||
	change;
    }
    if (!change || a->p==NULL) break;
  }
}


box_t* box_meet_lincons_array(ap_manager_t* man,
			      bool destructive,
			      box_t* a,
			      const ap_lincons0_array_t* array)
{
  box_t* res;
  int i,k,kmax;
  bool change;
  box_internal_t* intern = (box_internal_t*)man->internal;

  man->result.flag_best = array->size>1 ? tbool_top : tbool_true;
  man->result.flag_exact = tbool_top;
  res = destructive ? a : box_copy(man,a);
  kmax = man->option.funopt[AP_FUNID_MEET_LINCONS_ARRAY].algorithm;
  if (kmax<1) kmax=2;
  
  /* we possibly perform kmax passes */
  for (k=0;k<kmax;k++){
    change = false;
    for (i=0; i<array->size; i++){
      box_lincons_set_lincons(intern,
			     &intern->meet_lincons_array_itvlincons,
			     &array->p[i]);
      change = 
	box_meet_itvlincons(intern,
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
void box_add_ray(box_internal_t* intern,
		 box_t* a,
		 const ap_generator0_t* gen)
{
  int i,sgn;
  ap_coeff_t* coeff;
  ap_dim_t dim;
  ap_linexpr0_t* expr;
  
  if (a->p==NULL){
    box_set_bottom(a);
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

box_t* box_add_ray_array(ap_manager_t* man,
			 bool destructive,
			 box_t* a,
			 const ap_generator0_array_t* array)
{
  int i;
  box_t* res;
  box_internal_t* intern = (box_internal_t*)man->internal;
  
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_top;
  res = destructive ? a : box_copy(man,a);
  if (a->p==NULL)
    return res;

  for (i=0;i<array->size; i++){
    box_add_ray(intern,res,&array->p[i]);
  }
  return res;
}
