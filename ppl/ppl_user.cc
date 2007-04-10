/*
 * ppl_user.cc
 *
 * APRON Library / PPL library wrapper
 *
 * Conversion between APRON user types and PPL classes.
 * These are shared by all PPL wrappers.
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

/* 
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include <assert.h>
#include <stdexcept>
#include "ppl_user.h"


/* Constraint => ap_lincons0_t (exact) */
ap_lincons0_t ap_ppl_to_lincons(const Constraint& c)
{
  ap_constyp_t t;
  ap_linexpr0_t* e;
  int i, n = c.space_dimension();
  /* special, inconsistent case */
  if (c.is_inconsistent())
    return ap_lincons0_make_unsat();
  /* other cases */
  e = ap_linexpr0_alloc(AP_LINEXPR_DENSE,n);
  assert(e);
  /* variable coefficients */
  for (i=0;i<n;i++)
    ap_ppl_mpz_to_scalar(e->p.coeff[i].val.scalar,c.coefficient(Variable(i)));
  /* constant coefficient */
  ap_ppl_mpz_to_scalar(e->cst.val.scalar,c.inhomogeneous_term());
  /* constraint type */
  if (c.is_nonstrict_inequality()) t = AP_CONS_SUPEQ;
  else if (c.is_equality()) t = AP_CONS_EQ;
  else if (c.is_strict_inequality()) t = AP_CONS_SUP;
  else throw invalid_argument("Constraint type in ap_ppl_to_lincons");
  return ap_lincons0_make(t,e,NULL);
}

/* Constraint_System => ap_lincons0_array_t (exact) */
ap_lincons0_array_t ap_ppl_to_lincons_array(const Constraint_System& c)
{
  ap_lincons0_array_t a;
  Constraint_System::const_iterator i, end = c.end();
  int k;
  /* first, compute system size */
  for (i=c.begin(),k=0;i!=end;i++,k++);
  a = ap_lincons0_array_make(k);
  /* then, convert constraints */
  for (i=c.begin(),k=0;i!=end;i++,k++)
    a.p[k] = ap_ppl_to_lincons(*i);
  return a;
}

/* Congruence => ap_lincons0_t (exact) */
ap_lincons0_t ap_ppl_to_lincons(const Congruence& c)
{
  ap_constyp_t t;
  ap_linexpr0_t* e;
  int i, n = c.space_dimension();
  /* special, inconsistent case */
  if (c.is_trivial_false())
    return ap_lincons0_make_unsat();
  /* other cases */
  e = ap_linexpr0_alloc(AP_LINEXPR_DENSE,n);
  assert(e);
  /* variable coefficients */
  for (i=0;i<n;i++)
    ap_ppl_mpz_to_scalar(e->p.coeff[i].val.scalar,c.coefficient(Variable(i)));
  /* constant coefficient */
  ap_ppl_mpz_to_scalar(e->cst.val.scalar,c.inhomogeneous_term());
  /* constraint type */
  if (c.is_equality()) return ap_lincons0_make(AP_CONS_EQ,e,NULL);
  else {
    ap_scalar_t* mod = ap_scalar_alloc();
    ap_ppl_mpz_to_scalar(mod,c.modulus());
    return ap_lincons0_make(AP_CONS_EQMOD,e,mod);
  }
}

/* Congruence_System => ap_lincons0_array_t (exact) */
ap_lincons0_array_t ap_ppl_to_lincons_array(const Congruence_System& c)
{
  ap_lincons0_array_t a;
  Congruence_System::const_iterator i, end = c.end();
  int k;
  /* first, compute system size */
  for (i=c.begin(),k=0;i!=end;i++,k++);
  a = ap_lincons0_array_make(k);
  /* then, convert constraints */
  for (i=c.begin(),k=0;i!=end;i++,k++)
    a.p[k] = ap_ppl_to_lincons(*i);
  return a;
}

/* Generator => ap_generator0_t (may set inexact to true) */
ap_generator0_t ap_ppl_to_generator(const Generator& c, bool& inexact)
{
  ap_gentyp_t t;
  ap_linexpr0_t* e;
  int i, n = c.space_dimension();
  e = ap_linexpr0_alloc(AP_LINEXPR_DENSE,n);
  assert(e);
  ap_linexpr0_set_cst_scalar_int(e,0);
  if (c.is_ray() || c.is_line()) {
    /* ray or line: no divisor */
    for (i=0;i<n;i++)
      ap_ppl_mpz_to_scalar(e->p.coeff[i].val.scalar,c.coefficient(Variable(i)));
    return ap_generator0_make(c.is_ray() ? AP_GEN_RAY : AP_GEN_LINE,e);
  }
  else {
    /* point or closure point: has divisor */
    const mpz_class& div = c.divisor();
    if (c.is_closure_point()) inexact = true;
    for (i=0;i<n;i++)
      ap_ppl_mpz2_to_scalar(e->p.coeff[i].val.scalar,c.coefficient(Variable(i)),div);
    return ap_generator0_make(AP_GEN_VERTEX,e);
  }
}

/* Generator_System => ap_generator0_array_t (may set inexact to true) */
ap_generator0_array_t ap_ppl_to_generator_array(const Generator_System& c,
						bool& inexact)
{
  ap_generator0_array_t a;
  Generator_System::const_iterator i, end = c.end();
  int k;
  /* first, compute system size */
  for (i=c.begin(),k=0;i!=end;i++,k++);
  a = ap_generator0_array_make(k);
  /* then, convert generators */
  for (i=c.begin(),k=0;i!=end;i++,k++)
    a.p[k] = ap_ppl_to_generator(*i,inexact);
  return a;
}

/* Grid_Generator => ap_generator0_t (exact) */
ap_generator0_t ap_ppl_to_generator(const Grid_Generator& c)
{
  ap_gentyp_t t;
  ap_linexpr0_t* e;
  int i, n = c.space_dimension();
  e = ap_linexpr0_alloc(AP_LINEXPR_DENSE,n);
  assert(e);
  ap_linexpr0_set_cst_scalar_int(e,0);
  if (c.is_line()) {
    /* line: no divisor */
    for (i=0;i<n;i++)
      ap_ppl_mpz_to_scalar(e->p.coeff[i].val.scalar,c.coefficient(Variable(i)));
    return ap_generator0_make(AP_GEN_LINE,e);
  }
  else {
    /* point or parameter: has divisor */
    const mpz_class& div = c.divisor();
    for (i=0;i<n;i++)
      ap_ppl_mpz2_to_scalar(e->p.coeff[i].val.scalar,c.coefficient(Variable(i)),div);
    return ap_generator0_make(c.is_point() ? AP_GEN_VERTEX : AP_GEN_LINEMOD,e);
  }
}

/* Grid_Generator_System => ap_generator0_array_t (exact) */
ap_generator0_array_t ap_ppl_to_generator_array(const Grid_Generator_System& c)
{
  ap_generator0_array_t a;
  Grid_Generator_System::const_iterator i, end = c.end();
  int k;
  /* first, compute system size */
  for (i=c.begin(),k=0;i!=end;i++,k++);
  a = ap_generator0_array_make(k);
  /* then, convert generators */
  for (i=c.begin(),k=0;i!=end;i++,k++){
    a.p[k] = ap_ppl_to_generator(*i);
  }
  return a;
}

/* whole universe as a generator system */
ap_generator0_array_t ap_ppl_generator_universe(size_t dim)
{
  ap_generator0_array_t ar = ap_generator0_array_make(dim+1);
  /* origin vertex */
  ar.p[0] = ap_generator0_make(AP_GEN_VERTEX,ap_linexpr0_alloc(AP_LINEXPR_SPARSE,0));
  /* one line for each dimension */
  for (size_t i=0;i<dim;i++) {
    ap_linexpr0_t* e = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,1);
    e->p.linterm[0].dim = i;
    ap_coeff_set_scalar_int(&e->p.linterm[0].coeff,1);
    ar.p[i+1] = ap_generator0_make(AP_GEN_LINE,e);
  }
  return ar;  
}

/* whole universe as a box */
ap_interval_t** ap_ppl_box_universe(ap_interval_t** i,size_t nb)
{
  for (size_t j=0;j<nb;j++)
    ap_interval_set_top(i[j]);
  return i;
}

/* ap_interval_t box => Constraint_System (return inexact) */
bool ap_ppl_of_box(Constraint_System& r,size_t nb, ap_interval_t** a)
{
  bool inexact = false;
  size_t i;
  mpq_class temp;
  r.clear();
  for (i=0;i<nb;i++) {
    /* inf */
    switch (ap_scalar_infty(a[i]->inf)) {
    case 0:
      inexact =	
	ap_mpq_set_scalar(temp.get_mpq_t(),a[i]->inf,GMP_RNDD) || inexact;
      r.insert( Constraint( temp.get_den() * Variable(i) >= temp.get_num() ));
      break;
    case -1:
      break;
    case 1:
      r = Constraint_System::zero_dim_empty();
      return false;
    default: 
      assert(0);
    }
    /* sup */
    switch (ap_scalar_infty(a[i]->sup)) {
    case 0:
      inexact = 
	ap_mpq_set_scalar(temp.get_mpq_t(),a[i]->sup,GMP_RNDU) || inexact;
      r.insert( Constraint( temp.get_den() * Variable(i) <= temp.get_num() ));
      break;
    case 1:
      break;
    case -1:
      r = Constraint_System::zero_dim_empty();
      return false;
    default: 
      assert(0);
    }
  }
  return inexact;
}

/* ap_interval_t box => Congruence_System (return inexact) */
bool ap_ppl_of_box(Congruence_System& r,size_t nb, ap_interval_t** a)
{
  bool inexact = false;
  size_t i;
  mpq_class temp;
  r.clear();
  for (i=0;i<nb;i++) {
    int inf = ap_scalar_infty(a[i]->inf);
    int sup = ap_scalar_infty(a[i]->sup);
    /* unsatisfiable */
    if (inf==1 || sup==-1) {
      r = Congruence_System::zero_dim_empty();
      return false;
    }
    /* no-singleton */
    if (inf || sup || 
	!ap_scalar_equal(a[i]->inf,a[i]->sup) ||
	ap_mpq_set_scalar(temp.get_mpq_t(),a[i]->inf,GMP_RNDD)) {
      inexact = true;
    }
    /* singleton */
    else r.insert(Constraint(Variable(i)==temp));
  }
  return inexact;
}

/* ap_linexpr0_t => Linear_Expression (may raise cannot_convert) */
/* NOTE: we perform each converson twice; could be optimized */
void ap_ppl_of_linexpr(Linear_Expression& r,mpz_class& den,ap_linexpr0_t* c)
{
  mpq_class temp;
  ap_ppl_mpq_of_coef(temp,c->cst);
  den = temp.get_den();
  r = Linear_Expression::zero();
  if (c->discr==AP_LINEXPR_DENSE) {
    /* compute lcm of denominators in den */
    for (size_t i=0;i<c->size;i++) {
      ap_ppl_mpq_of_coef(temp,c->p.coeff[i]);
      mpz_lcm(den.get_mpz_t(),den.get_mpz_t(),temp.get_den().get_mpz_t());
    }
    /* add variable coefficients * den */
    for (size_t i=0;i<c->size;i++) {
      ap_ppl_mpq_of_coef(temp,c->p.coeff[i]);
      temp *= den;
      r += Variable(i) * temp.get_num();
    }
  }
  else  {
    /* compute lcm of denominators in den */
    for (size_t i=0;i<c->size;i++) {
      ap_ppl_mpq_of_coef(temp,c->p.linterm[i].coeff);
      mpz_lcm(den.get_mpz_t(),den.get_mpz_t(),temp.get_den().get_mpz_t());
    }
    /* add variable coefficients * den */
    for (size_t i=0;i<c->size;i++) {
      ap_ppl_mpq_of_coef(temp,c->p.linterm[i].coeff);
      temp *= den;
      r += Variable(c->p.linterm[i].dim) * temp.get_num();
    }
  }
  /* add constant coefficient * den */
  ap_ppl_mpq_of_coef(temp,c->cst);
  temp *= den;
  r += temp.get_num();
}

/* ap_lincons0_t => Constraint (may raise cannot_convert, return inexact) */
/* congruences are overapproximated as linear equalities */
bool ap_ppl_of_lincons(Constraint& r,ap_lincons0_t* c,bool allow_strict)
{
  Linear_Expression l;
  mpz_class den;
  ap_ppl_of_linexpr(l,den,c->linexpr0);
  switch (c->constyp) {
  case AP_CONS_SUPEQ: r = ( l >= 0 ); return false;
  case AP_CONS_EQ:    r = ( l == 0 ); return false;
  case AP_CONS_SUP:   
    if (allow_strict) { r = ( l > 0 ); return false; }
    else { r = (l >= 0); return true; }
  case AP_CONS_EQMOD:
  case AP_CONS_DISEQ: 
    throw cannot_convert();
  default: 
    throw invalid_argument("Constraint type in ap_ppl_of_lincons");
  }
}

/* ap_lincons0_t => Congruence (may raise cannot_convert, return inexact) */
bool ap_ppl_of_lincons(Congruence& r,ap_lincons0_t* c)
{
  Linear_Expression l;
  mpz_class den;
  ap_ppl_of_linexpr(l,den,c->linexpr0);
  switch (c->constyp) {
  case AP_CONS_SUPEQ: 
  case AP_CONS_SUP:  
  case AP_CONS_DISEQ: throw cannot_convert();
  case AP_CONS_EQ:    r = ( l %= 0 ) / 0; return false;
  case AP_CONS_EQMOD:
    {
      mpq_class mod;
      if (!c->scalar ||
	  ap_scalar_infty(c->scalar) || 
	  ap_mpq_set_scalar(mod.get_mpq_t(),c->scalar,GMP_RNDU)) {
	r = ( l %= 0 ) / 0; 
	return true;
      }
      else {
	r = ( l * mod.get_den() %= 0) / mod.get_num();
	return false;
      }
    }
  default: 
    throw invalid_argument("Constraint type in ap_ppl_of_lincons");
  }
}

/* ap_lincons0_array_t => Constraint_System 
   returns true if inexact (some constraint was dropped or approximated) */
bool ap_ppl_of_lincons_array(Constraint_System& r,ap_lincons0_array_t* a,bool allow_strict)
{
  bool inexact = false;
  size_t i;
  Constraint c = Constraint::zero_dim_positivity();
  r.clear();
  for (i=0;i<a->size;i++) {
    try {
      inexact = ap_ppl_of_lincons(c,&a->p[i],allow_strict) || inexact;
      r.insert(c);
    }
    catch (cannot_convert w) { inexact = true; }
  }
  return inexact;
}

/* ap_lincons0_array_t => Congruence_System 
   returns true if inexact (some constraint was dropped or approximated) */
bool ap_ppl_of_lincons_array(Congruence_System& r,ap_lincons0_array_t* a)
{
  bool inexact = false;
  size_t i;
  Congruence c = Congruence::zero_dim_false();
  r.clear();
  for (i=0;i<a->size;i++) {
    try {
      inexact = ap_ppl_of_lincons(c,&a->p[i]) || inexact;
      r.insert(c);
    }
    catch (cannot_convert w) { inexact = true; }
  }
  return inexact;
}

/* ap_generator0_t => Generator (may raise cannot_convert, or return true) */
bool ap_ppl_of_generator(Generator& r,ap_generator0_t* c)
{
  Linear_Expression l;
  mpz_class den;
  ap_ppl_of_linexpr(l,den,c->linexpr0);
  switch (c->gentyp) {
  case AP_GEN_VERTEX:  r = Generator::point(l,den); return false;
  case AP_GEN_RAY:     r = Generator::ray(l);       return false;
  case AP_GEN_LINE:    r = Generator::line(l);      return false;
  case AP_GEN_RAYMOD:  r = Generator::ray(l);       return true;
  case AP_GEN_LINEMOD: r = Generator::line(l);      return true;
  default: 
    throw invalid_argument("Generator type in ap_ppl_of_generator");
  }
}

/* Test if the linear part of the generator is 0 
   Needed because PPL refuses non-vertex generators with such expressions */

bool ap_ppl_ap_generator0_select(ap_generator0_t* g)
{
  if (g->gentyp==AP_GEN_VERTEX) {
    return true;
  }
  else {
    size_t i;
    ap_dim_t dim;
    ap_coeff_t* coeff;
    ap_linexpr0_t* e = g->linexpr0;

    bool res = false;
    ap_linexpr0_ForeachLinterm(e,i,dim,coeff){
      if (ap_coeff_zero(coeff)==false){
	res = true;
	break;
      }
    }
    return res;
  }
}

/* ap_generator0_array_t => Generator_System 
   (may raise cannot_convert, or return true)
*/
bool ap_ppl_of_generator_array(Generator_System& r,ap_generator0_array_t* a)
{
  bool inexact = false;
  size_t i;
  Generator c = Generator::zero_dim_point();
  r.clear();
  for (i=0;i<a->size;i++) {
    if (ap_ppl_ap_generator0_select(&a->p[i])){
      inexact = ap_ppl_of_generator(c,&a->p[i]) || inexact;
      r.insert(c);
    }
  }
  return inexact;
}

/* ap_generator0_t => Grid_Generator (may raise cannot_convert, or return true) */
bool ap_ppl_of_generator(Grid_Generator& r,ap_generator0_t* c)
{
  Linear_Expression l;
  mpz_class den;
  ap_ppl_of_linexpr(l,den,c->linexpr0);
  switch (c->gentyp) {
  case AP_GEN_VERTEX:  r = Grid_Generator::point(l,den);     return false;
  case AP_GEN_RAY:     r = Grid_Generator::line(l);          return true;
  case AP_GEN_LINE:    r = Grid_Generator::line(l);          return true;
  case AP_GEN_RAYMOD:  r = Grid_Generator::parameter(l,den); return true;
  case AP_GEN_LINEMOD: r = Grid_Generator::parameter(l,den); return false;
  default: 
    throw invalid_argument("Generator type in ap_ppl_of_generator");
  }
}

/* ap_generator0_array_t => Grid_Generator_System 
   (may raise cannot_convert, or return true)
*/
bool ap_ppl_of_generator_array(Grid_Generator_System& r,ap_generator0_array_t* a)
{
  bool inexact = false;
  size_t i;
  Grid_Generator c = Grid_Generator::point();
  r.clear();
  for (i=0;i<a->size;i++) {
    if (ap_ppl_ap_generator0_select(&a->p[i])){
      inexact = ap_ppl_of_generator(c,&a->p[i]) || inexact;
      r.insert(c);
    }
  }
  return inexact;
}
