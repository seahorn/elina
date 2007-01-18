/* ********************************************************************** */
/* box_internal.h: abstract lattice of intervals */
/* ********************************************************************** */

#include "box_interval.h"

#ifndef _BOX_INTERNAL_H_
#define _BOX_INTERNAL_H_

#include <string.h>
#include <stdio.h>

#include "box_config.h"

#ifdef __cplusplus
extern "C" {
#endif

ap_manager_t* box_manager_alloc();

typedef struct box_internal_t {
  num_t itv_canonicalize_num;
  bound_t itv_muldiv_bound;
  num_t bound_mul_scalar_num;
  ap_scalar_t* itv_set_interval_scalar;
  ap_interval_t* itv_is_leq_interval_interval;
  bound_t itv_mul_scalar_bound;
  ap_scalar_t* itv_mul_interval2_scalar;
  ap_interval_t* itv_mul_interval1_interval;
  itv_t itv_mul_interval1_itvinterval;
  itv_t itv_mul_interval_itvinterval;
  itv_t itv_mul_interval_itvinterval2;
  itv_t bound_linexpr_internal_itvinterval;
  itv_t bound_box_linexpr_itvinterval;
  itv_t sat_lincons_itvinterval;
  itv_t bound_linexpr_itvinterval;
  num_t meet_box_lincons_num;
  itv_t meet_box_lincons_itvinterval;
  box_lincons_t meet_lincons_array_itvlincons;
  itv_t substitute_linexpr_itvinterval;
  itv_t substitute_linexpr_itvinterval2;
  box_lincons_t substitute_linexpr_itvlincons;
} box_internal_t;

void box_internal_init(box_internal_t* intern);
void box_internal_clear(box_internal_t* intern);

box_internal_t* box_internal_alloc();
void box_internal_free(box_internal_t* intern);

/* Initializes some fields of pk from manager */
static inline box_internal_t* box_init_from_manager(ap_manager_t* man, ap_funid_t funid)
{
  box_internal_t* itv = (box_internal_t*)man->internal;
  return itv;
}

#ifdef __cplusplus
}
#endif

#endif
