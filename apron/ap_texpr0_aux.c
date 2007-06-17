/* ************************************************************************* */
/* ap_texpr0_aux.c: tree expressions, submodule  */
/* ************************************************************************* */

#include "ap_texpr0_aux.h"
#include "num.h"
#include "bound.h"
#include "itv.h"
#include "itv_linexpr.h"

#define LOGDEBUG 0

/* ====================================================================== */
/* Interval evaluation */
/* ====================================================================== */

/* round interval bounds */
static void 
itv_round(ap_texpr_rtype_t t,
	  ap_texpr_rdir_t d,
	  itv_t res, itv_t arg)
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

/* evaluates node into interval res, assuming operator arguments are arg1 and
   (for binary operator) arg2
*/
static void 
ap_texpr0_node_eval(itv_internal_t* intern,
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
  itv_round(n->type,n->dir,res,res);
}

/* evaluates expr into intervalres, 
   assuming env maps dimensions to interval values */
static void 
ap_texpr0_eval_rec(itv_internal_t* intern,
		   ap_texpr0_t* expr,
		   itv_t* env,
		   itv_t res)
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
      ap_texpr0_eval_rec(intern,expr->val.node->exprA,env,x);
      ap_texpr0_eval_rec(intern,expr->val.node->exprB,env,res);
      ap_texpr0_node_eval(intern,expr->val.node,res,x,res);
      itv_clear(x);
    }
    else {
      /* unary */
      ap_texpr0_eval_rec(intern,expr->val.node->exprA,env,res);
      ap_texpr0_node_eval(intern,expr->val.node,res,res,res);
    }
    break;
  default:
    assert(false);
  }
}


/* ====================================================================== */
/* Constants */
/* ====================================================================== */

/* some useful local constant, computed on the first call to make_const */

typedef struct {
  itv_t ulp;        /* [-1;1] * unit in the least place */
  itv_t min;        /* [-1;1] * minimum positive denormal */
  itv_t min_normal; /* [-1;1] * minimum positive normal */
  itv_t max;        /* [-1;1] * maximum non +oo  */
  itv_t max_exact;  /* [-1;1] * maximum exactly representable integer */
} float_const;

static float_const cst_half, cst_single, cst_double, cst_extended, cst_quad;

static itv_t itv_half; /* [-0.5,0.5] */

static void make_float_const(int frac_bits, int exp_bits, int exp_bias,
			     float_const* cst)
{
  bound_t b,c;
  bound_init(b); bound_init(c);
  itv_init(cst->ulp); itv_init(cst->min); itv_init(cst->max);

  bound_set_int(b,1); 
  bound_mul_2exp(b,b,-frac_bits);
  itv_set_unit_bound(cst->ulp,b);

  bound_set_int(b,1); 
  bound_mul_2exp(b,b,1-exp_bias-frac_bits);
  itv_set_unit_bound(cst->min,b);

  bound_set_int(b,1); 
  bound_mul_2exp(b,b,1-exp_bias);
  itv_set_unit_bound(cst->min_normal,b);

  bound_set_int(b,2); 
  bound_set_int(c,1);
  bound_mul_2exp(c,c,-frac_bits);
  bound_sub(b,b,c);
  bound_mul_2exp(b,b,(1<<exp_bits)-2-exp_bias);
  itv_set_unit_bound(cst->max,b);

  bound_set_int(b,1);
  bound_mul_2exp(b,b,frac_bits);
  itv_set_unit_bound(cst->max_exact,b);

  bound_clear(b); bound_clear(c);
}

static void make_const(void)
{
  static int done = 0;
  if (!done) {
    /* execute only once */
    done = 1;
    make_float_const(10,5,15,&cst_half);         /* 16-bit */
    make_float_const(23,8,127,&cst_single);      /* 32-bit */
    make_float_const(52,11,1023,&cst_double);    /* 64-bit */
    make_float_const(63,15,16383,&cst_extended); /* 80-bit, no hidden bit */
    make_float_const(112,15,16383,&cst_quad);    /* 128-bit */

    itv_init(itv_half);
    itv_set_int2(itv_half,-1,1);
    itv_mul_2exp(itv_half,itv_half,-1);

  }
}


/* ====================================================================== */
/* Linearization */
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
ap_texpr0_round_float(itv_internal_t* intern, 
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
    make_const();
    itv_add(l->cst,l->cst,itv_half);
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

  make_const();

  switch (dst) {
  case AP_RTYPE_INT:
    if (org==AP_RTYPE_INT) return org;
    ap_texpr0_to_int(intern,l,i,d);
    break;
  case AP_RTYPE_SINGLE:
    if (org==AP_RTYPE_SINGLE) return org;
    itv_linexpr_round_float_lin(intern,l,&cst_single);
    break;
  case AP_RTYPE_DOUBLE:
    if (org==AP_RTYPE_SINGLE || org==AP_RTYPE_DOUBLE) return org;
    itv_linexpr_round_float_lin(intern,l,&cst_double);
    break;
  case AP_RTYPE_EXTENDED:
    if (org==AP_RTYPE_SINGLE && org==AP_RTYPE_DOUBLE && org==AP_RTYPE_EXTENDED)
      return org;
    itv_linexpr_round_float_lin(intern,l,&cst_extended);
    break;
  case AP_RTYPE_QUAD:
    if (org==AP_RTYPE_SINGLE || org==AP_RTYPE_DOUBLE ||
	org==AP_RTYPE_EXTENDED || org==AP_RTYPE_QUAD) return org;
    itv_linexpr_round_float_lin(intern,l,&cst_quad);
    break;
  default:
    assert(0);
  }

  itv_round(dst,d,i,i);
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
  itv_eval_linexpr(intern,tmp,env,l);
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
ap_texpr0_linearize_rec(itv_internal_t* intern,
			ap_texpr0_t* expr,
			itv_t* env, size_t intdim,
			itv_linexpr_t* lres /* out */,
			itv_t ires /* out */);

static int debug_indent = 0;

static ap_texpr_rtype_t
ap_texpr0_node_linearize(itv_internal_t* intern,
			 ap_texpr0_node_t* n, 
			 itv_t* env, size_t intdim,
			 itv_linexpr_t* lres /* out */, itv_t ires /* out */)
{
  itv_t i1,i2;
  itv_linexpr_t l1,l2;
  ap_texpr_rtype_t t1,t2;

  switch (n->op) {
  case AP_TEXPR_NEG:
    /* negate linear form & interval, no rounding */
    t1 = ap_texpr0_linearize_rec(intern,n->exprA,env,intdim,lres,ires);
    itv_linexpr_neg(lres);
    itv_neg(ires,ires);
    return t1;

  case AP_TEXPR_CAST:
    /* round linear form & interval */
    t1 = ap_texpr0_linearize_rec(intern,n->exprA,env,intdim,lres,ires);
    ap_texpr0_round(intern,lres,ires,t1,n->type,n->dir);
    ap_texpr0_reduce(intern,env,lres,ires);
    break;

  case AP_TEXPR_SQRT:
    /* linearize argument, lres is not used */
    ap_texpr0_linearize_rec(intern,n->exprA,env,intdim,lres,ires);
    /* interval square root */
    itv_sqrt(intern,ires,ires);
    itv_round(n->type,n->dir,ires,ires);
    itv_linexpr_reinit(lres,0);
    itv_set(lres->cst,ires);
    break;

  case AP_TEXPR_ADD:
  case AP_TEXPR_SUB:
    itv_init(i1);
    /* linearize arguments */
    t1 = ap_texpr0_linearize_rec(intern,n->exprA,env,intdim,&l1,i1);
    t2 = ap_texpr0_linearize_rec(intern,n->exprB,env,intdim,&l2,ires);
    /* add/sub linear form & interval */
    if (n->op==AP_TEXPR_ADD) {
      *lres = itv_linexpr_add(intern,&l1,&l2);
      itv_add(ires,i1,ires);
    }
    else {
      *lres = itv_linexpr_sub(intern,&l1,&l2);
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
    itv_linexpr_clear(&l1); itv_linexpr_clear(&l2);
    break;

  case AP_TEXPR_DIV:
    itv_init(i1);
    /* linearize arguments, l1 is not used */
    ap_texpr0_linearize_rec(intern,n->exprA,env,intdim,lres,ires);
    ap_texpr0_linearize_rec(intern,n->exprB,env,intdim,&l1,i1);
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
    /* linearize arguments */
    t1 = ap_texpr0_linearize_rec(intern,n->exprA,env,intdim,&l1,i1);
    t2 = ap_texpr0_linearize_rec(intern,n->exprB,env,intdim,&l2,ires);
    /* multiply one linear form with the other interval */
    if (ap_texpr0_cmp_range(intern,&l1,i1,&l2,ires))  {
      /* r = ires * l1 */
#if LOGDEBUG
      printf("%*s lin * inter\n",2*debug_indent,""); 
#endif
      itv_linexpr_clear(&l2);
      *lres = l1;
      itv_linexpr_scale(intern,lres,ires);
    }
    else {
      /* r = i1 * l2 */
#if LOGDEBUG
      printf("%*s inter * lin\n",2*debug_indent,""); 
#endif
      itv_linexpr_clear(&l1);
      *lres = l2;
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
    /* linearize arguments, lres & l1 are not used */
    ap_texpr0_linearize_rec(intern,n->exprA,env,intdim,lres,ires);
    ap_texpr0_linearize_rec(intern,n->exprB,env,intdim,&l1,i1);
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
ap_texpr0_linearize_rec(itv_internal_t* intern,
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
    itv_linexpr_init(lres,0);
    itv_set(lres->cst,ires);
    lres->equality = false;
    t = itv_is_int(intern,lres->cst) ? AP_RTYPE_INT : AP_RTYPE_REAL;
    break;
  case AP_TEXPR_DIM:
    itv_set(ires,env[expr->val.dim]);
    itv_linexpr_init(lres,1);
    lres->linterm[0].dim = expr->val.dim;
    lres->linterm[0].equality = true;
    itv_set_int(lres->linterm[0].itv,1);
    t = (expr->val.dim<intdim) ? AP_RTYPE_INT : AP_RTYPE_REAL;
    break;
  case AP_TEXPR_NODE:
    t = ap_texpr0_node_linearize(intern,expr->val.node,env,intdim,lres,ires);
    break;
  default:
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

static ap_linexpr0_t*
ap_texpr0_do_linearize(itv_internal_t* intern,
		       ap_texpr0_t* expr,
		       itv_t* env, size_t intdim,
		       bool quasilinearise)
{
  ap_linexpr0_t* l = NULL;
  itv_linexpr_t r;
  itv_t i;
  itv_init(i);
  ap_texpr0_linearize_rec(intern,expr,env,intdim,&r,i);
  if (!itv_is_bottom(intern,i) && !itv_is_bottom(intern,r.cst)) {
    if (quasilinearise) itv_linexpr_quasilinearize(intern,&r,env);
    ap_linexpr0_set_itv_linexpr(intern,&l,&r);
  }
  itv_linexpr_clear(&r);
  itv_clear(i);
  return l;
}




/* ====================================================================== */
/* Main */
/* ====================================================================== */


ap_linexpr0_t* 
ITVFUN(ap_texpr0_linearize)(ap_manager_t* man,
			    ap_abstract0_t* abs,
			    ap_texpr0_t* expr,
			    bool quasilinearize,
			    bool* pexact)
{
  itv_internal_t* intern;
  ap_dimension_t dim;
  ap_interval_t** aenv;
  itv_t* ienv;
  ap_linexpr0_t* res;
  if (pexact) *pexact = false;
  aenv = ap_abstract0_to_box(man,abs);
  if (!aenv) return NULL;
  dim = ap_abstract0_dimension(man,abs);
  intern = itv_internal_alloc();
  itv_array_set_ap_interval_array(intern,&ienv,aenv,dim.intdim+dim.realdim);
  res = ap_texpr0_do_linearize(intern,expr,ienv,dim.intdim,quasilinearize);
  itv_internal_free(intern);
  ap_interval_array_free(aenv,dim.intdim+dim.realdim);
  itv_array_free(ienv,dim.intdim+dim.realdim);
  return res;
}

ap_interval_t* 
ITVFUN(ap_texpr0_eval)(ap_manager_t* man,
		       ap_abstract0_t* abs,
		       ap_texpr0_t* expr,
		       bool* pexact)
{
  itv_internal_t* intern;
  ap_dimension_t dim;
  ap_interval_t** aenv;
  itv_t* ienv;
  itv_t res;
  ap_interval_t* r = ap_interval_alloc();
  if (pexact) *pexact = false;
  aenv = ap_abstract0_to_box(man,abs);
  if (!aenv) { 
    ap_interval_set_bottom(r); 
    return r; 
  }
  dim = ap_abstract0_dimension(man,abs);
  intern = itv_internal_alloc();
  itv_init(res);
  itv_array_set_ap_interval_array(intern,&ienv,aenv,dim.intdim+dim.realdim);
  ap_texpr0_eval_rec(intern,expr,ienv,res);
  ap_interval_set_itv(intern,r,res);
  itv_internal_free(intern);
  ap_interval_array_free(aenv,dim.intdim+dim.realdim);
  itv_array_free(ienv,dim.intdim+dim.realdim);
  itv_clear(res);
  return r;
}
