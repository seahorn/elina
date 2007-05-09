
/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _AP_GENERIC_H_
#define _AP_GENERIC_H_

#include "ap_manager.h"
#include "ap_expr0.h"
#include "ap_abstract0.h"

#ifdef __cplusplus
extern "C" {
#endif

/* These functions are dedicated to implementors of domains. They offer generic
   default implementations for some of the operations required by the APRON
   API, when there is no more specific and efficient implementation for the
   domain being implemented.

   To use them, the function allocating manager, which is specific to the domain,
   should put the corresponding pointers in the virtual table to them.

   They manipulated "unboxed" abstract values, which are native to the
   underlying library: they are not yet boxed with the manager in the type
   ap_abstract0_t.
*/

/* ********************************************************************** */
/* I. General management */
/* ********************************************************************** */

/* ********************************************************************** */
/* II. Constructors */
/* ********************************************************************** */

void*
ap_generic_of_lincons_array(ap_manager_t* man,
			    size_t intdim, size_t realdim,
			    ap_lincons0_array_t* array);
  /* This function implements a generic of_lincons_array operation using top and
     meet_lincons_array operations.
  */

/* ********************************************************************** */
/* III. Operations */
/* ********************************************************************** */

void*
ap_generic_meet_array(ap_manager_t* man,
		      void** tab, size_t size);
  /* This function implements a generic meet_array operation using copy and meet
     operations. */

void*
ap_generic_join_array(ap_manager_t* man,
		      void** tab, size_t size);
  /* This function implements a generic join_array operation using copy and meet
     operations. */

void*
ap_generic_assign_linexpr_array(ap_manager_t* man,
				bool destructive,
				void* abs,
				ap_dim_t* tdim,
				ap_linexpr0_t** texpr,
				size_t size,
				void* dest);
  /*
     This function implements generic parallel assignement operation by
     1. introducing primed dimensions
     2. transforming linear expressions into equality constraints relating the
	assigned primed dimension and the linear expression
     3. intersecting the obtained abstract value with the constraints
     4. exchanging primed and unprimed dimensions
     5. removing the introduced (primed) dimensions

     It relies on: is_bottom, copy, dimension, add_dimensions,
     permute_dimensions, remove_dimensions, meet_lincons_array abstract
     operations.
  */
void*
ap_generic_substitute_linexpr_array(ap_manager_t* man,
				    bool destructive,
				    void* abs,
				    ap_dim_t* tdim,
				    ap_linexpr0_t** texpr,
				    size_t size,
				    void* dest);
  /*
     This function implements generic parallel substitution operation by
     1. introducing primed dimensions
     2. transforming linear expressions into equality constraints relating the
        assigned primed dimension and the linear expression
     3. exchanging primed and unprimed dimensions
     4. intersecting the obtained abstract value with the constraints
     5. removing the introduced (primed) dimensions

     It relies on: is_bottom, copy, dimension, add_dimensions,
     permute_dimensions, remove_dimensions, meet_lincons_array abstract
     operations.
  */

#ifdef __cplusplus
}
#endif

#endif
