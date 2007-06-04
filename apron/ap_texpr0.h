/* ************************************************************************* */
/* ap_texpr0.h: tree expressions */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include <stdarg.h>

#ifndef _AP_TEXPR0_H_
#define _AP_TEXPR0_H_

#include "ap_dimension.h"
#include "ap_coeff.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ====================================================================== */
/* Datatypes */
/* ====================================================================== */

typedef enum ap_texpr_op_t {
  /* Operators */
  AP_TEXPR_ADD, AP_TEXPR_SUB, AP_TEXPR_MUL, AP_TEXPR_DIV,
  AP_TEXPR_MOD, AP_TEXPR_SQRT,

  /* Conversions */
  AP_TEXPR_FLOAT_OF_DOUBLE, AP_TEXPR_INT_OF_FLOAT
} ap_texpr_op_t;

typedef enum ap_texpr_mode_t {
  AP_MODE_NEAREST = GMP_RNDN, /* Nearest */
  AP_MODE_ZERO    = GMP_RNDZ, /* Zero */
  AP_MODE_UP      = GMP_RNDU, /* + Infinity */
  AP_MODE_DOWN    = GMP_RNDD, /* - Infinity */
  AP_MODE_RND,    /* All possible mode, non deterministically */
  AP_MODE_EXACT,
  AP_MODE_SIZE    /* Not to be used ! */
} ap_texpr_mode_t;

typedef struct ap_texpr_node_t {
  ap_texpr_op_t op;
  ap_texpr_mode_t mode;
  struct ap_texpr0_t* exprA; /* First operand */
  struct ap_texpr0_t* exprB; /* Second operand (for binary operations) */
} ap_texpr_node_t;

typedef enum ap_texpr_discr_t {
  AP_TEXPR_CST, AP_TEXPR_DIM, AP_TEXPR_NODE
} ap_texpr_discr_t;

typedef struct ap_texpr0_t {
  ap_texpr_discr_t discr;
  union {
    ap_coeff_t cst;
    ap_dim_t dim;
    ap_texpr_node_t* node;
  } val;
} ap_texpr0_t;

/* ====================================================================== */
/* I. Constructors and Destructors */
/* ====================================================================== */

ap_texpr0_t* ap_texpr0_cst(ap_coeff_t* coeff);
ap_texpr0_t* ap_texpr0_dim(ap_dim_t dim);
ap_texpr0_t* ap_texpr0_unop(ap_texpr_op_t op, ap_texpr_mode_t mode, 
			    ap_texpr0_t* opA);
ap_texpr0_t* ap_texpr0_binop(ap_texpr_op_t op, ap_texpr_mode_t mode, 
			     ap_texpr0_t* opA, ap_texpr0_t* opB);

ap_texpr0_t* ap_texpr0_copy(ap_texpr0_t* expr);
void ap_texpr0_free(ap_texpr0_t* expr);

/* Internal */
ap_texpr0_t* ap_texpr0_node(ap_texpr_op_t op, ap_texpr_mode_t mode, 
			    ap_texpr0_t* opA, ap_texpr0_t* opB);
  void ap_texpr0_node_free(ap_texpr_node_t* node);

/* ====================================================================== */
/* II. Tests */
/* ====================================================================== */

static inline bool ap_texpr0_is_unop(ap_texpr_op_t op){
  return (op>=AP_TEXPR_SQRT && op <= AP_TEXPR_INT_OF_FLOAT);
}
static inline bool ap_texpr0_is_binop(ap_texpr_op_t op){
  return (op<=AP_TEXPR_MOD);
}

/* ====================================================================== */
/* III. Linearization */
/* ====================================================================== */

ap_linexpr0_t* ap_texpr0_intlinearize(ap_texpr0_t* expr, 
				      ap_scalar_discr_t discr,
				      bool quasilinearize);

/* ====================================================================== */
/* IV. Change of dimensions and permutations */
/* ====================================================================== */

ap_texpr0_t* ap_texpr0_add_dimensions(ap_texpr0_t* expr,
				      ap_dimchange_t* dimchange);
ap_texpr0_t* ap_texpr0_permute_dimensions(ap_texpr0_t* expr,
					  ap_dimperm_t* dimperm);
void ap_texpr0_add_dimensions_with(ap_texpr0_t* expr,
				   ap_dimchange_t* dimchange);
void ap_texpr0_permute_dimensions_with(ap_texpr0_t* expr,
				       ap_dimperm_t* perm);

#ifdef __cplusplus
}
#endif

#endif
