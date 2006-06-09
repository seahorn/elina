/* ********************************************************************** */
/* itv_internal.c */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "itv_internal.h"
#include "itv.h"

void itv_internal_init(itv_internal_t* intern)
{
  num_init(intern->itv_interval_canonicalize_num);
  bound_init(intern->itv_interval_muldiv_bound);
  num_init(intern->bound_mul_scalar_num);
  intern->itv_interval_set_interval_scalar = ap_scalar_alloc();
  intern->itv_interval_is_leq_interval_interval = ap_interval_alloc();
  bound_init(intern->itv_interval_mul_scalar_bound);
  intern->itv_interval_mul_interval2_scalar = ap_scalar_alloc();
  intern->itv_interval_mul_interval1_interval = ap_interval_alloc();
  itv_interval_init(intern->itv_interval_mul_interval1_itvinterval);
  itv_interval_init(intern->itv_interval_mul_interval_itvinterval);
  itv_interval_init(intern->itv_interval_mul_interval_itvinterval2);
  itv_interval_init(intern->bound_linexpr_internal_itvinterval);
  itv_interval_init(intern->bound_itv_linexpr_itvinterval);
  itv_interval_init(intern->sat_lincons_itvinterval);
  itv_interval_init(intern->bound_linexpr_itvinterval);
  num_init(intern->meet_itv_lincons_num);
  itv_interval_init(intern->meet_itv_lincons_itvinterval);
  itv_lincons_init(&intern->meet_lincons_array_itvlincons);
  itv_interval_init(intern->substitute_linexpr_itvinterval);
  itv_interval_init(intern->substitute_linexpr_itvinterval2);
  itv_lincons_init(&intern->substitute_linexpr_itvlincons);
}
void itv_internal_clear(itv_internal_t* intern)
{
  num_clear(intern->itv_interval_canonicalize_num);
  bound_clear(intern->itv_interval_muldiv_bound);
  num_clear(intern->bound_mul_scalar_num);
  ap_scalar_free(intern->itv_interval_set_interval_scalar);
  ap_interval_free(intern->itv_interval_is_leq_interval_interval);
  bound_clear(intern->itv_interval_mul_scalar_bound);
  ap_scalar_free(intern->itv_interval_mul_interval2_scalar);
  ap_interval_free(intern->itv_interval_mul_interval1_interval);
  itv_interval_clear(intern->itv_interval_mul_interval1_itvinterval);
  itv_interval_clear(intern->itv_interval_mul_interval_itvinterval);
  itv_interval_clear(intern->itv_interval_mul_interval_itvinterval2);
  itv_interval_clear(intern->bound_linexpr_internal_itvinterval);
  itv_interval_clear(intern->bound_itv_linexpr_itvinterval);
  itv_interval_clear(intern->sat_lincons_itvinterval);
  itv_interval_clear(intern->bound_linexpr_itvinterval);
  num_clear(intern->meet_itv_lincons_num);
  itv_interval_clear(intern->meet_itv_lincons_itvinterval);
  itv_lincons_clear(&intern->meet_lincons_array_itvlincons);
  itv_interval_clear(intern->substitute_linexpr_itvinterval);
  itv_interval_clear(intern->substitute_linexpr_itvinterval2);
  itv_lincons_clear(&intern->substitute_linexpr_itvlincons);
}

itv_internal_t* itv_internal_alloc()
{
  itv_internal_t* intern = malloc(sizeof(itv_internal_t));
  itv_internal_init(intern);
  return intern;
}
void itv_internal_free(itv_internal_t* intern)
{
  itv_internal_clear(intern);
  free(intern);
}

ap_manager_t* itv_manager_alloc()
{
  size_t i;
  itv_internal_t* itv;
  ap_manager_t* man;
  void** funptr;

  itv = itv_internal_alloc();
  man = ap_manager_alloc("itv",
#if defined(NUM_MPQ)
			 "1.0 with NUM_MPQ",
#elif defined(NUM_LONGLONGRAT)
			 "1.0 with NUM_LONGLONGRAT",
#elif defined(NUM_DOUBLE)
			 "1.0 with NUM_DOUBLE",
#else
#error "here"
#endif
			 itv, (void (*)(void*))itv_internal_free);
  funptr = man->funptr;
  
  funptr[AP_FUNID_COPY] = &itv_copy;
  funptr[AP_FUNID_FREE] = &itv_free;
  funptr[AP_FUNID_ASIZE] = &itv_size;
  funptr[AP_FUNID_MINIMIZE] = &itv_minimize;
  funptr[AP_FUNID_CANONICALIZE] = &itv_canonicalize;
  funptr[AP_FUNID_APPROXIMATE] = &itv_approximate;
  funptr[AP_FUNID_IS_MINIMAL] = &itv_is_minimal;
  funptr[AP_FUNID_IS_CANONICAL] = &itv_is_canonical;
  funptr[AP_FUNID_FPRINT] = &itv_fprint;
  funptr[AP_FUNID_FPRINTDIFF] = &itv_fprintdiff;
  funptr[AP_FUNID_FDUMP] = &itv_fdump;
  funptr[AP_FUNID_SERIALIZE_RAW] = &itv_serialize_raw;
  funptr[AP_FUNID_DESERIALIZE_RAW] = &itv_deserialize_raw;
  funptr[AP_FUNID_BOTTOM] = &itv_bottom;
  funptr[AP_FUNID_TOP] = &itv_top;
  funptr[AP_FUNID_OF_BOX] = &itv_of_box;
  funptr[AP_FUNID_OF_LINCONS_ARRAY] = &itv_of_lincons_array;
  funptr[AP_FUNID_DIMENSION] = &itv_dimension;
  funptr[AP_FUNID_IS_BOTTOM] = &itv_is_bottom;
  funptr[AP_FUNID_IS_TOP] = &itv_is_top;
  funptr[AP_FUNID_IS_LEQ] = &itv_is_leq;
  funptr[AP_FUNID_IS_EQ] = &itv_is_eq;
  funptr[AP_FUNID_IS_DIMENSION_UNCONSTRAINED] = &itv_is_dimension_unconstrained;
  funptr[AP_FUNID_SAT_INTERVAL] = &itv_sat_interval;
  funptr[AP_FUNID_SAT_LINCONS] = &itv_sat_lincons;
  funptr[AP_FUNID_BOUND_DIMENSION] = &itv_bound_dimension;
  funptr[AP_FUNID_BOUND_LINEXPR] = &itv_bound_linexpr;
  funptr[AP_FUNID_TO_BOX] = &itv_to_box;
  funptr[AP_FUNID_TO_LINCONS_ARRAY] = &itv_to_lincons_array;
  funptr[AP_FUNID_TO_GENERATOR_ARRAY] = &itv_to_generator_array;
  funptr[AP_FUNID_MEET] = &itv_meet;
  funptr[AP_FUNID_MEET_ARRAY] = &itv_meet_array;
  funptr[AP_FUNID_MEET_LINCONS_ARRAY] = &itv_meet_lincons_array;
  funptr[AP_FUNID_JOIN] = &itv_join;
  funptr[AP_FUNID_JOIN_ARRAY] = &itv_join_array;
  funptr[AP_FUNID_ADD_RAY_ARRAY] = &itv_add_ray_array;
  funptr[AP_FUNID_ASSIGN_LINEXPR] = &itv_assign_linexpr;
  funptr[AP_FUNID_SUBSTITUTE_LINEXPR] = &itv_substitute_linexpr;
  funptr[AP_FUNID_ASSIGN_LINEXPR_ARRAY] = &itv_assign_linexpr_array;
  funptr[AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY] = &itv_substitute_linexpr_array;
  funptr[AP_FUNID_ADD_DIMENSIONS] = &itv_add_dimensions;
  funptr[AP_FUNID_REMOVE_DIMENSIONS] = &itv_remove_dimensions;
  funptr[AP_FUNID_PERMUTE_DIMENSIONS] = &itv_permute_dimensions;
  funptr[AP_FUNID_FORGET_ARRAY] = &itv_forget_array;
  funptr[AP_FUNID_EXPAND] = &itv_expand;
  funptr[AP_FUNID_FOLD] = &itv_fold;
  funptr[AP_FUNID_WIDENING] = &itv_widening;
  funptr[AP_FUNID_CLOSURE] = &itv_closure;

  for (i=0; i<AP_EXC_SIZE; i++){
    ap_manager_set_abort_if_exception(man, i, false);
  }
  return man;
}
 
