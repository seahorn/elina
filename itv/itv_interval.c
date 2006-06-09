/* ********************************************************************** */
/* itvinterval.c: abstract lattice of (unidimensional) intervals */
/* ********************************************************************** */

#include "itv_config.h"
#include "itv_interval.h"
#include "itv_internal.h"

/* ********************************************************************** */
/* Normalization and tests */
/* ********************************************************************** */

bool itv_interval_canonicalize(itv_internal_t* intern,
			       itv_interval_t a, bool integer)
{
  bool exc;

  if (integer){
    bound_floor(a->inf,a->inf);
    bound_floor(a->sup,a->sup);
  }
  if (bound_infty(a->inf) || bound_infty(a->sup)) return false;

  /* Check that it is not bottom */
  exc = false;
  num_neg(intern->itv_interval_canonicalize_num,bound_numref(a->inf));
  if (bound_cmp_num(a->sup,intern->itv_interval_canonicalize_num) < 0)
    exc = true;
  return exc;
}

/* ********************************************************************** */
/* Arithmetic operations */
/* ********************************************************************** */
void itv_interval_mul_num(itv_internal_t* intern,
			 itv_interval_t a, const itv_interval_t b, const num_t c)
{
  if (num_sgn(c)>=0){
    bound_mul_num(a->sup,b->sup,c);
    bound_mul_num(a->inf,b->inf,c);
  }
  else {
    if (a!=b){
      bound_mul_num(a->sup,b->inf,c);
      bound_mul_num(a->inf,b->sup,c);
    }
    else {
      bound_set(intern->itv_interval_muldiv_bound,a->sup);
      bound_mul_num(a->sup,a->inf,c);
      bound_mul_num(a->inf,intern->itv_interval_muldiv_bound,c);
    }
  }
}
void itv_interval_div_num(itv_internal_t* intern,
			 itv_interval_t a, const itv_interval_t b, const num_t c)
{
  if (num_sgn(c)>0){
    bound_div_num(a->sup,b->sup,c);
    bound_div_num(a->inf,b->inf,c);
  }
  else {
    if (a!=b){
      bound_div_num(a->sup,b->inf,c);
      bound_div_num(a->inf,b->sup,c);
    }
    else {
      bound_set(intern->itv_interval_muldiv_bound,a->sup);
      bound_div_num(a->sup,a->inf,c);
      bound_div_num(a->inf,intern->itv_interval_muldiv_bound,c);
    }
  }
}

void itv_interval_neg(itv_interval_t a, const itv_interval_t b)
{
  if (a!=b){
    bound_neg(a->inf,b->sup);
    bound_neg(a->sup,b->inf);
  } else {
    bound_swap(a->inf,a->sup);
    bound_neg(a->inf,a->inf);
    bound_neg(a->sup,a->sup);
  }
}

/* ********************************************************************** */
/* Printing */
/* ********************************************************************** */

void itv_interval_fprint(FILE* stream, const itv_interval_t a)
{
  num_t num;

  fprintf(stream,"[");
  if (bound_infty(a->inf))
    fprintf(stream,"-oo");
  else {
    num_init(num);
    num_neg(num,bound_numref(a->inf));
    num_fprint(stream,num);
    num_clear(num);
  }
  fprintf(stream,",");
  bound_fprint(stream,a->sup);
  fprintf(stream,"]");
}

int itv_interval_snprint(char* s, size_t size, const itv_interval_t a)
{
  num_t num;
  int count = 0;

  count += snprintf(s+count,size-count,"[");
  if (bound_infty(a->inf))
    count += snprintf(s+count,size-count,"-oo");
  else {
    num_init(num);
    num_neg(num,bound_numref(a->inf));
    count += num_snprint(s+count,size-count,num);
    num_clear(num);
  }
  count += snprintf(s+count,size-count,",");
  bound_snprint(s+count,size-count,a->sup);
  count += snprintf(s+count,size-count,"]");
  return count;
}

/* ********************************************************************** */
/* Conversions */
/* ********************************************************************** */

/* ====================================================================== */
/* Numbers */
/* ====================================================================== */

void num_set_scalar(num_t a, const ap_scalar_t* b, int round)
{
  assert(!ap_scalar_infty(b));
  switch(b->discr){
  case AP_SCALAR_MPQ:
    num_set_mpq(a,b->val.mpq);
    break;
  case AP_SCALAR_DOUBLE:
    num_set_double(a,b->val.dbl);
    break;
  }
}
void bound_set_scalar(num_t a, const ap_scalar_t* b, int round)
{
  if (ap_scalar_infty(b)){
    bound_set_infty(a);
  }
  else {
    num_set_scalar(bound_numref(a),b,round);
  }
}
void ap_scalar_set_num(ap_scalar_t* a, const num_t b, int round)
{
#if defined(NUM_DOUBLE)
  ap_scalar_reinit(a,AP_SCALAR_DOUBLE);
  a->val.dbl = b;
#else
  ap_scalar_reinit(a,AP_SCALAR_MPQ);
  mpq_set_num(a->val.mpq,b);
#endif
}
void ap_scalar_set_bound(ap_scalar_t* a, const bound_t b, int round)
{
  if (bound_infty(b))
    ap_scalar_set_infty(a,1);
  else {
    ap_scalar_reinit(a,AP_SCALAR_MPQ);
    mpq_set_num(a->val.mpq,bound_numref(b));
  }
}
/* infty * 0 = 0 , scalar != -oo */
void bound_mul_scalar(itv_internal_t* intern,
		      bound_t a, const bound_t b, const ap_scalar_t* c)
{
  if (bound_infty(b)){
    if (ap_scalar_sgn(c))
      bound_set_infty(a);
    else
      bound_set_int(a,0);
  }
  else if (ap_scalar_infty(c)){
    if (bound_sgn(b))
      bound_set_infty(a);
    else
      bound_set_int(a,0);
  }
  else{
    bound_set(a,b);
    num_set_scalar(intern->bound_mul_scalar_num,c,+1);
    num_mul(bound_numref(a),bound_numref(a),intern->bound_mul_scalar_num);
  }
}

/* ====================================================================== */
/* Intervals */
/* ====================================================================== */

void itv_interval_set_interval(itv_internal_t* intern,
			       itv_interval_t a, const ap_interval_t* b)
{
  ap_scalar_neg(intern->itv_interval_set_interval_scalar, b->inf);
  bound_set_scalar(a->inf,intern->itv_interval_set_interval_scalar,+1);
  bound_set_scalar(a->sup,b->sup,+1);
}
void ap_interval_set_itv_interval(ap_interval_t* a, const itv_interval_t b)
{
  ap_scalar_set_bound(a->inf,b->inf,+1);
  ap_scalar_neg(a->inf,a->inf);
  ap_scalar_set_bound(a->sup,b->sup,+1);
}

bool itv_interval_is_leq_interval(itv_internal_t* intern,
				  const itv_interval_t a, const ap_interval_t* b)
{
  bool res;

  ap_interval_set_itv_interval(intern->itv_interval_is_leq_interval_interval,a);
  res = ap_interval_is_leq(intern->itv_interval_is_leq_interval_interval,b);
  return res;
}

void itv_interval_mul_scalar(itv_internal_t* intern,
			     itv_interval_t a,
			     const itv_interval_t b, const ap_scalar_t* c)
{
  if (ap_scalar_sgn(c)>=0){
    bound_mul_scalar(intern,a->sup,b->sup,c);
    bound_mul_scalar(intern,a->inf,b->inf,c);
  }
  else {
    if (a!=b){
      bound_mul_scalar(intern,a->sup,b->inf,c);
      bound_mul_scalar(intern,a->inf,b->sup,c);
    }
    else {
      bound_set(intern->itv_interval_mul_scalar_bound,b->sup);
      bound_mul_scalar(intern,a->sup,b->inf,c);
      bound_mul_scalar(intern,a->inf,intern->itv_interval_mul_scalar_bound,c);
    }
  }
}

/* Assume that both intervals are either positive or negative */
void itv_interval_mul_interval2(itv_internal_t* intern,
				itv_interval_t a,
				const itv_interval_t b,
				const ap_interval_t* c)
{
  ap_scalar_t* cinf;

  if(a==b) abort();
  cinf = intern->itv_interval_mul_interval2_scalar;
  ap_scalar_neg(cinf,c->inf);
  if (bound_sgn(b->inf)<=0){
    /* b>=0 */
    if (ap_scalar_sgn(cinf)<=0){
      /* c>=0 */
      bound_mul_scalar(intern,a->inf,b->inf,cinf);
      bound_neg(a->inf,a->inf);
      bound_mul_scalar(intern,a->sup,b->sup,c->sup);
    }
    else {
      assert(ap_scalar_sgn(c->sup)<=0);
      /* c<=0 */
      bound_mul_scalar(intern,a->inf,b->sup,cinf);
      bound_mul_scalar(intern,a->sup,b->inf,c->sup);
      bound_neg(a->sup,a->sup);
    }
  }
  else {
    assert (bound_sgn(b->sup)<=0);
    /* b<=0 */
    if (ap_scalar_sgn(cinf)<=0){
      /* c>=0 */
      bound_mul_scalar(intern,a->inf,b->inf,c->sup);
      bound_mul_scalar(intern,a->sup,b->sup,cinf);
      bound_neg(a->sup,a->sup);
    }
    else {
      assert (ap_scalar_sgn(c->sup)<=0);
      /* c<=0 */
      bound_mul_scalar(intern,a->inf,b->sup,c->sup);
      bound_neg(a->inf,a->inf);
      bound_mul_scalar(intern,a->sup,b->inf,cinf);
    }
  }
}

/* Assume that first interval is either positive or negative */
void itv_interval_mul_interval1(itv_internal_t* intern,
				itv_interval_t a,
				const itv_interval_t b,
				const ap_interval_t* c)
{

  if (ap_scalar_sgn(c->inf)>=0 || ap_scalar_sgn(c->sup)<=0){
    itv_interval_mul_interval2(intern,a,b,c);
  }
  else {
    ap_scalar_set(intern->itv_interval_mul_interval1_interval->inf,
	       c->inf);
    ap_scalar_set_int(intern->itv_interval_mul_interval1_interval->sup,
		   0);
    itv_interval_mul_interval2(intern,
			       intern->itv_interval_mul_interval1_itvinterval,
			       b,
			       intern->itv_interval_mul_interval1_interval);
    ap_scalar_set_int(intern->itv_interval_mul_interval1_interval->inf,
		   0);
    ap_scalar_set(intern->itv_interval_mul_interval1_interval->sup,
	       c->sup);
    itv_interval_mul_interval2(intern,
			       a,
			       b,
			       intern->itv_interval_mul_interval1_interval);
    itv_interval_join(a,a,intern->itv_interval_mul_interval1_itvinterval);
  }
}

void itv_interval_mul_interval(itv_internal_t* intern,
			       itv_interval_t a,
			       const itv_interval_t b,
			       const ap_interval_t* c)
{
  if (ap_scalar_equal(c->inf,c->sup)){
    itv_interval_mul_scalar(intern,a,b,c->inf);
  }
  else if (bound_sgn(b->inf)<=0 || bound_sgn(b->sup)<=0){
    itv_interval_mul_interval1(intern,a,b,c);
  }
  else {
    bound_set(intern->itv_interval_mul_interval_itvinterval->inf,b->inf);
    bound_set_int(intern->itv_interval_mul_interval_itvinterval->sup,0);
    itv_interval_mul_interval2(intern,
			      intern->itv_interval_mul_interval_itvinterval2,
			      intern->itv_interval_mul_interval_itvinterval,
			      c);
    bound_set_int(intern->itv_interval_mul_interval_itvinterval->inf,0);
    bound_set(intern->itv_interval_mul_interval_itvinterval->sup,b->sup);
    itv_interval_mul_interval2(intern,
			      a,
			      intern->itv_interval_mul_interval_itvinterval,
			      c);
    itv_interval_join(a,a,intern->itv_interval_mul_interval_itvinterval2);
  }
}

/* ********************************************************************** */
/* Linear expressions */
/* ********************************************************************** */

void itv_linexpr_reinit(itv_linexpr_t* expr, size_t size)
{
  size_t i;
  expr->linterm = realloc(expr->linterm,size*sizeof(itv_linterm_t));
  
  for (i=expr->size;i<size;i++){
    num_init(expr->linterm[i].num);
  }
  for  (i=size; i<expr->size; i++){
    num_clear(expr->linterm[i].num);
  }
  expr->size = size;
  return;
}
void itv_linexpr_init(itv_linexpr_t* expr, size_t size)
{
  expr->linterm = NULL;
  expr->size = 0;
  itv_interval_init(expr->cst);
  itv_linexpr_reinit(expr,size);
}
void itv_linexpr_clear(itv_linexpr_t* expr)
{
  size_t i;
  if (expr->linterm){
    for (i=0;i<expr->size;i++){
      num_clear(expr->linterm[i].num);
    }
    free(expr->linterm);
    expr->linterm = NULL;
    expr->size = 0;
  }
  itv_interval_clear(expr->cst);
}
void itv_linexpr_set(itv_linexpr_t* a, const itv_linexpr_t* b)
{
  size_t i;
  itv_linexpr_reinit(a,b->size);
  if (b->linterm){
    for (i=0;i<b->size;i++){
      a->linterm[i].dim = b->linterm[i].dim;
      num_set(a->linterm[i].num,b->linterm[i].num);
    }
  }
  itv_interval_set(a->cst,b->cst);
}

void itv_linexpr_set_linexpr(itv_internal_t* intern,
			    itv_linexpr_t* expr, const ap_linexpr0_t* linexpr0)
{
  size_t i,k,size;
  ap_dim_t dim;
  ap_coeff_t* coeff;

  size=0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
    size++;
  }
  itv_linexpr_reinit(expr,size);
  k = 0;
  ap_linexpr0_ForeachLinterm(linexpr0,i,dim,coeff){
    assert(coeff->discr==AP_COEFF_SCALAR && 
	   ap_scalar_infty(coeff->val.scalar)==0);
    expr->linterm[k].dim = dim;
    num_set_scalar(expr->linterm[k].num,coeff->val.scalar,0);
    k++;
  }
  switch(linexpr0->cst.discr){
  case AP_COEFF_INTERVAL:
    itv_interval_set_interval(intern,expr->cst,linexpr0->cst.val.interval);
    break;
  case AP_COEFF_SCALAR:
    bound_set_scalar(expr->cst->sup,linexpr0->cst.val.scalar,+1);
    bound_neg(expr->cst->inf,expr->cst->sup);
    break;
  }
  return;
}
void itv_lincons_set_lincons(itv_internal_t* intern,
			    itv_lincons_t* cons, const ap_lincons0_t* lincons0)
{
  itv_linexpr_set_linexpr(intern, 
			 &cons->itvlinexpr,
			 lincons0->linexpr0);
  cons->constyp = lincons0->constyp;
}

void itv_lincons_clear(itv_lincons_t* cons)
{
  itv_linexpr_clear(&cons->itvlinexpr);
}
void itv_lincons_set(itv_lincons_t* a, const itv_lincons_t* b)
{
  itv_linexpr_set(&a->itvlinexpr,&b->itvlinexpr);
  a->constyp = b->constyp;
}

itv_lincons_array_t itv_lincons_array_make(size_t size)
{
  size_t i;
  itv_lincons_array_t array;
  
  array.p = malloc(size*sizeof(itv_lincons_t));
  array.size = size;
  for (i=0; i<size; i++){
    itv_linexpr_init(&array.p[i].itvlinexpr,0);
  }
  return array;
}

void itv_lincons_array_clear(itv_lincons_array_t* array)
{
  size_t i;
  if (array->p){
    for (i=0; i<array->size; i++){
      itv_linexpr_clear(&array->p[i].itvlinexpr);
    }
    free(array->p);
  }
  array->p = NULL;
  array->size = 0;
}
