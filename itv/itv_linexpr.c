/* ********************************************************************** */
/* itv_linexpr.c: */
/* ********************************************************************** */

#include "itv_linexpr.h"

void ITVFUN(linexpr_init)(itv_linexpr_t* expr, size_t size)
{
  expr->linterm = NULL;
  expr->size = 0;
  itv_init(expr->cst);
  expr->equality = true;
  itv_linexpr_reinit(expr,size);
}
void ITVFUN(linexpr_reinit)(itv_linexpr_t* expr, size_t size)
{
  size_t i;
  expr->linterm = realloc(expr->linterm,size*sizeof(itv_linterm_t));

  for (i=expr->size;i<size;i++){
    itv_init(expr->linterm[i].itv);
    expr->linterm[i].equality = true;
  }
  for  (i=size; i<expr->size; i++){
    itv_clear(expr->linterm[i].itv);
  }
  expr->size = size;
  return;
}
void ITVFUN(linexpr_clear)(itv_linexpr_t* expr)
{
  size_t i;
  if (expr->linterm){
    for (i=0;i<expr->size;i++){
      itv_clear(expr->linterm[i].itv);
    }
    free(expr->linterm);
    expr->linterm = NULL;
    expr->size = 0;
  }
  itv_clear(expr->cst);
}
bool ITVFUN(linexpr_set_ap_linexpr0)(itv_internal_t* intern,
				     itv_linexpr_t* expr, const ap_linexpr0_t* linexpr0)
{
  size_t i,k,size;
  ap_dim_t dim;
  ap_coeff_t* coeff;
  bool res,exact;

  size=0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
    size++;
  }
  itv_linexpr_reinit(expr,size);
  exact = itv_set_ap_coeff(intern, expr->cst, &linexpr0->cst);
  expr->equality = exact && linexpr0->cst.discr==AP_COEFF_SCALAR;
  res = exact;
  k = 0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
    expr->linterm[k].dim = dim;
    exact = itv_set_ap_coeff(intern,
			     expr->linterm[k].itv,
			     coeff);
    expr->linterm[k].equality = exact && coeff->discr==AP_COEFF_SCALAR;
    res = res && exact;
    k++;
  }
  return res;
}
bool ITVFUN(lincons_set_ap_lincons0)(itv_internal_t* intern,
				     itv_lincons_t* cons, const ap_lincons0_t* lincons0)
{
  bool exact1 = itv_linexpr_set_ap_linexpr0(intern, &cons->linexpr,lincons0->linexpr0);
  cons->constyp = lincons0->constyp;
  if (lincons0->scalar){
    bool exact2 = num_set_ap_scalar(cons->num,lincons0->scalar);
    return exact1 && exact2;
  }
  else {
    num_set_int(cons->num,0);
    return exact1;
  }
}

/* Evaluate an interval linear expression */
void ITVFUN(eval_itv_linexpr)(itv_internal_t* intern,
			      itv_t itv,
			      const itv_t* p,
			      const itv_linexpr_t* expr)
{
  int i;
  ap_dim_t dim;
  itv_ptr pitv;
  bool* peq;
  assert(p);

  itv_set(intern->eval_itv2, expr->cst);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
    if (*peq){
      if (bound_sgn(pitv->sup)!=0){
	itv_mul_bound(intern,
		      intern->eval_itv,
		      p[dim],
		      pitv->sup);
	itv_add(intern->eval_itv2, intern->eval_itv2, intern->eval_itv);
      }
    }
    else {
      itv_mul(intern,
	      intern->eval_itv,
	      p[dim],
	      pitv);
      itv_add(intern->eval_itv2, intern->eval_itv2, intern->eval_itv);
    }
    if (itv_is_top(intern->eval_itv2))
      break;
  }
  itv_set(itv,intern->eval_itv2);
}

/* Evaluate an interval linear expression */
bool ITVFUN(eval_ap_linexpr0)(itv_internal_t* intern,
			      itv_t itv,
			      const itv_t* p,
			      const ap_linexpr0_t* expr)
{
  int i;
  ap_dim_t dim;
  ap_coeff_t* pcoeff;
  bool exact,res;
  assert(p);

  exact = itv_set_ap_coeff(intern, intern->eval_itv3, &expr->cst);
  res = exact;
  ap_linexpr0_ForeachLinterm(expr,i,dim,pcoeff){
    bool exact = itv_set_ap_coeff(intern,intern->eval_itv2,pcoeff);
    res = res && exact;
    bool eq = exact && pcoeff->discr==AP_COEFF_SCALAR;
    if (eq){
      if (bound_sgn(intern->eval_itv2->sup)!=0){
	itv_mul_bound(intern,
		      intern->eval_itv,
		      p[dim],
		      intern->eval_itv2->sup);
	itv_add(intern->eval_itv3, intern->eval_itv3, intern->eval_itv);
      }
    }
    else {
      itv_mul(intern,
	      intern->eval_itv,
	      p[dim],
	      intern->eval_itv2);
      itv_add(intern->eval_itv3, intern->eval_itv3, intern->eval_itv);
    }
    if (itv_is_top(intern->eval_itv3))
      break;
  }
  itv_set(itv,intern->eval_itv3);
  return res;
}
