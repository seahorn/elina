/*
 * oct_predicate.c
 *
 * Predicates on octagons, extraction functions
 *
 * APRON Library / Octagonal Domain
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

#include "oct.h"
#include "oct_internal.h"



/* ============================================================ */
/* Tests */
/* ============================================================ */

tbool_t oct_is_bottom(ap_manager_t* man, const oct_t* a)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_IS_BOTTOM,0);
  if (pr->funopt->algorithm>=0) oct_cache_closure(pr,a);
  if (a->closed) {
    /* definitively non empty on Q */
    if (num_incomplete || a->intdim) { flag_incomplete; return tbool_top; }
    else return tbool_false;
  }
  else if (!a->m)
    /* definitively empty */
    return tbool_true;
  else {
    /* no closure => we know that we don't know */
    flag_incomplete; 
    return tbool_top;
  }
}

tbool_t oct_is_top(ap_manager_t* man, const oct_t* a)
{
  size_t i,j;
  bound_t* m = a->m ? a->m : a->closed;
  if (!m) return tbool_false;
  for (i=0;i<2*a->dim;i++)
    for (j=0;j<=(i|1);j++,m++)
      if (!bound_infty(*m) && i!=j) 
	return tbool_false;
  return tbool_true;
}

tbool_t oct_is_leq(ap_manager_t* man, const oct_t* a1, const oct_t* a2)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_IS_LEQ,0);
  arg_assert(a1->dim==a2->dim && a1->intdim==a2->intdim,return tbool_top;);
  if (pr->funopt->algorithm>=0) oct_cache_closure(pr,a1);
  if (!a1->closed && !a1->m) {
    /* a1 definitively empty */
    return tbool_true;   
  }
  else if (!a2->closed && !a2->m) {
    /* a2 definitively empty */
    if (a1->closed) {
      /* a1 not empty on Q */
      if (num_incomplete || a1->intdim) { flag_incomplete; return tbool_top; }
      else return tbool_false;
    }
    else { flag_algo; return tbool_top; }
  }
  else {
    size_t i,j;
    bound_t *x = a1->closed ? a1->closed : a1->m;
    bound_t *y = a2->closed ? a2->closed : a2->m;
    for (i=0;i<2*a1->dim;i++)
      for (j=0;j<=(i|1);j++,x++,y++)
	if (bound_cmp(*x,*y)>0) {
	  if (a1->closed) {
	    /* not included on Q */
	    if (num_incomplete || a1->intdim) 
	      { flag_incomplete; return tbool_top; }
	    else return tbool_false;
	  }
	  else { flag_algo; return tbool_top; }
	}
    /* definitively included */
    return tbool_true;
  }
}


tbool_t oct_is_eq(ap_manager_t* man, const oct_t* a1, const oct_t* a2)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_IS_EQ,0);
  arg_assert(a1->dim==a2->dim && a1->intdim==a2->intdim,return tbool_top;);
  if (pr->funopt->algorithm>=0) {
    oct_cache_closure(pr,a1);
    oct_cache_closure(pr,a2);
  }
  if (!a1->closed && !a1->m) {
    if (!a2->closed && !a2->m) {
      /* both are empty */
      return tbool_true;
    }
    else if (a2->closed) {
      /* a1 empty, e2 not empty on Q */
      if (num_incomplete || a1->intdim) { flag_incomplete; return tbool_top; }
      else return tbool_false;
    }
    else { flag_algo; return tbool_top; }
  }
  else if (!a2->closed && !a2->m) {
    if (a1->closed) {
      /* a2 empty, e1 not empty on Q */
      if (num_incomplete || a1->intdim) { flag_incomplete; return tbool_top; }
      else return tbool_false;
    }
    else { flag_algo; return tbool_top; }
  }
  else {
    size_t i,j;
    bound_t *x = a1->closed ? a1->closed : a1->m;
    bound_t *y = a2->closed ? a2->closed : a2->m;
    for (i=0;i<2*a1->dim;i++)
      for (j=0;j<=(i|1);j++,x++,y++)
	if (bound_cmp(*x,*y)) {
	  if (a1->closed) {
	    /* not equal on Q */
	    if (num_incomplete || a1->intdim) 
	      { flag_incomplete; return tbool_top; }
	    else return tbool_false;
	  }
	  else { flag_algo; return tbool_top; }
	}
    /* definitively equal */
    return tbool_true;
  }
}


tbool_t oct_sat_interval(ap_manager_t* man, const oct_t* a,
			 ap_dim_t dim, const ap_interval_t* i)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_SAT_INTERVAL,0);
  arg_assert(dim<a->dim,return tbool_top;);
  if (pr->funopt->algorithm>=0) oct_cache_closure(pr,a);
  if (!a->closed && !a->m) {
    /* really empty */
    return tbool_true;
  }
  else {
    bound_t* m = a->closed ? a->closed : a->m;
    ap_interval_t* b = ap_interval_alloc();
    bool r;
    /* get (possibly approximated) bounds */
    interval_of_bounds(pr,b,
		       m[matpos(2*dim,2*dim+1)],m[matpos(2*dim+1,2*dim)],true);
    /* compare with i */
    r = (ap_scalar_cmp(b->inf,i->inf)>=0) && (ap_scalar_cmp(b->sup,i->sup)<=0);
    ap_interval_free(b);
    if (r) return tbool_true; /* definitively saturates */
    else
      if (!a->closed) 
	{ flag_algo; return tbool_top; } 
      else if (num_incomplete || a->intdim || pr->conv)
	{ flag_incomplete; return tbool_top; } 
      else return tbool_false; /* definitely does not saturate */
  }
}


tbool_t oct_is_dimension_unconstrained(ap_manager_t* man, const oct_t* a,
				       ap_dim_t dim)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_SAT_INTERVAL,0);
  arg_assert(dim<a->dim,return tbool_top;);
  if (!a->closed && !a->m)
    /* definitively empty */
    return tbool_false;
  else {
    bound_t* b = a->closed ? a->closed : a->m;
    size_t i, d2=2*dim;
    for (i=0;i<2*a->dim;i++) {
      if (!bound_infty(b[matpos2(i,d2)]) && i!=d2) return tbool_false;
      if (!bound_infty(b[matpos2(i,d2+1)]) && i!=d2+1) return tbool_false;
    }
    return tbool_true;
  }
}


/* NOT IMPLEMENTED: always return top */
tbool_t oct_sat_lincons(ap_manager_t* man, const oct_t* a, 
			const ap_lincons0_t* lincons)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_SAT_LINCONS,0);
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,pr->funid,
			     "not implemented");
  return tbool_top;
}


/* ============================================================ */
/* Extraction of properties */
/* ============================================================ */

/* we use only variable bounds and interval arithmetics, 
   we don't use relational information
   => this is not very precise...
 */
ap_interval_t* oct_bound_linexpr(ap_manager_t* man,
				 const oct_t* a, const ap_linexpr0_t* expr)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_BOUND_LINEXPR,12);
  ap_interval_t* r = ap_interval_alloc();
  if (pr->funopt->algorithm>=0) oct_cache_closure(pr,a);
  if (!a->closed && !a->m) {
    /* really empty */
    ap_interval_set_bottom(r);
  }
  else {
    bound_t* m = a->closed ? a->closed : a->m;
    size_t i;
    bounds_of_coeff(pr,pr->tmp[0],pr->tmp[1],expr->cst,true);
    switch (expr->discr) {
    case AP_LINEXPR_DENSE:
      arg_assert(expr->size<=a->dim,ap_interval_free(r);return NULL;);
      for (i=0;i<expr->size;i++) {
	bounds_of_coeff(pr,pr->tmp[2],pr->tmp[3],expr->p.coeff[i],false);
	bounds_mul(pr->tmp[2],pr->tmp[3],
		   m[matpos(2*i,2*i+1)],m[matpos(2*i+1,2*i)],
		   pr->tmp[2],pr->tmp[3],pr->tmp+4);
	bound_add(pr->tmp[0],pr->tmp[0],pr->tmp[2]);
	bound_add(pr->tmp[1],pr->tmp[1],pr->tmp[3]);
      }
      break;
    case AP_LINEXPR_SPARSE:
      for (i=0;i<expr->size;i++) {	
	ap_dim_t d = expr->p.linterm[i].dim;
	arg_assert(d<a->dim,ap_interval_free(r);return NULL;);
	bounds_of_coeff(pr,pr->tmp[2],pr->tmp[3],expr->p.linterm[i].coeff,
			false);
	bounds_mul(pr->tmp[2],pr->tmp[3],
		   m[matpos(2*d,2*d+1)],m[matpos(2*d+1,2*d)],
		   pr->tmp[2],pr->tmp[3],pr->tmp+4);
	bound_add(pr->tmp[0],pr->tmp[0],pr->tmp[2]);
	bound_add(pr->tmp[1],pr->tmp[1],pr->tmp[3]);
      }
      break;
    default: arg_assert(0,ap_interval_free(r);return NULL;);
    }
    interval_of_bounds(pr,r,pr->tmp[0],pr->tmp[1],true);
    /* we are never optimal, even when closing a */
    flag_incomplete;
  }
  return r;
}

ap_interval_t* oct_bound_dimension(ap_manager_t* man,
				   const oct_t* a, ap_dim_t dim)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_BOUND_DIMENSION,0);
  ap_interval_t* r = ap_interval_alloc();
  arg_assert(dim<a->dim,ap_interval_free(r);return NULL;);
  if (pr->funopt->algorithm>=0) oct_cache_closure(pr,a);
  if (!a->closed && !a->m) {
    /* really empty */
    ap_interval_set_bottom(r);
  }
  else if (a->closed) {
    /* tightest bounds in Q */
    interval_of_bounds(pr,r,
		       a->closed[matpos(2*dim,2*dim+1)],
		       a->closed[matpos(2*dim+1,2*dim)],true);
    if (num_incomplete || a->intdim) flag_incomplete;
    else if (pr->conv) flag_conv;
  }
  else {
    /* no tightest bounds */
    interval_of_bounds(pr,r,
		       a->m[matpos(2*dim,2*dim+1)],a->m[matpos(2*dim+1,2*dim)],
		       true);
    flag_algo;
  }
  return r;
}


ap_lincons0_array_t oct_to_lincons_array(ap_manager_t* man, const oct_t* a)
{
  ap_lincons0_array_t ar;
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_TO_LINCONS_ARRAY,0);
  if (!a->closed && !a->m) {
    /* definitively empty */
    ar = ap_lincons0_array_make(1);
    ar.p[0] = ap_lincons0_make_unsat();
  }
  else {
    /* put non-oo constraint bounds only */
    bound_t* m = a->m ? a->m : a->closed; /* not closed => less constraints */
    size_t i,j,n=0;
    ar = ap_lincons0_array_make(matsize(a->dim));
    for (i=0;i<2*a->dim;i++)
      for (j=0;j<=(i|1);j++,m++) {
	if (i==j || bound_infty(*m)) continue;
	ar.p[n] = lincons_of_bound(pr,i,j,*m);
	n++;
      }
    ar.size = n;
    if (pr->conv) flag_conv;
  }
  return ar;
}

ap_interval_t** oct_to_box(ap_manager_t* man, const oct_t* a)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_TO_BOX,0);
  ap_interval_t** in = ap_interval_array_alloc(a->dim);
  size_t i;
  if (pr->funopt->algorithm>=0) oct_cache_closure(pr,a);
  if (!a->closed && !a->m) {
    /* definitively empty */
    for (i=0;i<a->dim;i++)
      ap_interval_set_bottom(in[i]);
  }
  else {
    /* put variable bounds */
    bound_t* m = a->closed ? a->closed : a->m;
    for (i=0;i<a->dim;i++)
      interval_of_bounds(pr,in[i],
			 m[matpos(2*i,2*i+1)],m[matpos(2*i+1,2*i)],true);
    if (!a->closed) flag_algo;
    else if (num_incomplete || a->intdim) flag_incomplete;
    else if (pr->conv) flag_conv;
    else man->result.flag_exact = tbool_false;
  }
  return in;
}

/* not really implemented (returns either top or bottom) */
ap_generator0_array_t oct_to_generator_array(ap_manager_t* man, const oct_t* a)
{
  oct_internal_t* pr = oct_init_from_manager(man,AP_FUNID_TO_GENERATOR_ARRAY,0);
  if (pr->funopt->algorithm>=0) oct_cache_closure(pr,a);
  if (!a->closed && !a->m) {
    /* definitively empty */
    return ap_generator0_array_make(0);
  }
  else {
    /* not empty => full universe */
    ap_generator0_array_t ar = ap_generator0_array_make(a->dim+1);
    size_t i;
    /* origin vertex */
    ar.p[0] = ap_generator0_make(AP_GEN_VERTEX,
				 ap_linexpr0_alloc(AP_LINEXPR_SPARSE,0));
    /* one line for each dimension */
    for (i=0;i<a->dim;i++) {
      ap_linexpr0_t* e = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,1);
      e->p.linterm[0].dim = i;
      ap_coeff_set_scalar_int(&e->p.linterm[0].coeff,1);
      ar.p[i+1] = ap_generator0_make(AP_GEN_LINE,e);
    }
    flag_incomplete;
    return ar;
  }
}
