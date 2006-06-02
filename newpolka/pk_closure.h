/* ********************************************************************** */
/* pk_closure.h:  topological closure */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _PK_CLOSURE_H_
#define _PK_CLOSURE_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_bit.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"

poly_t* poly_closure(ap_manager_t* man, bool destructive, poly_t* pa);

#endif
