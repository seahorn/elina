/* ************************************************************************* */
/* internal.h: internal manager */
/* ************************************************************************* */

#ifndef _PK_INTERNAL_H_
#define _PK_INTERNAL_H_

#include "pk_config.h"
#include "pk_bit.h"

/* ********************************************************************** */
/* I. Types */
/* ********************************************************************** */

/* These variables are used by various functions.  The prefix XXX_
   indicates that the variable is used by the module XXX. */

struct pk_internal_t {
  enum ap_exc_t exn;

  bool strict;
  size_t dec;

  size_t maxdims;
  size_t maxcols;
  size_t maxrows;

  ap_funid_t funid;
  ap_funopt_t* funopt;

  size_t max_coeff_size; /* Used for overflow exception in vector_combine */
  size_t approximate_max_coeff_size;

  numint_t* vector_numintp; /* of size maxcols */

  mpq_t* vector_mpqp; /* of size maxdims+3 */

  numint_t* vector_tmp;    /* of size 5 */
  
  ap_dim_t* matrix_dimp;                /* of size maxdims */
  numint_t matrix_acc;
  numint_t matrix_prod;

  /* bitstring_t* cherni_bitstringp; */ /* of size maxrows */
  int* cherni_intp;                /* of size maxcols */
  numint_t cherni_prod;             

  numint_t* poly_numintp;            /* of size maxcols */
  numint_t* poly_numintp2;           /* of size maxcols */
  /* bitstring_t* poly_bitstringp; */    /* of size maxrows */
  ap_dim_t* poly_dimp;                /* of size maxdims */
  ap_dim_t* poly_dimp2;               /* of size maxdims */
  ap_dim_t* poly_fold_dimp;               /* of size maxdims */
  struct matrix_t* poly_matspecial; 
  numint_t poly_prod; 
};
typedef struct pk_internal_t pk_internal_t;

/* I. Constructor and destructor for internal */

/* Allocates pk and initializes it with a default size */
pk_internal_t* pk_internal_alloc(bool strict);
/* Clear and free pk */
void pk_internal_free(pk_internal_t* pk);
/* Reallocate pk only if a bigger dimension is required */
void pk_internal_realloc_lazy(pk_internal_t* pk, size_t maxdims);

/* Initializes some fields of pk from manager */
static inline pk_internal_t* pk_init_from_manager(ap_manager_t* man, ap_funid_t funid);

/* II. Options */
void pk_set_max_coeff_size(pk_internal_t* pk, size_t size);
void pk_set_approximate_max_coeff_size(pk_internal_t* pk, size_t size);
size_t pk_get_max_coeff_size(pk_internal_t* pk);
size_t pk_get_approximate_max_coeff_size(pk_internal_t* pk);


ap_manager_t* pk_manager_alloc(bool strict);

/* ********************************************************************** */
/* Definition of inline functions */
static inline pk_internal_t* pk_init_from_manager(ap_manager_t* man, ap_funid_t funid)
{
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  pk->funid = funid;
  pk->funopt = &man->option.funopt[funid];
  man->result.flag_exact = man->result.flag_best = tbool_top;
  return pk;
}

#endif
