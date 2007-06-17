/* ********************************************************************** */
/* itv_linexpr.c: */
/* ********************************************************************** */

#include "itv_linexpr.h"

/* ********************************************************************** */
/* I. Constructor and Destructor */
/* ********************************************************************** */

void ITVFUN(itv_linexpr_init)(itv_linexpr_t* expr, size_t size)
{
  expr->linterm = NULL;
  expr->size = 0;
  itv_init(expr->cst);
  expr->equality = true;
  itv_linexpr_reinit(expr,size);
}
void ITVFUN(itv_linexpr_reinit)(itv_linexpr_t* expr, size_t size)
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
void ITVFUN(itv_linexpr_clear)(itv_linexpr_t* expr)
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

void ITVFUN(itv_linexpr_fprint)(FILE* stream, itv_linexpr_t* expr, char** name)
{
  itv_linexpr_t* e;
  size_t i;
  ap_dim_t dim;
  itv_ptr pitv;
  bool* peq;
  itv_fprint(stream,expr->cst);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq) {
    printf(" + ");
    itv_fprint(stream,pitv);
    if (name) fprintf(stream,"%s",name[dim]);
    else fprintf(stream,"x%lu",(unsigned long)dim);
  }
}

void ITVFUN(itv_lincons_fprint)(FILE* stream, itv_lincons_t* cons, char** name)
{
  ITVFUN(itv_linexpr_fprint)(stream,&cons->linexpr,name);
  fprintf(stream,
	  cons->constyp == AP_CONS_EQ || cons->constyp == AP_CONS_EQMOD ?
	  " = 0" :
	  ( cons->constyp == AP_CONS_SUPEQ ?
	    " >= 0" :
	    (cons->constyp == AP_CONS_SUP ?
	     " > 0" :
	     "\"ERROR in itv_lincons_fprint\"")));
  if (cons->constyp == AP_CONS_EQMOD){
    fprintf(stream," mod ");
    num_fprint(stream,cons->num);
  }
}

/* ********************************************************************** */
/* II. Conversions from and to APRON datatypes */
/* ********************************************************************** */

bool ITVFUN(itv_linexpr_set_ap_linexpr0)(itv_internal_t* intern,
					 itv_linexpr_t* expr,
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
  itv_linexpr_reinit(expr,size);
  exact = itv_set_ap_coeff(intern, expr->cst, &linexpr0->cst);
  expr->equality = exact && linexpr0->cst.discr==AP_COEFF_SCALAR;
  res = exact;
  k = 0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
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
  itv_linexpr_reinit(expr,k);
  return res;
}

bool ITVFUN(itv_lincons_set_ap_lincons0)(itv_internal_t* intern,
					 itv_lincons_t* cons, 
					 ap_lincons0_t* lincons0)
{
  bool exact1 = itv_linexpr_set_ap_linexpr0(intern, 
					    &cons->linexpr,
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

void ITVFUN(ap_linexpr0_set_itv_linexpr)(itv_internal_t* intern,
					 ap_linexpr0_t** plinexpr0,
					 itv_linexpr_t* linexpr)
{
  ap_linexpr0_t* linexpr0;
  size_t i,k;
  ap_dim_t dim;
  bool* peq;
  itv_ptr pitv;

  linexpr0 = *plinexpr0;
  if (linexpr0==NULL){
    linexpr0 = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,linexpr->size);
  }
  else {
    ap_linexpr0_realloc(linexpr0,linexpr->size);
  }
  ap_coeff_set_itv(intern,&linexpr0->cst,linexpr->cst);
  k = 0;
  itv_linexpr_ForeachLinterm(linexpr,i,dim,pitv,peq){
    linexpr0->p.linterm[k].dim = dim;
    ap_coeff_set_itv(intern,&linexpr0->p.linterm[k].coeff,pitv);
    k++;
  }
  *plinexpr0 = linexpr0;
  return;
}

void ITVFUN(ap_lincons0_set_itv_lincons)(itv_internal_t* intern,
					 ap_lincons0_t* plincons0,
					 itv_lincons_t* lincons)
{
  ap_linexpr0_set_itv_linexpr(intern,&plincons0->linexpr0,&lincons->linexpr);
  plincons0->constyp = lincons->constyp;
  if (num_sgn(lincons->num)){
    if (plincons0->scalar==NULL){
      plincons0->scalar = ap_scalar_alloc();
    }
    ap_scalar_set_num(plincons0->scalar,lincons->num);
  }
  else {
    if (plincons0->scalar!=NULL){
      ap_scalar_free(plincons0->scalar);
      plincons0->scalar = NULL;
    }
  }
}



/* ********************************************************************** */
/* III. Evaluation of expressions  */
/* ********************************************************************** */

/* Evaluate an interval linear expression */
void ITVFUN(itv_eval_linexpr)(itv_internal_t* intern,
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
bool ITVFUN(itv_eval_ap_linexpr0)(itv_internal_t* intern,
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
/* IV. (Quasi)linearisation */
/* ********************************************************************** */

bool ITVFUN(itv_linexpr_quasilinearize)(itv_internal_t* intern, itv_linexpr_t* linexpr, itv_t* titv)
{
  size_t i,k;
  ap_dim_t dim;
  itv_ptr itv;
  bool* peq;

  k = 0;
  itv_linexpr_ForeachLinterm(linexpr,i,dim,itv,peq){
    if (*peq == false){
      /* Compute the middle of the interval */
      if (bound_infty(itv->inf)){
	if (bound_infty(itv->sup))
	  num_set_int(intern->quasi_num,0);
	else 
	  num_set(intern->quasi_num,
		  bound_numref(itv->sup));
      }
      else if (bound_infty(itv->sup))
	num_neg(intern->quasi_num,
		bound_numref(itv->inf));
      else {
	num_sub(intern->quasi_num,
		bound_numref(itv->sup),
		bound_numref(itv->inf));
	num_div_2(intern->quasi_num,
		  intern->quasi_num);
      }
      /* Residue (interval-middle) */
      itv_sub_num(intern->eval_itv2,itv,intern->quasi_num);
      /* Multiplication */
      itv_mul(intern,
	      intern->eval_itv,
	      intern->eval_itv2,
	      titv[dim]);
      /* Addition to the constant coefficient */
      itv_add(linexpr->cst,linexpr->cst,intern->eval_itv);
      if (itv_is_top(linexpr->cst)){
	k = 0;
	break;
      }
      /* Addition of the linear term */
      if (num_sgn(intern->quasi_num)!=0){
	linexpr->linterm[k].equality = true;
	linexpr->linterm[k].dim = dim;
	itv_set_num(linexpr->linterm[k].itv,intern->quasi_num);
	k++;
      }
    }
    else k++;
  }
  itv_linexpr_reinit(linexpr,k);
#if defined(NUM_FLOAT) || defined(NUM_DOUBLE) || defined(NUM_LONGDOUBLE)
  return false;
#else
  return true;
#endif
}

bool ITVFUN(itv_lincons_quasilinearize)(itv_internal_t* intern, itv_lincons_t* lincons, itv_t* titv)
{
  return itv_linexpr_quasilinearize(intern,&lincons->linexpr,titv);
}

/* ********************************************************************** */
/* V. Arithmetic */
/* ********************************************************************** */

void ITVFUN(itv_linexpr_neg)(itv_linexpr_t* expr)
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
void ITVFUN(itv_linexpr_scale)(itv_internal_t* intern,
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
      *peq = *peq /* speed-up */ && itv_is_point(intern,pitv);
    }
  }
  return;
}
void ITVFUN(itv_linexpr_div)(itv_internal_t* intern,
			     itv_linexpr_t* expr, itv_t coeff)
{
  size_t i;
  ap_dim_t dim;
  bool* peq;
  itv_ptr pitv;

  itv_div(intern,expr->cst,expr->cst,coeff);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
    itv_div(intern,pitv,pitv,coeff);
    *peq = *peq /* speed-up */ && itv_is_point(intern,pitv);
  }
  return;
}

itv_linexpr_t ITVFUN(itv_linexpr_add)(itv_internal_t* intern,
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
    if (endA || (!endB && exprB->linterm[i].dim < exprA->linterm[i].dim)){
      itv_set(res.linterm[k].itv, exprB->linterm[j].itv);
      res.linterm[k].equality = exprB->linterm[j].equality;
      res.linterm[k].dim = exprB->linterm[j].dim;
      k++; j++;
    }
    else if (endB || (!endA && exprA->linterm[i].dim < exprB->linterm[j].dim)){
      itv_set(res.linterm[k].itv, exprA->linterm[i].itv);
      res.linterm[k].equality = exprA->linterm[i].equality;
      res.linterm[k].dim = exprA->linterm[i].dim;
      k++; i++;
    }
    else {
      itv_add(res.linterm[k].itv, exprA->linterm[i].itv,exprB->linterm[j].itv);
      res.linterm[k].equality = 
	exprA->linterm[i].equality && exprB->linterm[i].equality && /* speed-up */
	itv_is_point(intern,res.linterm[k].itv);
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
itv_linexpr_t ITVFUN(itv_linexpr_sub)(itv_internal_t* intern,
				      itv_linexpr_t* exprA,
				      itv_linexpr_t* exprB)
{
  itv_linexpr_t res;
  itv_linexpr_neg(exprB);
  res = itv_linexpr_add(intern,exprA,exprB);
  itv_linexpr_neg(exprB);
  return res;
}
