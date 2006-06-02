/* ********************************************************************** */
/* pk_extract.h: property extraction */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _PK_EXTRACT_H_
#define _PK_EXTRACT_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"

/* Bounding the value of a dimension in a matrix of generators.
   mode == 1: sup bound
   mode == -1: inf bound
*/
void matrix_bound_dimension(pk_internal_t* pk,
			    mpq_t mpq,
			    ap_dim_t dim,
			    matrix_t* F,
			    int mode);

/* Bounding the value of a linear expression in a matrix of generators.
   vec is supposed to be of size F->nbcolumns.
   
   mode == 1: sup bound
   mode == -1: inf bound
*/
void matrix_bound_linexpr(pk_internal_t* pk,
			  mpq_t mpq,
			  const numint_t* vec,
			  matrix_t* F,
			  int mode);

/* Bounding the value of a linear expression in a polyhedra 
   algorithm > 0: compute canonical form
   algorithm <= 0: compute dual representation only if necessary
*/

ap_interval_t* poly_bound_linexpr(ap_manager_t* man,
			      const poly_t* po,
			      const ap_linexpr0_t* expr);

/* Bounding the value of a dimension in a polyhedra 
   algorithm > 0: compute canonical form
   algorithm <= 0: compute dual representation only if necessary
*/
ap_interval_t* poly_bound_dimension(ap_manager_t* man,
				const poly_t* po,
				ap_dim_t dim);

/* Converting to a set of constraints
   Always consider canonical form
*/
ap_lincons0_array_t poly_to_lincons_array(ap_manager_t* man,
				       const poly_t* po);
/* Converting to a box 
   algorithm >= 0: compute canonical form
   algorithm < 0: compute dual representation only if necessary
*/
ap_interval_t** poly_to_box(ap_manager_t* man,
			const poly_t* po);

/* Converting to a set of generators
   Always consider canonical form
*/
ap_generator0_array_t poly_to_generator_array(ap_manager_t* man,
					  const poly_t* po);

#endif
