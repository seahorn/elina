/* ********************************************************************** */
/* itv_linexpr.c: */
/* ********************************************************************** */

#include "itv_linexpr.h"

/* ********************************************************************** */
/* I. Constructor and Destructor */
/* ********************************************************************** */

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

  for  (i=size; i<expr->size; i++){
    itv_clear(expr->linterm[i].itv);
  }
  expr->linterm = realloc(expr->linterm,size*sizeof(itv_linterm_t));
  for (i=expr->size;i<size;i++){
    itv_init(expr->linterm[i].itv);
    expr->linterm[i].equality = true;
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

/* ********************************************************************** */
/* II. Conversions from and to APRON datatypes */
/* ********************************************************************** */

bool ITVFUN(linexpr_set_ap_linexpr0)(itv_internal_t* intern,
				     itv_linexpr_t* expr,
				     itv_t* p,
				     ap_linexpr0_t* linexpr0)
{

  size_t i,k,size;
  ap_dim_t dim;
  ap_coeff_t* coeff;
  bool res,exact, eq;

  size=0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
    size++;
  }
  if (size>expr->size){
    itv_linexpr_reinit(expr,size);
  }
  exact = itv_set_ap_coeff(intern, expr->cst, &linexpr0->cst);
  expr->equality = exact && linexpr0->cst.discr==AP_COEFF_SCALAR;
  res = exact;
  k = 0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
    if (p==NULL){
      exact = itv_set_ap_coeff(intern,
			       expr->linterm[k].itv,
			       coeff);
      if (!itv_is_zero(expr->linterm[k].itv)){
	eq = exact && coeff->discr==AP_COEFF_SCALAR;
	res = res && exact;
	expr->linterm[k].equality = eq;
	expr->linterm[k].dim = dim;
	k++;
      }
    }
    else {
      exact = itv_set_ap_coeff(intern,
			       intern->eval_itv3,
			       coeff);
      eq = exact && coeff->discr==AP_COEFF_SCALAR;
      res = res && exact;
      /* Compute the middle of the interval */
      if (bound_infty(intern->eval_itv3->inf)){
	if (bound_infty(intern->eval_itv3->sup))
	  num_set_int(intern->quasi_num,0);
	else 
	  num_set(intern->quasi_num,
		  bound_numref(intern->eval_itv3->sup));
      }
      else if (bound_infty(intern->eval_itv3->sup))
	num_neg(intern->quasi_num,
		bound_numref(intern->eval_itv3->inf));
      else {
	num_sub(intern->quasi_num,
		bound_numref(intern->eval_itv3->sup),
		bound_numref(intern->eval_itv3->inf));
	num_div_2(intern->quasi_num,
		  intern->quasi_num);
      }
      /* Compute - ((-inf) - middle) */
      bound_neg(intern->eval_itv2->inf,
		intern->eval_itv3->inf);
      bound_sub_num(intern->eval_itv2->inf,
		    intern->eval_itv2->inf,
		    intern->quasi_num);
      bound_neg(intern->eval_itv2->inf,
		intern->eval_itv2->inf);
      /* Compute sup - middle */
      bound_sub_num(intern->eval_itv2->sup,
		    intern->eval_itv3->sup,
		    intern->quasi_num);
      /* Multiplication */
      itv_mul(intern,
	      intern->eval_itv,
	      intern->eval_itv2,
	      p[dim]);
      /* Addition to the constant coefficient */
      itv_add(expr->cst,expr->cst,intern->eval_itv);
      if (itv_is_top(expr->cst)){
	k = 0;
	break;
      }
      /* Addition of the linear term */
      if (num_sgn(intern->quasi_num)!=0){
	expr->linterm[k].equality = true;
	expr->linterm[k].dim = dim;
	itv_set_num(expr->linterm[k].itv,intern->quasi_num);
	k ++;
      }
    }
  }
  itv_linexpr_reinit(expr,k);
  return res;
}

bool ITVFUN(ap_linexpr0_set_ap_linexpr0)(itv_internal_t* intern,
					 ap_linexpr0_t* rlinexpr0,
					 itv_t* p,
					 ap_linexpr0_t* linexpr0)
{
  bool exact = true;
  itv_linexpr_t linexpr;
  bool* peq;
  itv_ptr pitv;
  size_t i;
  ap_dim_t dim;

  itv_linexpr_init(&linexpr,0);
  exact = itv_linexpr_set_ap_linexpr0(intern,&linexpr,p,linexpr0) && exact;
  ap_linexpr0_realloc(rlinexpr0,linexpr.size);
  assert(rlinexpr0->discr==AP_LINEXPR_SPARSE);
  exact = ap_coeff_set_itv(intern,&rlinexpr0->cst,linexpr.cst) && exact;
  itv_linexpr_ForeachLinterm(&linexpr,i,dim,pitv,peq){
    rlinexpr0->p.linterm[i].dim = dim;
    exact = ap_coeff_set_itv(intern,&rlinexpr0->p.linterm[i].coeff,pitv) && exact;
  }
  itv_linexpr_clear(&linexpr);
  return exact;
}
bool ITVFUN(lincons_set_ap_lincons0)(itv_internal_t* intern,
				     itv_lincons_t* cons, 
				     itv_t* p,
				     ap_lincons0_t* lincons0)
{
  bool exact1 = itv_linexpr_set_ap_linexpr0(intern, 
					    &cons->linexpr,
					    p,
					    lincons0->linexpr0);
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

/* ********************************************************************** */
/* III. Evaluation of expressions  */
/* ********************************************************************** */

/* Evaluate an interval linear expression */
void ITVFUN(eval_itv_linexpr)(itv_internal_t* intern,
			      itv_t itv,
			      itv_t* p,
			      itv_linexpr_t* expr)
{
  size_t i;
  ap_dim_t dim;
  itv_ptr pitv;
  bool* peq;
  assert(p);

  itv_set(intern->eval_itv2, expr->cst);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
    if (*peq){
      if (bound_sgn(pitv->sup)!=0){
	itv_mul_bound(intern->eval_itv,
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
			      itv_t* p,
			      ap_linexpr0_t* expr)
{
  size_t i;
  ap_dim_t dim;
  ap_coeff_t* pcoeff;
  bool exact,res;
  assert(p);

  exact = itv_set_ap_coeff(intern, intern->eval_itv3, &expr->cst);
  res = exact;
  ap_linexpr0_ForeachLinterm(expr,i,dim,pcoeff){
    exact = itv_set_ap_coeff(intern,intern->eval_itv2,pcoeff);
    res = res && exact;
    bool eq = exact && pcoeff->discr==AP_COEFF_SCALAR;
    if (eq){
      if (bound_sgn(intern->eval_itv2->sup)!=0){
	itv_mul_bound(intern->eval_itv,
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

/* ********************************************************************** */
/* IV. Arithmetic */
/* ********************************************************************** */

void ITVFUN(linexpr_neg)(itv_linexpr_t* expr)
{
  size_t i;
  ap_dim_t dim;
  bool* peq;
  itv_ptr pitv;

  itv_neg(expr->cst,expr->cst);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
    itv_neg(pitv,pitv);
  }
  return;
}
void ITVFUN(linexpr_scale)(itv_internal_t* intern,
			   itv_linexpr_t* expr, itv_t coeff)
{
  size_t i;
  ap_dim_t dim;
  bool* peq;
  itv_ptr pitv;

  if (itv_is_zero(coeff)){
    itv_set(expr->cst,coeff);
    itv_linexpr_reinit(expr,0);
    return;
  }  
  itv_mul(intern,expr->cst,expr->cst,coeff);
  if (itv_is_top(expr->cst)){
    itv_linexpr_reinit(expr,0);
    return;
  }
  else {
    itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
      itv_mul(intern,pitv,pitv,coeff);
    }
  }
  return;
}
void ITVFUN(linexpr_div)(itv_internal_t* intern,
			 itv_linexpr_t* expr, itv_t coeff)
{
  size_t i;
  ap_dim_t dim;
  bool* peq;
  itv_ptr pitv;

  itv_div(intern,expr->cst,expr->cst,coeff);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
    itv_div(intern,pitv,pitv,coeff);
  }
  return;
}

itv_linexpr_t ITVFUN(linexpr_add)(itv_internal_t* intern,
				  itv_linexpr_t* exprA,
				  itv_linexpr_t* exprB)
{
  size_t i,j,k;
  itv_linexpr_t res;
  bool endA,endB;

  itv_linexpr_init(&res,exprA->size+exprB->size);
  i = j = k = 0;
  endA = endB = false;
  itv_add(res.cst,exprA->cst,exprB->cst);
  if (itv_is_top(res.cst))
    goto _itv_linexpr_add_return;
  while (true){
    endA = endA || (i==exprA->size) || exprA->linterm[i].dim == AP_DIM_MAX;
    endB = endB || (j==exprB->size) || exprB->linterm[j].dim == AP_DIM_MAX;
    if (endA && endB) 
      break;
    if (endA || exprB->linterm[i].dim < exprA->linterm[i].dim){
      itv_set(res.linterm[k].itv, exprB->linterm[j].itv);
      res.linterm[k].equality = exprB->linterm[j].equality;
      res.linterm[k].dim = exprB->linterm[j].dim;
      k++; j++;
    }
    else if (endB || exprA->linterm[i].dim < exprB->linterm[j].dim){
      itv_set(res.linterm[k].itv, exprA->linterm[i].itv);
      res.linterm[k].equality = exprA->linterm[i].equality;
      res.linterm[k].dim = exprA->linterm[i].dim;
      k++; i++;
    }
    else {
      itv_add(res.linterm[k].itv, exprA->linterm[i].itv,exprB->linterm[j].itv);
      res.linterm[k].equality = itv_is_point(intern,res.linterm[k].itv);
      res.linterm[k].dim = exprA->linterm[i].dim;
      if (!itv_is_zero(res.linterm[k].itv)){
	k++;
      }
      i++; j++;
    }
  }
 _itv_linexpr_add_return:
  itv_linexpr_reinit(&res,k);
  return res;
}
itv_linexpr_t ITVFUN(linexpr_sub)(itv_internal_t* intern,
				  itv_linexpr_t* exprA,
				  itv_linexpr_t* exprB)
{
  itv_linexpr_t res;
  itv_linexpr_neg(exprB);
  res = itv_linexpr_add(intern,exprA,exprB);
  itv_linexpr_neg(exprB);
  return res;
}
