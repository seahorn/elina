/* ********************************************************************** */
/* itv_internal.h: abstract lattice of intervals */
/* ********************************************************************** */

#include "itv_interval.h"

#ifndef _ITV_INTERNAL_H_
#define _ITV_INTERNAL_H_

#include <string.h>
#include <stdio.h>

#include "itv_config.h"

#ifdef __cplusplus
extern "C" {
#endif

ap_manager_t* itv_manager_alloc();

typedef struct itv_internal_t {
  num_t itv_interval_canonicalize_num;
  bound_t itv_interval_muldiv_bound;
  num_t bound_mul_scalar_num;
  ap_scalar_t* itv_interval_set_interval_scalar;
  ap_interval_t* itv_interval_is_leq_interval_interval;
  bound_t itv_interval_mul_scalar_bound;
  ap_scalar_t* itv_interval_mul_interval2_scalar;
  ap_interval_t* itv_interval_mul_interval1_interval;
  itv_interval_t itv_interval_mul_interval1_itvinterval;
  itv_interval_t itv_interval_mul_interval_itvinterval;
  itv_interval_t itv_interval_mul_interval_itvinterval2;
  itv_interval_t bound_linexpr_internal_itvinterval;
  itv_interval_t bound_itv_linexpr_itvinterval;
  itv_interval_t sat_lincons_itvinterval;
  itv_interval_t bound_linexpr_itvinterval;
  num_t meet_itv_lincons_num;
  itv_interval_t meet_itv_lincons_itvinterval;
  itv_lincons_t meet_lincons_array_itvlincons;
  itv_interval_t substitute_linexpr_itvinterval;
  itv_interval_t substitute_linexpr_itvinterval2;
  itv_lincons_t substitute_linexpr_itvlincons;
} itv_internal_t;

void itv_internal_init(itv_internal_t* intern);
void itv_internal_clear(itv_internal_t* intern);

itv_internal_t* itv_internal_alloc();
void itv_internal_free(itv_internal_t* intern);

/* Initializes some fields of pk from manager */
static inline itv_internal_t* itv_init_from_manager(ap_manager_t* man, ap_funid_t funid)
{
  itv_internal_t* itv = (itv_internal_t*)man->internal;
  return itv;
}

#ifdef __cplusplus
}
#endif

#endif
