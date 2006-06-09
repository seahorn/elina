/* ********************************************************************** */
/* itv_int.h: abstract lattice of intervals */
/* ********************************************************************** */

#ifndef _ITV_INT_H_
#define _ITV_INT_H_

#include "itv.h"
#include "itv_config.h"
#include "itv_interval.h"

/*
bool expr_is_invertible(ap_dim_t dim,linexpr_t* a);
void linexpr_invert(linexpr_t* a, ap_dim_t dim, linexpr_t* b);
void linexpr_add_cst_pkint(linexpr_t* a, linexpr_t* b, bound_t c);
void linexpr_sub_cst_pkint(linexpr_t* a, linexpr_t* b, bound_t c);
void linexpr_neg(linexpr_t* a, linexpr_t* b);
*/

struct itv_t {
  itv_interval_t* p;
  size_t intdim;
  size_t realdim;
};

#endif
