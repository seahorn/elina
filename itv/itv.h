/* ********************************************************************** */
/* itv.h: abstract lattice of intervals */
/* ********************************************************************** */

#ifndef _ITV_H_
#define _ITV_H_

#include <stdio.h>
#include "ap_global0.h"

#ifdef __cplusplus
extern "C" {
#endif

/* OCaml interface */
ap_manager_t* itv_manager_alloc();

typedef struct itv_t itv_t;

/* ********************************************************************** */
/* I. General management */
/* ********************************************************************** */

/* ============================================================ */
/* I.1 Memory */
/* ============================================================ */

itv_t* itv_copy(ap_manager_t* man, const itv_t* a);
  /* Return a copy of an itv value, on 
     which destructive update does not affect the initial value. */

void itv_free(ap_manager_t* man, itv_t* a);
  /* Free all the memory used by the itv value */

size_t itv_size(ap_manager_t* man, const itv_t* a); 
  /* Return the itv size of an itv value (see ap_manager_t) */

/* ============================================================ */
/* I.2 Control of internal representation */
/* ============================================================ */

void itv_minimize(ap_manager_t* man, const itv_t* a);
  /* Minimize the size of the representation of a. 
     This may result in a later recomputation of internal information.
  */

void itv_canonicalize(ap_manager_t* man, const itv_t* a);
  /* Put the itv value in canonical form. (not yet clear definition) */

void itv_approximate(ap_manager_t* man, itv_t* a, int algorithm);
  /* Perform some transformation on the itv value, guided by the 
     field algorithm. 

     The transformation may lose information.  The argument "algorithm"
     overrides the field algorithm of the structure of type foption_t
     associated to itv_approximate (commodity feature). */

tbool_t itv_is_minimal(ap_manager_t* man, const itv_t* a);
tbool_t itv_is_canonical(ap_manager_t* man, const itv_t* a);

/* ============================================================ */
/* I.3 Printing */
/* ============================================================ */

void itv_fprint(FILE* stream, 
	       ap_manager_t* man, 
	       const itv_t* a, 
	       char** name_of_dim);
  /* Print the itv value in a pretty way, using function 
     name_of_dim to name dimensions */

void itv_fprintdiff(FILE* stream, 
		   ap_manager_t* man, 
		   const itv_t* a1, const itv_t* a2, 
		   char** name_of_dim);
/* Print the difference between a1 (old value) and a2 (new value), 
     using function name_of_dim to name dimensions.
     The meaning of difference is library dependent. */

void itv_fdump(FILE* stream, ap_manager_t* man, const itv_t* a);
  /* Dump the internal representation of an itv value, 
     for debugging purposes */

/* ============================================================ */
/* I.4 Serialization */
/* ============================================================ */

ap_membuf_t itv_serialize_raw(ap_manager_t* man, const itv_t* a); 
/* Allocate a memory bfer (with malloc), output the itv value in raw
   binary format to it and return a pointer on the memory buffer and the size
   of bytes written.  It is the user responsability to free the memory
   afterwards (with free). */

itv_t* itv_deserialize_raw(ap_manager_t* man, void* ptr); 
/* Return the itv value read in raw binary format from the input stream
   and store in size the number of bytes read */
/* ********************************************************************** */
/* II. Constructor, accessors, tests and property extraction */
/* ********************************************************************** */

/* ============================================================ */
/* II.1 Basic constructors */
/* ============================================================ */

/* We assume that dimensions [0..intdim-1] correspond to integer variables, and
   dimensions [intdim..intdim+realdim-1] to real variables */

itv_t* itv_bottom(ap_manager_t* man, size_t intdim, size_t realdim);
  /* Create a bottom (empty) value */

itv_t* itv_top(ap_manager_t* man, size_t intdim, size_t realdim);
  /* Create a top (universe) value */

itv_t* itv_of_box(ap_manager_t* man, 
		  size_t intdim, size_t realdim,
		  const ap_interval_t*const* tinterval);
  /* Itv an hypercube defined by the array of intervals 
     of size intdim+realdim */

itv_t* itv_of_lincons_array(ap_manager_t* man, 
			    size_t intdim, size_t realdim,
			    const ap_lincons0_array_t* array);
/* Itv a convex polyhedra defined by the array of linear constraints 
     of size size */

/* ============================================================ */
/* II.2 Accessors */
/* ============================================================ */

ap_dimension_t itv_dimension(ap_manager_t* man, const itv_t* a);
  /* Return the dimensionality of the abstract values */

/* ============================================================ */
/* II.3 Tests */
/* ============================================================ */

/* If any of the following functions returns tbool_top, this means that 
   an exception has occured, or that the exact computation was 
   considered too expensive to be performed (according to the options). 
   The flag exact and best should be cleared in such a case. */

tbool_t itv_is_bottom(ap_manager_t* man, const itv_t* a);
tbool_t itv_is_top(ap_manager_t* man, const itv_t* a);

tbool_t itv_is_leq(ap_manager_t* man, const itv_t* a1, const itv_t* a2);
  /* inclusion check */

tbool_t itv_is_eq(ap_manager_t* man, const itv_t* a1, const itv_t* a2);
  /* equality check */

tbool_t itv_sat_lincons(ap_manager_t* man, const itv_t* a, const ap_lincons0_t* cons);
  /* does the itv value satisfy the linear constraint ? */

tbool_t itv_sat_interval(ap_manager_t* man,
			 const itv_t* a, 
			 ap_dim_t dim, const ap_interval_t* interval);
  /* is the dimension included in the interval in the itv value ? */

tbool_t itv_is_dimension_unconstrained(ap_manager_t* man, 
				       const itv_t* a, ap_dim_t dim);
  /* is the dimension unconstrained in the abstract value ?  If it
     is the case, we have forget(man,a,dim) == a */

/* ============================================================ */
/* II.4 Extraction of properties */
/* ============================================================ */

ap_interval_t* itv_bound_dimension(ap_manager_t* man, 
				const itv_t* a, ap_dim_t dim);
  /* Returns the interval taken by a linear expression  
     over the itv value */

ap_interval_t* itv_bound_linexpr(ap_manager_t* man, 
			      const itv_t* a, const ap_linexpr0_t* expr);
  /* Returns the interval taken by a linear expression  
     over the itv value */

ap_interval_t** itv_to_box(ap_manager_t* man, const itv_t* a);
  /* Converts an itv value to an interval/hypercube.
     The size of the resulting array is itv_dimension(man,a).  This
     function can be reimplemented by using itv_bound_linexpr */

ap_lincons0_array_t itv_to_lincons_array(ap_manager_t* man, const itv_t* a);
  /* Converts an itv value to a polyhedra 
     (conjunction of linear constraints).
     The size of the returned array is stored in size. */

ap_generator0_array_t itv_to_generator_array(ap_manager_t* man, const itv_t* a);
  /* Converts an abstract value to a system of generators. */

/* ********************************************************************** */
/* III. Operations: functional version */
/* ********************************************************************** */

/* ============================================================ */
/* III.1 Meet and Join */
/* ============================================================ */


itv_t* itv_meet(ap_manager_t* man, bool destructive, itv_t* a1, const itv_t* a2);

itv_t* itv_join(ap_manager_t* man, bool destructive, itv_t* a1, const itv_t* a2);
  /* Meet and Join of 2 abstract values */


itv_t* itv_meet_array(ap_manager_t* man, const itv_t*const* tab, size_t size);

itv_t* itv_join_array(ap_manager_t* man, const itv_t*const* tab, size_t size);
  /* Meet and Join of an array of abstract values.
     Raises an [[exc_invalid_argument]] exception if [[size==0]]
     (no way to define the dimensionality of the result in such a case */


itv_t* itv_meet_lincons_array(ap_manager_t* man,
			      bool destructive,
			      itv_t* a,
			      const ap_lincons0_array_t* array);
  /* Meet of an abstract value with a set of constraints */


itv_t* itv_add_ray_array(ap_manager_t* man,
			 bool destructive,
			 itv_t* a,
			 const ap_generator0_array_t* array);
  /* Generalized time elapse operator */

/* ============================================================ */
/* III.2 Assignement and Substitutions */
/* ============================================================ */


itv_t* itv_assign_linexpr(ap_manager_t* man,
			  bool destructive,
			  itv_t* org,
			  ap_dim_t dim, const ap_linexpr0_t* expr,
			  const itv_t* dest);

itv_t* itv_substitute_linexpr(ap_manager_t* man,
			      bool destructive,
			      itv_t* org,
			      ap_dim_t dim, const ap_linexpr0_t* expr,
			      const itv_t* dest);
  /* Assignement and Substitution of a single dimension by a (interval)
     linear expression in abstract value org.

     dest is an optional argument. If not NULL, semantically speaking,
     the result of the transformation is intersected with dest. This is
     useful for precise backward transformations in lattices like intervals or
     octagons.
 */


itv_t* itv_assign_linexpr_array(ap_manager_t* man,
				bool destructive,
				itv_t* a,
				const ap_dim_t* tdim,
				const ap_linexpr0_t*const* texpr,
				size_t size,
				const itv_t* dest);

itv_t* itv_substitute_linexpr_array(ap_manager_t* man,
				    bool destructive,
				    itv_t* org,
				    const ap_dim_t* tdim,
				    const ap_linexpr0_t*const* texpr,
				    size_t size,
				    const itv_t* dest);
  /* Parallel Assignement and Substitution of several dimensions by
     linear expressions in abstract value org.

     dest is an optional argument. If not NULL, semantically speaking,
     the result of the transformation is intersected with dest. This is
     useful for precise backward transformations in lattices like intervals or
     octagons. */

/* ============================================================ */
/* III.3 Projections */
/* ============================================================ */

itv_t* itv_forget_array(ap_manager_t* man,
			bool destructive,
			itv_t* a, const ap_dim_t* tdim, size_t size,
			bool project);

/* ============================================================ */
/* III.4 Change and permutation of dimensions */
/* ============================================================ */

itv_t* itv_add_dimensions(ap_manager_t* man,
			  bool destructive,
			  itv_t* a,
			  const ap_dimchange_t* dimchange);
itv_t* itv_remove_dimensions(ap_manager_t* man,
			     bool destructive,
			     itv_t* a,
			     const ap_dimchange_t* dimchange);
/* Size of the permutation is supposed to be equal to
   the dimension of the abstract value */
itv_t* itv_permute_dimensions(ap_manager_t* man,
			      bool destructive,
			      itv_t* a,
			      const ap_dimperm_t* perm);

/* ============================================================ */
/* III.5 Expansion and folding of dimensions */
/* ============================================================ */


itv_t* itv_expand(ap_manager_t* man,
		  bool destructive,
		  itv_t* a,
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


itv_t* itv_fold(ap_manager_t* man,
		bool destructive,
		itv_t* a,
		const ap_dim_t* tdim,
		size_t size);
  /* Fold the dimensions in the array tdim of size n>=1 and put the result
     in the first dimension in the array. The other dimensions of the array
     are then removed (using itv_permute_remove_dimensions). */

/* ============================================================ */
/* III.6 Widening */
/* ============================================================ */

itv_t* itv_widening(ap_manager_t* man,
		    const itv_t* a1, const itv_t* a2);

/* ============================================================ */
/* III.7 Closure operation */
/* ============================================================ */

/* Returns the topological closure of a possibly opened abstract value */

itv_t* itv_closure(ap_manager_t* man, bool destructive, itv_t* a);

#ifdef __cplusplus
}
#endif

#endif
