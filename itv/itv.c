/* ********************************************************************** */
/* itv.c: (unidimensional) intervals */
/* ********************************************************************** */

#include "itv.h"

void itv_internal_init(itv_internal_t* intern)
{
  num_init(intern->canonicalize_num);
  bound_init(intern->muldiv_bound);
  bound_init(intern->mul_bound);
  itv_init(intern->mul_itv);
  itv_init(intern->mul_itv2);
  intern->ap_conversion_scalar = ap_scalar_alloc();
  bound_init(intern->ap_conversion_bound);
  itv_init(intern->eval_itv);
  itv_init(intern->eval_itv2);
}
void itv_internal_clear(itv_internal_t* intern)
{
  num_clear(intern->canonicalize_num);
  bound_clear(intern->muldiv_bound);
  bound_clear(intern->mul_bound);
  itv_clear(intern->mul_itv);
  itv_clear(intern->mul_itv2);
  ap_scalar_free(intern->ap_conversion_scalar); intern->ap_conversion_scalar = NULL;
  bound_clear(intern->ap_conversion_bound);
  itv_clear(intern->eval_itv);
  itv_clear(intern->eval_itv2);
}

itv_internal_t* itv_internal_alloc()
{
  itv_internal_t* intern = malloc(sizeof(itv_internal_t));
  itv_internal_init(intern);
  return intern;
}
void itv_internal_free(itv_internal_t* intern)
{
  itv_internal_clear(intern);
  free(intern);
}

/* ********************************************************************** */
/* Normalization and tests */
/* ********************************************************************** */

/* If integer is true, narrow the interval to integer bounds.
   In any case, return true if the interval is bottom
*/
bool itv_canonicalize(itv_internal_t* intern,
		      itv_t a, bool integer)
{
  bool exc;

  if (integer){
    bound_floor(a->inf,a->inf);
    bound_floor(a->sup,a->sup);
  }
  if (bound_infty(a->inf) || bound_infty(a->sup)) return false;

  /* Check that it is not bottom */
  exc = false;
  num_neg(intern->canonicalize_num,bound_numref(a->inf));
  if (bound_cmp_num(a->sup,intern->canonicalize_num) < 0)
    exc = true;
  return exc;
}

/* ********************************************************************** */
/* Arithmetic operations */
/* ********************************************************************** */

/* We assume no aliasing between 

   - an itv and a num or a bound, 
*/

void itv_mul_bound(itv_internal_t* intern,
		   itv_t a, const itv_t b, const bound_t c)
{
  assert (c!=a->inf && c!=a->sup && c!=b->inf && c!=b->sup);
  if (bound_sgn(c)>=0){
    bound_mul(a->sup,b->sup,c);
    bound_mul(a->inf,b->inf,c);
  }
  else {
    if (a!=b){
      bound_mul(a->sup,b->inf,c);
      bound_mul(a->inf,b->sup,c);
      bound_neg(a->sup,a->sup);
      bound_neg(a->inf,a->inf);
    }
    else {
      bound_neg(intern->muldiv_bound,a->sup);
      bound_mul(a->sup,a->inf,c);
      bound_neg(a->sup,a->sup);
      bound_mul(a->inf,intern->muldiv_bound,c);
    }
  }
}

void itv_div_bound(itv_internal_t* intern,
		   itv_t a, const itv_t b, const bound_t c)
{
  assert (c!=a->inf && c!=a->sup && c!=b->inf && c!=b->sup);
  if (bound_sgn(c)>0){
    bound_div(a->sup,b->sup,c);
    bound_div(a->inf,b->inf,c);
  }
  else {
    if (a!=b){
      bound_div(a->sup,b->inf,c);
      bound_div(a->inf,b->sup,c);
      bound_neg(a->sup,a->sup);
      bound_neg(a->inf,a->inf);
    }
    else {
      bound_neg(intern->muldiv_bound,a->sup);
      bound_div(a->sup,a->inf,c);
      bound_neg(a->sup,a->sup);
      bound_div(a->inf,intern->muldiv_bound,c);
    }
  }
}
void itv_sub(itv_t a, const itv_t b, const itv_t c)
{
  if (a!=c){
    bound_add(a->inf,b->inf,c->sup);
    bound_add(a->sup,b->sup,c->inf);
  } else {
    bound_swap(a->inf,a->sup);
    itv_add(a,b,c);
  }
}
void itv_neg(itv_t a, const itv_t b)
{
  if (a!=b){
    bound_set(a->inf,b->sup);
    bound_set(a->sup,b->inf);
  } else {
    bound_swap(a->inf,a->sup);
  }
}

/* ====================================================================== */
/* Multiplication */
/* ====================================================================== */

/* Assume that both intervals are positive */
static
void itv_mulpp(itv_internal_t* intern,
	       itv_t a,
	       const itv_t b,
	       const itv_t c)
{
  assert(bound_sgn(b->inf)<=0 && bound_sgn(c->inf)<=0);
  bound_mul(a->inf,b->inf,c->inf);
  bound_neg(a->inf,a->inf);
  bound_mul(a->sup,b->sup,c->sup);
}
/* Assume that both intervals are negative */
static
void itv_mulnn(itv_internal_t* intern,
	       itv_t a,
	       const itv_t b,
	       const itv_t c)
{
  assert(bound_sgn(b->sup)<=0 && bound_sgn(c->sup)<=0);
  bound_mul(a->inf,b->inf,c->inf);
  bound_mul(a->sup,b->sup,c->sup);
  bound_neg(a->sup,a->sup);
  bound_swap(a->inf,a->sup);
}
/* Assume that b is positive and c negative */
static
void itv_mulpn(itv_internal_t* intern,
	       itv_t a,
	       const itv_t b,
	       const itv_t c)
{
  assert(bound_sgn(b->inf)<=0 && bound_sgn(c->sup)<=0);
  if (a!=b && a!=c){
    bound_mul(a->sup,b->inf,c->sup);
    bound_neg(a->sup,a->sup);
    bound_mul(a->inf,b->sup,c->inf);
  }
  else {
    bound_mul(intern->mul_bound,b->inf,c->sup);
    bound_mul(a->inf,b->sup,c->inf);
    bound_neg(a->sup,intern->mul_bound);
  }
}
/* Assume that interval c is positive */
static
void itv_mulp(itv_internal_t* intern,
	      itv_t a,
	      const itv_t b,
	      const itv_t c)
{
  assert(bound_sgn(c->inf)<=0);

  if (bound_sgn(b->inf)<=0){
    /* b is positive */
    itv_mulpp(intern,a,b,c);
  }
  else if (bound_sgn(b->sup)<=0){
    /* b is negative */
    itv_mulpn(intern,a,c,b);
  }
  else {
    /* 0 is in the middle of b: one multiplies b by c->sup */
    bound_mul(a->inf,b->inf,c->sup);
    bound_mul(a->sup,b->sup,c->sup);
  }
}
/* Assume that interval c is negative */
static
void itv_muln(itv_internal_t* intern,
	      itv_t a,
	      const itv_t b,
	      const itv_t c)
{
  assert(bound_sgn(c->sup)<=0);

  if (bound_sgn(b->inf)<=0){
    /* b is positive */
    itv_mulpn(intern,a,b,c);
  }
  else if (bound_sgn(b->sup)<=0){
    /* b is negative */
    itv_mulnn(intern,a,b,c);
  }
  else {
    /* 0 is in the middle of b: one multiplies b by c->inf */
    bound_mul(a->sup,b->sup,c->inf);
    bound_mul(a->inf,b->inf,c->inf);
    bound_swap(a->inf,a->sup);
  }
}

void itv_mul(itv_internal_t* intern,
	     itv_t a,
	     const itv_t b,
	     const itv_t c)
{
  if (bound_sgn(c->inf)<=0){
    /* c is positive, */
    itv_mulp(intern,a,b,c);
  }
  else if (bound_sgn(c->sup)<=0){
    /* c is negative */
    itv_muln(intern,a,b,c);
  }
  else if (bound_sgn(b->inf)<=0){
    /* b is positive, */
    itv_mulp(intern,a,c,b);
  }
  else if (bound_sgn(c->sup)<=0){
    /* b is negative */
    itv_muln(intern,a,c,b);
  }
  else {
    /* divide c */
    bound_set(intern->mul_itv->inf,c->inf);
    bound_set_int(intern->mul_itv->sup,0);
    itv_muln(intern,intern->mul_itv2,b,intern->mul_itv);
 
    bound_set_int(intern->mul_itv->inf,0);
    bound_set(intern->mul_itv->sup,c->sup);
    itv_mulp(intern,a,b,intern->mul_itv);
    
    itv_join(a,a,intern->mul_itv2);
  }
}

/* ********************************************************************** */
/* Printing */
/* ********************************************************************** */

void itv_fprint(FILE* stream, const itv_t a)
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

int itv_snprint(char* s, size_t size, const itv_t a)
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

bool itv_set_ap_scalar(itv_internal_t* intern,
		       itv_t a, const ap_scalar_t* b)
{
  assert (ap_scalar_infty(b)==0);
  bool exact = bound_set_ap_scalar(a->sup,b);
  if (exact){
    bound_neg(a->inf,a->sup);
    return true;
  }
  else {
    ap_scalar_neg(intern->ap_conversion_scalar, b);
    bound_set_ap_scalar(a->inf,intern->ap_conversion_scalar);
    return false;
  }
}
bool itv_set_ap_interval(itv_internal_t* intern,
			 itv_t a, const ap_interval_t* b)
{
  ap_scalar_neg(intern->ap_conversion_scalar, b->inf);
  bool b1 = bound_set_ap_scalar(a->inf,intern->ap_conversion_scalar);
  bool b2 = bound_set_ap_scalar(a->sup,b->sup);
  return b1 && b2;
}
bool itv_set_ap_coeff(itv_internal_t* intern,
		      itv_t itv, const ap_coeff_t* coeff)
{
  switch(coeff->discr){
  case AP_COEFF_SCALAR:
    return itv_set_ap_scalar(intern,itv,coeff->val.scalar);
    break;
  case AP_COEFF_INTERVAL:
    return itv_set_ap_interval(intern, itv, coeff->val.interval);
    break;
  default:
    abort();
  }
}

bool ap_interval_set_itv(itv_internal_t* intern,
			 ap_interval_t* a, const itv_t b)
{
  bool b1 = ap_scalar_set_bound(a->inf,b->inf);
  ap_scalar_neg(a->inf,a->inf);
  bool b2 = ap_scalar_set_bound(a->sup,b->sup);
  return b1 && b2;
}
bool ap_coeff_set_itv(itv_internal_t* intern,
		      ap_coeff_t* a, const itv_t b)
{
  bool exact;
  if (bound_infty(b->inf) || bound_infty(b->sup))
    goto ap_coeff_set_itv_default;
  bound_neg(intern->ap_conversion_bound, b->inf);
  if (!bound_equal(intern->ap_conversion_bound, b->sup))
    goto ap_coeff_set_itv_default;
  exact = ap_scalar_set_bound(intern->ap_conversion_scalar, b->sup);
  if (exact){
    ap_coeff_set_scalar(a, intern->ap_conversion_scalar);
    return true;
  }
  else {
    ap_coeff_reinit(a,AP_COEFF_INTERVAL, 
		    intern->ap_conversion_scalar->discr);
    ap_scalar_set(a->val.interval->sup, intern->ap_conversion_scalar);
    ap_scalar_set_bound(a->val.interval->inf, intern->ap_conversion_bound);
    return false;
  }
 ap_coeff_set_itv_default:
  ap_coeff_reinit(a,AP_COEFF_INTERVAL, 
#if defined(NUM_NUMFLT)
		  AP_SCALAR_DOUBLE
#else
		  AP_SCALAR_MPQ
#endif
		  );
  return ap_interval_set_itv(intern,a->val.interval,b);
}

