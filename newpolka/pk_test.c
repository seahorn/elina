/* ********************************************************************** */
/* pk_test.c: tests on polyhedra */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk_int.h"
#include "pk_user.h"
#include "pk_representation.h"
#include "pk_constructor.h"
#include "pk_test.h"

/* ====================================================================== */
/* Emptiness test */
/* ====================================================================== */

tbool_t poly_is_bottom(ap_manager_t* man, const poly_t* po)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_IS_BOTTOM);
  if (!po->C && !po->F)
    return tbool_true;

  if (po->F){
      man->result.flag_exact = man->result.flag_best = 
	po->intdim>0 ? tbool_top : tbool_true;
      return tbool_false;
  }
  else {
    if (pk->funopt->algorithm<0){
      return (po->C ? tbool_top : tbool_true);
    }
    else {
      poly_chernikova(man,po,NULL);
      if (pk->exn){
	pk->exn = AP_EXC_NONE;
	return tbool_top;
      }
      man->result.flag_exact = man->result.flag_best = 
	po->intdim>0 && po->F ? tbool_top : tbool_true;
      return tbool_of_int(po->F == NULL);
    }
  }
}

/* ====================================================================== */
/* Universe test */
/* ====================================================================== */

tbool_t poly_is_top(ap_manager_t* man, const poly_t* po)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_IS_TOP);
  man->result.flag_exact = man->result.flag_best = tbool_true;

  if (pk->funopt->algorithm>=0)
    poly_chernikova(man,po,NULL);
  if (!po->C && !po->F)
    return tbool_false;
  else if (po->C && po->F)
    return tbool_of_int(po->C->nbrows == pk->dec - 1);
  else
    return tbool_top;
}

/* ====================================================================== */
/* Inclusion test */
/* ====================================================================== */

/* ---------------------------------------------------------------------- */
/* Do generators satisfies a constraint ? */
/* ---------------------------------------------------------------------- */

/*
F is suppposed to be a valid matrix of ray (i.e., corresponding
to a non empty polyhedron.

The epsilon component of the constraint is not taken into account.  The
constraint is considered as strict only if the is_strict paramater telles so.
This enables to test the satisfiability of a strict constraint in non-strict
mode for the library.

*/

bool do_generators_sat_constraint(pk_internal_t* pk, const matrix_t* F, const numint_t* tab, bool is_strict)
{
  int i;

  if (numint_sgn(tab[0])==0){
    /* 1. constraint is an equality */
    for (i=0; i<F->nbrows; i++){
      vector_product_strict(pk,pk->poly_prod,
			    (const numint_t*)F->p[i],
			    (const numint_t*)tab,F->nbcolumns);
      if (numint_sgn(pk->poly_prod)) return false;
    }
    return true;
  }
  else {
    /* 2. constraint is an inequality */
    int sign;      /* sign of the scalar product */

    for (i=0; i<F->nbrows; i++){
      vector_product_strict(pk,pk->poly_prod,
			    (const numint_t*)F->p[i],
			    (const numint_t*)tab,F->nbcolumns);
      sign = numint_sgn(pk->poly_prod);

      if (sign<0){
	return false;
      }
      else {
	if (numint_sgn(F->p[i][0])==0){
	  /* line */
	  if (sign!=0) return false;
	}
	else {
	  /* ray or vertex */
	  if (is_strict && sign==0 && 
	      (pk->strict ? numint_sgn(F->p[i][polka_eps])>0 : true))
	    return false;
	}
      }
    }
    return true;
  }
}

/* This test requires frames of pa and constraints of pb. The
   result is true if and only if all frames of pa verify the
   constraints of pb. We do not require minimality. */

tbool_t poly_is_leq(ap_manager_t* man, const poly_t* pa, const poly_t* pb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_IS_LEQ);

  if (pk->funopt->algorithm>0)
    poly_chernikova(man,pa,"of the first argument");
  else
    poly_obtain_F(man,pa,"of the first argument");

  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    return tbool_top;
  }
  if (!pa->F){ /* pa is empty */
    man->result.flag_exact = man->result.flag_best = tbool_true;  
    return tbool_true;
  }
  if (pk->funopt->algorithm>0)
    poly_chernikova(man,pb,"of the second argument");
  else
    poly_obtain_C(man,pb,"of the second argument");
  
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    return tbool_top;
  }
  if (!pb->C){/* pb is empty */
    man->result.flag_exact = man->result.flag_best = tbool_true;  
    return tbool_false;
  }
  man->result.flag_exact = man->result.flag_best = 
    pa->intdim>0 ? tbool_top : tbool_true;  
  /* if both are mininmal, check the dimensions */
  if (pa->C && pa->F && pb->C && pb->F
      && (pa->nbeq < pb->nbeq || pa->nbline > pb->nbline))
    {
      return tbool_false;
    }
  else {
    /* does the frames of pa satisfy constraints of pb ? */
    int i;
    for (i=0; i<pb->C->nbrows; i++){
      bool sat = do_generators_sat_constraint(pk,
					      pa->F,
					      (const numint_t*)pb->C->p[i],
					      pk->strict && 
					      numint_sgn(pb->C->p[i][polka_eps])<0);
      if (sat==false) return tbool_false;
    }
    return tbool_true;
  }
}

/* ====================================================================== */
/* Equality test */
/* ====================================================================== */

tbool_t poly_is_eq(ap_manager_t* man, const poly_t* pa, const poly_t* pb)
{
  pk_init_from_manager(man,AP_FUNID_IS_EQ);

  man->result.flag_exact = man->result.flag_best = 
    pa->intdim>0 ? tbool_top : tbool_true;  
  if (pa->C && pa->F && pb->C && pb->F &&
      (pa->nbeq != pb->nbeq || pa->nbline != pb->nbline) ){
    return tbool_false;
  }
  else {
    tbool_t res = poly_is_leq(man,pa,pb);
    if (res == tbool_true){
      res = poly_is_leq(man,pb,pa);
    }
    if (res==tbool_true){
      man->result.flag_exact = man->result.flag_best = tbool_true;
    }
    return res;
  }
}

/* ====================================================================== */
/* Satisfiability of a linear constraint */
/* ====================================================================== */

tbool_t poly_sat_lincons(ap_manager_t* man, const poly_t* po, const ap_lincons0_t* lincons)
{
  bool sat;
  size_t dim;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_SAT_LINCONS);

  if (pk->funopt->algorithm>0)
    poly_chernikova(man,po,NULL);
  else
    poly_obtain_F(man,po,NULL);
  
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    return tbool_top;
  }
  if (!po->F){ /* po is empty */
    man->result.flag_exact = man->result.flag_best = tbool_true;
    return tbool_true;
  }
  switch (lincons->constyp){
  case AP_CONS_EQ:
  case AP_CONS_SUPEQ:
  case AP_CONS_SUP:
    break;
  default:
    man->result.flag_exact = man->result.flag_best = tbool_top;
    return tbool_top;
  }
  dim = po->intdim + po->realdim;
  vector_set_lincons(pk,
		     pk->poly_numintp,
		     lincons,
		     po->intdim, po->realdim, false);
  sat = do_generators_sat_constraint(pk,po->F,
				     (const numint_t*)pk->poly_numintp,
				     lincons->constyp==AP_CONS_SUP);
  man->result.flag_exact = man->result.flag_best = 
    sat ? 
    tbool_true : 
    (
     ( (pk->funopt->flag_exact_wanted || pk->funopt->flag_best_wanted) &&
       ap_linexpr0_is_real(lincons->linexpr0,po->intdim) ) ?
     tbool_true :
     tbool_top );

  return tbool_of_bool(sat);
}

/* ====================================================================== */
/* Inclusion of a dimension in an interval */
/* ====================================================================== */
/* ---------------------------------------------------------------------- */
/* Do generators satisfies a bound for one dimension ? */
/* ---------------------------------------------------------------------- */

/*
F is suppposed to be a valid matrix of ray (i.e., corresponding
to a non empty polyhedron.

Assume coeff is not an infinite number.

tests if:
- dim <= bound if sgn>0
- dim = bound if sgn=0
- dim >= bound if sgn<0
*/

bool do_generators_sat_bound(pk_internal_t* pk, const matrix_t* F, 
			     ap_dim_t dim, const ap_scalar_t* scalar,
			     int sgn)
{
  size_t index;
  int i, sgn2;

  index  = pk->dec + dim;
  ap_mpq_set_scalar(pk->vector_mpqp[0],scalar,0);
  for (i=0; i<F->nbrows; i++){
    sgn2 = numint_sgn(F->p[i][index]);
    if (numint_sgn(F->p[i][0])==0){
      /* line */
      if (sgn2) return false;
    }
    else if (numint_sgn(F->p[i][polka_cst])==0){
      /* ray */
      if ( (sgn>=0 && sgn2>0) || (sgn<=0 && sgn2<0) )
	return false;
    }
    else {
      /* vertex */
      mpz_set_numint(mpq_numref(pk->vector_mpqp[1]), F->p[i][index]);
      mpz_set_numint(mpq_denref(pk->vector_mpqp[1]), F->p[i][polka_cst]);
      mpq_canonicalize(pk->vector_mpqp[1]);
      if (sgn==0){
	if (!mpq_equal(pk->vector_mpqp[1],pk->vector_mpqp[0]))
	  return false;
      }
      else {
	sgn2 = mpq_cmp(pk->vector_mpqp[1],pk->vector_mpqp[0]);
	if ( (sgn>0 && sgn2>0) || (sgn<0 && sgn2<0) )
	  return false;
      }
    }
  }
  return true;
}

tbool_t poly_sat_interval(ap_manager_t* man, const poly_t* po,
			  ap_dim_t dim, const ap_interval_t* interval)
{
  bool sat;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_SAT_INTERVAL);

  if (pk->funopt->algorithm>0)
    poly_chernikova(man,po,NULL);
  else
    poly_obtain_F(man,po,NULL);

  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    return tbool_top;
  }
  if (!po->F){ /* po is empty */
    man->result.flag_exact = man->result.flag_best = tbool_true;
    return tbool_true;
  }
  if (ap_scalar_equal(interval->inf,interval->sup)){
    /* interval is a point */
    assert(!ap_scalar_infty(interval->inf));
    sat = do_generators_sat_bound(pk,po->F,dim,interval->inf,0);
  }
  else {
    sat = true;
    /* inferior bound */
    if (!ap_scalar_infty(interval->inf)){
      sat = do_generators_sat_bound(pk,po->F,dim,interval->inf,-1);
      if (!sat) goto poly_sat_interval_exit0;
    }
    /* superior bound */
    if (!ap_scalar_infty(interval->sup)){
      sat = do_generators_sat_bound(pk,po->F,dim,interval->sup,1);
    }
  }
 poly_sat_interval_exit0:
  man->result.flag_exact = man->result.flag_best = 
    sat ? tbool_true :
    (dim < po->intdim ? tbool_top : tbool_true);  
  return tbool_of_bool(sat);
}

/* ====================================================================== */
/* Is a dimension unconstrained ? */
/* ====================================================================== */

tbool_t poly_is_dimension_unconstrained(ap_manager_t* man, const poly_t* po,
					ap_dim_t dim)
{
  size_t i,j;
  tbool_t res;
  matrix_t* F;
  matrix_t* C;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_SAT_INTERVAL);

  if (pk->funopt->algorithm>0)
    poly_chernikova(man,po,NULL);
  else
    poly_obtain_C(man,po,NULL);

  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    return tbool_top;
  }
  if (!po->C){ /* po is empty */
    man->result.flag_exact = man->result.flag_best = tbool_true;
    return tbool_false;
  }
  if (po->F){
    /* We test if there exists the line of direction dim */ 
    poly_chernikova3(man,po,NULL);
    F = po->F;
    res = tbool_false;
    for (i=0; i<po->nbline; i++){
      if (numint_sgn(F->p[i][pk->dec+dim])){
	res = tbool_true;
	for(j=0; j<F->nbcolumns; j++){
	  if (j!=pk->dec+dim && numint_sgn(F->p[i][j])){
	    res = tbool_false;
	    break;
	  }
	}
	break;
      }
    }
  }
  else {
    /* We test if there exists a constraint with non zero coeff for dim */ 
    C = po->C;
    res = tbool_true;
    for (i=0; i<C->nbrows; i++){
      if (numint_sgn(C->p[i][pk->dec+dim])){
	res = tbool_false;
	break;
      }
    }
  }
  man->result.flag_exact = man->result.flag_best = tbool_true;
  return res;  
}

