/* ************************************************************************* */
/* internal.c: internal manager */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_internal.h"
#include "pk_vector.h"
#include "pk_matrix.h"
#include "pk_satmat.h"
#include "pk_int.h"

/* ********************************************************************** */
/* I. Constructor and destructor for internal */
/* ********************************************************************** */

/* Initialize pk with size maxdims */

void pk_internal_init(pk_internal_t* pk, size_t maxdims)
{
  int i;

  pk->maxdims = maxdims;
  pk->maxcols = maxdims+3;
  
  pk->vector_numintp = vector_alloc(pk->maxcols);
  pk->vector_mpqp = malloc( (maxdims+3)*sizeof(mpq_t));
  for (i=0; i<maxdims+3; i++)
    mpq_init(pk->vector_mpqp[i]);

  pk->vector_tmp = vector_alloc(5);

  pk->matrix_dimp = malloc(pk->maxdims*sizeof(ap_dim_t));
  numint_init(pk->matrix_acc);
  numint_init(pk->matrix_prod);

  /*  pk->cherni_bitstringp = bitstring_alloc(bitindex_size(pk->maxrows));*/
  pk->cherni_intp = (int*)malloc(pk->maxcols * sizeof(int));
  numint_init(pk->cherni_prod);

  pk->poly_numintp = vector_alloc(pk->maxcols);
  pk->poly_numintp2 = vector_alloc(pk->maxcols);
  pk->poly_dimp = malloc(pk->maxdims*sizeof(ap_dim_t));
  pk->poly_dimp2 = malloc(pk->maxdims*sizeof(ap_dim_t));
  pk->poly_fold_dimp = malloc(pk->maxdims*sizeof(ap_dim_t));
  /* pk->poly_bitstringp = bitstring_alloc(bitindex_size(pk->maxrows)); */
  pk->poly_matspecial = matrix_alloc(1,pk->maxcols,true);
  numint_init(pk->poly_prod);
}

/* Allocates pk and initializes it with a default size */
pk_internal_t* pk_internal_alloc(bool strict)
{
  pk_internal_t* pk = (pk_internal_t*)malloc(sizeof(pk_internal_t));

  pk->strict = strict;
  pk->dec = strict ? 3 : 2;
  pk->max_coeff_size = 0;
  pk->approximate_max_coeff_size = 2;
  pk_internal_init(pk,10);

  return pk;
}

/* Clear pk */
void pk_internal_clear(pk_internal_t* pk)
{
  int i;

  if (pk->vector_numintp) vector_free(pk->vector_numintp,pk->maxcols);
  pk->vector_numintp = 0;

  if (pk->vector_tmp) vector_free(pk->vector_tmp,5);
  pk->vector_tmp = 0;

  if (pk->vector_mpqp){ 
    for (i=0; i<pk->maxdims+3; i++)
      mpq_clear(pk->vector_mpqp[i]);
    free(pk->vector_mpqp);
  }
  pk->vector_mpqp = 0;

  if (pk->matrix_dimp) free(pk->matrix_dimp);
  pk->matrix_dimp = 0;

  numint_clear(pk->matrix_acc);
  numint_clear(pk->matrix_prod);

  /*  if (pk->cherni_bitstringp) free(pk->cherni_bitstringp);
  pk->cherni_bitstringp = 0;
  */
  if (pk->cherni_intp) free(pk->cherni_intp);
  pk->cherni_intp = 0;

  numint_clear(pk->cherni_prod);

  if (pk->poly_numintp) vector_free(pk->poly_numintp, pk->maxcols);
  pk->poly_numintp = 0; 

  if (pk->poly_numintp2) vector_free(pk->poly_numintp2, pk->maxcols);
  pk->poly_numintp2 = 0;

  if (pk->poly_dimp) free(pk->poly_dimp);
  pk->poly_dimp = 0;
  if (pk->poly_dimp2) free(pk->poly_dimp2);
  pk->poly_dimp2 = 0;
  if (pk->poly_fold_dimp) free(pk->poly_fold_dimp);
  pk->poly_fold_dimp = 0;

  /* if (pk->poly_bitstringp) bitstring_free(pk->poly_bitstringp);
  pk->poly_bitstringp = 0; 
  */
  if (pk->poly_matspecial) matrix_free(pk->poly_matspecial);
  pk->poly_matspecial = 0;

  numint_clear(pk->poly_prod);

  pk->maxdims = 0;
  pk->maxrows = 0;
  pk->maxcols = 0;
}

/* Clear and free pk */
void pk_internal_free(pk_internal_t* pk)
{
  pk_internal_clear(pk);
  free(pk);
}

/* Reallocate pk */
void pk_internal_realloc(pk_internal_t* pk, size_t maxdims)
{
  if (pk->maxdims != maxdims){
    pk_internal_clear(pk);
    pk_internal_init(pk,maxdims);
  }
}

/* Reallocate pk only if a bigger dimension is required */
void pk_internal_realloc_lazy(pk_internal_t* pk, size_t maxdims)
{
  if (maxdims > pk->maxdims){
    pk_internal_clear(pk);
    pk_internal_init(pk,maxdims);
  }
}

/* ********************************************************************** */
/* II. Options */
/* ********************************************************************** */

pk_internal_t* pk_manager_get_internal(ap_manager_t* man){
  return man->internal;
}

void pk_set_max_coeff_size(pk_internal_t* pk, size_t size){
  pk->max_coeff_size = size;
}
void pk_set_approximate_max_coeff_size(pk_internal_t* pk, size_t size){  
  pk->approximate_max_coeff_size = size;
}
size_t pk_get_max_coeff_size(pk_internal_t* pk){
  return pk->max_coeff_size;
}
size_t pk_get_approximate_max_coeff_size(pk_internal_t* pk){
  return pk->approximate_max_coeff_size;
}

/* ********************************************************************** */
/* III. Initialization from manager */
/* ********************************************************************** */

ap_manager_t* pk_manager_alloc(bool strict)
{
  size_t i;
  pk_internal_t* pk;
  ap_manager_t* man;
  void** funptr;

  pk = pk_internal_alloc(strict);
  pk_set_approximate_max_coeff_size(pk, 1);
  man = ap_manager_alloc("polka",
#if defined(NUMINT_LONGINT)
		      "3.0 with NUMINT_LONGINT",
#elif defined(NUMINT_LONGLONGINT)
		      "3.0 with NUMINT_LONGLONGINT",
#elif defined(NUMINT_MPZ)
		      "3.0 with NUMINT_MPZ",
#else
#error "here"
#endif
		      pk, (void (*)(void*))pk_internal_free);
  funptr = man->funptr;
  
  funptr[AP_FUNID_COPY] = &poly_copy;
  funptr[AP_FUNID_FREE] = &poly_free;
  funptr[AP_FUNID_ASIZE] = &poly_size;
  funptr[AP_FUNID_MINIMIZE] = &poly_minimize;
  funptr[AP_FUNID_CANONICALIZE] = &poly_canonicalize;
  funptr[AP_FUNID_APPROXIMATE] = &poly_approximate;
  funptr[AP_FUNID_FPRINT] = &poly_fprint;
  funptr[AP_FUNID_FPRINTDIFF] = &poly_fprintdiff;
  funptr[AP_FUNID_FDUMP] = &poly_fdump;
  funptr[AP_FUNID_SERIALIZE_RAW] = &poly_serialize_raw;
  funptr[AP_FUNID_DESERIALIZE_RAW] = &poly_deserialize_raw;
  funptr[AP_FUNID_BOTTOM] = &poly_bottom;
  funptr[AP_FUNID_TOP] = &poly_top;
  funptr[AP_FUNID_OF_BOX] = &poly_of_box;
  funptr[AP_FUNID_OF_LINCONS_ARRAY] = &poly_of_lincons_array;
  funptr[AP_FUNID_DIMENSION] = &poly_dimension;
  funptr[AP_FUNID_IS_BOTTOM] = &poly_is_bottom;
  funptr[AP_FUNID_IS_TOP] = &poly_is_top;
  funptr[AP_FUNID_IS_LEQ] = &poly_is_leq;
  funptr[AP_FUNID_IS_EQ] = &poly_is_eq;
  funptr[AP_FUNID_IS_DIMENSION_UNCONSTRAINED] = &poly_is_dimension_unconstrained;
  funptr[AP_FUNID_SAT_INTERVAL] = &poly_sat_interval;
  funptr[AP_FUNID_SAT_LINCONS] = &poly_sat_lincons;
  funptr[AP_FUNID_BOUND_DIMENSION] = &poly_bound_dimension;
  funptr[AP_FUNID_BOUND_LINEXPR] = &poly_bound_linexpr;
  funptr[AP_FUNID_TO_BOX] = &poly_to_box;
  funptr[AP_FUNID_TO_LINCONS_ARRAY] = &poly_to_lincons_array;
  funptr[AP_FUNID_TO_GENERATOR_ARRAY] = &poly_to_generator_array;
  funptr[AP_FUNID_MEET] = &poly_meet;
  funptr[AP_FUNID_MEET_ARRAY] = &poly_meet_array;
  funptr[AP_FUNID_MEET_LINCONS_ARRAY] = &poly_meet_lincons_array;
  funptr[AP_FUNID_JOIN] = &poly_join;
  funptr[AP_FUNID_JOIN_ARRAY] = &poly_join_array;
  funptr[AP_FUNID_ADD_RAY_ARRAY] = &poly_add_ray_array;
  funptr[AP_FUNID_ASSIGN_LINEXPR] = &poly_assign_linexpr;
  funptr[AP_FUNID_SUBSTITUTE_LINEXPR] = &poly_substitute_linexpr;
  funptr[AP_FUNID_ASSIGN_LINEXPR_ARRAY] = &poly_assign_linexpr_array;
  funptr[AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY] = &poly_substitute_linexpr_array;
  funptr[AP_FUNID_ADD_DIMENSIONS] = &poly_add_dimensions;
  funptr[AP_FUNID_REMOVE_DIMENSIONS] = &poly_remove_dimensions;
  funptr[AP_FUNID_PERMUTE_DIMENSIONS] = &poly_permute_dimensions;
  funptr[AP_FUNID_FORGET_ARRAY] = &poly_forget_array;
  funptr[AP_FUNID_EXPAND] = &poly_expand;
  funptr[AP_FUNID_FOLD] = &poly_fold;
  funptr[AP_FUNID_WIDENING] = &poly_widening;
  funptr[AP_FUNID_CLOSURE] = &poly_closure;

  for (i=0; i<AP_EXC_SIZE; i++){
    ap_manager_set_abort_if_exception(man, i, false);
  }
  return man;
}

/* ********************************************************************** */
/* IV. Conversions */
/* ********************************************************************** */

poly_t* pk_to_poly(ap_abstract0_t* abstract)
{
  ap_manager_t* man = abstract->man;
  if (strcmp(man->library,"polka")!=0){
    ap_manager_raise_exception(man,AP_EXC_INVALID_ARGUMENT,
			       AP_FUNID_UNKNOWN,
			       "pk_to_poly: attempt to extract a NewPolka polyhedra from an abstract value which is not a wrapper around a NewPOlka polyhedra");
    return NULL;
  }
  return (poly_t*)abstract->value;
}

ap_abstract0_t* pk_of_poly(ap_manager_t* man, poly_t* poly)
{
  if (strcmp(man->library,"polka")!=0){
    ap_manager_raise_exception(man,AP_EXC_INVALID_ARGUMENT,
			       AP_FUNID_UNKNOWN,
			       "pk_to_poly: attempt to extract a NewPolka polyhedra from an abstract value which is not a wrapper around a NewPOlka polyhedra");
    return ap_abstract0_top(man,poly->intdim,poly->realdim);
  }
  ap_abstract0_t* res = malloc(sizeof(ap_abstract0_t));
  res->value = poly;
  res->man = ap_manager_copy(man);
  return res;
}
