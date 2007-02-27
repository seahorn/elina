/* ********************************************************************** */
/* pk_widening.h: widening  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _PK_WIDENING_H_
#define _PK_WIDENING_H_


#include "pk_config.h"
#include "pk_vector.h"
#include "pk_bit.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"

#ifdef __cplusplus
extern "C" {
#endif

poly_t* poly_widening(ap_manager_t* man, poly_t* pa, poly_t* pb);
poly_t* poly_widening_threshold(ap_manager_t* man, 
				poly_t* pa, poly_t* pb, 
				ap_lincons0_array_t* array);

#ifdef __cplusplus
}
#endif

#endif
