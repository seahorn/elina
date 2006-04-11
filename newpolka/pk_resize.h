/* ********************************************************************** */
/* pk_resize.h: change and permutation of dimensions  */
/* ********************************************************************** */

/* This header file define operations allowing to convert polyhedra
from one representation to the dual one. */

#ifndef __PK_RESIZE_H__
#define __PK_RESIZE_H__

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"

/* ********************************************************************** */
/*  I. Vectors */
/* ********************************************************************** */

void vector_resize_dimensions(numint_t* q2, const numint_t* q1, size_t size, int diff);
void vector_add_dimensions(pk_internal_t* pk,
			   numint_t* newq,
			   const numint_t* q, size_t size,
			   const ap_dimchange_t* dimchange);
void vector_remove_dimensions(pk_internal_t* pk,
			      numint_t* newq,
			      const numint_t* q, size_t size,
			      const ap_dimchange_t* dimchange);
void vector_permute_dimensions(pk_internal_t* pk,
			       numint_t* newq,
			       const numint_t* q, size_t size,
			       const ap_dim_t* permut);

/* ====================================================================== */
/* II. Matrices */
/* ====================================================================== */

void matrix_resize(matrix_t* mat, int diff);
matrix_t* matrix_add_dimensions(pk_internal_t* pk,
				bool destructive, matrix_t* mat,
				const ap_dimchange_t* dimchange);
matrix_t* matrix_permute_dimensions(pk_internal_t* pk,
				bool destructive, matrix_t* mat,
				const ap_dim_t* permut);

/* ********************************************************************** */
/* III. Factorized forms */
/* ********************************************************************** */

poly_t* cherni_add_dimensions(pk_internal_t* pk,
			      bool destructive, poly_t* pa,
			      const ap_dimchange_t* dimchange);

/* ********************************************************************** */
/* IV. Exported functions */
/* ********************************************************************** */
poly_t* poly_add_dimensions(ap_manager_t* man,
			    bool destructive, poly_t* a,
			    const ap_dimchange_t* dimchange);

poly_t* poly_remove_dimensions(ap_manager_t* man,
			    bool destructive, poly_t* a,
			    const ap_dimchange_t* dimchange);
poly_t* poly_permute_dimensions(ap_manager_t* man,
				bool destructive,
				poly_t* a,
				const ap_dimperm_t* permutation);

#endif
