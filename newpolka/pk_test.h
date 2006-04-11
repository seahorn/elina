/* ********************************************************************** */
/* pk_test.h: tests on polyhedra */
/* ********************************************************************** */

#ifndef _PK_TEST_H_
#define _PK_TEST_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_matrix.h"
#include "pk.h"

/* Strict behaviour means here that the answer is either tbool_true or
   tbool_false, unless an exception occurs. */

/* Emptiness test 
   algorithm >= 0: strict behaviour, compute canonical form if necessary
   algorithm < 0: lazy behaviour, always cheap
*/
tbool_t poly_is_bottom(ap_manager_t* man, const poly_t* po);

/* Universe test 
   algorithm >= 0: strict behaviour, compute canonical form if necessary
   algorithm < 0: lazy behaviour, always cheap
*/
tbool_t poly_is_top(ap_manager_t* man, const poly_t* po);

/* Inclusion test: 
   Is always strict
   algorithm > 0: (nearly always) compute canonical forms
   algorithm <= 0: compute dual representations only if necessary
*/
tbool_t poly_is_leq(ap_manager_t* man, const poly_t* pa, const poly_t* pb);

/* Equality test:
   Is always strict
   Use algorithm field of is_leq.
*/

tbool_t poly_is_eq(ap_manager_t* man, const poly_t* pa, const poly_t* pb);

/* Satisfiability of a linear constraint
   Is always strict
   algorithm > 0: (nearly always) compute canonical form
   algorithm <= 0: compute dual representation only if necessary
*/
tbool_t poly_sat_lincons(ap_manager_t* man, const poly_t* po, const ap_lincons0_t* lincons);

/* Inclusion of a dimension in an interval 
   Is always strict
   algorithm > 0: (nearly always) compute canonical form
   algorithm <= 0: compute dual representation only if necessary
*/
tbool_t poly_sat_interval(ap_manager_t* man, const poly_t* po,
			  ap_dim_t dim, const ap_interval_t* interval);

/* Is a dimension unconstrained ?
   Is always strict
   algorithm > 0: compute canonical form
   algorithm <= 0: compute dual representation only if necessary
*/
tbool_t poly_is_dimension_unconstrained(ap_manager_t* man, const poly_t* po,
					ap_dim_t dim);

/*
F is suppposed to be a valid matrix of ray (i.e., corresponding
to a non empty polyhedron.

The epsilon component of the constraint is not taken into account.  The
constraint is considered as strict only if the is_strict paramater telles so.
This enables to test the satisfiability of a strict constraint in non-strict
mode for the library.
*/
bool do_generators_sat_constraint(pk_internal_t* pk, 
				  const matrix_t* F, 
				  const numint_t* tab, bool is_strict);

#endif
