/* ********************************************************************** */
/* pk_expandfold.h: expanding and folding dimensions */
/* ********************************************************************** */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_bit.h"
#include "pk_satmat.h"
#include "pk_matrix.h"

poly_t* poly_expand(ap_manager_t* man,
		    bool destructive, poly_t* pa, 
		    ap_dim_t dim, size_t dimsup);
poly_t* poly_fold(ap_manager_t* man, 
		  bool destructive, poly_t* pa, 
		  const ap_dim_t* tdim, size_t size);

