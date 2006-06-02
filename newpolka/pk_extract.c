/* ********************************************************************** */
/* pk_extract.c: property extraction */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"
#include "pk_user.h"
#include "pk_representation.h"
#include "pk_extract.h"

/* ====================================================================== */
/* Auxiliary functions */
/* ====================================================================== */

/* Comparison of extended GMP rationals where +infty is encoded with 1/0 and
   -infty with -1/0. */

static inline void mpqi_set_infty(mpq_t a, int sgn)
{
  mpz_set_si(mpq_numref(a),sgn);
  mpz_set_ui(mpq_denref(a),0);
}

static int mpqi_cmp(mpq_t a, mpq_t b)
{
  int infa = mpz_sgn(mpq_denref(a))==0 ? mpz_sgn(mpq_numref(a)) : 0;
  int infb = mpz_sgn(mpq_denref(b))==0 ? mpz_sgn(mpq_numref(b)) : 0;

  if (infa==infb){
    if (infa!=0)
      return 0;
    else
      return mpq_cmp(a,b);
  }
  else
    return infa-infb;
}
static inline void mpqi_min(mpq_t a, mpq_t b)
{ if (mpqi_cmp(a,b)>0) mpq_set(a,b); }
static inline void mpqi_max(mpq_t a, mpq_t b)
{ if (mpqi_cmp(a,b)<0) mpq_set(a,b); }

/* Bounding the value of a dimension in a matrix of generators.
   mode == 1: sup bound
   mode == -1: inf bound
*/

void matrix_bound_dimension(pk_internal_t* pk,
			    mpq_t mpq,
			    ap_dim_t dim,
			    matrix_t* F,
			    int mode)
{
  size_t i, index;
  int sgn;

  assert(mode!=0 && pk->dec+dim<F->nbcolumns);
  
  mpqi_set_infty(mpq,-mode);
  index = pk->dec+dim;
  for (i=0; i<F->nbrows; i++){
    if (!pk->strict || numint_sgn(F->p[i][polka_eps])==0){
      sgn = numint_sgn(F->p[i][index]);
      if (numint_sgn(F->p[i][0])==0){
	/* line: result should be zero */
	if (sgn){
	  mpqi_set_infty(mpq,mode);
	  return;
	}
      }
      else if (numint_sgn(F->p[i][polka_cst])==0){
	/* ray */
	if ( (sgn>0 && mode>0) || (sgn<0 && mode<0) ){
	  mpqi_set_infty(mpq,mode);
	  return;
	}
      }
      else {
	/* point */
	mpz_set_numint(mpq_numref(pk->vector_mpqp[0]),F->p[i][index]);
	mpz_set_numint(mpq_denref(pk->vector_mpqp[0]),F->p[i][polka_cst]);
	mpq_canonicalize(pk->vector_mpqp[0]);
	if (mode>0)
	  /* superior bound */
	  mpqi_max(mpq,pk->vector_mpqp[0]);
	else
	  /* inferior bound */
	  mpqi_min(mpq,pk->vector_mpqp[0]);
      }
    }
  }
}


/* Bounding the value of a linear expression in a matrix of generators.
   vec is supposed to be of size F->nbcolumns.

   mode == 1: sup bound
   mode == -1: inf bound
*/

void matrix_bound_linexpr(pk_internal_t* pk,
			  mpq_t mpq,
			  const numint_t* vec,
			  matrix_t* F,
			  int mode)
{
  size_t i;
  int sgn;
  
  assert(mode!=0);
  mpqi_set_infty(mpq,-mode);

  for (i=0; i<F->nbrows; i++){
    if (!pk->strict || numint_sgn(F->p[i][polka_eps])==0 ){
      vector_product_strict(pk,
			    pk->poly_prod,
			    (const numint_t*)F->p[i],
			    vec, F->nbcolumns);
      sgn = numint_sgn(pk->poly_prod);
      if (numint_sgn(F->p[i][0])==0){
	/* line: result should be zero */
	if (sgn){
	  mpqi_set_infty(mpq,mode);
	  return;
	}
      }
      else if (numint_sgn(F->p[i][polka_cst])==0){
	/* ray */
	if ((sgn>0 && mode>0) || (sgn<0 && mode<0)){
	  mpqi_set_infty(mpq,mode);
	  return;
	}
      }
      else {
	/* point */
	mpz_set_numint(mpq_numref(pk->vector_mpqp[0]),pk->poly_prod);
	mpz_set_numint(mpq_denref(pk->vector_mpqp[0]),F->p[i][polka_cst]);
	mpq_canonicalize(pk->vector_mpqp[0]);
	if (mode>0)
	  /* superior bound */
	  mpqi_max(mpq,pk->vector_mpqp[0]);
	else
	  /* inferior bound */
	  mpqi_min(mpq,pk->vector_mpqp[0]);
      }
    }
  }
  mpz_set_numint(mpq_denref(pk->vector_mpqp[0]),vec[0]);
  mpz_mul(mpq_denref(mpq),mpq_denref(mpq),mpq_denref(pk->vector_mpqp[0]));
  mpq_canonicalize(mpq);
}



/* ====================================================================== */
/* Bounding the value of a linear expression in a polyhedra */
/* ====================================================================== */

ap_interval_t* poly_bound_linexpr(ap_manager_t* man,
			       const poly_t* po,
			       const ap_linexpr0_t* expr)
{
  ap_interval_t* interval;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_BOUND_LINEXPR);

  interval = ap_interval_alloc();
  ap_interval_reinit(interval,AP_SCALAR_MPQ);
  if (pk->funopt->algorithm>0)
    poly_chernikova(man,po,NULL);
  else
    poly_obtain_F(man,po,NULL);

  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    ap_interval_set_top(interval);
    return interval;
  }

  if (!po->F){ /* po is empty */
    ap_interval_set_bottom(interval);
    man->result.flag_exact = man->result.flag_best = tbool_true;
    return interval;
  }
  
  /* we fill the vector with the expression, taking lower bound of the interval
     constant */
  vector_set_linexpr(pk,pk->poly_numintp,expr,po->intdim+po->realdim,-1);
  matrix_bound_linexpr(pk,
		       interval->inf->val.mpq,
		       (const numint_t*)pk->poly_numintp,po->F,
		       -1);

  if (expr->cst.discr == AP_COEFF_INTERVAL){
    /* We change the expression, taking upper bound of the interval
       constant  */
    vector_set_linexpr(pk,pk->poly_numintp,expr,po->intdim+po->realdim,+1);
  }
  matrix_bound_linexpr(pk,
		       interval->sup->val.mpq,
		       (const numint_t*)pk->poly_numintp,po->F,
		       +1);

  man->result.flag_exact = man->result.flag_best = 
    ( (pk->funopt->flag_exact_wanted || pk->funopt->flag_best_wanted) &&
      ap_linexpr0_is_real(expr,po->intdim) ) ? 
    tbool_true : 
    tbool_top;
  return interval;
}

/* ====================================================================== */
/* Bounding the value of a dimension in a polyhedra */
/* ====================================================================== */

ap_interval_t* poly_bound_dimension(ap_manager_t* man,
				       const poly_t* po,
				       ap_dim_t dim)
{
  ap_interval_t* interval;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_BOUND_DIMENSION);

  interval = ap_interval_alloc();
  ap_interval_reinit(interval,AP_SCALAR_MPQ);
  if (pk->funopt->algorithm>0)
    poly_chernikova(man,po,NULL);
  else
    poly_obtain_F(man,po,NULL);

  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    ap_interval_set_top(interval);
    return interval;
  }

  if (!po->F){ /* po is empty */
    ap_interval_set_bottom(interval);
    man->result.flag_exact = man->result.flag_best = tbool_true;
    return interval;
  }

  matrix_bound_dimension(pk,
			 interval->inf->val.mpq,
			 dim,po->F,
			 -1);
  matrix_bound_dimension(pk,
			 interval->sup->val.mpq,
			 dim,po->F,
			 +1);

  man->result.flag_exact = man->result.flag_best = 
    dim<po->intdim ? tbool_top : tbool_true;
  return interval;
}


/* ====================================================================== */
/* Converting to a set of constraints */
/* ====================================================================== */

ap_lincons0_array_t poly_to_lincons_array(ap_manager_t* man,
				      const poly_t* po)
{
  ap_lincons0_array_t array;
  matrix_t* C;
  int i,k;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_TO_LINCONS_ARRAY);

  man->result.flag_exact = man->result.flag_best = tbool_true;

  poly_chernikova3(man,po,NULL);
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_exact = man->result.flag_best = tbool_false;
    array = ap_lincons0_array_make(0);
    return array;
  }
  if (!po->C){ /* po is empty */
    array = ap_lincons0_array_make(1);
    array.p[0] = ap_lincons0_make_unsat();
    return array;
  }
  poly_obtain_sorted_C(pk,po);
  C = po->C;
  array = ap_lincons0_array_make(C->nbrows);
  for (i=0,k=0; i<C->nbrows; i++){
    if (! vector_is_dummy_constraint(pk,
				     (const numint_t*)C->p[i], C->nbcolumns)){
      array.p[k] = lincons_of_vector(pk, C->p[i], C->nbcolumns);
      k++;
    }
  }
  array.size = k;
  return array;
}

/* ====================================================================== */
/* Converting to a box */
/* ====================================================================== */

ap_interval_t** poly_to_box(ap_manager_t* man,
			       const poly_t* po)
{
  ap_interval_t** interval;
  size_t i,dim;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_TO_BOX);

  dim = po->intdim + po->realdim;
  if (pk->funopt->algorithm>=0)
    poly_chernikova(man,po,NULL);
  else
    poly_obtain_F(man,po,NULL);


  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_exact = man->result.flag_best = tbool_false;
    interval = ap_interval_array_alloc(dim);
    for (i=0; i<dim; i++){
      ap_interval_set_top(interval[i]);
    }
    return interval;
  }
  interval = malloc(dim*sizeof(ap_interval_t*));
  for (i=0; i<dim; i++){
    interval[i] = poly_bound_dimension(man,po,i);
  }
  man->result.flag_exact = man->result.flag_best = tbool_true;
  return interval;
}

/* ====================================================================== */
/* Converting to a set of generators */
/* ====================================================================== */

/* The function returns the set of generators for the topological closure of
   the polyhedron. */

ap_generator0_array_t poly_to_generator_array(ap_manager_t* man,
					   const poly_t* po)
{
  ap_generator0_array_t array;
  matrix_t* F;
  int i,k;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_TO_GENERATOR_ARRAY);

  man->result.flag_exact = man->result.flag_best = tbool_true;

  poly_chernikova3(man,po,NULL);
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_exact = man->result.flag_best = tbool_false;
    array = ap_generator0_array_make(0);
    return array;
  }
  if (!po->F){ /* po is empty */
    array = ap_generator0_array_make(0);
    return array;
  }
  F = po->F;
  poly_obtain_sorted_F(pk,po);
  array = ap_generator0_array_make(F->nbrows);
  for (i=0,k=0; i<F->nbrows; i++){
    if (! vector_is_dummy_or_strict_generator(pk,
					      (const numint_t*)F->p[i], F->nbcolumns)){
      array.p[k] = generator_of_vector(pk, F->p[i], F->nbcolumns);
      k++;
    }
  }
  array.size = k;
  return array;
}
