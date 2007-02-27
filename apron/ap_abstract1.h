/* ************************************************************************* */
/* ap_abstract1.h: generic operations on abstract values at level 1 */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

/* GENERATED FROM ap_abstract1.nw: DO NOT MODIFY ! */

#ifndef _AP_ABSTRACT1_H_
#define _AP_ABSTRACT1_H_

#include "ap_manager.h"
#include "ap_abstract0.h"
#include "ap_expr1.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ap_abstract1_t {
  ap_abstract0_t* abstract0;
  ap_environment_t* env;
} ap_abstract1_t;
  /* data structure invariant:
     ap_abstract0_integer_dimension(man,abstract0)== env->intdim &&
     ap_abstract0_real_dimension(man,abstract0)== env->realdim */

typedef struct ap_box1_t {
  ap_interval_t** p;
  ap_environment_t* env;
} ap_box1_t;
void ap_box1_fprint(FILE* stream, ap_box1_t* box);
void ap_box1_clear(ap_box1_t* box);

/* ********************************************************************** */
/* I. General management */
/* ********************************************************************** */

/* ============================================================ */
/* I.1 Memory */
/* ============================================================ */

ap_abstract1_t ap_abstract1_copy(ap_manager_t* man, ap_abstract1_t* a);
  /* Return a copy of an abstract value, on
     which destructive update does not affect the initial value. */

void ap_abstract1_clear(ap_manager_t* man, ap_abstract1_t* a);
  /* Free all the memory used by the abstract value */

size_t ap_abstract1_size(ap_manager_t* man, ap_abstract1_t* a);
  /* Return the abstract size of an abstract value (see ap_manager_t) */


/* ============================================================ */
/* I.2 Control of internal representation */
/* ============================================================ */

void ap_abstract1_minimize(ap_manager_t* man, ap_abstract1_t* a);
  /* Minimize the size of the representation of a.
     This may result in a later recomputation of internal information.
  */

void ap_abstract1_canonicalize(ap_manager_t* man, ap_abstract1_t* a);
  /* Put the abstract value in canonical form. (not yet clear definition) */

void ap_abstract1_approximate(ap_manager_t* man, ap_abstract1_t* a, int algorithm);
  /* Perform some transformation on the abstract value, guided by the
     field algorithm.

     The transformation may lose information.  The argument "algorithm"
     overrides the field algorithm of the structure of type foption_t
     associated to ap_abstract1_approximate (commodity feature). */

/* ============================================================ */
/* I.3 Printing */
/* ============================================================ */

void ap_abstract1_fprint(FILE* stream,
			 ap_manager_t* man,
			 ap_abstract1_t* a);
  /* Print the abstract value in a pretty way */

void ap_abstract1_fprintdiff(FILE* stream,
			     ap_manager_t* man,
			     ap_abstract1_t* a1, ap_abstract1_t* a2);
  /* Print the difference between a1 (old value) and a2 (new value).
     The meaning of difference is library dependent. */

void ap_abstract1_fdump(FILE* stream, ap_manager_t* man, ap_abstract1_t* a);
  /* Dump the internal representation of an abstract value,
     for debugging purposes. */


/* ============================================================ */
/* I.4 Serialization */
/* ============================================================ */

ap_membuf_t ap_abstract1_serialize_raw(ap_manager_t* man, ap_abstract1_t* a);
/* Allocate a memory buffer (with malloc), output the abstract value in raw
   binary format to it and return a pointer on the memory buffer and the size
   of bytes written.  It is the user responsability to free the memory
   afterwards (with free). */

ap_abstract1_t ap_abstract1_deserialize_raw(ap_manager_t* man, void* ptr, size_t* size);
/* Return the abstract value read in raw binary format from the input stream
   and store in size the number of bytes read */

/* ********************************************************************** */
/* II. Constructor, accessors, tests and property extraction */
/* ********************************************************************** */

/* ============================================================ */
/* II.1 Basic constructors */
/* ============================================================ */

ap_abstract1_t ap_abstract1_bottom(ap_manager_t* man, ap_environment_t* env);
  /* Create a bottom (empty) value defined on the environment */

ap_abstract1_t ap_abstract1_top(ap_manager_t* man, ap_environment_t* env);
  /* Create a top (universe) value defined on the environment */

ap_abstract1_t ap_abstract1_of_box(ap_manager_t* man,
				   ap_environment_t* env,
				   ap_var_t* tvar,
				   ap_interval_t** tinterval,
				   size_t size);
  /* Abstract an hypercube defined by the arrays tvar and tinterval,
     satisfying: forall i, tvar[i] in tinterval[i].

     If no inclusion is specified for a variable in the environement, its value
     is no constrained in the resulting abstract value.
  */

ap_abstract1_t ap_abstract1_of_lincons_array(ap_manager_t* man,
					     ap_environment_t* env,
					     ap_lincons1_array_t* array);
  /* Abstract a convex polyhedra defined on the environment
     by the array of linear constraints
  */

/* ============================================================ */
/* II.2 Accessors */
/* ============================================================ */

ap_environment_t* ap_abstract1_environment(ap_manager_t* man, ap_abstract1_t* a);
ap_abstract0_t* ap_abstract1_abstract0(ap_manager_t* man, ap_abstract1_t* a);

/* ============================================================ */
/* II.3 Tests */
/* ============================================================ */

/* If any of the following functions returns tbool_top, this means that
   an exception has occured, or that the exact computation was
   considered too expensive to be performed (according to the options).
   The flag exact and best should be cleared in such a case. */

tbool_t ap_abstract1_is_bottom(ap_manager_t* man, ap_abstract1_t* a);
tbool_t ap_abstract1_is_top(ap_manager_t* man, ap_abstract1_t* a);

tbool_t ap_abstract1_is_leq(ap_manager_t* man, ap_abstract1_t* a1, ap_abstract1_t* a2);
  /* inclusion check */

tbool_t ap_abstract1_is_eq(ap_manager_t* man, ap_abstract1_t* a1, ap_abstract1_t* a2);
  /* equality check */

tbool_t ap_abstract1_sat_lincons(ap_manager_t* man, ap_abstract1_t* a, ap_lincons1_t* lincons);
  /* does the abstract value satisfy the linear constraint ? */

tbool_t ap_abstract1_sat_interval(ap_manager_t* man, ap_abstract1_t* a,
				  ap_var_t var, ap_interval_t* interval);
  /* is the dimension included in the interval in the abstract value ?
     - Raises an exception if var is unknown in the environment of the abstract value
  */
tbool_t ap_abstract1_is_variable_unconstrained(ap_manager_t* man, ap_abstract1_t* a,
					       ap_var_t var);
  /* is the variable unconstrained in the abstract value ?
     - Raises an exception if var is unknown in the environment of the abstract value
  */

/* ============================================================ */
/* II.4 Extraction of properties */
/* ============================================================ */

ap_interval_t* ap_abstract1_bound_linexpr(ap_manager_t* man,
					  ap_abstract1_t* a, ap_linexpr1_t* expr);
  /* Returns the interval taken by a linear expression
     over the abstract value */

ap_interval_t* ap_abstract1_bound_variable(ap_manager_t* man,
					   ap_abstract1_t* a, ap_var_t var);
  /* Returns the interval taken by the variable
     over the abstract value
     - Raises an exception if var is unknown in the environment of the abstract value
  */

ap_lincons1_array_t ap_abstract1_to_lincons_array(ap_manager_t* man, ap_abstract1_t* a);
  /* Converts an abstract value to a polyhedra
     (conjunction of linear constraints).
     - The environment of the result is a copy of the environment of the abstract value.
  */

ap_box1_t ap_abstract1_to_box(ap_manager_t* man, ap_abstract1_t* a);
  /* Converts an abstract value to an interval/hypercube. */

ap_generator1_array_t ap_abstract1_to_generator_array(ap_manager_t* man, ap_abstract1_t* a);
  /* Converts an abstract value to a system of generators.
     - The environment of the result is a copy of the environment of the abstract value.
 */

/* ********************************************************************** */
/* III. Operations: functional version */
/* ********************************************************************** */

/* ============================================================ */
/* III.1 Meet and Join */
/* ============================================================ */

ap_abstract1_t ap_abstract1_meet(ap_manager_t* man, bool destructive, ap_abstract1_t* a1, ap_abstract1_t* a2);
ap_abstract1_t ap_abstract1_join(ap_manager_t* man, bool destructive, ap_abstract1_t* a1, ap_abstract1_t* a2);
  /* Meet and Join of 2 abstract values
     - The environment of the result is the lce of the arguments
     - Raises an EXC_INVALID_ARGUMENT exception if the lce does not exists
  */

ap_abstract1_t ap_abstract1_meet_array(ap_manager_t* man, ap_abstract1_t* tab, size_t size);
ap_abstract1_t ap_abstract1_join_array(ap_manager_t* man, ap_abstract1_t* tab, size_t size);
  /* Meet and Join of an array of abstract values.
     - Raises an [[exc_invalid_argument]] exception if [[size==0]]
       (no way to define the dimensionality of the result in such a case
     - The environment of the result is the lce of the arguments
     - Raises an EXC_INVALID_ARGUMENT exception if the lce does not exists
  */

ap_abstract1_t ap_abstract1_meet_lincons_array(ap_manager_t* man,
					       bool destructive,
					       ap_abstract1_t* a,
					       ap_lincons1_array_t* array);
  /* Meet of an abstract value with a set of constraints
     (generalize ap_abstract1_of_lincons_array) */

ap_abstract1_t ap_abstract1_add_ray_array(ap_manager_t* man,
					  bool destructive,
					  ap_abstract1_t* a,
					  ap_generator1_array_t* array);
  /* Generalized time elapse operator */

/* ============================================================ */
/* III.2 Assignement and Substitutions */
/* ============================================================ */

/* Assignement and Substitution of a single
   dimension by a interval linear expression */
ap_abstract1_t ap_abstract1_assign_linexpr(ap_manager_t* man,
					   bool destructive, ap_abstract1_t* a,
					   ap_var_t var, ap_linexpr1_t* expr,
					   ap_abstract1_t* dest);
ap_abstract1_t ap_abstract1_substitute_linexpr(ap_manager_t* man,
					       bool destructive, ap_abstract1_t* a,
					       ap_var_t var, ap_linexpr1_t* expr,
					       ap_abstract1_t* dest);

/* Parallel Assignement and Substitution of several dimensions by
   linear expressons. */
ap_abstract1_t 
ap_abstract1_assign_linexpr_array(ap_manager_t* man,
				  bool destructive, ap_abstract1_t* a,
				  ap_var_t* tvar, ap_linexpr1_t* texpr, size_t size,
				  ap_abstract1_t* dest);
ap_abstract1_t 
ap_abstract1_substitute_linexpr_array(ap_manager_t* man,
				      bool destructive, ap_abstract1_t* a,
				      ap_var_t* tvar, ap_linexpr1_t* texpr, size_t size,
				      ap_abstract1_t* dest);
  
/* ============================================================ */
/* III.3 Projections */
/* ============================================================ */

ap_abstract1_t ap_abstract1_forget_array(ap_manager_t* man,
					 bool destructive, ap_abstract1_t* a, 
					 ap_var_t* tvar, size_t size,
					 bool project);

/* ============================================================ */
/* III.4 Change of environnement */
/* ============================================================ */

ap_abstract1_t 
ap_abstract1_change_environment(ap_manager_t* man,
				bool destructive, ap_abstract1_t* a,
				ap_environment_t* nenv,
				bool project);

ap_abstract1_t 
ap_abstract1_minimize_environment(ap_manager_t* man,
				  bool destructive, ap_abstract1_t* a);
  /* Remove from the environment of the abstract value
     variables that are unconstrained in it. */

ap_abstract1_t 
ap_abstract1_rename_array(ap_manager_t* man,
			  bool destructive, ap_abstract1_t* a,
			  ap_var_t* var, ap_var_t* nvar, size_t size);
  /* Parallel renaming. The new variables should not interfere with the variables that are not renamed. */

/* ============================================================ */
/* III.5 Expansion and folding of dimensions */
/* ============================================================ */

ap_abstract1_t ap_abstract1_expand(ap_manager_t* man,
				   bool destructive, ap_abstract1_t* a,
				   ap_var_t var,
				   ap_var_t* tvar, size_t size);
  /* Expand the variable var into itself + the size additional variables 
     of the array tvar, which are given the same type as var.

     It results in (size+1) unrelated variables having same
     relations with other variables.

     The additional variables are added to the environment
     of the argument for making the environment of the result.
  */

ap_abstract1_t ap_abstract1_fold(ap_manager_t* man,
				 bool destructive, ap_abstract1_t* a,
				 ap_var_t* tvar, size_t size);
  /* Fold the dimensions in the array tvar of size n>=1 and put the result
     in the first variable in the array.

     The other variables of the array are then forgot
     and removed from the environment. */


/* ============================================================ */
/* III.6 Widening */
/* ============================================================ */

/* Widening */
ap_abstract1_t ap_abstract1_widening(ap_manager_t* man,
				     ap_abstract1_t* a1, ap_abstract1_t* a2);
/* Widening with threshold */
ap_abstract1_t ap_abstract1_widening_threshold(ap_manager_t* man,
					       ap_abstract1_t* a1, ap_abstract1_t* a2,
					       ap_lincons1_array_t* array);


/* ============================================================ */
/* III.7 Closure operation */
/* ============================================================ */

/* Returns the topological closure of a possibly opened abstract value */

ap_abstract1_t ap_abstract1_closure(ap_manager_t* man, bool destructive, ap_abstract1_t* a);

#ifdef __cplusplus
}
#endif

#endif
