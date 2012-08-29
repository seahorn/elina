/* ********************************************************************** */
/* pk_approximate.c: approximate function  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk_cherni.h"
#include "pk.h"
#include "pk_user.h"
#include "pk_representation.h"
#include "pk_extract.h"
#include "pk_constructor.h"

static
void poly_set_save_C(pk_t* po, pk_t* pa)
{
  if (po != pa){
    po->F = pa->F ? matrix_copy(pa->F) : NULL;
    po->satC = pa->satC ? satmat_copy(pa->satC) : NULL;
    po->satF = pa->satF ? satmat_copy(pa->satF) : NULL;
    po->status = pa->status;
      po->nbeq = pa->nbeq;
      po->nbline = pa->nbline;
      po->status = pa->status;
  }
}

/* ---------------------------------------------------------------------- */
/* Normalize integer constraints.  Return true if there is some change. */
/* ---------------------------------------------------------------------- */
static
bool poly_approximate_n1(ap_manager_t* man, pk_t* po, pk_t* pa, int algorithm)
{
  if (po->intdim>0){
    pk_internal_t* pk = (pk_internal_t*)man->internal;
    bool change;

    poly_obtain_C(man,pa,NULL);
    if (!pa->C){
      poly_set(po,pa);
      return false;
    }
    if (po!=pa){
      po->C = matrix_copy(pa->C);
    }
    change = matrix_normalize_constraint_int(pk,po->C,po->intdim,po->realdim);
    if (change){
      {
	/* Add positivity and strictness that may not be implied any more */
	size_t nbrows = po->C->nbrows;
	matrix_resize_rows_lazy(po->C,nbrows+pk->dec-1);
	matrix_fill_constraint_top(pk,po->C,nbrows);
      }
      if (po==pa){
	if (po->F){ matrix_free(po->F); po->F = NULL; }
	if (po->satC){ satmat_free(po->satC); po->satC = NULL; }
	if (po->satF){ satmat_free(po->satF); po->satF = NULL; }
      }
      po->status = 0;
      po->C->_sorted = false;
      man->result.flag_exact = false;
    } else {
      poly_set_save_C(po,pa);
    }
    return change;
  }
  else
    return false;
}

/* ---------------------------------------------------------------------- */
/* Remove constraints with coefficients of size greater than
   pk->max_coeff_size, if pk->max_coeff_size > 0 */
/* ---------------------------------------------------------------------- */
static
bool matrix_approximate_constraint_1(pk_internal_t* pk, matrix_t* C)
{
  size_t i,j;
  bool change,removed;
  change = false;
  i=0;
  while (i<C->nbrows){
    removed = false;
    if (numint_sgn(C->p[i][0])){
      for (j=pk->dec; j<C->nbcolumns; j++){
	if (numint_size2(C->p[i][j]) > pk->approximate_max_coeff_size){
	  change = true;
	  removed = true;
	  C->nbrows--;
	  matrix_exch_rows(C,i,C->nbrows);
	  break;
	}
      }
    }
    if (!removed) i++;
  }
  if (change){
    /* Add positivity and strictness that may not be implied any more */
    size_t nbrows = C->nbrows;
    matrix_resize_rows_lazy(C,nbrows+pk->dec-1);
    matrix_fill_constraint_top(pk,C,nbrows);
    C->_sorted = false;
  }
  return change;
}
static
bool poly_approximate_1(ap_manager_t* man, pk_t* po, pk_t* pa)
{
  bool change;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  poly_obtain_C(man,pa,NULL);
  if (!pa->C){
    poly_set(po,pa);
    return false;
  }
  if (po!=pa){
    po->C = matrix_copy(pa->C);
  }
  change = matrix_approximate_constraint_1(pk,po->C);
   if (change){
     if (po==pa){
       if (po->F){ matrix_free(po->F); po->F = NULL; }
       if (po->satC){ satmat_free(po->satC); po->satC = NULL; }
       if (po->satF){ satmat_free(po->satF); po->satF = NULL; }
     }
     po->status = 0;
     man->result.flag_exact = false;
   } else {
     poly_set_save_C(po,pa);
     man->result.flag_exact = true;
   }
   return change;
}

/* ---------------------------------------------------------------------- */
/* 1. Remove constraints with too big coefficients
   2. Add interval constraints
   3. Add octagonal constraints
*/
/* ---------------------------------------------------------------------- */
static
void poly_approximate_123(ap_manager_t* man, pk_t* po, int algorithm)
{
  bool change;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  if (algorithm==1){
    poly_approximate_1(man,po,po);
  }
  else {
    size_t nbrows, nbrows2;
    ap_dim_t dim;
    ap_dim_t i,j;
    int sgn, sgny;
    itv_t itv;

    pk_t* pa = poly_alloc(po->intdim,po->realdim);

    change = poly_approximate_1(man,pa,po);
    if (!change){
      pk_free(man,pa);
      return;
    }
    poly_obtain_F(man,po,NULL);
    if (!po->F){
      if (!po->C){
	pk_free(man,pa);
	poly_set_bottom(pk,po);
	man->result.flag_exact = true;
	return;
      }
      else {
	poly_clear(po);
	*po = *pa;
	free(pa);
      }
      return;
    }
    dim = pa->intdim + pa->realdim;
    nbrows = pa->C->nbrows;
    itv_init(itv);
    if (algorithm>=2){ /* Add interval constraints */
      nbrows2 = 2*dim;
      matrix_resize_rows_lazy(pa->C, nbrows + nbrows2);
      pa->C->_sorted = false;
      for (i=0; i<dim;i++){
	matrix_bound_dimension(pk,itv,i,po->F);
	if (!bound_infty(itv->inf)){
	  vector_set_dim_bound(pk,
			       pa->C->p[nbrows],
			       i, bound_numref(itv->inf),
			       -1,
			       po->intdim, po->realdim, false);
	  nbrows++;
	}
	if (!bound_infty(itv->sup)){
	  vector_set_dim_bound(pk,
			       pa->C->p[nbrows],
			       i, bound_numref(itv->sup),
			       1,
			       po->intdim, po->realdim, false);
	  nbrows++;
	}
      }
    }
    pa->C->nbrows = nbrows;
    if (algorithm>=3){ /* Add octagonal constraints */
      vector_clear(pk->poly_numintp,po->F->nbcolumns);
      numint_set_int(pk->poly_numintp[0],1);
      for (i=0; i<dim;i++){
	numint_set_int(pk->poly_numintp[pk->dec+i],1);
	nbrows2 = 4*(dim-i-1);
	matrix_resize_rows_lazy(pa->C, nbrows + nbrows2);
	for (j=i+1; j<dim;j++){
	  for (sgny=-1; sgny<=1; sgny += 2){
	    numint_set_int(pk->poly_numintp[pk->dec+j],sgny);
	    matrix_bound_vector(pk,itv,pk->poly_numintp,po->F);
	    if (!bound_infty(itv->inf)){
	      vector_set_linexpr_bound(pk,
				       pa->C->p[nbrows], pk->poly_numintp,
				       itv->inf, -1,
				       po->intdim, po->realdim, false);
	      nbrows++;
	    }
	    if (!bound_infty(itv->sup)){
	      vector_set_linexpr_bound(pk,
				       pa->C->p[nbrows], pk->poly_numintp,
				       itv->sup, 1,
				       po->intdim, po->realdim, false);
	      nbrows++;
	    }
	  }
	  numint_set_int(pk->poly_numintp[pk->dec+j],0);
	}
	numint_set_int(pk->poly_numintp[pk->dec+i],0);
      }
      pa->C->nbrows = nbrows;
    }
    itv_clear(itv);
    poly_clear(po);
    *po = *pa;
    free(pa);
    return;
  }
}

/* ---------------------------------------------------------------------- */
/* 10. Round constraints with too big coefficients */
/* ---------------------------------------------------------------------- */
static
bool matrix_approximate_constraint_10(pk_internal_t* pk, matrix_t* C, matrix_t* F)
{
  size_t i,j,size;
  bool change,removed;
  itv_t itv;

  itv_init(itv);
  change = false;
  i = 0;
  while (i<C->nbrows){
    removed = false;
    if (numint_sgn(C->p[i][0]) &&
	(pk->strict ? numint_sgn(C->p[i][polka_eps])<=0 : true)){
      /* Look for a too big coefficient in the row */
      size=0; /* for next test */
      for (j=pk->dec; j<C->nbcolumns; j++){
	size = numint_size2(C->p[i][j]);
	if (size > pk->approximate_max_coeff_size){
	  /* Too big coefficient detected in the row */
	  break;
	}
      }
      if (size > pk->approximate_max_coeff_size){
	/* Too big coefficient detected in the row */
	/* A. Compute maximum magnitude */
	size_t maxsize = size;
	for (j=j+1; j<C->nbcolumns; j++){
	  size = numint_size2(C->p[i][j]);
	  if (size>maxsize) maxsize=size;
	}
	/* B. Relax if constraint is strict */
	if (pk->strict){
	  numint_set_int(C->p[i][polka_eps],0);
	}
	/* C. Perform rounding of non constant coefficients */
	size = maxsize - pk->approximate_max_coeff_size;
	for (j=pk->dec; j<C->nbcolumns; j++){
	  numint_tdiv_q_2exp(C->p[i][j],C->p[i][j], size);
	}
	/* D. Compute new constant coefficient */
	numint_set_int(C->p[i][0],1);
	numint_set_int(C->p[i][polka_cst],0);
	matrix_bound_vector(pk,itv,C->p[i],F);
	if (bound_infty(itv->inf)){
	  /* If no bound, we remove the constraint */
	  C->nbrows--;
	  matrix_exch_rows(C,i,C->nbrows);
	  removed = true;
	}
	else {
	  if (false){
	    /* we multiply by the denominator of inf value */
	    if (numint_cmp_int(numrat_denref(bound_numref(itv->inf)),1)!=0){
	      for (j=pk->dec; j<C->nbcolumns; j++){
		numint_mul(C->p[i][j],C->p[i][j],numrat_denref(bound_numref(itv->inf)));
	      }
	    }
	    numint_neg(C->p[i][polka_cst],numrat_numref(bound_numref(itv->inf)));
	  } else {
	    /* Otherwise, we round the constant to an integer */
	    for (j=pk->dec; j<C->nbcolumns; j++){
	      numint_mul_2exp(C->p[i][j],C->p[i][j],1);
	    }
	    bound_neg(itv->inf,itv->inf);
	    numint_mul_2exp(numrat_numref(bound_numref(itv->inf)),
			     numrat_numref(bound_numref(itv->inf)),
			     1);
	    numint_fdiv_q(numrat_numref(bound_numref(itv->inf)),
			  numrat_numref(bound_numref(itv->inf)),
			  numrat_denref(bound_numref(itv->inf)));
	    numint_neg(C->p[i][polka_cst],numrat_numref(bound_numref(itv->inf)));
	  }
	  vector_normalize(pk,C->p[i],C->nbcolumns);
	}
      	change = true;
      }
    }
    if (!removed) i++;
  }
  if (change){
    /* Add positivity and strictness that may not be implied any more */
    size_t nbrows = C->nbrows;
    matrix_resize_rows_lazy(C,nbrows+pk->dec-1);
    matrix_fill_constraint_top(pk,C,nbrows);
    C->_sorted = false;
  }
  itv_clear(itv);
  return change;
}
static
void poly_approximate_10(ap_manager_t* man, pk_t* po)
{
  bool change;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  poly_chernikova(man,po,"of the argument");
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    return;
  }
  if (!po->C && !po->F){
    return;
  }
  assert(po->C && po->F);
  change = matrix_approximate_constraint_10(pk, po->C, po->F);
  if (change){
    if (po->F) matrix_free(po->F);
    if (po->satC) satmat_free(po->satC);
    if (po->satF) satmat_free(po->satF);
    po->F = NULL;
    po->satC = NULL;
    po->satF = NULL;
    man->result.flag_exact = false;
  }
  else {
     man->result.flag_exact = true;
  }
}

/*
Approximation:

- algorithm==0: do nothing

- algorithm==-1: normalize integer minimal constraints (induces a smaller
		 polyhedron)

- algorithm==1: remove constraints with coefficients of size greater than
		max_coeff_size, if max_coeff_size > 0
- algorithm==2: in addition, keep same bounding box (more precise)
- algorithm==3: in addition, keep same bounding octagon (even more precise)

- algorithm==10: round constraints with too big coefficients, of size greater than
		 approximate_max_coeff_size, if approximate_max_coeff_size>0

*/
void pk_approximate(ap_manager_t* man, pk_t* po, int algorithm)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_APPROXIMATE);

  man->result.flag_exact = false;
  man->result.flag_best = false;

  if (!po->C && !po->F)
    return;

  switch (algorithm){
  case -1:
    if (po->intdim>0)
      poly_approximate_n1(man,po,po,algorithm);
    break;
  case 1:
  case 2:
  case 3:
    if (pk->approximate_max_coeff_size>0)
      poly_approximate_123(man,po,algorithm);
    break;
  case 10:
     if (pk->approximate_max_coeff_size>0)
       poly_approximate_10(man,po);
     break;
  }
  assert(poly_check(pk,po));
}
