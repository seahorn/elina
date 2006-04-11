/* ********************************************************************** */
/* pk_user.h: conversions with interface datatypes */
/* ********************************************************************** */

/* This header file defines operations on vectors. A vector is just an array of
   elements of type pkint_t, as a consequence functions need to be given also
   their size. */

#ifndef __PK_USER_H__
#define __PK_USER_H__

#include "pk_config.h"
#include "pk_internal.h"
#include "pk_vector.h"
#include "pk_matrix.h"

/* Conversions with user datatypes */

void vector_set_linexpr(pk_internal_t* pk,
			numint_t* vec,
			const ap_linexpr0_t* linexpr,
			size_t dim,
			int mode);
void vector_set_lincons(pk_internal_t* pk,
			numint_t* vec,
			const ap_lincons0_t* lincons,
			size_t intdim, size_t realdim,
			bool integer);

void vector_set_generator(pk_internal_t* pk,
			  numint_t* vec,
			  const ap_generator0_t* generator,
			  size_t intdim, size_t realdim);

ap_lincons0_t lincons_of_vector(pk_internal_t* pk,
			    numint_t* q,
			    size_t size);
ap_generator0_t generator_of_vector(pk_internal_t* pk,
				numint_t* q,
				size_t size);

/* Conversion t and from user-datatypes */
matrix_t* matrix_of_lincons_array(pk_internal_t* pk, const ap_lincons0_array_t* array, size_t intdim, size_t realdim, bool integer);
matrix_t* matrix_of_generator_array(pk_internal_t* pk, const ap_generator0_array_t* array, size_t intdim, size_t realdim);


#endif
