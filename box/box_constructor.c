/* ********************************************************************** */
/* box_constructor.c: constructors */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "box_internal.h"
#include "box_representation.h"
#include "ap_generic.h"

#include "itv_linexpr.h"

/* ********************************************************************** */
/* 1. Basic constructors */
/* ********************************************************************** */

/* We assume that dimensions [0..intdim-1] correspond to integer variables, and
   dimensions [intdim..intdim+realdim-1] to real variables */

/* Create a bottom (empty) value */
box_t* box_bottom(ap_manager_t* man, size_t intdim, size_t realdim)
{
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  return box_alloc(intdim,realdim);
}

/* Create a top (universe) value */
box_t* box_top(ap_manager_t* man, size_t intdim, size_t realdim)
{
  int i;
  box_t* a = box_alloc(intdim,realdim);
  box_init(a);
  for(i=0;i<a->intdim+a->realdim; i++){
    itv_set_top(a->p[i]);
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  return a;
}

/* Abstract an hypercube defined by the array of intervals
   of size intdim+realdim */
box_t* box_of_box(ap_manager_t* man,
		  size_t intdim, size_t realdim,
		  const ap_interval_t*const* tinterval)
{
  int i;
  bool exc;
  box_internal_t* intern = box_init_from_manager(man,AP_FUNID_OF_BOX);

  box_t* a = box_alloc(intdim,realdim);
  if (intdim+realdim!=0){
    box_init(a);
    for(i=0;i<intdim+realdim; i++){
      itv_set_ap_interval(intern->itv,a->p[i],tinterval[i]);
      exc = itv_canonicalize(intern->itv,a->p[i],i<intdim);
      if (exc) { box_set_bottom(a); break; }
    }
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  return a;
}

box_t* box_of_lincons_array(ap_manager_t* man,
			    size_t intdim, size_t realdim,
			    const ap_lincons0_array_t* array)
{
  return (box_t*)ap_generic_of_lincons_array(man,intdim,realdim,array);
}

/* ********************************************************************** */
/* 2. Accessors */
/* ********************************************************************** */

ap_dimension_t box_dimension(ap_manager_t* man, const box_t* a)
{ 
  ap_dimension_t res;
  res.intdim = a->intdim;
  res.realdim = a->realdim;
  return res;
}

/* ********************************************************************** */
/* 3. Tests */
/* ********************************************************************** */

/* If any of the following functions returns tbool_top, this means that
   an exception has occured, or that the exact computation was
   considered too expensive to be performed (according to the options).
   The flag exact and best should be cleared in such a case. */

tbool_t box_is_bottom(ap_manager_t* man, const box_t* a)
{
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  return tbool_of_bool(a->p==NULL && a->intdim + a->realdim>0);
}

tbool_t box_is_top(ap_manager_t* man, const box_t* a)
{
  int i;
  tbool_t res;
  size_t nbdims = a->intdim + a->realdim;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  if (nbdims==0)
    return tbool_true;

  if (a->p==NULL)
    return tbool_false;

  res = tbool_true;
  for (i=0;i<nbdims;i++){
    if (! itv_is_top(a->p[i])){
      res = tbool_false;
      break;
    }
  }
  return res;
}

/* inclusion check */
tbool_t box_is_leq(ap_manager_t* man, const box_t* a, const box_t* b)
{
  int i;
  tbool_t res;
  size_t nbdims;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  nbdims = a->intdim + a->realdim;
  if (a->p==NULL)
    return tbool_true;
  else if (b->p==NULL)
    return tbool_false;

  res = tbool_true;
  for (i=0;i<nbdims;i++){
    if (! itv_is_leq(a->p[i],b->p[i])){
      res = tbool_false;
      break;
    }
  }
  return res;
}

/* equality check */
tbool_t box_is_eq(ap_manager_t* man, const box_t* a, const box_t* b)
{
  int i;
  tbool_t res;
  size_t nbdims;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  nbdims = a->intdim + a->realdim;
  if (a->p==NULL)
    return tbool_of_bool(b->p==NULL);
  else if (b->p==NULL)
    return tbool_false;

  res = tbool_true;
  for (i=0;i<nbdims;i++){
    if (! itv_is_eq(a->p[i],b->p[i])){
      res = tbool_false;
      break;
    }
  }
  return res;
}

tbool_t box_is_dimension_unconstrained(ap_manager_t* man, const box_t* a, const ap_dim_t dim)
{
  return (itv_is_top(a->p[dim])) ? tbool_true : tbool_false;
}

/* is the dimension included in the interval in the abstract value ? */
tbool_t box_sat_interval(ap_manager_t* man, 
			 const box_t* a,
			 ap_dim_t dim, const ap_interval_t* interval)
{
  box_internal_t* intern = box_init_from_manager(man,AP_FUNID_SAT_INTERVAL);
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  if (a->p==NULL)
    return tbool_true;

  ap_interval_set_itv(intern->itv, intern->sat_interval_interval, a->p[dim]);
  return tbool_of_bool(ap_interval_is_leq(intern->sat_interval_interval,interval));
}

/* does the abstract value satisfy the linear constraint ? */
tbool_t box_sat_lincons(ap_manager_t* man, 
			const box_t* a, const ap_lincons0_t* cons)
{
  tbool_t res;
  box_internal_t* intern = box_init_from_manager(man,AP_FUNID_SAT_LINCONS);
  
  man->result.flag_best = tbool_top;
  man->result.flag_exact = tbool_top;
  
  if (a->p==NULL)
    return tbool_true;
  
  bool exact = itv_eval_ap_linexpr0(intern->itv,
				    intern->sat_lincons_itv, 
				    a->p, 
				    cons->linexpr0);

  man->result.flag_exact = man->result.flag_best = 
    exact ? tbool_true : tbool_top;

  switch (cons->constyp){
  case AP_CONS_EQ:
    res =
      (bound_sgn(intern->sat_lincons_itv->inf)==0 &&
       bound_sgn(intern->sat_lincons_itv->sup)==0) ?
      tbool_true :
      tbool_top;
    break;
  case AP_CONS_SUPEQ:
    if (bound_sgn(intern->sat_lincons_itv->inf)<=0)
      res = tbool_true;
    else if (bound_sgn(intern->sat_lincons_itv->sup)<0)
      res = tbool_false;
    else
      res = tbool_top;
    break;
  case AP_CONS_SUP:
     if (bound_sgn(intern->sat_lincons_itv->inf)<0)
      res = tbool_true;
    else if (bound_sgn(intern->sat_lincons_itv->sup)<=0)
      res = tbool_false;
    else
      res = tbool_top;
     break;
  default:
    abort();
    res = tbool_top;
  }
  return res;
}

/* ********************************************************************** */
/* II.4 Extraction of properties */
/* ********************************************************************** */

ap_interval_t* box_bound_dimension(ap_manager_t* man,
				   const box_t* a, ap_dim_t dim)
{
  bool exact;
  box_internal_t* intern = (box_internal_t*)man->internal;
  ap_interval_t* interval = ap_interval_alloc();
  if (a->p==NULL){
    ap_interval_set_bottom(interval);
    exact = true;
  }
  else {
    exact = ap_interval_set_itv(intern->itv,interval,a->p[dim]);
    
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = exact ? tbool_true : tbool_top;
  return interval;
}

/* Returns the interval taken by a linear expression
   over the abstract value */
ap_interval_t* box_bound_linexpr(ap_manager_t* man,
				 const box_t* a, const ap_linexpr0_t* expr)
{
  bool exact;
  ap_interval_t* interval = ap_interval_alloc();
  box_internal_t* intern = (box_internal_t*)man->internal;

  if (a->p==NULL){
    ap_interval_set_bottom(interval);
    exact = true;
  }
  else {
    exact = itv_eval_ap_linexpr0(intern->itv,
				 intern->bound_linexpr_itv,
				 a->p,
				 expr);
    ap_interval_set_itv(intern->itv, interval,intern->bound_linexpr_itv);
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = exact ? tbool_true : tbool_top;
  return interval;
}

/* Converts an abstract value to a polyhedra
   (conjunction of linear constraints).
   The size of the returned array is stored in size. */
ap_lincons0_array_t box_to_lincons_array(ap_manager_t* man, const box_t* a)
{
  int i;
  ap_lincons0_array_t array;
  size_t nbdims = a->intdim + a->realdim;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  if (nbdims==0){
    array = ap_lincons0_array_make(0);
  }
  else if (a->p==NULL){
    array = ap_lincons0_array_make(1);
    array.p[0] = ap_lincons0_make_unsat();
  }
  else {
    size_t size;
    ap_linexpr0_t* expr;
    ap_scalar_t* scalar;

    size = 0;
    for (i=0;i<nbdims;i++){
      if (!bound_infty(a->p[i]->inf)) size++;
      if (!bound_infty(a->p[i]->sup)) size++;
    }
    array = ap_lincons0_array_make(size);
    size = 0;
    for (i=0;i<nbdims;i++){
      if (!bound_infty(a->p[i]->inf)){
	expr = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,1);
	ap_coeff_set_scalar_int(&expr->p.linterm[0].coeff, 1);
	expr->p.linterm[0].dim = i;

	ap_coeff_reinit(&expr->cst,AP_COEFF_SCALAR,AP_SCALAR_DOUBLE);
	scalar = expr->cst.val.scalar;

	ap_scalar_set_bound(scalar,a->p[i]->inf);
	ap_scalar_neg(scalar,scalar);
	array.p[size].constyp = AP_CONS_SUPEQ;
	array.p[size].linexpr0 = expr;
	size++;
      }
      if (!bound_infty(a->p[i]->sup)){
	expr = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,1);
	ap_coeff_set_scalar_int(&expr->p.linterm[0].coeff, -1);
	expr->p.linterm[0].dim = i;
	ap_scalar_set_bound(expr->cst.val.scalar,a->p[i]->sup);
	array.p[size].constyp = AP_CONS_SUPEQ;
	array.p[size].linexpr0 = expr;
	size++;
      }
    }
  }
  return array;
}

ap_generator0_array_t box_to_generator_array(ap_manager_t* man, const box_t* a)
{
  size_t i,j,size;
  size_t nbcoeffs,nblines,nbrays,nbvertices,l,r,v;
  ap_generator0_array_t array;
  ap_linexpr0_t* vertex;
  ap_scalar_t scalar;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  
  size = a->intdim+a->realdim;
  if (size==0 || a->p==NULL){
    array = ap_generator0_array_make(0);
    return array;
  }
  /* Count the number of generators */
  nbvertices = 1; /* we take into account the vertex */
  nbrays = 0;
  nblines = 0;
  nbcoeffs = 0;
  for (i=0;i<size;i++){
    itv_ptr itv = a->p[i];
    bool iinf = bound_infty(itv->inf);
    bool isup = bound_infty(itv->sup);
    if (iinf && isup){
      nblines++;
    }
    else {
      nbcoeffs++;
      if (iinf || isup){
	nbrays++;
      }
      else {
	nbvertices *= 2;
      }
    }
  }
  /* Preparation */
  array = ap_generator0_array_make(nblines+nbrays+nbvertices);
  ap_scalar_init(&scalar,AP_SCALAR_DOUBLE);
  ap_scalar_set_double(&scalar,0.0);
  /* Let's go now ! */  
  v = r = l = 0;
  /* Creates the vertices */
  vertex = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,nbcoeffs);
  for (i=0;i<size;i++){
    itv_ptr itv = a->p[i];
    bool iinf = bound_infty(itv->inf);
    bool isup = bound_infty(itv->sup);
    if (!iinf || !isup){
      ap_linexpr0_set_coeff_scalar(vertex,i,&scalar);
    }
  }
  array.p[nblines+nbrays + v] = ap_generator0_make(AP_GEN_VERTEX,vertex);
  v=1;
  for (i=0; i<size; i++){
    itv_ptr itv = a->p[i];
    bool iinf = bound_infty(itv->inf);
    bool isup = bound_infty(itv->sup);
    if (!iinf && !isup){
      /* Duplication, and in same time set inf and sup */
      for (j=0; j<v; j++){
	array.p[nblines+nbrays + v + j] = 
	  ap_generator0_copy(&array.p[nblines+nbrays + j]);
	ap_scalar_set_bound(&scalar,itv->inf);
	ap_scalar_neg(&scalar,&scalar);
	ap_linexpr0_set_coeff_scalar(array.p[nblines+nbrays + j].linexpr0,
				     i,&scalar);
	ap_scalar_set_bound(&scalar,itv->sup);
	ap_linexpr0_set_coeff_scalar(array.p[nblines+nbrays + v + j].linexpr0,
				     i,&scalar);
      }
      v *= 2;
    }
  }
  /* Create lines and rays */
  for (i=0; i<size; i++){
    ap_linexpr0_t* expr;
    itv_ptr itv = a->p[i];
    bool iinf = bound_infty(itv->inf);
    bool isup = bound_infty(itv->sup);
    if (iinf || isup){
      expr = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,1);
      if (iinf && isup){
	ap_linexpr0_set_coeff_scalar_double(expr,i,1.0);
	array.p[l] = ap_generator0_make(AP_GEN_LINE,expr);
	l++;
      }
      else if (isup){
	ap_linexpr0_set_coeff_scalar_double(expr,i,1.0);
	array.p[nblines+r] = ap_generator0_make(AP_GEN_RAY,expr);
	r++;
	for (j=nblines+nbrays; j<nblines+nbrays+v; j++){
	  ap_scalar_set_bound(&scalar,itv->inf);
	  ap_scalar_neg(&scalar,&scalar);
	  ap_linexpr0_set_coeff_scalar(expr,i,&scalar);
	}	   
      }
      else if (iinf){
	ap_linexpr0_set_coeff_scalar_double(expr,i,-1.0);
	array.p[nblines+r] = ap_generator0_make(AP_GEN_RAY,expr);
	r++;
	for (j=nblines+nbrays; j<nblines+nbrays+v; j++){
	  ap_scalar_set_bound(&scalar,itv->sup);
	  ap_linexpr0_set_coeff_scalar(expr,i,&scalar);
	}	   
      }
    }
  }
  /* Clear things */
  ap_scalar_clear(&scalar);
  return array;
}

/* Converts an abstract value to an interval/hypercube.
   The size of the resulting array is box_dimension(man,a).  This
   function can be reimplemented by using box_bound_linexpr */
ap_interval_t** box_to_box(ap_manager_t* man, const box_t* a)
{
  int i;
  ap_interval_t** interval;
  size_t nbdims;
  box_internal_t* intern = (box_internal_t*)man->internal;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  nbdims = a->intdim+a->realdim;
  if (nbdims==0){
    interval = NULL;
  }
  else {
    interval = ap_interval_array_alloc(nbdims);
     for (i=0; i<nbdims; i++){
       if (a->p==NULL){
	 ap_interval_set_top(interval[i]);
       } else {
	 ap_interval_set_itv(intern->itv,interval[i],a->p[i]);
       }
     }
  }
  return interval;
}
