/* ************************************************************************* */
/* ap_texpr0.c: tree expressions */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "ap_texpr0.h"

#include <stdarg.h>

/* ====================================================================== */
/* I. Constructors and Destructors */
/* ====================================================================== */

ap_texpr0_t* ap_texpr0_cst(ap_coeff_t* coeff)
{
  ap_texpr0_t* res = malloc(sizeof(ap_texpr0_t));
  res->discr = AP_TEXPR_CST;
  ap_coeff_init_set(&res->val.cst,coeff);
  return res;
}
ap_texpr0_t* ap_texpr0_dim(ap_dim_t dim)
{
  ap_texpr0_t* res = malloc(sizeof(ap_texpr0_t));
  res->discr = AP_TEXPR_DIM;
  res->val.dim = dim;
  return res;
}
ap_texpr0_t* ap_texpr0_node(ap_texpr_op_t op, ap_texpr_mode_t mode, ap_texpr0_t* opA, ap_texpr0_t* opB)
{
  ap_texpr_node_t* node = malloc(sizeof(ap_texpr_node_t));
  node->op = op;
  node->mode = mode;
  node->exprA = opA;
  node->exprB = opB;
  ap_texpr0_t* res = malloc(sizeof(ap_texpr0_t));
  res->discr = AP_TEXPR_NODE;
  res->val.node = node;
  return res;
}
ap_texpr0_t* ap_texpr0_unop(ap_texpr_op_t op, ap_texpr_mode_t mode, ap_texpr0_t* opA)
{
  if (!ap_texpr0_is_unop(op)){
    fprintf(stderr,"ap_texpr0.c: ap_texpr0_unop: unary operator expected\n");
    abort();
  }
  return ap_texpr0_node(op,mode,opA,NULL);
}
ap_texpr0_t* ap_texpr0_binop(ap_texpr_op_t op, ap_texpr_mode_t mode, ap_texpr0_t* opA, ap_texpr0_t* opB)
{
  if (!ap_texpr0_is_binop(op)){
    fprintf(stderr,"ap_texpr0.c: ap_texpr0_binop: binary operator expected\n");
    abort();
  }
  return ap_texpr0_node(op,mode,opA,opB);
}
ap_texpr0_node_t* ap_texpr0_node_copy(ap_texpr_node_t* node)
{
  ap_texpr_node_t* res = malloc(sizeof(ap_texpr_node_t));
  res->op = node->op;
  res->mode = node->mode;
  res->exprA = ap_texpr0_copy(node->exprA);
  res->exprB = node->exprB==NULL ? NULL : ap_texpr0_copy(node->exprB);
  return res;
}
ap_texpr0_t* ap_texpr0_copy(ap_texpr0_t* expr)
{
  ap_texpr0_t* res = malloc(sizeof(ap_texpr0_t));
  res->discr = expr->discr;
  switch (expr->discr){
  case AP_TEXPR_CST:
    return ap_texpr0_cst(&expr->val.cst);
  case AP_TEXPR_DIM:
    return ap_texpr0_dim(expr->val.dim);
  case AP_TEXPR_NODE:
    return ap_texpr0_node_copy(expr->val.node);
  default:
    assert(false);
    return NULL;
  } 
}
void ap_texpr0_node_free(ap_texpr_node_t* node)
{
  if (node->exprA) ap_texpr0_free(node->exprA);
  if (node->exprB) ap_texpr0_free(node->exprB);
  free(node);
}
void ap_texpr0_free(ap_texpr0_t* expr)
{
  switch(expr->discr){
  case AP_TEXPR_CST:
    ap_coeff_free(expr->val.cst);
    break;
  case AP_TEXPR_DIM:
    break;
  case AP_TEXPR_NODE:
    ap_texpr0_node_free(expr->val.node);
    break;
  }
  free(expr);
}

/* ====================================================================== */
/* III. Linearization */
/* ====================================================================== */

ap_linexpr0_t* ap_texpr0_intlinearize(ap_texpr0_t* expr, 
				      ap_scalar_discr_t discr,
				      bool quasilinearize)
{
  ap_linexpr0_t* linexpr0;
  switch (discr){
  case AP_SCALAR_MPQ:
    linexpr0 = ap_texpr0_intlinearize_aux_mpq(expr,quasilinearize);
    break;
  case AP_SCALAR_DBL:
    linexpr0 = ap_texpr0_intlinearize_aux_dbl(expr,quasilinearize);
    break;
  default:
    assert(false);
    return NULL; 
    break;
  }
  return res;
}

/* ====================================================================== */
/* IV. Change of dimensions and permutations */
/* ====================================================================== */

ap_texpr0_t* ap_texpr0_add_dimensions(ap_texpr0_t* expr,
				      ap_dimchange_t* dimchange)
{
  ap_texpr0_add_dimensions_with(ap_texpr0_copy(expr),dimchange);
}
void ap_texpr0_add_dimensions_with(ap_texpr0_t* expr,
				   ap_dimchange_t* dimchange)
{
  if (expr==NULL) return;
  switch(expr->discr){
  case AP_TEXPR_CST:
    return;
  case AP_TEXPR_DIM:
    {
      size_t dimsup = dimchange->intdim+dimchange->realdim;
      size_t k = 0;
      while (k<dimsup && expr->val.dim>=dimchange->dim[k]){
	k++;
      }
      expr->val.dim += k;
    }
    return;
  case AP_TEXPR_NODE:
    ap_texpr0_add_dimensions_with(expr->val.node->exprA,dimchange);
    if (expr->val.node->exprB)
      ap_texpr0_add_dimensions_with(expr->val.node->exprB,dimchange);
    return;
  default:
    assert(false);
  }
}
ap_texpr0_t* ap_texpr0_permute_dimensions(ap_texpr0_t* expr,
					    ap_dimperm_t* dimperm)
{
  ap_texpr0_permute_dimensions_with(ap_texpr0_copy(expr),dimperm);
}
void ap_texpr0_permute_dimensions_with(ap_texpr0_t* expr,
				       ap_dimperm_t* perm)
{
  if (expr==NULL) return;
  switch(expr->discr){
  case AP_TEXPR_CST:
    return;
  case AP_TEXPR_DIM:
    expr->val.dim = perm->dim[expr->val.dim];
    return;
  case AP_TEXPR_NODE:
    ap_texpr0_add_dimensions_with(expr->val.node->exprA,dimchange);
    if (expr->val.node->exprB)
      ap_texpr0_add_dimensions_with(expr->val.node->exprB,dimchange);
    return;
  default:
    assert(false);
  }
}    
