/* ********************************************************************** */
/* util.h */
/* ********************************************************************** */

#ifndef _ITV_UTIL_H_
#define _ITV_UTIL_H_

#include "itv_config.h"

#include "manager.h"
#include "itv.h"

#ifdef __cplusplus
extern "C" {
#endif

bool manager_check_dim_dim(manager_t* man,
			     itv_t* a, dim_t dim,
			     char* prefix);

bool manager_check_dim_bin(manager_t* man,
			     itv_t* pa, itv_t* pb,
			     char* prefix);

bool manager_check_dim_array(manager_t* man,
			       itv_t** po, size_t size, char* prefix);

bool manager_check_dim_linexpr(manager_t* man,
				 itv_t* a, linexpr_t* expr,
				 char* prefix);

#ifdef __cplusplus
}
#endif

#endif
