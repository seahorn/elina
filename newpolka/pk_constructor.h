/* ********************************************************************** */
/* pk_constructor.h: constructors and accessors */
/* ********************************************************************** */

#ifndef _PK_CONSTRUCTOR_H_
#define _PK_CONSTRUCTOR_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"

/* Fill the first (pk->dec-1) rows of the matrix with the constraints of the
   universe polyhedron */
void _matrix_fill_constraint_top(pk_internal_t* pk, matrix_t* C, size_t start);

int _matrix_fill_constraint_box(pk_internal_t* pk, 
				matrix_t* C, size_t start,
				const ap_interval_t** box, 
				size_t intdim, size_t realdim,
				bool integer);

/* Fills the vector with the constraint:
   dim <= bound if sgn>0,
   dim = bound if sgn=0
   dim >= bound if sgn<0

   Normally returns 0,
   Returns 1 if equality of an integer dimension with a non-integer numbers
*/

bool vector_set_dim_bound(pk_internal_t* pk,
			  numint_t* vec,
			  ap_dim_t dim,
			  mpq_t mpq,
			  size_t intdim, size_t realdim,
			  int sgn,
			  bool integer);

/* Fills the vector with the constraint:
   expr <= bound if sgn>0,
   expr = bound if sgn=0
   expr >= bound if sgn<0

   Normally returns 0,
   Returns 1 if equality with a non-integer numbers (like 2x=1).
*/

bool vector_set_linexpr_bound(pk_internal_t* pk,
			      numint_t* vec,
			      const numint_t* vec2,
			      mpq_t mpq,
			      size_t intdim, size_t realdim,
			      int sgn,
			      bool integer);

/* Assign with GMP semantics the given polyhedron with the empty
   (resp. universe) polyhedron, of same dimensions */
void poly_set_bottom(pk_internal_t* pk, poly_t* po);
void poly_set_top(pk_internal_t* pk, poly_t* po);

/* Return the empty (resp. universe) polyhedron of the specified dimensions */
poly_t* poly_bottom(ap_manager_t* man, size_t intdim, size_t realdim);
poly_t* poly_top(ap_manager_t* man, size_t intdim, size_t realdim);

/* Abstract an hypercube defined by the array of intervals of size
   intdim+realdim:

   Return a polyhedron of the specified dimensions defined by the
   array of intervals, one for each dimension. The array is assumed to be of size
   (intdim+realdim). */ 
poly_t* poly_of_box(ap_manager_t* man, 
		    size_t intdim, size_t realdim, 
		    const ap_interval_t** array);

/* Abstract a convex polyhedra defined by the array of linear constraints.:

   Return a polyhedron of the specified dimensions defined by the array of
   linear constraints. */
poly_t* poly_of_lincons_array(ap_manager_t* man,
			      size_t intdim, size_t realdim,
			      const ap_lincons0_array_t* cons);

/* Return the dimensions of the polyhedra */
ap_dimension_t poly_dimension(ap_manager_t* man, const poly_t* po);

#endif
