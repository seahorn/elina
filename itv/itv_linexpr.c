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
void ITVFUN(itv_linexpr_init_set)(itv_linexpr_t* res, itv_linexpr_t* expr)
{
  size_t i;

  itv_init_set(res->cst,expr->cst);
  res->equality = expr->equality;
  res->linterm = expr->size ? malloc(expr->size*sizeof(itv_linterm_t)) : NULL;
  for (i=0;i<expr->size;i++){
    itv_linterm_init_set(&res->linterm[i],&expr->linterm[i]);
  }
  res->size = expr->size;
}
void ITVFUN(itv_linexpr_set)(itv_linexpr_t* res, itv_linexpr_t* expr)
{
  size_t i,size;

  if (res==expr) return;

  itv_set(res->cst,expr->cst);
  res->equality = expr->equality;
  for  (i=expr->size; i<res->size; i++){
    itv_linterm_clear(&res->linterm[i]);
  }
  res->linterm = realloc(res->linterm,expr->size*sizeof(itv_linterm_t));
  size = res->size < expr->size ? res->size : expr->size;
  for (i=0;i<size;i++){
    itv_linterm_set(&res->linterm[i],&expr->linterm[i]);
  }
  for (i=size; i<expr->size;i++){
    itv_linterm_init_set(&res->linterm[i],&expr->linterm[i]);
  }
  res->size = expr->size;
}

void ITVFUN(itv_linexpr_reinit)(itv_linexpr_t* expr, size_t size)
{
  size_t i;

  for  (i=size; i<expr->size; i++){
    itv_linterm_clear(&expr->linterm[i]);
  }
  expr->linterm = realloc(expr->linterm,size*sizeof(itv_linterm_t));
  for (i=expr->size;i<size;i++){
    itv_linterm_init(&expr->linterm[i]);
  }
  expr->size = size;
  return;
}
void ITVFUN(itv_linexpr_clear)(itv_linexpr_t* expr)
{
  size_t i;
  if (expr->linterm){
    for (i=0;i<expr->size;i++){
      itv_linterm_clear(&expr->linterm[i]);
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

void ITVFUN(itv_lincons_array_init)(itv_lincons_array_t* array, size_t size)
{
  size_t i;
  array->size = size;
  array->p = malloc(size*sizeof(itv_lincons_t));
  for (i=0; i<size; i++) itv_lincons_init(&array->p[i]);
}
void ITVFUN(itv_lincons_array_reinit)(itv_lincons_array_t* array, size_t size)
{
  size_t i;
  if (size == array->size) return;
  if (size < array->size){
    for (i=size; i<array->size; i++){
      itv_lincons_clear(&array->p[i]);
    }
    array->p = realloc(array->p,size*sizeof(itv_lincons_t));
  }
  else { /* size > array->size */
    array->p = realloc(array->p,size*sizeof(itv_lincons_t));
    for (i=array->size; i<size; i++){
      itv_lincons_init(&array->p[i]);
    }
  }
  array->size = size;
  return;
}
void ITVFUN(itv_lincons_array_clear)(itv_lincons_array_t* array)
{
  size_t i;
  for (i=0; i<array->size; i++) itv_lincons_clear(&array->p[i]);
  free(array->p);
  array->size = 0;
  array->p = NULL;
}
void ITVFUN(itv_lincons_array_fprint)(FILE* stream, itv_lincons_array_t* array, char** name)
{
  size_t i;
  fprintf(stream,"array of size %d_n",(int)array->size);
  for (i=0; i<array->size; i++){
    itv_lincons_fprint(stream,&array->p[i],name);
    fprintf(stream,"\n");
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

bool ITVFUN(itv_lincons_array_set_ap_lincons0_array)(itv_internal_t* intern,
						     itv_lincons_array_t* tcons,
						     ap_lincons0_array_t* tlincons0)
{
  size_t i;
  bool exact = true;

  itv_lincons_array_reinit(tcons,tlincons0->size);
  for (i=0; i<tlincons0->size; i++){
    exact = itv_lincons_set_ap_lincons0(intern,&tcons->p[i],&tlincons0->p[i]) && exact;
  }
  return exact;
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
/* III. Arithmetic */
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

void ITVFUN(itv_linexpr_add)(itv_internal_t* intern,
			     itv_linexpr_t* res, 
			     itv_linexpr_t* exprA,
			     itv_linexpr_t* exprB)
{
  size_t i,j,k;
  itv_linexpr_t expr;
  bool endA,endB;

  if (res==exprA || res==exprB){
    itv_linexpr_init(&expr,exprA->size+exprB->size);
  }
  else {
    expr = *res;
    itv_linexpr_reinit(&expr,exprA->size+exprB->size);
  }
  i = j = k = 0;
  endA = endB = false;
  itv_add(expr.cst,exprA->cst,exprB->cst);
  expr.equality = exprA->equality && exprB->equality && itv_is_point(intern,expr.cst);
  if (itv_is_top(expr.cst))
    goto _itv_linexpr_add_return;
  while (true){
    endA = endA || (i==exprA->size) || exprA->linterm[i].dim == AP_DIM_MAX;
    endB = endB || (j==exprB->size) || exprB->linterm[j].dim == AP_DIM_MAX;
    if (endA && endB)
      break;
    if (endA || (!endB && exprB->linterm[j].dim < exprA->linterm[i].dim)){
      itv_set(expr.linterm[k].itv, exprB->linterm[j].itv);
      expr.linterm[k].equality = exprB->linterm[j].equality;
      expr.linterm[k].dim = exprB->linterm[j].dim;
      k++; j++;
    }
    else if (endB || (!endA && exprA->linterm[i].dim < exprB->linterm[j].dim)){
      itv_set(expr.linterm[k].itv, exprA->linterm[i].itv);
      expr.linterm[k].equality = exprA->linterm[i].equality;
      expr.linterm[k].dim = exprA->linterm[i].dim;
      k++; i++;
    }
    else {
      itv_add(expr.linterm[k].itv, exprA->linterm[i].itv,exprB->linterm[j].itv);
      expr.linterm[k].equality =
	exprA->linterm[i].equality && exprB->linterm[j].equality && /* speed-up */
	itv_is_point(intern,expr.linterm[k].itv);
      expr.linterm[k].dim = exprA->linterm[i].dim;
      if (!itv_is_zero(expr.linterm[k].itv)){
	k++;
      }
      i++; j++;
    }
  }
 _itv_linexpr_add_return:
  itv_linexpr_reinit(&expr,k);
  if (res==exprA || res==exprB){
    itv_linexpr_clear(res);
  }
  *res = expr;
  return;
}
void ITVFUN(itv_linexpr_sub)(itv_internal_t* intern,
			     itv_linexpr_t* res,
			     itv_linexpr_t* exprA,
			     itv_linexpr_t* exprB)
{
  if (exprA==exprB){
    itv_linexpr_t expr;
    itv_linexpr_init_set(&expr,exprB);
    itv_linexpr_neg(&expr);
    itv_linexpr_add(intern,res,exprA,&expr);
    itv_linexpr_clear(&expr);
  }
  else {
    itv_linexpr_neg(exprB);
    itv_linexpr_add(intern,res,exprA,exprB);
    if (exprB!=res){
      itv_linexpr_neg(exprB);
    }
  }
}
