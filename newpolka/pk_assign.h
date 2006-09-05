/* ********************************************************************** */
/* pk_assign.h: Assignements and Substitutions */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _PK_ASSIGN_H_
#define _PK_ASSIGN_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"

#ifdef __cplusplus
extern "C" {
#endif

poly_t* poly_asssub_linear_linexpr(bool assign,
				   ap_manager_t* man,
				   bool destructive,
				   poly_t* pa,
				   ap_dim_t dim, const ap_linexpr0_t* linexpr);

poly_t* poly_asssub_linexpr(bool assign,
			    bool lazy,
			    ap_manager_t* man,
			    bool destructive,
			    poly_t* pa,
			    ap_dim_t dim, const ap_linexpr0_t* linexpr);

poly_t* poly_asssub_linear_linexpr_array(bool assign,
					 ap_manager_t* man,
					 bool destructive,
					 poly_t* pa,
					 const ap_dim_t* tdim, const ap_linexpr0_t** texpr, 
					 size_t size);

poly_t*
poly_asssub_quasilinear_linexpr_array(bool assign,
				      ap_manager_t* man,
				      bool destructive,
				      poly_t* pa,
				      const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
				      size_t intdimsup,
				      size_t realdimsup);

poly_t* poly_asssub_linexpr_array(bool assign, 
				  bool lazy,
				  ap_manager_t* man,
				  bool destructive,
				  poly_t* pa,
				  const ap_dim_t* tdim,
				  const ap_linexpr0_t** texpr,
				  size_t size);

poly_t* poly_assign_linexpr(ap_manager_t* man,
			    bool destructive, poly_t* pa, 
			    ap_dim_t dim, const ap_linexpr0_t* linexpr,
			    const poly_t* pb);

poly_t* poly_assign_linexpr_array(ap_manager_t* man,
				  bool destructive, poly_t* pa,
				  const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
				  size_t size,
				  const poly_t* pb);

poly_t* poly_substitute_linexpr(ap_manager_t* man,
				bool destructive, poly_t* pa, 
				ap_dim_t dim, const ap_linexpr0_t* linexpr,
				const poly_t* pb);

poly_t* poly_substitute_linexpr_array(ap_manager_t* man,
				      bool destructive, poly_t* pa,
				      const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
				      size_t size,
				      const poly_t* pb);


#ifdef __cplusplus
}
#endif

#endif
