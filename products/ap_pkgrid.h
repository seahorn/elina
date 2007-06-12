/* ************************************************************************* */
/* ap_poly_grid.h: reduced product of NewPolka polyhedra and PPL grids */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _AP_PKGRID_H_
#define _AP_PKGRID_H_

#include "ap_reducedproduct.h"

#ifdef __cplusplus
extern "C" {
#endif

ap_manager_t* ap_pkgrid_manager_alloc(bool strict);

void ap_pkgrid_reduce(ap_manager_t* manager,
		      ap_reducedproduct_t* a);

void ap_pkgrid_approximate(ap_manager_t* manager,
			   ap_reducedproduct_t* a,
			   int n);


#ifdef __cplusplus
}
#endif

#endif
