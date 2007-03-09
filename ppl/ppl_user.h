/*
 * ppl_user.cc
 *
 * APRON Library / PPL library wrapper
 *
 * Conversion between APRON user types and PPL classes.
 * These are used internally by all PPL wrappers, but not exported to the user.
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

/* 
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

/* APRON includes */
#include "apron_ppl.h"

/* PPL includes */
#include "ppl.hh"
using namespace std;
using namespace Parma_Polyhedra_Library;

/* inidicates that a an object cannot be exactly converted into another one
   should be caught in transfer functions
*/
class cannot_convert: public exception {
public: 
  cannot_convert() {};
};


/* GMP => scalar (exact) */
static inline void ap_ppl_mpz_to_scalar(ap_scalar_t* s, const mpz_class& i)
{
  ap_scalar_reinit(s,AP_SCALAR_MPQ);
  mpq_set_num(s->val.mpq,i.get_mpz_t());
}

/* GMP / GMP => scalar (exact) */
static inline void ap_ppl_mpz2_to_scalar(ap_scalar_t* s, 
					 const mpz_class& n, const mpz_class& d)
{
  ap_scalar_reinit(s,AP_SCALAR_MPQ);
  mpq_set_num(s->val.mpq,n.get_mpz_t());
  mpq_set_den(s->val.mpq,d.get_mpz_t());
}

/* coeff => GMP */
/* fail if the coeff cannot be exactely represented as a finite mpq */
static inline void ap_ppl_mpq_of_coef(mpq_class& res, const ap_coeff_t coef)
{

  switch (coef.discr) {
  case AP_COEFF_SCALAR:
    if (ap_scalar_infty(coef.val.scalar) ||
	ap_mpq_set_scalar(res.get_mpq_t(),coef.val.scalar,GMP_RNDU))
      throw cannot_convert();
    break;
  case AP_COEFF_INTERVAL:
    {
      mpq_class tmp;
      if (ap_scalar_infty(coef.val.interval->sup) ||
	  ap_scalar_infty(coef.val.interval->inf) ||
	  ap_mpq_set_scalar(res.get_mpq_t(),coef.val.interval->sup,GMP_RNDU) ||
	  ap_mpq_set_scalar(tmp.get_mpq_t(),coef.val.interval->inf,GMP_RNDD) ||
	  tmp!=res) 
	throw cannot_convert();
    }
    break;
  default: 
    throw invalid_argument("Coefficient type ap_ppl_mpq_of_coef");
  }
}

/* some of these may raise cannot_convert */
/* returned booleans are true when inexact */
/* reference booleans are set to true when inexact, left unchanged otherwise */
extern ap_lincons0_t ap_ppl_to_lincons(const Constraint& c);
extern ap_lincons0_t ap_ppl_to_lincons(const Congruence& c);
extern ap_lincons0_array_t ap_ppl_to_lincons_array(const Constraint_System& c);
extern ap_lincons0_array_t ap_ppl_to_lincons_array(const Congruence_System& c);
extern ap_generator0_t ap_ppl_to_generator(const Generator& c, bool& exact);
extern ap_generator0_t ap_ppl_to_generator(const Grid_Generator& c);
extern ap_generator0_array_t ap_ppl_to_generator_array(const Generator_System& c, bool& exact);
extern ap_generator0_array_t ap_ppl_to_generator_array(const Grid_Generator_System& c);
extern ap_generator0_array_t ap_ppl_generator_universe(size_t dim);
extern ap_interval_t** ap_ppl_box_universe(ap_interval_t** i,size_t nb);
extern void ap_ppl_of_linexpr(Linear_Expression& r,mpz_class& den,ap_linexpr0_t* c);
extern bool ap_ppl_of_lincons(Constraint& r,ap_lincons0_t* c,bool allow_strict);
extern bool ap_ppl_of_lincons(Congruence& r,ap_lincons0_t* c);
extern bool ap_ppl_of_lincons_array(Constraint_System& r,ap_lincons0_array_t* a,bool allow_strict);
extern bool ap_ppl_of_lincons_array(Congruence_System& r,ap_lincons0_array_t* a);
extern bool ap_ppl_of_generator(Generator& r,ap_generator0_t* c);
extern bool ap_ppl_of_generator(Grid_Generator& r,ap_generator0_t* c);
extern bool ap_ppl_of_generator_array(Generator_System& r,ap_generator0_array_t* a);
extern bool ap_ppl_of_generator_array(Grid_Generator_System& r,ap_generator0_array_t* a);
extern bool ap_ppl_of_box(Constraint_System& r,size_t nb, ap_interval_t** a);
extern bool ap_ppl_of_box(Congruence_System& r,size_t nb, ap_interval_t** a);

static ap_abstract0_t* ap_ppl_make_abstract0(ap_manager_t* man, void* v)
{
  ap_abstract0_t* r = (ap_abstract0_t*)malloc(sizeof(ap_abstract0_t));
  assert(r);
  r->value = v;
  r->man = ap_manager_copy(man);
  return r;
}

/* returns an element with the correct manager and, if possible, size */
static ap_abstract0_t* ap_ppl_invalid_abstract0(ap_manager_t* man, ap_abstract0_t* org = NULL)
{
  if (org) {
    ap_dimension_t d = ap_abstract0_dimension(org->man,org);
    return ap_abstract0_top(man,d.intdim,d.realdim);
  }
  else 
    return ap_abstract0_top(man,0,1);
}

#define arg_assert(cond,action,funid)					\
  do { if (!(cond)) {							\
      char buf_[1024];							\
      snprintf(buf_,sizeof(buf_),					\
	       "assertion (%s) failed in %s at %s:%i",			\
	       #cond, __func__, __FILE__, __LINE__);			\
      ap_manager_raise_exception(man,AP_EXC_INVALID_ARGUMENT, funid,buf_); \
      action }								\
  } while(0)
