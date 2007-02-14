/* ********************************************************************** */
/* box_meetjoin.c */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "box_internal.h"
#include "box_representation.h"
#include "box_constructor.h"
#include "box_meetjoin.h"

#include "itv_linexpr.h"

/* ============================================================ */
/* Meet and Join */
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
    exc = itv_meet(intern->itv,res->p[i],a1->p[i],a2->p[i]);
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

/* ============================================================ */
/* Add_ray_array */
/* ============================================================ */


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


/* ============================================================ */
/* Meet_lincons */
/* ============================================================ */

/* Meet of an abstract value with a constraint */
bool box_meet_lincons_internal(box_internal_t* intern,
			       box_t* a,
			       itv_lincons_t* cons)
{
  size_t nbcoeffs,nbdims;
  ap_dim_t dim;
  int i;
  itv_ptr pitv;
  itv_linexpr_t* expr;
  bool *peq;
  bool equality,change,globalchange;
  bool exc;

  assert(cons->constyp == AP_CONS_EQ ||
	 cons->constyp == AP_CONS_SUPEQ ||
	 cons->constyp == AP_CONS_SUP);

  if (a->p==NULL){
    box_set_bottom(a);
    return false;
  }
  nbdims = a->intdim + a->realdim;
  expr = &cons->linexpr;
  globalchange = false;

  /* Iterates on coefficients */
  nbcoeffs = 0;
  bound_set_int(intern->meet_lincons_internal_itv2->inf,0);
  bound_set_int(intern->meet_lincons_internal_itv2->sup,0);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
    nbcoeffs++;
    /* 1. We decompose the expression e = ax+e' */
    /* We save the linterm */
    itv_swap(intern->meet_lincons_internal_itv2,pitv);
    equality = *peq;
    *peq = true;
    /* 2. evaluate e' */
    itv_eval_itv_linexpr(intern->itv,
			 intern->meet_lincons_internal_itv3,
			 (const itv_t*)a->p,
			 expr);
    change = false;
    if (!itv_is_top(intern->meet_lincons_internal_itv3)){
      if (equality){
	int sgn = bound_sgn(intern->meet_lincons_internal_itv2->sup);
	if (sgn!=0){
	  /*
	    If we have ax+e' >= 0 with a>0
	    we can deduce that x>=-e'/a, or x>= inf(-e'/a)
	    If we have ax+e' >= 0 with a<0
	    we can deduce that -ax<=e', or x<= sup(e'/-a)
	    If we have ax+e'=0
	    we can deduce x=-e'/a, or inf(-e'/a)<= x <= sup(-e'/a)
	  */
	  if (sgn>0 || cons->constyp == AP_CONS_EQ){
	    /* We compute sup(e'/a)=)-inf(-e'/a) */
	    bound_div(intern->meet_lincons_internal_bound,
		      intern->meet_lincons_internal_itv3->sup,
		      intern->meet_lincons_internal_itv2->sup);
	    /* We update the interval */
	    if (bound_cmp(intern->meet_lincons_internal_bound, a->p[dim]->inf)<0){
	      change = true;
	      bound_set(a->p[dim]->inf, intern->meet_lincons_internal_bound);
	    }
	  }
	  if (sgn<0 || cons->constyp == AP_CONS_EQ){
	    /* We compute sup(e'/-a) */
	    bound_neg(intern->meet_lincons_internal_itv3->sup,
		      intern->meet_lincons_internal_itv3->sup);
	    bound_div(intern->meet_lincons_internal_bound,
		      intern->meet_lincons_internal_itv3->sup,
		      intern->meet_lincons_internal_itv2->sup);
	    /* We update the interval */
	    if (bound_cmp(intern->meet_lincons_internal_bound, a->p[dim]->sup)<0){
	      change = true;
	      bound_set(a->p[dim]->sup, intern->meet_lincons_internal_bound);
	    }
	  }
	}
      }
      else {
	/* We have an interval */
	/*
	  - If we have [m;M]x+e' >= 0 with m>0, then [m,M]x>=inf(-e')
	    so we need at least
	    * if inf(-e')>=0: x>=inf(-e')/M
	    * if inf(-e')<=0: x>=inf(-e')/m
	  - If we have [m;M]x+e' >= 0 with M<0, then [-M,-m]x<=sup(e')
	    so we need at least
	    * if sup(e')>=0: x<=sup(e')/-M
	    * if sup(e')<=0: x<=sup(e')/-m
	  - If we have [m,M]x+e'<=0 with m>0, then [m,M]x<=-sup(e')
	    * sup(e')>=0: x<=sup(e')/-M
	    * sup(e')<=0: x<=sup(e')/-m
	  - If we have [m,M]x+e'<=0 with M<0, then [-M,-m]x>=inf(e')
	    * inf(-e')>=0: x>=inf(e')/-M
	    * inf(-e')<=0: x>=inf(e')/-m
	*/
	int sgn = 
	  bound_sgn(intern->meet_lincons_internal_itv2->inf)<0 ?
	  1 :
	  ( bound_sgn(intern->meet_lincons_internal_itv2->sup)<0 ?
	    -1 :
	    0 );
	if (sgn != 0){
	  int sgninf = bound_sgn(intern->meet_lincons_internal_itv3->inf);
	  int sgnsup = bound_sgn(intern->meet_lincons_internal_itv3->sup);
	  if (sgn>0 || (cons->constyp==AP_CONS_EQ && sgn<0)){
	    if (sgninf>=0){
	      /* We compute inf(-e')/M */
	      bound_div(intern->meet_lincons_internal_bound,
			intern->meet_lincons_internal_itv3->inf,
			intern->meet_lincons_internal_itv2->sup);
	    } else {
	      /* We compute inf(-e')/m */
	      bound_neg(intern->meet_lincons_internal_itv3->inf,
			intern->meet_lincons_internal_itv3->inf);
	      bound_div(intern->meet_lincons_internal_bound,
			intern->meet_lincons_internal_itv3->inf,
			intern->meet_lincons_internal_itv2->inf);
	    }
	    /* We update the interval */
	    if (bound_cmp(intern->meet_lincons_internal_bound, a->p[dim]->inf)<0){
	      change = true;
	      bound_set(a->p[dim]->inf, intern->meet_lincons_internal_bound);
	    }
	  }
	  if (sgn<0 || (cons->constyp==AP_CONS_EQ && sgn>0)){
	    if (sgnsup>=0){
	      /* We compute sup(e')/-M */
	      bound_neg(intern->meet_lincons_internal_itv3->sup,
			intern->meet_lincons_internal_itv3->sup);
	      bound_div(intern->meet_lincons_internal_bound,
			intern->meet_lincons_internal_itv3->sup,
			intern->meet_lincons_internal_itv2->sup);
	    } else {
	      /* We compute sup(e')/-m */
	      bound_div(intern->meet_lincons_internal_bound,
			intern->meet_lincons_internal_itv3->sup,
			intern->meet_lincons_internal_itv2->inf);
	    }
	    /* We update the interval */
	    if (bound_cmp(intern->meet_lincons_internal_bound, a->p[dim]->sup)<0){
	      change = true;
	      bound_set(a->p[dim]->sup, intern->meet_lincons_internal_bound);
	    }
	  }
	}
      }
    }
    itv_swap(intern->meet_lincons_internal_itv2,pitv);
    *peq = equality;
    if (change){
      globalchange = true;
      exc = itv_canonicalize(intern->itv,a->p[dim],dim<a->intdim);
      if (exc){
	box_set_bottom(a);
	goto _box_meet_box_lincons_exit;
      }
    }
  }
  if (nbcoeffs==0){ /* Maybe an unsatisfiable constraint */
    bool unsat;
    int sgnsup = bound_sgn(expr->cst->sup);
    int sgninf = expr->equality ? sgnsup : -bound_sgn(expr->cst->inf);
    switch(cons->constyp){
    case AP_CONS_EQ:
      unsat = sgnsup < 0 || sgninf > 0;
      break;
    case AP_CONS_SUPEQ:
      unsat = sgnsup<0;
      break;
    case AP_CONS_SUP:
      unsat = (sgnsup<=0);
      break;
    default:
      abort();
    }
    if (unsat){
      box_set_bottom(a);
      globalchange = true;
    }
    else
      globalchange = false;
  }
 _box_meet_box_lincons_exit:  
  return globalchange;
}

/* Meet of an abstract value with an array of constraint */
void box_meet_lincons_array_internal(box_internal_t* intern,
				     box_t* a,
				     const ap_lincons0_array_t* array,
				     size_t kmax)
{
  size_t i,k;
  bool change;
  itv_lincons_t cons;
  
  if (kmax<1) kmax=2;
  
  /* We initialize stuff */
  itv_lincons_init(&cons);

  /* we possibly perform kmax passes */
  for (k=0;k<kmax;k++){
    change = false;
    for (i=0; i<array->size; i++){
      if (array->p[i].constyp==AP_CONS_EQ ||
	  array->p[i].constyp==AP_CONS_SUPEQ ||
	  array->p[i].constyp==AP_CONS_SUP){
	itv_lincons_set_ap_lincons0(intern->itv,&cons,&array->p[i]);
	change = 
	  box_meet_lincons_internal(intern,a,&cons)
	  ||
	  change
	  ;
	if (a->p==NULL) break;
      }
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
  size_t kmax;
  box_internal_t* intern = (box_internal_t*)man->internal;

  res = destructive ? a : box_copy(man,a);
  if (a->p==NULL){
    man->result.flag_best = tbool_true;
    man->result.flag_exact = tbool_true;
  }
  else {
    man->result.flag_best = array->size>1 ? tbool_top : tbool_true;
    man->result.flag_exact = tbool_top;
    kmax = man->option.funopt[AP_FUNID_MEET_LINCONS_ARRAY].algorithm;
    if (kmax<1) kmax=2;
    box_meet_lincons_array_internal(intern,res,array,kmax);
  }
  return res;
}
