/*
 * oct_transfer.c
 *
 * Assignment and guard transfer functions
 *
 * APRON Library / Octagonal Domain
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

#include "oct.h"
#include "oct_internal.h"


/* ============================================================ */
/* Utilities */
/* ============================================================ */

/* expression classification */
typedef struct {
  enum { 
    ZERO,     /* 0 */
    UNARY,    /* unary unit expression */
    BINARY,   /* binary unit expression */
    OTHER,
  } type;

  /* index and coefficient for unary / binary unit expressions */
  size_t i,j;
  int coef_i,coef_j; /* -1 or 1 */

} uexpr;

/* convert expression to bounds, look for unit unary or binary form */
static uexpr uexpr_of_linexpr(oct_internal_t* pr, bound_t* dst, 
			      const ap_linexpr0_t* e, size_t dim)
{
  
#define CLASS_COEF(idx,coef)						\
  if (!bound_cmp_int(dst[2*idx+2],-coef) &&				\
      !bound_cmp_int(dst[2*idx+3],coef)) {				\
    if (u.type==ZERO) { u.type = UNARY;  u.i = idx; u.coef_i = coef; }	\
    else              { u.type = BINARY; u.j = idx; u.coef_j = coef; }	\
    continue;								\
  }
  
#define CLASS_VAR(idx)							\
  if (!bound_sgn(dst[2*idx+2]) && !bound_sgn(dst[2*idx+3])) continue;	\
  if (u.type>=BINARY) { u.type = OTHER; continue; }			\
  CLASS_COEF(idx,1);							\
  CLASS_COEF(idx,-1);							\
  u.type = OTHER;
  
  uexpr u = { ZERO, 0, 0, 0, 0 };
  size_t i;
  bounds_of_coeff(pr,dst[0],dst[1],e->cst,true);
  switch (e->discr) {
  case AP_LINEXPR_DENSE:
    arg_assert(e->size<=dim,return u;);
    for (i=0;i<e->size;i++) {
      bounds_of_coeff(pr,dst[2*i+2],dst[2*i+3],e->p.coeff[i],false);
      CLASS_VAR(i);
    }
    for (;i<dim;i++) {
      bound_set_int(dst[2*i+2],0);
      bound_set_int(dst[2*i+3],0);
    }
    break;
  case AP_LINEXPR_SPARSE:
    for (i=0;i<dim;i++) {
      bound_set_int(dst[2*i+2],0);
      bound_set_int(dst[2*i+3],0);
    }
    for (i=0;i<e->size;i++) {
      size_t d = e->p.linterm[i].dim;
      arg_assert(d<dim,return u;);
      bounds_of_coeff(pr,dst[2*d+2],dst[2*d+3],e->p.linterm[i].coeff,false);
      CLASS_VAR(d);
    }
    break;
  default: arg_assert(0,return u;);
  }
  return u;
}

void bounds_of_generator(oct_internal_t* pr, bound_t* dst, 
			 const ap_linexpr0_t* e, size_t dim)
{
  size_t i;
  switch (e->discr) {
  case AP_LINEXPR_DENSE:
    arg_assert(e->size<=dim,return;);
    for (i=0;i<e->size;i++)
      bounds_of_coeff(pr,dst[2*i],dst[2*i+1],e->p.coeff[i],false);
    for (;i<dim;i++) {
      bound_set_int(dst[2*i],0);
      bound_set_int(dst[2*i+1],0);
    }
    break;
  case AP_LINEXPR_SPARSE:
    for (i=0;i<dim;i++) {
      bound_set_int(dst[2*i],0);
      bound_set_int(dst[2*i+1],0);
    }
    for (i=0;i<e->size;i++) {
      size_t d = e->p.linterm[i].dim;
      arg_assert(d<dim,return;);
      bounds_of_coeff(pr,dst[2*d],dst[2*d+1],e->p.linterm[i].coeff,false);
    }
    break;
  default: arg_assert(0,return;);
  }
}



/* ============================================================ */
/* Adding constraints / generators */
/* ============================================================ */

void hmat_add_lincons(oct_internal_t* pr, bound_t* b, size_t dim,
		      const ap_lincons0_array_t* ar)
{
  size_t i;
  for (i=0;i<ar->size;i++) {
    uexpr u = uexpr_of_linexpr(pr,pr->tmp,ar->p[i].linexpr0,dim);
    /* TODO */
  }
}


bound_t* hmat_add_generators(oct_internal_t* pr, bound_t* b, size_t dim,
			     const ap_generator0_array_t* ar)
{
  uexpr u;
  size_t i,j,k;
  bound_t* bb;

  /* add vertices */
  for (i=0;i<ar->size;i++) {
    if (ar->p[i].gentyp!=AP_GEN_VERTEX) continue;
    bounds_of_generator(pr,pr->tmp,ar->p[i].linexpr0,dim);
    if (b)
      /* not first vertex */
      for (bb=b,j=0;j<2*dim;j++) 
	for (k=0;k<=(j|1);k++,bb++) {
	  bound_sub(pr->tmp[2*dim],pr->tmp[j],pr->tmp[k]);
	  bound_max(*bb,*bb,pr->tmp[2*dim]);
	}
    else {
      /* first vertex */
      b = hmat_alloc_top(pr,dim);
      for (bb=b,j=0;j<2*dim;j++) 
	for (k=0;k<=(j|1);k++,bb++)
	  bound_sub(*bb,pr->tmp[j],pr->tmp[k]);
    }
  }

  if (!b) return NULL;

  /* add rays */
  for (i=0;i<ar->size;i++) {
    if (ar->p[i].gentyp!=AP_GEN_RAY) continue;
    bounds_of_generator(pr,pr->tmp,ar->p[i].linexpr0,dim);
    for (bb=b,j=0;j<2*dim;j++) 
      for (k=0;k<=(j|1);k++,bb++) {
	if (bound_cmp(pr->tmp[j],pr->tmp[k])>0)
	  bound_set_infty(*bb);
      }
   }

  /* add lines */
  for (i=0;i<ar->size;i++) {
    if (ar->p[i].gentyp!=AP_GEN_LINE) continue;
    bounds_of_generator(pr,pr->tmp,ar->p[i].linexpr0,dim);
    for (bb=b,j=0;j<2*dim;j++) 
      for (k=0;k<=(j|1);k++,bb++) {
	if (bound_cmp(pr->tmp[j],pr->tmp[k]))
	  bound_set_infty(*bb);
      }
   }

  return b;
}



/* ============================================================ */
/* Meet and Join */
/* ============================================================ */


oct_t* oct_meet_lincons_array(ap_manager_t* man,
			      bool destructive, oct_t* a,
			      const ap_lincons0_array_t* array)
{
  oct_internal_t* pr = 
    oct_init_from_manager(man,AP_FUNID_MEET_LINCONS_ARRAY,2*(a->dim+1));
  if (!a->closed && !a->m)
    /* definitively empty */
    return oct_set_mat(pr,a,NULL,NULL,destructive);
  else {
    size_t i;
    bound_t* m = a->closed ? a->closed : a->m;
    if (destructive) m = hmat_copy(pr,m,a->dim);
    hmat_add_lincons(pr,m,a->dim,array);
    flag_incomplete;
    return oct_set_mat(pr,a,m,NULL,destructive);
  }
}

oct_t* oct_add_ray_array(ap_manager_t* man,
			 bool destructive, oct_t* a,
			 const ap_generator0_array_t* array)
{
  oct_internal_t* pr = 
    oct_init_from_manager(man,AP_FUNID_ADD_RAY_ARRAY,2*(a->dim+1));
  if (!a->closed && !a->m)
    /* definitively empty */
    return oct_set_mat(pr,a,NULL,NULL,destructive);
  else {
    size_t i;
    bound_t* m = a->closed ? a->closed : a->m;
    if (destructive) m = hmat_copy(pr,m,a->dim);
    hmat_add_generators(pr,m,a->dim,array);
    /* best in Q and Z, except for conversion errors */
    man->result.flag_exact = tbool_top;
    if (pr->conv) flag_conv;
    return oct_set_mat(pr,a,m,NULL,destructive);
  }
}


/* ============================================================ */
/* Assignement and Substitutions */
/* ============================================================ */

oct_t* oct_assign_linexpr(ap_manager_t* man,
			  bool destructive, oct_t* a,
			  ap_dim_t dim, const ap_linexpr0_t* expr,
			  const oct_t* dest)
{
  return NULL; /* TODO */
}

oct_t* oct_substitute_linexpr(ap_manager_t* man,
			      bool destructive, oct_t* a,
			      ap_dim_t dim, const ap_linexpr0_t* expr,
			      const oct_t* dest)
{
  return NULL; /* TODO */
}

oct_t* oct_assign_linexpr_array(ap_manager_t* man,
				bool destructive, oct_t* a,
				const ap_dim_t* tdim,
				const ap_linexpr0_t** texpr,
				size_t size,
				const oct_t* dest)
{
  return NULL; /* TODO */
}

oct_t* oct_substitute_linexpr_array(ap_manager_t* man,
				    bool destructive, oct_t* a,
				    const ap_dim_t* tdim,
				    const ap_linexpr0_t** texpr,
				    size_t size,
				    const oct_t* dest)
{
  return NULL; /* TODO */
}
