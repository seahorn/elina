/* ********************************************************************** */
/* pk_project.h: projections  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _PK_PROJECT_H_
#define _PK_PROJECT_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_bit.h"
#include "pk_satmat.h"
#include "pk_matrix.h"

#ifdef __cplusplus
extern "C" {
#endif

void _poly_projectforget_array(bool project,
			       bool lazy,
			       ap_manager_t* man,	
			       poly_t* po, poly_t* pa, 
			       ap_dim_t* tdim, size_t size);

poly_t* poly_forget_array(ap_manager_t* man, 
			  bool destructive, poly_t* pa, 
			  ap_dim_t* tdim, size_t size,
			  bool project);

#ifdef __cplusplus
}
#endif

#endif
