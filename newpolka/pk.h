/* ********************************************************************** */
/* pk.h: Interface of the polka library  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

/* The invariant of the representation of a polyhedron is the following:
if the polyhedron is empty, then \verb-C=F=satC=satF=0-. Else, we have
\verb-(C || F) && (satC || satF || !(C && F))-. This means that a
polyhedron has a minimal representation minimal if and only if \verb-C && F-
if and only if \verb-satC || satF-. */

#ifndef __PK_H__
#define __PK_H__

#include "ap_global0.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct poly_t poly_t;
typedef struct pk_internal_t pk_internal_t;

/* 

  Important remark: the newpolka library is normally intended to be accessed
  through the APRON itnerface, i.e., through abstract0_XX and abstract1_XX
  functions. If it is accessed directly with poly_XXX functions, many checks on
  arguments will not be performed.

*/


/* ============================================================ */
/* A. Constructor for APRON manager (to be freed with ap_manager_free). */
/* ============================================================ */

ap_manager_t* pk_manager_alloc(bool strict);

/* ============================================================ */
/* B. Options */
/* ============================================================ */

pk_internal_t* pk_manager_get_internal(ap_manager_t* man);

/* For setting options when one has a ap_manager_t object, one can use the
   APRON function ap_manager_get_internal with a cast. */

void pk_set_max_coeff_size(pk_internal_t* pk, size_t size);
void pk_set_approximate_max_coeff_size(pk_internal_t* pk, size_t size);
size_t pk_get_max_coeff_size(pk_internal_t* pk);
size_t pk_get_approximate_max_coeff_size(pk_internal_t* pk);

/* ============================================================ */
/* D. Conversions */
/* ============================================================ */

poly_t* pk_to_poly(ap_abstract0_t* abstract);
ap_abstract0_t* pk_of_poly(ap_manager_t* man, poly_t* poly);

/* ============================================================ */
/* D. Constructor and destructor for internal manager */
/* ============================================================ */

/* Allocates pk and initializes it with a default size */
struct pk_internal_t* pk_internal_alloc(bool strict);
/* Clear and free pk */
void pk_internal_free(pk_internal_t* pk);

/* ********************************************************************** */
/* I. General management */
/* ********************************************************************** */


/* ============================================================ */
/* I.1 Memory */
/* ============================================================ */

poly_t* poly_copy(ap_manager_t* man, const poly_t* a);
  /* Return a copy of an abstract value, on
     which destructive update does not affect the initial value. */

void poly_free(ap_manager_t* man, poly_t* a);
  /* Free all the memory used by the abstract value */

size_t poly_size(ap_manager_t* man, const poly_t* a);
  /* Return the abstract size of an abstract value (see ap_manager_t) */


/* ============================================================ */
/* I.2 Control of internal representation */
/* ============================================================ */

void poly_minimize(ap_manager_t* man, const poly_t* a);
  /* Minimize the size of the representation of a.
     This may result in a later recomputation of internal information.
  */

void poly_canonicalize(ap_manager_t* man, const poly_t* a);
  /* Put the abstract value in canonical form. (not yet clear definition) */

void poly_approximate(ap_manager_t* man, poly_t* a, int algorithm);
  /* Perform some transformation on the abstract value, guided by the
     field algorithm.

     The transformation may lose information.  The argument "algorithm"
     overrides the field algorithm of the structure of type foption_t
     associated to poly_approximate (commodity feature). */

tbool_t poly_is_minimal(ap_manager_t* man, const poly_t* a);
tbool_t poly_is_canonical(ap_manager_t* man, const poly_t* a);


/* ============================================================ */
/* I.3 Printing */
/* ============================================================ */

void poly_fprint(FILE* stream,
		ap_manager_t* man,
		const poly_t* a,
		char** name_of_dim);
  /* Print the abstract value in a pretty way, using function
     name_of_dim to name dimensions */

void poly_fprintdiff(FILE* stream,
		     ap_manager_t* man,
		     const poly_t* a1, const poly_t* a2,
		     char** name_of_dim);
  /* Print the difference between a1 (old value) and a2 (new value),
     using function name_of_dim to name dimensions.
     The meaning of difference is library dependent. */

void poly_fdump(FILE* stream, ap_manager_t* man, const poly_t* a);
  /* Dump the internal representation of an abstract value,
     for debugging purposes */


/* ============================================================ */
/* I.4 Serialization */
/* ============================================================ */

ap_membuf_t poly_serialize_raw(ap_manager_t* man, const poly_t* a);
/* Allocate a memory buffer (with malloc), output the abstract value in raw
   binary format to it and return a pointer on the memory buffer and the size
   of bytes written.  It is the user responsability to free the memory
   afterwards (with free). */

poly_t* poly_deserialize_raw(ap_manager_t* man, void* ptr, size_t* size);
/* Return the abstract value read in raw binary format from the input stream
   and store in size the number of bytes read */

/* ********************************************************************** */
/* II. Constructor, accessors, tests and property extraction */
/* ********************************************************************** */

/* ============================================================ */
/* II.1 Basic constructors */
/* ============================================================ */

/* We assume that dimensions [0..intdim-1] correspond to integer variables, and
   dimensions [intdim..intdim+realdim-1] to real variables */

poly_t* poly_bottom(ap_manager_t* man, size_t intdim, size_t realdim);
  /* Create a bottom (empty) value */

poly_t* poly_top(ap_manager_t* man, size_t intdim, size_t realdim);
  /* Create a top (universe) value */


poly_t* poly_of_box(ap_manager_t* man,
		    size_t intdim, size_t realdim,
		    const ap_interval_t** tinterval);
  /* Abstract an hypercube defined by the array of intervals
     of size intdim+realdim */

poly_t* poly_of_lincons_array(ap_manager_t* man,
			      size_t intdim, size_t realdim,
			      const ap_lincons0_array_t* array);
  /* Abstract a convex polyhedra defined by the array of linear constraints
     of size size */


/* ============================================================ */
/* II.2 Accessors */
/* ============================================================ */

ap_dimension_t poly_dimension(ap_manager_t* man, const poly_t* a);
/* Return the total number of dimensions of the abstract values */

/* ============================================================ */
/* II.3 Tests */
/* ============================================================ */

/* If any of the following functions returns tbool_top, this means that
   an exception has occured, or that the exact computation was
   considered too expensive to be performed (according to the options).
   The flag exact and best should be cleared in such a case. */

tbool_t poly_is_bottom(ap_manager_t* man, const poly_t* a);
tbool_t poly_is_top(ap_manager_t* man, const poly_t* a);

tbool_t poly_is_leq(ap_manager_t* man, const poly_t* a1, const poly_t* a2);
  /* inclusion check */

tbool_t poly_is_eq(ap_manager_t* man, const poly_t* a1, const poly_t* a2);
  /* equality check */

tbool_t poly_sat_lincons(ap_manager_t* man, const poly_t* a, const ap_lincons0_t* lincons);
  /* does the abstract value satisfy the linear constraint ? */

tbool_t poly_sat_interval(ap_manager_t* man, const poly_t* a,
			  ap_dim_t dim, const ap_interval_t* interval);
  /* is the dimension included in the interval in the abstract value ? */

tbool_t poly_is_dimension_unconstrained(ap_manager_t* man, const poly_t* po,
					ap_dim_t dim);
  /* is the dimension unconstrained ? */

/* ============================================================ */
/* II.4 Extraction of properties */
/* ============================================================ */

ap_interval_t* poly_bound_linexpr(ap_manager_t* man,
			      const poly_t* a, const ap_linexpr0_t* expr);
  /* Returns the interval taken by a linear expression
     over the abstract value */

ap_interval_t* poly_bound_dimension(ap_manager_t* man,
				const poly_t* a, ap_dim_t dim);
  /* Returns the interval taken by the dimension
     over the abstract value */

ap_lincons0_array_t poly_to_lincons_array(ap_manager_t* man, const poly_t* a);
  /* Converts an abstract value to a polyhedra
     (conjunction of linear constraints). */

ap_interval_t** poly_to_box(ap_manager_t* man, const poly_t* a);
  /* Converts an abstract value to an interval/hypercube.
     The size of the resulting array is poly_dimension(man,a).  This
     function can be reimplemented by using poly_bound_linexpr */

ap_generator0_array_t poly_to_generator_array(ap_manager_t* man, const poly_t* a);
  /* Converts an abstract value to a system of generators. */


/* ********************************************************************** */
/* III. Operations */
/* ********************************************************************** */

/* ============================================================ */
/* III.1 Meet and Join */
/* ============================================================ */

poly_t* poly_meet(ap_manager_t* man, bool destructive, poly_t* a1, const poly_t* a2);
poly_t* poly_join(ap_manager_t* man, bool destructive, poly_t* a1, const poly_t* a2);
  /* Meet and Join of 2 abstract values */

poly_t* poly_meet_array(ap_manager_t* man, const poly_t** tab, size_t size);
poly_t* poly_join_array(ap_manager_t* man, const poly_t** tab, size_t size);
  /* Meet and Join of an array of abstract values.
     Raises an [[exc_invalid_argument]] exception if [[size==0]]
     (no way to define the dimensionality of the result in such a case */

poly_t* poly_meet_lincons_array(ap_manager_t* man,
				bool destructive, poly_t* a,
				const ap_lincons0_array_t* array);
  /* Meet of an abstract value with a set of constraints
     (generalize poly_of_lincons_array) */

poly_t* poly_add_ray_array(ap_manager_t* man,
			   bool destructive, poly_t* a,
			   const ap_generator0_array_t* array);
  /* Generalized time elapse operator */

/* ============================================================ */
/* III.2 Assignement and Substitutions */
/* ============================================================ */

poly_t* poly_assign_linexpr(ap_manager_t* man,
			    bool destructive, poly_t* a,
			    ap_dim_t dim, const ap_linexpr0_t* expr,
			    const poly_t* dest);
poly_t* poly_substitute_linexpr(ap_manager_t* man,
				bool destructive, poly_t* a,
				ap_dim_t dim, const ap_linexpr0_t* expr,
				const poly_t* dest);
  /* Assignement and Substitution of a single dimension by resp.
     a linear expression and a interval linear expression */

poly_t* poly_assign_linexpr_array(ap_manager_t* man,
				  bool destructive, poly_t* a,
				  const ap_dim_t* tdim,
				  const ap_linexpr0_t** texpr,
				  size_t size,
				  const poly_t* dest);
poly_t* poly_substitute_linexpr_array(ap_manager_t* man,
				      bool destructive, poly_t* a,
				      const ap_dim_t* tdim,
				      const ap_linexpr0_t** texpr,
				      size_t size,
				      const poly_t* dest);
  /* Parallel Assignement and Substitution of several dimensions by
     linear expressons. */

/* ============================================================ */
/* III.3 Projections */
/* ============================================================ */

poly_t* poly_forget_array(ap_manager_t* man,
			  bool destructive, poly_t* a,
			  const ap_dim_t* tdim, size_t size,
			  bool project);

/* ============================================================ */
/* III.4 Change and permutation of dimensions */
/* ============================================================ */

poly_t* poly_add_dimensions(ap_manager_t* man,
			    bool destructive, poly_t* a,
			    const ap_dimchange_t* dimchange,
			    bool project);

poly_t* poly_remove_dimensions(ap_manager_t* man,
			    bool destructive, poly_t* a,
			    const ap_dimchange_t* dimchange);
poly_t* poly_permute_dimensions(ap_manager_t* man,
				     bool destructive,
				     poly_t* a,
				     const ap_dimperm_t* permutation);

/* ============================================================ */
/* III.5 Expansion and folding of dimensions */
/* ============================================================ */

poly_t* poly_expand(ap_manager_t* man,
		    bool destructive, poly_t* a,
		    ap_dim_t dim,
		    size_t n);
  /* Expand the dimension dim into itself + n additional dimensions.
     It results in (n+1) unrelated dimensions having same
     relations with other dimensions. The (n+1) dimensions are put as follows:

     - original dimension dim

     - if the dimension is integer, the n additional dimensions are put at the
       end of integer dimensions; if it is real, at the end of the real
       dimensions.
  */

poly_t* poly_fold(ap_manager_t* man,
		  bool destructive, poly_t* a,
		  const ap_dim_t* tdim,
		  size_t size);
  /* Fold the dimensions in the array tdim of size n>=1 and put the result
     in the first dimension in the array. The other dimensions of the array
     are then removed (using poly_permute_remove_dimensions). */

/* ============================================================ */
/* III.6 Widening */
/* ============================================================ */

/* Widening */
poly_t* poly_widening(ap_manager_t* man,
		      const poly_t* a1, const poly_t* a2);

/* ============================================================ */
/* III.7 Closure operation */
/* ============================================================ */

/* Returns the topological closure of a possibly opened abstract value */

poly_t* poly_closure(ap_manager_t* man, bool destructive, poly_t* a);

#ifdef __cplusplus
}
#endif

#endif
