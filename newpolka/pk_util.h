/* ********************************************************************** */
/* pk_util.h */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _PK_UTIL_H_
#define _PK_UTIL_H_

#include "pk_config.h"
#include "pk.h"

#ifdef __cplusplus
extern "C" {
#endif

bool manager_check_dim_dim(manager_t* man,
			   const poly_t* a, dim_t dim,
			   funid_t funid);

bool manager_check_dim_bin(manager_t* man,
			   const poly_t* pa, const poly_t* pb,
			   funid_t funid);

bool manager_check_dim_array(manager_t* man,
			     const poly_t** po, size_t size, funid_t funid);

bool manager_check_dim_linexpr(manager_t* man,
			       const poly_t* a, const linexpr0_t* expr,
			       funid_t funid);

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED)
static inline char* strdup(const char* s){
  char* s2;

  s2 = malloc(strlen(s)+1);
  strcpy(s2,s);
  return s2;
}
#endif

#ifdef __cplusplus
}
#endif

#endif
