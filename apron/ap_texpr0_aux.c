/* ************************************************************************* */
/* ap_texpr0_aux.c: tree expressions, submodule  */
/* ************************************************************************* */

#include "ap_texpr0_aux.h"
#include "num.h"
#include "bound.h"
#include "itv.h"
#include "itv_linexpr.h"

#if defined(NUM_MPQ)
/* Assume b>=0, return true if exact. round indicates the rounding mode */
static bool mpq_sqrt(mpq_t a, mpq_t b, int round)
{
  if (mpz_sgn(mpq_denref(b))==0){
    mpq_set(a,b);
    return true;
  }
  else {
    bool exact, exact2;
    exact = mpz_perfect_square_p(mpq_numref(b));
    mpz_sqrt(mpq_numref(a),mpq_numref(b));
    if (!exact && round>0){
      mpz_add_ui(mpq_numref(a),mpq_numref(a),1);
    }
    exact2 = mpz_perfect_square_p(mpq_denref(b));
    mpz_sqrt(mpq_denref(a),mpq_denref(b));
    if (!exact2 && round<0){
      mpz_sub_ui(mpq_denref(a),mpq_denref(a),1);
      assert(mpz_sgn(mpq_denref(a))>0);
    }
    exact = exact && exact2;
    mpq_canonicalize(a);
    return exact;
  }
}
#endif


/* Assume b>=0, return true if exact */
static
bool itv_sqrt(itv_t a, itv_t b)
#if defined(NUM_MPQ)
{
  bool exact = true;
  
  bound_neg(b->inf,b->inf);
  exact = mpq_sqrt(bound_numref(a->inf),bound_numref(b->inf),-1) && exact;
  bound_neg(b->inf,b->inf);
  if (a!=b) bound_neg(a->inf,a->inf);
  exact = mpq_sqrt(bound_numref(a->sup),bound_numref(b->sup),+1) && exact;
  return exact;
}
#else
/* Currently not sound, because lower bound is upper-approximated */
{
  bool exact = false;
  
  bound_neg(b->inf,b->inf);
  *(a->inf) = sqrt(*(b->inf));
  bound_neg(b->inf,b->inf);
  if (a!=b) bound_neg(a->inf,a->inf);
  *(a->inf) = nextafter(*(a->inf),(double)1.0/(double)0.0);
  *(b->sup) = sqrt(*(b->sup));
  return exact;
}
#endif

/* Compare ranges for multiplication */
static 
int itv_cmp_range(itv_t a, itv_t b)
{
  int sgn;
  int cmpinf = bound_cmp(a->inf,b->inf);
  int cmpsup = bound_cmp(a->sup,b->sup);
  if (cmpinf==cmpsup || cmpsup==0)
    sgn = cmpinf;
  else if (cmpinf==0)
    sgn = cmpsup;
  else if (bound_infty(a->sup) && bound_infty(b->inf)){
    /* If for instance we compare [-2,oo] with [-oo,-3],
       we return -1 (the second one is considered smaller) */
    bound_t neg;
    bound_init(neg);
    bound_neg(neg,a->inf);
    sgn = -bound_cmp(neg,b->sup);
    bound_clear(neg);
  }
  else if (bound_infty(a->inf) && bound_infty(b->sup)){
    /* Symmetrical situation */
    bound_t neg;
    bound_init(neg);
    bound_neg(neg,b->inf);
    sgn = -bound_cmp(a->sup,neg);
    bound_clear(neg);
  }
  else {
    /* Really compares ranges */
    bound_t rangeA, rangeB;
    bound_init(rangeA); bound_add(rangeA,a->sup,a->inf);
    bound_init(rangeB); bound_add(rangeA,b->sup,b->inf);
    sgn = bound_cmp(rangeA,rangeB);
    /* if equality, we could choose the smallest magnitude (not done) */
    bound_clear(rangeA);
    bound_clear(rangeB);
  }
  return sgn;
}


static void
ap_texpr0_intlinearize_rec_unop(itv_internal_t* intern,	itv_t itv, itv_t* p,
				ap_texpr0_node_t* node,
				itv_linexpr_t* res,
				bool* pexact);
static itv_linexpr_t 
ap_texpr0_intlinearize_rec_binop(itv_internal_t* intern,
				 itv_t itv, itv_t* p,
				 ap_texpr0_node_t* node,
				 itv_linexpr_t* resA, itv_linexpr_t* resB,
				 bool* pexact);
static
itv_linexpr_t ap_texpr0_intlinearize_rec(itv_internal_t* intern,
					 itv_t itv,
					 itv_t* p,
					 ap_texpr0_t* expr0,
					 bool* pexact)
{
  bool exact = true;
  bool exact2;
  itv_linexpr_t res;
  switch (expr0->discr){
  case AP_TEXPR_CST:
    itv_linexpr_init(&res,0);
    exact = itv_set_ap_coeff(intern,res.cst,&expr0->val.cst);
    break;
  case AP_TEXPR_DIM:
    itv_linexpr_init(&res,1);
    res.linterm[0].dim = expr0->val.dim;
    res.linterm[0].equality = true;
    itv_set_int(res.linterm[0].itv,1);
    break;
  case AP_TEXPR_NODE:
    {
      ap_texpr0_node_t* node = expr0->val.node;
      if (ap_texpr0_is_unop(node->op)){
	/* Unary operator */
	res = ap_texpr0_intlinearize_rec(intern,itv,p,node->exprA,&exact2);
	exact = exact && exact2;
	if (!itv_is_bottom(intern,res.cst)){
	  ap_texpr0_intlinearize_rec_unop(intern,itv,p,
					  node,&res,
					  &exact2);
	  exact = exact && exact2;
	}
      }
      else {
	/* Binary operator */
	itv_linexpr_t resA, resB;
	resA = ap_texpr0_intlinearize_rec(intern,itv,p,node->exprA,&exact2);
	exact = exact && exact2;
	resB = ap_texpr0_intlinearize_rec(intern,itv,p,node->exprB,&exact2);
	exact = exact && exact2;
	if (itv_is_bottom(intern,resA.cst) || itv_is_bottom(intern,resB.cst)){
	  res = resA;
	  itv_linexpr_clear(&resB);
	}
	else {
	  res = ap_texpr0_intlinearize_rec_binop(intern,itv,p,
						 node,&resA,&resB,
						 &exact2);
	  exact = exact && exact2;
	}
      }
    }
    break;
  }
  *pexact = exact;
  return res;
}

/* ====================================================================== */
/* Unary operators */
/* ====================================================================== */

static 
void ap_texpr0_intlinearize_rec_unop(itv_internal_t* intern,
				     itv_t itv, itv_t* p,
				     ap_texpr0_node_t* node,
				     itv_linexpr_t* res,
				     bool* pexact)
{
  switch (node->op){
  case AP_TEXPR_SQRT:
    itv_eval_linexpr(intern,itv,p,res);
    if (bound_sgn(itv->sup)<0){
      /* sqrt(negative values) = bottom */
      itv_set_bottom(itv);
    }
    else {
      if (node->dir==AP_RDIR_EXACT || node->type==AP_RTYPE_REAL) {
	if (bound_sgn(itv->inf)>0){
	  /* filter to keep only positive values */
	  bound_set_int(itv->inf,0);
	}
	*pexact = itv_sqrt(itv,itv);
      }
      else {
	fprintf(stderr,"ap_texpr0_linearize_rec_unop: operator/mode not yet implemented");
	abort();
      }
    }
    itv_linexpr_reinit(res,0);
    itv_set(res->cst,itv);
    break;
  default:
    fprintf(stderr,"ap_texpr0_linearize_rec_unop: operator/mode not yet implemented");
    abort();
  }
}

/* ====================================================================== */
/* Binary operators */
/* ====================================================================== */

static 
itv_linexpr_t 
ap_texpr0_intlinearize_rec_binop(itv_internal_t* intern,
				 itv_t itv, itv_t* p,
				 ap_texpr0_node_t* node,
				 itv_linexpr_t* resA, itv_linexpr_t* resB,
				 bool* pexact)
{
  itv_linexpr_t res;
  if (node->dir==AP_RDIR_EXACT || node->type==AP_RTYPE_REAL) {
    switch (node->op){
    case AP_TEXPR_ADD:
      res = itv_linexpr_add(intern,resA,resB);
      itv_linexpr_clear(resA);
      itv_linexpr_clear(resB);
      break;
    case AP_TEXPR_SUB:
      res = itv_linexpr_sub(intern,resA,resB);
      itv_linexpr_clear(resA);
      itv_linexpr_clear(resB);
      break;
    case AP_TEXPR_MUL:
      if (resA->size==0){
	itv_linexpr_scale(intern,resB,resA->cst);
	itv_linexpr_clear(resA);
	res = *resB;
      }
      else if (resB->size==0){
	itv_linexpr_scale(intern,resA,resB->cst);
	itv_linexpr_clear(resB);
	res = *resA;
      }
      else {
	itv_t itvB;
	itv_init(itvB);
	itv_eval_linexpr(intern,itv,p,resA);
	itv_eval_linexpr(intern,itvB,p,resB);
	/* Choose the smallest interval (in range) */
	int sgn = itv_cmp_range(itv,itvB);
	if (sgn<=0){
	  itv_linexpr_scale(intern,resB,itv);
	  itv_linexpr_clear(resA);
	  res = *resB;
	}
	else {
	  itv_linexpr_scale(intern,resA,itvB);
	  itv_linexpr_clear(resB);
	  res = *resA;
	}
	itv_clear(itvB);
      }
      break;
    case AP_TEXPR_DIV:
      itv_eval_linexpr(intern,itv,p,resB);
      itv_linexpr_div(intern,resA,itv);
      itv_linexpr_clear(resB);
      res = *resA;
      break;
    default:
      fprintf(stderr,"ap_texpr0_linearize: operator not yet implemented");
      abort();
    }
  }
  else {
    fprintf(stderr,"ap_texpr0_linearize: operator not yet implemented");
    abort();
  }
  return res;
}

ap_linexpr0_t* ITVFUN(ap_texpr0_intlinearize)
  (ap_manager_t* man,
   void* abs,
   ap_texpr0_t* expr0,
   bool quasilinearize,
   bool* pexact)
{
  tbool_t (*is_bottom)(ap_manager_t*,...) = man->funptr[AP_FUNID_IS_BOTTOM];
  ap_interval_t** (*to_box)(ap_manager_t*,...) = man->funptr[AP_FUNID_TO_BOX];
  ap_dimension_t (*dimension)(ap_manager_t*,...) = man->funptr[AP_FUNID_DIMENSION];
  ap_interval_t** tinterval;
  bool exact,exact2;
  ap_dimension_t dim;
  itv_t* titv;
  itv_linexpr_t linexpr;
  itv_t itv;
  ap_linexpr0_t* linexpr0;
  size_t size;

  itv_internal_t* intern = itv_internal_alloc();
  itv_init(itv);

  assert(is_bottom(man,abs)!=tbool_true);
  exact = true;
  
  tinterval = to_box(man,abs);
  exact = (man->result.flag_exact == tbool_true) && exact;

  dim = dimension(man,abs);
  size = dim.intdim+dim.realdim;

  exact = itv_array_set_ap_interval_array(intern,&titv,tinterval,size);
  linexpr = ap_texpr0_intlinearize_rec(intern,itv,titv,expr0,&exact2);
  exact = exact && exact2;
  if (itv_is_bottom(intern,linexpr.cst)){
    linexpr0 = NULL;
  }
  else {
    if (quasilinearize){
      itv_linexpr_quasilinearize(intern,&linexpr,titv);
    }
    linexpr0 = NULL;
    ap_linexpr0_set_itv_linexpr(intern,&linexpr0,&linexpr);
  }
  itv_linexpr_clear(&linexpr);
  itv_clear(itv);
  itv_internal_free(intern);
  ap_interval_array_free(tinterval,size);
  return linexpr0;
}
