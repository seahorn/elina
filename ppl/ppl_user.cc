/*
 * apron_user.cc
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


/* Constraint => ap_lincons0_t */
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
  return ap_lincons0_make(t,e);
}

/* Constraint_System => ap_lincons0_array_t */
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

/* Generator => ap_geperator0_t */
ap_generator0_t ap_ppl_to_generator(const Generator& c)
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
      ap_ppl_mpz_to_scalar(e->p.coeff[0].val.scalar,c.coefficient(Variable(i)));
    return ap_generator0_make(c.is_ray() ? AP_GEN_RAY : AP_GEN_LINE,e);
  }
  else {
    /* point or closure point: has divisor */
    const mpz_class& div = c.divisor();
    for (i=0;i<n;i++)
      ap_ppl_mpz2_to_scalar(e->p.coeff[0].val.scalar,c.coefficient(Variable(i)),div);
    return ap_generator0_make(AP_GEN_VERTEX,e);
  }
}

/* Generator_System => ap_generator0_array_t */
ap_generator0_array_t ap_ppl_to_generator_array(const Generator_System& c)
{
  ap_generator0_array_t a;
  Generator_System::const_iterator i, end = c.end();
  int k;
  /* first, compute system size */
  for (i=c.begin(),k=0;i!=end;i++,k++);
  a = ap_generator0_array_make(k);
  /* then, convert generators */
  for (i=c.begin(),k=0;i!=end;i++,k++)
    a.p[k] = ap_ppl_to_generator(*i);
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



/* ap_interval_t box => Constraint_system */
void ap_ppl_of_box(Constraint_System& r,size_t nb, const ap_interval_t*const* a)
{
  size_t i;
  mpq_class temp;
  r.clear();
  for (i=0;i<nb;i++) {
    /* inf */
    if (!ap_scalar_infty(a[i]->inf)) {
      ap_mpq_set_scalar(temp.get_mpq_t(),a[i]->inf,0);
      r.insert( Constraint( temp.get_den() * Variable(i) >= temp.get_num() ));
    }
    /* sup */
    if (!ap_scalar_infty(a[i]->sup)) {
      ap_mpq_set_scalar(temp.get_mpq_t(),a[i]->sup,0);
      r.insert( Constraint( temp.get_den() * Variable(i) <= temp.get_num() ));
    }
  }
}

/* ap_linexpr0_t => Linear_Expression */
/* TODO: handle AP_COEFF_INTERVAL & infinities */
void ap_ppl_of_linexpr(Linear_Expression& r,mpz_class& den,const ap_linexpr0_t* c)
{
  mpq_class temp;
  if (c->cst.discr!=AP_COEFF_SCALAR)
    throw invalid_argument("interval coefficient not handled in ap_ppl_of_linexpr");
  if (ap_scalar_infty(c->cst.val.scalar)) 
    throw invalid_argument("infinite coefficients not handled in ap_ppl_of_linexpr");
  ap_mpq_set_scalar(temp.get_mpq_t(),c->cst.val.scalar,0);
  den = temp.get_den();
  r = Linear_Expression::zero();
  if (c->discr==AP_LINEXPR_DENSE) {
    /* compute lcm of denominators in den */
    for (size_t i=0;i<c->size;i++) {
      if (c->p.coeff[i].discr!=AP_COEFF_SCALAR)
	throw invalid_argument("interval coefficients not handled in ap_ppl_of_linexpr");
      if (ap_scalar_infty(c->p.coeff[i].val.scalar))
	throw invalid_argument("infinite coefficients not handled in ap_ppl_of_linexpr");
      ap_mpq_set_scalar(temp.get_mpq_t(),c->p.coeff[i].val.scalar,0);
      mpz_lcm(den.get_mpz_t(),den.get_mpz_t(),temp.get_den().get_mpz_t());
    }
    /* add variable coefficients * den */
    for (size_t i=0;i<c->size;i++) {
      ap_mpq_set_scalar(temp.get_mpq_t(),c->p.coeff[i].val.scalar,0);
      temp *= den;
      r += Variable(i) * temp.get_num();
    }
  }
  else  {
    /* compute lcm of denominators in den */
    for (size_t i=0;i<c->size;i++) {
      if (c->p.linterm[i].coeff.discr!=AP_COEFF_SCALAR)
	throw invalid_argument("interval coefficients not handled in ap_ppl_of_linexpr");
      if (ap_scalar_infty(c->p.linterm[i].coeff.val.scalar))
	throw invalid_argument("infinite coefficients not handled in ap_ppl_of_linexpr");
      ap_mpq_set_scalar(temp.get_mpq_t(),c->p.linterm[i].coeff.val.scalar,0);
      mpz_lcm(den.get_mpz_t(),den.get_mpz_t(),temp.get_den().get_mpz_t());
    }
    /* add variable coefficients * den */
    for (size_t i=0;i<c->size;i++) {
      ap_mpq_set_scalar(temp.get_mpq_t(),c->p.linterm[i].coeff.val.scalar,0);
      temp *= den;
      r += Variable(c->p.linterm[i].dim) * temp.get_num();
    }
  }
  /* add constant coefficient * den */
  ap_mpq_set_scalar(temp.get_mpq_t(),c->cst.val.scalar,0);
  temp *= den;
  r += temp.get_num();
}

/* ap_lincons0_t => Constraint */
void ap_ppl_of_lincons(Constraint& r,const ap_lincons0_t* c,bool allow_strict)
{
  Linear_Expression l;
  mpz_class den;
  ap_ppl_of_linexpr(l,den,c->linexpr0);
  switch (c->constyp) {
  case AP_CONS_SUPEQ: r = ( l >= 0 ); break;
  case AP_CONS_EQ:    r = ( l == 0 ); break;
  case AP_CONS_SUP:   if (allow_strict) r = ( l > 0 ); else r = (l >= 0); break;
  default: 
    throw invalid_argument("Constraint type in ap_ppl_of_lincons");
  }
}

/* ap_lincons0_array_t => Constraint_System */
void ap_ppl_of_lincons_array(Constraint_System& r,const ap_lincons0_array_t* a,bool allow_strict)
{
  size_t i;
  Constraint c = Constraint::zero_dim_positivity();
  r.clear();
  for (i=0;i<a->size;i++) {
    ap_ppl_of_lincons(c,&a->p[i],allow_strict);
    r.insert(c);
  }
}

/* ap_generator0_t => Generator */
void ap_ppl_of_generator(Generator& r,const ap_generator0_t* c)
{
  Linear_Expression l;
  mpz_class den;
  ap_ppl_of_linexpr(l,den,c->linexpr0);
  switch (c->gentyp) {
  case AP_GEN_VERTEX: r = Generator::point(l,den); break;
  case AP_GEN_RAY:    r = Generator::ray(l); break;
  case AP_GEN_LINE:   r = Generator::line(l); break;
  default: 
    throw invalid_argument("Generator type in ap_ppl_of_generator");
  }
}

/* ap_generator0_array_t => Generator_System */
void ap_ppl_of_generator_array(Generator_System& r,const ap_generator0_array_t* a)
{
  size_t i;
  Generator c = Generator::zero_dim_point();
  r.clear();
  for (i=0;i<a->size;i++) {
    ap_ppl_of_generator(c,&a->p[i]);
    r.insert(c);
  }
}
