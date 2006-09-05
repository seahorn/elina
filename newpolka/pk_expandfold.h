/* ********************************************************************** */
/* pk_expandfold.h: expanding and folding dimensions */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_bit.h"
#include "pk_satmat.h"
#include "pk_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

poly_t* poly_expand(ap_manager_t* man,
		    bool destructive, poly_t* pa, 
		    ap_dim_t dim, size_t dimsup);
poly_t* poly_fold(ap_manager_t* man, 
		  bool destructive, poly_t* pa, 
		  const ap_dim_t* tdim, size_t size);

#ifdef __cplusplus
}
#endif

