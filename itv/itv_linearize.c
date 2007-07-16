/* ********************************************************************** */
/* itv_linearize.c: */
/* ********************************************************************** */

#include "itv_linearize.h"

#define LOGDEBUG 0

/* ********************************************************************** */
/* I. Evaluation of interval linear expressions  */
/* ********************************************************************** */

/* Evaluate an ITV interval linear expression */
void ITVFUN(itv_eval_linexpr)(itv_internal_t* intern,
			      itv_t itv,
			      itv_linexpr_t* expr,
			      itv_t* env)
{
  size_t i;
  ap_dim_t dim;
  itv_ptr pitv;
  bool* peq;
  assert(env);

  itv_set(intern->eval_itv2, expr->cst);
  itv_linexpr_ForeachLinterm(expr,i,dim,pitv,peq){
    if (*peq){
      if (bound_sgn(pitv->sup)!=0){
	itv_mul_bound(intern->eval_itv,
		      env[dim],
		      pitv->sup);
	itv_add(intern->eval_itv2, intern->eval_itv2, intern->eval_itv);
      }
    }
    else {
      itv_mul(intern,
	      intern->eval_itv,
	      env[dim],
	      pitv);
      itv_add(intern->eval_itv2, intern->eval_itv2, intern->eval_itv);
    }
    if (itv_is_top(intern->eval_itv2))
      break;
  }
  itv_set(itv,intern->eval_itv2);
}

/* Evaluate an APRON interval linear expression */
bool ITVFUN(itv_eval_ap_linexpr0)(itv_internal_t* intern,
				  itv_t itv,
				  ap_linexpr0_t* expr,
				  itv_t* env)
{
  size_t i;
  ap_dim_t dim;
  ap_coeff_t* pcoeff;
  bool exact,res;
  assert(env);

  exact = itv_set_ap_coeff(intern, intern->eval_itv3, &expr->cst);
  res = exact;
  ap_linexpr0_ForeachLinterm(expr,i,dim,pcoeff){
    exact = itv_set_ap_coeff(intern,intern->eval_itv2,pcoeff);
    res = res && exact;
    bool eq = exact && pcoeff->discr==AP_COEFF_SCALAR;
    if (eq){
      if (bound_sgn(intern->eval_itv2->sup)!=0){
	itv_mul_bound(intern->eval_itv,
		      env[dim],
		      intern->eval_itv2->sup);
	itv_add(intern->eval_itv3, intern->eval_itv3, intern->eval_itv);
      }
    }
    else {
      itv_mul(intern,
	      intern->eval_itv,
	      env[dim],
	      intern->eval_itv2);
      itv_add(intern->eval_itv3, intern->eval_itv3, intern->eval_itv);
    }
    if (itv_is_top(intern->eval_itv3))
      break;
  }
  itv_set(itv,intern->eval_itv3);
  return res;
}

/* constant constraint */
tbool_t ITVFUN(itv_eval_cstlincons)(itv_internal_t* intern, itv_lincons_t* lincons)
{
  tbool_t res;
  itv_ptr itv = lincons->linexpr.cst;
  bool equality = lincons->linexpr.equality;
  assert(lincons->linexpr.size==0);

  switch (lincons->constyp){
  case AP_CONS_EQ:
    res =
      (equality &&
       bound_sgn(itv->sup)==0) ?
      tbool_true :
      tbool_top;
    break;
  case AP_CONS_DISEQ:
    res =
      (bound_sgn(itv->inf)<0 ||
       bound_sgn(itv->sup)<0) ?
      tbool_true :
      tbool_top;
    break;
  case AP_CONS_SUPEQ:
    res =
      (bound_sgn(itv->inf)<=0) ?
      tbool_true :
      tbool_top;
    break;
  case AP_CONS_SUP:
    res =
      (bound_sgn(itv->inf)<0) ?
      tbool_true :
      tbool_top;
    break;
  case AP_CONS_EQMOD:
    if (equality){
      if (bound_sgn(itv->sup)==0){
	res = tbool_true;
      }
      else if (num_cmp_int(lincons->num,0)){
	res = (bound_sgn(itv->sup)==0) ? tbool_true : tbool_top;
      }
      else {
#if defined(NUM_NUMRAT)
	numrat_t numrat;
	numrat_init(numrat);
	numrat_div(numrat,bound_numref(itv->sup),lincons->num);
	if (numint_cmp_int(numrat_denref(numrat),1)==0){
	  res = tbool_true;
	}
	else {
	  res = tbool_top;
	}
	numrat_clear(numrat);
#elif defined(NUM_NUMINT)
	numint_t numint;
	numint_init(numint);
	numint_mod(numint,bound_numref(itv->sup),lincons->num);
	if (numint_sgn(numint)==0){
	  res = tbool_true;
	}
	else {
	  res = tbool_top;
	}
	numint_clear(numint);
#else
	res = tbool_top;
#endif
      }
    }
    else {
      res = tbool_top;
    }
    break;
  default:
    abort();
  }
  return res;
}

/* ********************************************************************** */
/* II. (Quasi)linearisation of interval linear expressions */
/* ********************************************************************** */

bool ITVFUN(itv_quasilinearize_linexpr)(itv_internal_t* intern, itv_linexpr_t* linexpr, itv_t* env)
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
	      env[dim]);
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

bool ITVFUN(itv_quasilinearize_lincons)(itv_internal_t* intern, itv_lincons_t* lincons, itv_t* env)
{
  return itv_quasilinearize_linexpr(intern,&lincons->linexpr,env);
}


bool ITVFUN(itv_quasilinearize_lincons_array)(itv_internal_t* intern, itv_lincons_array_t* tlincons, itv_t* env, bool linearize)
{
  size_t i;
  bool exact = true;

  for (i=0; i<tlincons->size; i++){
    exact = itv_quasilinearize_linexpr(intern,&tlincons->p[i].linexpr,env) && exact;
  }
  if (linearize)
    itv_linearize_lincons_array(intern,tlincons);
  return exact;
}

/* Transform sets of quasilinear constraint as follows:
   e.x + [a,b] >= 0 ==> e.x + b >= 0
   e.x + [a,b] > 0  ==> e.x + b > 0
   e.x + [a,b] = 0  ==> e.x + b >= 0 and e.x + a <= 0 added at the end
   e.x + [a,b] = 0 mod k ==> unchanged

   Also remove (some) trivially true constraints e.x + oo >= 0

*/
static void itv_lincons_select_sup(itv_lincons_t* cons)
{
  bound_neg(cons->linexpr.cst->inf,cons->linexpr.cst->sup);
  cons->linexpr.equality = true;
}
static void itv_lincons_select_inf(itv_lincons_t* cons)
{
  size_t i;
  bound_neg(cons->linexpr.cst->inf,cons->linexpr.cst->inf);
  bound_neg(cons->linexpr.cst->sup,cons->linexpr.cst->inf);
  cons->linexpr.equality = true;
  for (i=0;i<cons->linexpr.size;i++){
    itv_neg(cons->linexpr.linterm[i].itv,cons->linexpr.linterm[i].itv);
  }
}

void ITVFUN(itv_linearize_lincons_array)(itv_internal_t* intern, itv_lincons_array_t* array)
{
  long index,last,lastorg;

  /* One first remove unsatisfiable constraints */
  index = 0;
  last = (long)array->size-1;
  while (index <= last && last!=0){
    itv_lincons_t* cons = &array->p[index];
    if (!cons->linexpr.equality){
      itv_ptr cst = cons->linexpr.cst;
      bool sup = bound_infty(cst->sup);
      bool toremove = false;
      switch (cons->constyp){
      case AP_CONS_EQ:
	{
	  bool inf = bound_infty(cst->inf);
	  toremove = inf && sup;
	}
	break;
      case AP_CONS_SUPEQ:
      case AP_CONS_SUP:
	toremove = sup;
	break;
      default:
	break;
      }
      if (toremove){
	itv_lincons_exch(&array->p[index],&array->p[last]);
	last--;
      }
      else {
	index++;
      }
    }
  }
  /* One now remove intervals when we can */
  lastorg = last;
  for (index=0; index<lastorg; index++){
    itv_lincons_t* cons = &array->p[index];
    if (!cons->linexpr.equality){
      itv_ptr cst = cons->linexpr.cst;
      bool sup = !bound_infty(cst->sup);
      switch (cons->constyp){
      case AP_CONS_EQ:
	{
	  bool inf = !bound_infty(cst->inf);
	  if (inf && sup){
	    last++;
	    if (last>=array->size){
	      itv_lincons_array_reinit(array,1+array->size/4);
	    }
	    /* be cautious: cons and cst may be invalid now */
	    itv_lincons_set(&array->p[last],&array->p[index]);
	    array->p[index].constyp = AP_CONS_SUPEQ;
	    array->p[last].constyp  = AP_CONS_SUPEQ;
	    itv_lincons_select_sup(&array->p[index]);
	    itv_lincons_select_inf(&array->p[last]);
	  }
	  else if (inf){
	    array->p[index].constyp = AP_CONS_SUPEQ;
	    itv_lincons_select_inf(&array->p[index]);
	  }
	  else if (sup){
	    array->p[index].constyp = AP_CONS_SUPEQ;
	    itv_lincons_select_sup(&array->p[index]);
	  }
	  else
	    assert(false);
	}
	break;
      case AP_CONS_SUPEQ:
      case AP_CONS_SUP:
	if (sup){
	  array->p[index].constyp = AP_CONS_SUPEQ;
	  itv_lincons_select_sup(&array->p[index]);
	}
	else {
	  assert(false);
	}
	break;
      default:
	break;
      }
    }
  }
  itv_lincons_array_reinit(array,last+1);
}



/* ********************************************************************** */
/* III. Evaluation of tree expressions  */
/* ********************************************************************** */

/* General rounding */
static void itv_round(itv_t res, itv_t arg,
		      ap_texpr_rtype_t t, ap_texpr_rdir_t d)
{
  switch (t) {

  case AP_RTYPE_REAL:
    if (&res!=&arg) itv_set(res,arg);
    break;

  case AP_RTYPE_INT:
    switch (d) {
    case AP_RDIR_ZERO:
      itv_trunc(res,arg);
      break;
    case AP_RDIR_UP:
      itv_ceil(res,arg);
      break;
    case AP_RDIR_DOWN:
      itv_floor(res,arg);
      break;
    case AP_RDIR_RND:
    case AP_RDIR_NEAREST: /* 'to nearest' case could be improved */
      itv_to_int(res,arg);
      break;
    default:
      assert(0);
    }
    break;

  case AP_RTYPE_SINGLE:
    /* directed rounding cases (+oo, -oo, 0) could be improved */
    itv_to_float(res,arg);
    break;

  case AP_RTYPE_QUAD:     /* 'round to quad' could be improved */
  case AP_RTYPE_EXTENDED: /* 'round to extended' could be improved */
  case AP_RTYPE_DOUBLE:
    /* directed rounding cases (+oo, -oo, 0) could be improved */
    itv_to_double(res,arg);
    break;

  default:
    assert(0);
  }
}

/* Evaluates node into interval res, assuming operator arguments are arg1 and
   (for binary operator) arg2
*/
static void
itv_eval_ap_texpr0_node(itv_internal_t* intern,
			ap_texpr0_node_t* n,
			itv_t res, itv_t arg1, itv_t arg2)
{
  switch (n->op) {
  case AP_TEXPR_NEG:
    itv_neg(res, arg1);
    return; /* no rounding */
  case AP_TEXPR_CAST:
    itv_set(res, arg1);
    break;
  case AP_TEXPR_SQRT:
    itv_sqrt(intern, res, arg1);
    break;
  case AP_TEXPR_ADD:
    itv_add(res, arg1, arg2);
    break;
  case AP_TEXPR_SUB:
    itv_sub(res, arg1, arg2);
    break;
  case AP_TEXPR_MUL:
    itv_mul(intern, res, arg1, arg2);
    break;
  case AP_TEXPR_DIV:
    itv_div(intern, res, arg1, arg2);
    break;
  case AP_TEXPR_MOD:
    itv_mod(intern, res, arg1, arg2, n->type==AP_RTYPE_INT);
    return; /* no rounding */
  default:
    assert(0);
  }
  itv_round(res,res,n->type,n->dir);
}

/* evaluates expr into intervalres,
   assuming env maps dimensions to interval values */
void ITVFUN(itv_eval_ap_texpr0)(itv_internal_t* intern,
				itv_t res,
				ap_texpr0_t* expr,
				itv_t* env)
{
  if (!expr) {
    itv_set_bottom(res);
    return;
  }

  switch(expr->discr){
  case AP_TEXPR_CST:
    itv_set_ap_coeff(intern,res,&expr->val.cst);
    break;
  case AP_TEXPR_DIM:
    itv_set(res,env[expr->val.dim]);
    break;
  case AP_TEXPR_NODE:
    if (expr->val.node->exprB) {
      /* binary */
      itv_t x;
      itv_init(x);
      itv_eval_ap_texpr0(intern,x,expr->val.node->exprA,env);
      itv_eval_ap_texpr0(intern,res,expr->val.node->exprB,env);
      itv_eval_ap_texpr0_node(intern,expr->val.node,res,x,res);
      itv_clear(x);
    }
    else {
      /* unary */
      itv_eval_ap_texpr0(intern,res,expr->val.node->exprA,env);
      itv_eval_ap_texpr0_node(intern,expr->val.node,res,res,res);
    }
    break;
  default:
    assert(false);
  }
}

/* ====================================================================== */
/* IV. Linearization of tree expressions */
/* ====================================================================== */

/* transform in-place
	[A0,B0] + sum Xi [Ai,Bi]
   into
       ([A0,B0] + [A0,B0][-ulp,ulp]) + [-mf,mf] +
       sum Xi ([Ai,Bi] + [Ai,Bi][-ulp,ulp])

   i.e., add a relative error of magnitude ulp as an interval linear form
*/
static void
itv_linexpr_round_float_lin(itv_internal_t* intern,
			    itv_linexpr_t* l /* in/out */,
			    float_const* f)
{
  size_t i;
  ap_dim_t dim;
  bool* peq;
  itv_ptr pitv;
  itv_magnitude(intern->linear_bound,l->cst);
  bound_mul(intern->linear_bound,intern->linear_bound,f->ulp->sup);
  bound_add(intern->linear_bound,intern->linear_bound,f->min->sup);
  itv_enlarge_bound(l->cst,l->cst,intern->linear_bound);
  l->equality = false;
  itv_linexpr_ForeachLinterm(l,i,dim,pitv,peq) {
    *peq = false;
    itv_magnitude(intern->linear_bound,pitv);
    bound_mul(intern->linear_bound,intern->linear_bound,f->ulp->sup);
    itv_enlarge_bound(pitv,pitv,intern->linear_bound);
  }
}


/* transform in-place
	[A0,B0] + sum Xi [Ai,Bi]
   into
       [A0,B0] + ([A0,B0] + sum [min Xi,max Xi][Ai,Bi]) [-ulp,ulp] + [-mf,mf]
	sum Xi [Ai,Bi]

   i.e., adds a relative error of magnitude ulp flattened into an interval
*/
static void
itv_linexpr_round_float(itv_internal_t* intern,
			itv_linexpr_t* l /* in/out */,
			float_const* f, itv_t* env)
{
  size_t i;
  ap_dim_t dim;
  bool* peq;
  itv_ptr pitv;
  itv_magnitude(intern->linear_bound,l->cst);
  l->equality = false;
  itv_linexpr_ForeachLinterm(l,i,dim,pitv,peq) {
    *peq = false;
    itv_magnitude(intern->linear_bound2,pitv);
    itv_magnitude(intern->linear_bound3,env[dim]);
    bound_mul(intern->linear_bound2,intern->linear_bound2,intern->linear_bound3);
    bound_add(intern->linear_bound,intern->linear_bound,intern->linear_bound2);
  }
  bound_mul(intern->linear_bound,intern->linear_bound,f->ulp->sup);
  bound_add(intern->linear_bound,intern->linear_bound,f->min->sup);
  itv_enlarge_bound(l->cst,l->cst,intern->linear_bound);
}

/* adds an absolute error to l corresponding to a conversion to int
   assumes that i overapproximates the values of l before conversion
 */
static void
ap_texpr0_to_int(itv_internal_t* intern,
		 itv_linexpr_t* l /* in/out */, itv_t i /* in */,
		 ap_texpr_rdir_t d)
{
  switch (d) {
  case AP_RDIR_UP:
    /* add [0,1] */
    bound_add_uint(l->cst->sup,l->cst->sup,1);
    break;
  case AP_RDIR_DOWN:
    /* add [-1,0] */
    bound_add_uint(l->cst->inf,l->cst->inf,1);
    break;
  case AP_RDIR_RND:
    /* add [-1,1] */
    bound_add_uint(l->cst->sup,l->cst->sup,1);
    bound_add_uint(l->cst->inf,l->cst->inf,1);
    break;
  case AP_RDIR_ZERO:
    /* UP or DOWN or RND, depending on sign of i */
    if (bound_sgn(i->inf)>0) bound_add_uint(l->cst->sup,l->cst->sup,1);
    if (bound_sgn(i->sup)>0) bound_add_uint(l->cst->inf,l->cst->inf,1);
    break;
  case AP_RDIR_NEAREST:
    /* [-0.5,0.5] */
    itv_add(l->cst,l->cst,intern->itv_half);
    break;
  default:
    assert(0);
  }
}

/* adds rounding error to both l and i to go from type org to type dst */
static ap_texpr_rtype_t
ap_texpr0_round(itv_internal_t* intern,
		itv_linexpr_t* l /* in/out */, itv_t i /* in/out */,
		ap_texpr_rtype_t org,
		ap_texpr_rtype_t dst, ap_texpr_rdir_t d)
{
  if (dst==AP_RTYPE_REAL) return org;
  switch (dst) {
  case AP_RTYPE_INT:
    if (org==AP_RTYPE_INT) return org;
    ap_texpr0_to_int(intern,l,i,d);
    break;
  case AP_RTYPE_SINGLE:
    if (org==AP_RTYPE_SINGLE) return org;
    itv_linexpr_round_float_lin(intern,l,&intern->cst_single);
    break;
  case AP_RTYPE_DOUBLE:
    if (org==AP_RTYPE_SINGLE || org==AP_RTYPE_DOUBLE) return org;
    itv_linexpr_round_float_lin(intern,l,&intern->cst_double);
    break;
  case AP_RTYPE_EXTENDED:
    if (org==AP_RTYPE_SINGLE && org==AP_RTYPE_DOUBLE && org==AP_RTYPE_EXTENDED)
      return org;
    itv_linexpr_round_float_lin(intern,l,&intern->cst_extended);
    break;
  case AP_RTYPE_QUAD:
    if (org==AP_RTYPE_SINGLE || org==AP_RTYPE_DOUBLE ||
	org==AP_RTYPE_EXTENDED || org==AP_RTYPE_QUAD) return org;
    itv_linexpr_round_float_lin(intern,l,&intern->cst_quad);
    break;
  default:
    assert(0);
  }

  itv_round(i,i,dst,d);
  return dst;
}

/* reduce l and i:
   - intersect i with the interval evaluation of l
   - if l is constant, replace it with i
   - check for emptiness
 */
static void
ap_texpr0_reduce(itv_internal_t* intern, itv_t* env,
		 itv_linexpr_t* l /* in/out */, itv_t i /* in/out */)
{
  itv_t tmp;
  itv_init(tmp);
  itv_eval_linexpr(intern,tmp,l,env);
  itv_meet(intern,i,i,tmp);
  if (itv_is_bottom(intern,i) || itv_is_bottom(intern,l->cst)) {
    itv_set_bottom(i);
    itv_set_bottom(l->cst);
    if (l->size>0) itv_linexpr_reinit(l,0);
  }
  else if (l->size==0) itv_set(l->cst,i);
  itv_clear(tmp);
}

/* multiplication heuristic: choose which interval to keep (0=a, 1=b) */
static int
ap_texpr0_cmp_range(itv_internal_t* intern,
		    itv_linexpr_t* la, itv_t ia,
		    itv_linexpr_t* lb, itv_t ib)
{
  int sgn_a,sgn_b;
  /* if one linear form is an interval keep it */
  if (la->size==0) return 0;
  if (lb->size==0) return 1;
  /* if only one interval has constant sign, keep it */
  sgn_a = itv_is_pos(ia) || itv_is_neg(ia);
  sgn_b = itv_is_pos(ib) || itv_is_neg(ib);
  if (sgn_a!=sgn_b) return sgn_a ? 0 : 1;
  /* otherwise, keep the interval with the smallest relative range */
  itv_range_rel(intern,intern->linear_bound,ia);
  itv_range_rel(intern,intern->linear_bound2,ib);
  if (bound_cmp(intern->linear_bound,intern->linear_bound2)<0) return 0;
  else return 1;
}


/* preconditions:
   - env assigns an interval to each dimension
   - dimensions strictly smaller that intdim are integer
   - lres is not initialised
   - ires is initialised

   postconditions:
   - stores an interval linear form in lres
   - stores an interval in ires
   both encompase the exact expression values (ires may be smaller than the
   interval evaluation of lres)
   - returns the expression type
*/

static ap_texpr_rtype_t
itv_intlinearize_texpr0_rec(itv_internal_t* intern,
			    ap_texpr0_t* expr,
			    itv_t* env, size_t intdim,
			    itv_linexpr_t* lres /* out */,
			    itv_t ires /* out */);

static int debug_indent = 0;

static ap_texpr_rtype_t
ap_texpr0_node_intlinearize(itv_internal_t* intern,
			    ap_texpr0_node_t* n,
			    itv_t* env, size_t intdim,
			    itv_linexpr_t* lres /* out */, itv_t ires /* out */)
{
  itv_t i1,i2;
  itv_linexpr_t l1;
  ap_texpr_rtype_t t1,t2;

  switch (n->op) {
  case AP_TEXPR_NEG:
    /* negate linear form & interval, no rounding */
    t1 = itv_intlinearize_texpr0_rec(intern,n->exprA,env,intdim,lres,ires);
    itv_linexpr_neg(lres);
    itv_neg(ires,ires);
    return t1;

  case AP_TEXPR_CAST:
    /* round linear form & interval */
    t1 = itv_intlinearize_texpr0_rec(intern,n->exprA,env,intdim,lres,ires);
    ap_texpr0_round(intern,lres,ires,t1,n->type,n->dir);
    ap_texpr0_reduce(intern,env,lres,ires);
    break;

  case AP_TEXPR_SQRT:
    /* intlinearize argument, lres is not used */
    itv_intlinearize_texpr0_rec(intern,n->exprA,env,intdim,lres,ires);
    /* interval square root */
    itv_sqrt(intern,ires,ires);
    itv_round(ires,ires,n->type,n->dir);
    itv_linexpr_reinit(lres,0);
    itv_set(lres->cst,ires);
    break;

  case AP_TEXPR_ADD:
  case AP_TEXPR_SUB:
    itv_init(i1);
    itv_linexpr_init(&l1,0);

    /* intlinearize arguments */
    t1 = itv_intlinearize_texpr0_rec(intern,n->exprA,env,intdim,&l1,i1);
    t2 = itv_intlinearize_texpr0_rec(intern,n->exprB,env,intdim,lres,ires);
    /* add/sub linear form & interval */
    if (n->op==AP_TEXPR_ADD) {
      itv_linexpr_add(intern,lres,&l1,lres);
      itv_add(ires,i1,ires);
    }
    else {
      itv_linexpr_sub(intern,lres,&l1,lres);
      itv_sub(ires,i1,ires);
    }
    /* round */
    ap_texpr0_round(intern,lres,ires,
		    (t1==AP_RTYPE_INT && t2==AP_RTYPE_INT) ?
		    AP_RTYPE_INT : AP_RTYPE_REAL,
		    n->type,n->dir);
    /* reduce */
    ap_texpr0_reduce(intern,env,lres,ires);
    itv_clear(i1);
    itv_linexpr_clear(&l1);
    break;

  case AP_TEXPR_DIV:
    itv_init(i1);
    itv_linexpr_init(&l1,0);
    /* intlinearize arguments, l1 is not used */
    itv_intlinearize_texpr0_rec(intern,n->exprA,env,intdim,lres,ires);
    itv_intlinearize_texpr0_rec(intern,n->exprB,env,intdim,&l1,i1);
    /* divide linear form & interval */
    itv_linexpr_div(intern,lres,i1);
    itv_div(intern,ires,ires,i1);
    /* round */
    ap_texpr0_round(intern,lres,ires,AP_RTYPE_REAL,n->type,n->dir);
    /* reduce */
    ap_texpr0_reduce(intern,env,lres,ires);
    itv_clear(i1);
    itv_linexpr_clear(&l1);
    break;

  case AP_TEXPR_MUL:
    itv_init(i1);
    itv_linexpr_init(&l1,0);
    /* intlinearize arguments */
    t1 = itv_intlinearize_texpr0_rec(intern,n->exprA,env,intdim,&l1,i1);
    t2 = itv_intlinearize_texpr0_rec(intern,n->exprB,env,intdim,lres,ires);
    /* multiply one linear form with the other interval */
    if (ap_texpr0_cmp_range(intern,&l1,i1,lres,ires))  {
      /* res = ires * l1 */
#if LOGDEBUG
      printf("%*s lin * inter\n",2*debug_indent,"");
#endif
      itv_linexpr_clear(lres);
      *lres = l1;
      itv_linexpr_scale(intern,lres,ires);
    }
    else {
      /* res = i1 * lres */
#if LOGDEBUG
      printf("%*s inter * lin\n",2*debug_indent,"");
#endif
      itv_linexpr_clear(&l1);
      itv_linexpr_scale(intern,lres,i1);
    }
    itv_mul(intern,ires,i1,ires);
    /* round */
    ap_texpr0_round(intern,lres,ires,
		    (t1==AP_RTYPE_INT && t2==AP_RTYPE_INT) ?
		    AP_RTYPE_INT : AP_RTYPE_REAL,
		    n->type,n->dir);
    /* reduce */
    ap_texpr0_reduce(intern,env,lres,ires);
    itv_clear(i1);
    break;

  case AP_TEXPR_MOD:
    itv_init(i1);
    itv_linexpr_init(&l1,0);
    /* intlinearize arguments, lres & l1 are not used */
    itv_intlinearize_texpr0_rec(intern,n->exprA,env,intdim,lres,ires);
    itv_intlinearize_texpr0_rec(intern,n->exprB,env,intdim,&l1,i1);
    /* interval modulo, no rounding */
    itv_mod(intern,ires,ires,i1,n->type==AP_RTYPE_INT);
    itv_linexpr_reinit(lres,0);
    itv_set(lres->cst,ires);
    itv_clear(i1);
    itv_linexpr_clear(&l1);
    break;

  default:
    assert(0);
  }

  return n->type;
}

static ap_texpr_rtype_t
itv_intlinearize_texpr0_rec(itv_internal_t* intern,
			    ap_texpr0_t* expr,
			    itv_t* env, size_t intdim,
			    itv_linexpr_t* lres /* out */, itv_t ires /* out */)
{
  itv_linexpr_t r;
  ap_texpr_rtype_t t;
  assert(expr);

#if LOGDEBUG
  printf("%*s @ ",2*debug_indent,"");
  ap_texpr0_print(expr,NULL);
  printf("\n");
  debug_indent++;
#endif

  switch(expr->discr){
  case AP_TEXPR_CST:
    itv_set_ap_coeff(intern,ires,&expr->val.cst);
    itv_linexpr_reinit(lres,0);
    itv_set(lres->cst,ires);
    lres->equality = false;
    t = itv_is_int(intern,lres->cst) ? AP_RTYPE_INT : AP_RTYPE_REAL;
    break;
  case AP_TEXPR_DIM:
    itv_set(ires,env[expr->val.dim]);
    itv_linexpr_reinit(lres,1);
    lres->linterm[0].dim = expr->val.dim;
    lres->linterm[0].equality = true;
    itv_set_int(lres->linterm[0].itv,1);
    t = (expr->val.dim<intdim) ? AP_RTYPE_INT : AP_RTYPE_REAL;
    break;
  case AP_TEXPR_NODE:
    t = ap_texpr0_node_intlinearize(intern,expr->val.node,env,intdim,lres,ires);
    break;
  default:
    t = 0;
    assert(false);
  }

#if LOGDEBUG
  debug_indent--;
  printf("%*s   = ",2*debug_indent,"");
  itv_linexpr_print(lres,NULL);
  printf(" /\\ ");
  itv_print(ires);
  printf(" t=%i\n",t);
#endif

  return t;
}

void
ITVFUN(itv_intlinearize_ap_texpr0)(itv_internal_t* intern,
				   itv_linexpr_t* res,
				   ap_texpr0_t* expr,
				   itv_t* env, size_t intdim,
				   bool quasilinearize)
{
  itv_t i;
  itv_init(i);
  itv_intlinearize_texpr0_rec(intern,expr,env,intdim,res,i);
  if (!itv_is_bottom(intern,i) && !itv_is_bottom(intern,res->cst)) {
    if (quasilinearize)
      itv_quasilinearize_linexpr(intern,res,env);
  }
  itv_clear(i);
  return;
}

void
ITVFUN(itv_intlinearize_ap_tcons0)(itv_internal_t* intern,
				   itv_lincons_t* res,
				   ap_tcons0_t* cons,
				   itv_t* env, size_t intdim,
				   bool quasilinearize)
{
  itv_t i;
  itv_init(i);
  itv_intlinearize_texpr0_rec(intern,cons->texpr0,env,intdim,&res->linexpr,i);
  res->constyp = cons->constyp;
  if (cons->scalar){
    num_set_ap_scalar(res->num,cons->scalar);
  }
  else {
    num_set_int(res->num,0);
  }
  if (!itv_is_bottom(intern,i) && !itv_is_bottom(intern,res->linexpr.cst)) {
    if (quasilinearize)
      itv_quasilinearize_lincons(intern,res,env);
  }
  itv_clear(i);
  return;
}
void
ITVFUN(itv_intlinearize_ap_tcons0_array)(itv_internal_t* intern,
					 itv_lincons_array_t* res,
					 ap_tcons0_array_t* array,
					 itv_t* env, size_t intdim,
					 ap_linexpr_type_t linearize)
{
  size_t i,index;

  itv_lincons_array_reinit(res,array->size);
  index = 0;
  for (i=0; i<array->size;i++){
    itv_intlinearize_ap_tcons0(intern,
			       &res->p[index],&array->p[i],env,intdim,
			       linearize != AP_LINEXPR_INTLINEAR);
    if (!itv_is_bottom(intern,res->p[index].linexpr.cst))
      index++;
  }
  itv_lincons_array_reinit(res,index);
  if (linearize)
    itv_linearize_lincons_array(intern,res);
  return;
}
