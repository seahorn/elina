/*
 * apron_ppl.cc
 *
 * APRON Library / PPL library wrapper
 *
 * Wrappers specific to the Polyhedron PPL classes.
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

/* 
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#include <assert.h>
#include "apron_ppl.h"
#include "ppl_user.h"

#include <ppl.hh>
using namespace std;
using namespace Parma_Polyhedra_Library;

/* wrapper: adds initdim & strictness info */
class PPL_Poly {

 public:
  Polyhedron* p;
  size_t intdim;
  bool strict;

  PPL_Poly(const PPL_Poly& x) 
    : intdim(x.intdim), strict(x.strict) 
  {
    if (strict) p = new NNC_Polyhedron(*(NNC_Polyhedron*)x.p);
    else p = new C_Polyhedron(*(C_Polyhedron*)x.p);
  }

  PPL_Poly(size_t intdim,size_t realdim,Degenerate_Element kind,bool strict)
    : intdim(intdim), strict(strict)
  {
    try {
      if (strict) p = new NNC_Polyhedron(intdim+realdim,kind);
      else p = new C_Polyhedron(intdim+realdim,kind);
    }
    catch (std::logic_error e) {
      intdim = 0;
      if (strict) p = new NNC_Polyhedron(1,kind);
      else p = new C_Polyhedron(1,kind);
    }
  }

  ~PPL_Poly() { delete p; }
};


/* error handlers */

/* returns a polyhedron, of specified size if possible */
#define CATCH_WITH_DIM(funid,intdim,realdim,strict)			\
  catch (cannot_convert w) {						\
    /* bailing out, not an error */					\
    man->result.flag_exact = man->result.flag_best = tbool_top;		\
    return new PPL_Poly(intdim,realdim,UNIVERSE,strict);		\
  }									\
  catch (std::logic_error e) {						\
    /* actual error */							\
    ap_manager_raise_exception(man,AP_EXC_INVALID_ARGUMENT,funid,e.what()); \
    return new PPL_Poly(intdim,realdim,UNIVERSE,strict);		\
  }

/* returns a polyhedron, with size compatible with poly if possible */
#define CATCH_WITH_POLY(funid,poly)					\
  CATCH_WITH_DIM(funid,poly->intdim,poly->p->space_dimension()-poly->intdim,poly->strict)

/* returns v */
#define CATCH_WITH_VAL(funid,v)						\
  catch (cannot_convert w) {						\
    /* bailing out, not an error */					\
    man->result.flag_exact = man->result.flag_best = tbool_top;		\
    return v;								\
  }									\
  catch (std::logic_error e) {						\
    /* actual error */							\
    ap_manager_raise_exception(man,AP_EXC_INVALID_ARGUMENT,funid,e.what()); \
    return v;								\
  }

/* returns nothing */
#define CATCH_WITH_VOID(funid) CATCH_WITH_VAL(funid,)

/* prints message */
#define CATCH_WITH_MSG(funid)						\
  catch (cannot_convert w) {						\
    /* bailing out, not an error */					\
    fprintf(stream,"!exception!");					\
  }									\
  catch (std::logic_error e) {						\
    /* actual error */							\
    ap_manager_raise_exception(man,AP_EXC_INVALID_ARGUMENT,funid,e.what()); \
    fprintf(stream,"!exception!");					\
  }


extern "C"
PPL_Poly* ap_ppl_poly_copy(ap_manager_t* man, const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    return new PPL_Poly(*a); 
  }
  CATCH_WITH_POLY(AP_FUNID_COPY,a);
}

extern "C" 
void ap_ppl_poly_free(ap_manager_t* man, PPL_Poly* a)
{ 
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    delete a; 
  }
  CATCH_WITH_VOID(AP_FUNID_FREE);
}

extern "C" 
size_t ap_ppl_poly_size(ap_manager_t* man, const PPL_Poly* a)
{ 
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    return a->p->total_memory_in_bytes(); 
  }
  CATCH_WITH_VAL(AP_FUNID_ASIZE,0);
}

extern "C" 
void ap_ppl_poly_minimize(ap_manager_t* man, const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    /* the calls force in-place minimisation */
    (void)a->p->minimized_constraints();
    (void)a->p->minimized_generators();
  }
  CATCH_WITH_VOID(AP_FUNID_MINIMIZE);
}

/* NOT IMPLEMENTED! */
extern "C" 
void ap_ppl_poly_canonicalize(ap_manager_t* man, const PPL_Poly* a)
{
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_CANONICALIZE,"not implemented");
}

/* NOT IMPLEMENTED! */
extern "C" 
void ap_ppl_poly_approximate(ap_manager_t* man, PPL_Poly* a, int algorithm)
{
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_APPROXIMATE,"not implemented");
}

extern "C" 
void ap_ppl_poly_fprint(FILE* stream,
			ap_manager_t* man,
			const PPL_Poly* a,
			char** name_of_dim)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    ap_lincons0_array_t ar = ap_ppl_to_lincons_array(a->p->minimized_constraints());
    ap_lincons0_array_fprint(stream,&ar,name_of_dim);
    ap_lincons0_array_clear(&ar);
  }
  CATCH_WITH_MSG(AP_FUNID_FPRINT);
}

/* NOT IMPLEMENTED! */
extern "C" 
void ap_ppl_poly_fprintdiff(FILE* stream,
			     ap_manager_t* man,
			     const PPL_Poly* a1, const PPL_Poly* a2,
			     char** name_of_dim)
{
  fprintf(stream,"not implemented");
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_FPRINTDIFF,"not implemented");
}

/* fdump will print the unminimized contraint & generator systems */
extern "C" 
void ap_ppl_poly_fdump(FILE* stream, ap_manager_t* man, const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    /* dump constraints */
    fprintf(stream,"constraints: ");
    ap_lincons0_array_t ar = ap_ppl_to_lincons_array(a->p->constraints());
    ap_lincons0_array_fprint(stream,&ar,NULL);
    ap_lincons0_array_clear(&ar);
    /* dump generators */
    fprintf(stream,"generators: ");
    bool inexact = false;
    ap_generator0_array_t ar2 = 
      ap_ppl_to_generator_array(a->p->generators(),inexact);
    if (inexact) fprintf(stream,"(inexact) ");
    ap_generator0_array_fprint(stream,&ar2,NULL);
    ap_generator0_array_clear(&ar2);
  }
  CATCH_WITH_MSG(AP_FUNID_FPRINTDIFF);
}

/* NOT IMPLEMENTED! */
extern "C" 
ap_membuf_t ap_ppl_poly_serialize_raw(ap_manager_t* man, const PPL_Poly* a)
{
  ap_membuf_t membuf;
  membuf.ptr = NULL;
  membuf.size = 0;
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_SERIALIZE_RAW,"not implemented");
  return membuf;
}

/* NOT IMPLEMENTED! */
extern "C" 
PPL_Poly* ap_ppl_poly_deserialize_raw(ap_manager_t* man, void* ptr, size_t* size)
{  
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_DESERIALIZE_RAW,"not implemented");
  return NULL;
}

static
PPL_Poly* ap_ppl_poly_bottom(ap_manager_t* man, size_t intdim, size_t realdim,
			     bool strict)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    return new PPL_Poly(intdim,realdim,EMPTY,strict);
  }
  CATCH_WITH_DIM(AP_FUNID_BOTTOM,intdim,realdim,strict);
}

static
PPL_Poly* ap_ppl_poly_top(ap_manager_t* man, size_t intdim, size_t realdim, bool strict)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    return new PPL_Poly(intdim,realdim,UNIVERSE,strict);
  }
  CATCH_WITH_DIM(AP_FUNID_TOP,intdim,realdim,strict);
}

static
PPL_Poly* ap_ppl_poly_of_box(ap_manager_t* man,
			     size_t intdim, size_t realdim,
			     const ap_interval_t*const* tinterval,
			     bool strict)
{
  man->result.flag_exact = man->result.flag_best = 
    intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = new PPL_Poly(intdim,realdim,UNIVERSE,strict);
    Constraint_System c;
    if (ap_ppl_of_box(c,intdim+realdim,tinterval))
      man->result.flag_exact = man->result.flag_best = tbool_top;
    r->p->add_constraints(c);
    return r;
  }
  CATCH_WITH_DIM(AP_FUNID_OF_BOX,intdim,realdim,strict);
}

static
PPL_Poly* ap_ppl_poly_of_lincons_array(ap_manager_t* man,
				       size_t intdim, size_t realdim,
				       const ap_lincons0_array_t* array,
				       bool strict)
{
  man->result.flag_exact = man->result.flag_best = 
    intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = new PPL_Poly(intdim,realdim,UNIVERSE,strict);
    Constraint_System c;
    /* if strict=false, c will not contain any strict constraint */
    if (ap_ppl_of_lincons_array(c,array,strict))
      man->result.flag_exact = man->result.flag_best = tbool_top;
    r->p->add_constraints(c);
    return r;
  }
  CATCH_WITH_DIM(AP_FUNID_OF_LINCONS_ARRAY,intdim,realdim,strict);
}



/* C_Polyhedron versions */

extern "C" 
PPL_Poly* ap_ppl_cpoly_bottom(ap_manager_t* man, size_t intdim, size_t realdim)
{
  return ap_ppl_poly_bottom(man,intdim,realdim,false);
}

extern "C" 
PPL_Poly* ap_ppl_cpoly_top(ap_manager_t* man, size_t intdim, size_t realdim)
{
  return ap_ppl_poly_top(man,intdim,realdim,false);
}

extern "C" 
PPL_Poly* ap_ppl_cpoly_of_box(ap_manager_t* man,
			      size_t intdim, size_t realdim,
			      const ap_interval_t*const* tinterval)
{
  return ap_ppl_poly_of_box(man,intdim,realdim,tinterval,false);
}

extern "C" 
PPL_Poly* ap_ppl_cpoly_of_lincons_array(ap_manager_t* man,
					size_t intdim, size_t realdim,
					const ap_lincons0_array_t* array)
{
  return ap_ppl_poly_of_lincons_array(man,intdim,realdim,array,false);
}


/* NNC_Polyhedron versions */

extern "C" 
PPL_Poly* ap_ppl_nncpoly_bottom(ap_manager_t* man, size_t intdim, size_t realdim)
{
  return ap_ppl_poly_bottom(man,intdim,realdim,true);
}

extern "C" 
PPL_Poly* ap_ppl_nncpoly_top(ap_manager_t* man, size_t intdim, size_t realdim)
{
  return ap_ppl_poly_top(man,intdim,realdim,true);
}

extern "C" 
PPL_Poly* ap_ppl_nncpoly_of_box(ap_manager_t* man,
				size_t intdim, size_t realdim,
				const ap_interval_t*const* tinterval)
{
  return ap_ppl_poly_of_box(man,intdim,realdim,tinterval,true);
}

extern "C" 
PPL_Poly* ap_ppl_nncpoly_of_lincons_array(ap_manager_t* man,
					  size_t intdim, size_t realdim,
					  const ap_lincons0_array_t* array)
{
  return ap_ppl_poly_of_lincons_array(man,intdim,realdim,array,true);
}


/* functions common to NNC_Polyhedron and C_Polyhedron */

extern "C" 
ap_dimension_t ap_ppl_poly_dimension(ap_manager_t* man, const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  ap_dimension_t d = { 0, 0 };
  try {
    d.intdim = a->intdim;
    d.realdim = a->p->space_dimension()-a->intdim;
    return d;
  }
  CATCH_WITH_VAL(AP_FUNID_DIMENSION,d);
}

extern "C" 
tbool_t ap_ppl_poly_is_bottom(ap_manager_t* man, const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  try {
    return a->p->is_empty() ? tbool_true : (a->intdim ? tbool_top : tbool_false);
  }
  CATCH_WITH_VAL(AP_FUNID_IS_BOTTOM,tbool_top);
}

extern "C" 
tbool_t ap_ppl_poly_is_top(ap_manager_t* man, const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    return a->p->is_universe() ? tbool_true : tbool_false;
  }
  CATCH_WITH_VAL(AP_FUNID_IS_TOP,tbool_top);
}

extern "C" 
tbool_t ap_ppl_poly_is_leq(ap_manager_t* man, const PPL_Poly* a1, 
			   const PPL_Poly* a2)
{
  man->result.flag_exact = man->result.flag_best = 
    a1->intdim ? tbool_top : tbool_true;
  try {
    return a2->p->contains(*a1->p) ? tbool_true : (a1->intdim ? tbool_top : tbool_false);
  }
  CATCH_WITH_VAL(AP_FUNID_IS_LEQ,tbool_top);
}

extern "C" 
tbool_t ap_ppl_poly_is_eq(ap_manager_t* man, const PPL_Poly* a1, 
			  const PPL_Poly* a2)
{
  if (a1->intdim) man->result.flag_exact = man->result.flag_best = tbool_top;
  else man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    return *a1->p == *a2->p ? tbool_true : (a1->intdim ? tbool_top : tbool_false);
  }
  CATCH_WITH_VAL(AP_FUNID_IS_EQ,tbool_top);
}

extern "C" 
tbool_t ap_ppl_poly_sat_lincons(ap_manager_t* man, const PPL_Poly* a, 
				const ap_lincons0_t* lincons)
{
  man->result.flag_exact = man->result.flag_best = tbool_top;
  try {
    Constraint c = Constraint::zero_dim_positivity();
    /* may throw cannot_convert, which will be caught by CATCH_WITH_VAL */
    /* also, we allow strict constraint even if a->strict=false */
    bool inexact = ap_ppl_of_lincons(c,lincons,true);
    if (a->p->relation_with(c) == Poly_Con_Relation::is_included()) {
      if (inexact) return tbool_top;
      return tbool_true;
    }
    if (a->intdim) return tbool_top;
    return tbool_false;
  }
  CATCH_WITH_VAL(AP_FUNID_SAT_LINCONS,tbool_top);
}

extern "C" 
tbool_t ap_ppl_poly_sat_interval(ap_manager_t* man, const PPL_Poly* a,
				 ap_dim_t dim, const ap_interval_t* i)
{
  man->result.flag_exact = man->result.flag_best = tbool_top;
  try {
    mpq_class temp;
    /* inf */
    bool inexact = ap_mpq_set_scalar(temp.get_mpq_t(),i->inf,GMP_RNDD);
    Constraint c = ( temp.get_den() * Variable(dim) >= temp.get_num() );
    if (a->p->relation_with(c) != Poly_Con_Relation::is_included())
      return a->intdim ? tbool_top :  tbool_false;
    if (inexact) {
      ap_mpq_set_scalar(temp.get_mpq_t(),i->inf,GMP_RNDU);
      c = ( temp.get_den() * Variable(dim) >= temp.get_num() );
      if (a->p->relation_with(c) != Poly_Con_Relation::is_included())
	return tbool_top;
    }
    /* sup */
    inexact = ap_mpq_set_scalar(temp.get_mpq_t(),i->sup,GMP_RNDU);
    c = temp.get_den() * Variable(dim) <= temp.get_num();
    if (a->p->relation_with(c) != Poly_Con_Relation::is_included()) 
      return a->intdim ? tbool_top : tbool_false;
    if (inexact) {
       ap_mpq_set_scalar(temp.get_mpq_t(),i->sup,GMP_RNDD);
       c = temp.get_den() * Variable(dim) <= temp.get_num();
       if (a->p->relation_with(c) != Poly_Con_Relation::is_included()) 
	 return tbool_top;
    }   
    return tbool_true;
  }
  CATCH_WITH_VAL(AP_FUNID_SAT_INTERVAL,tbool_top);
}

extern "C" 
tbool_t ap_ppl_poly_is_dimension_unconstrained(ap_manager_t* man, 
					       const PPL_Poly* a, 
					       ap_dim_t dim)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    Generator g = Generator::line(Variable(dim));
    return a->p->relation_with(g) == Poly_Gen_Relation::subsumes() ? tbool_true : tbool_false;
  }
  CATCH_WITH_VAL(AP_FUNID_IS_DIMENSION_UNCONSTRAINED,tbool_top);
}

extern "C" 
ap_interval_t* ap_ppl_poly_bound_linexpr(ap_manager_t* man,
					 const PPL_Poly* a, 
					 const ap_linexpr0_t* expr)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  ap_interval_t* r = ap_interval_alloc();
  try {
    if (a->p->is_empty()) {
      /* empty */
      ap_interval_set_bottom(r);
    }
    else {
      /* not empty */
      Coefficient sup_n,sup_d,inf_n,inf_d;
      Linear_Expression l;
      mpz_class den;
      bool ok;
      /* may throw cannot_convert, which will be caught by CATCH_WITH_VAL */
      ap_ppl_of_linexpr(l,den,expr);
      /* sup bound */
      if (a->p->maximize(l,sup_n,sup_d,ok)) {
	sup_d *= den;
	ap_ppl_mpz2_to_scalar(r->sup,sup_n,sup_d);
      }
      else ap_scalar_set_infty(r->sup,1);
      /* inf bound */
      if (a->p->minimize(l,inf_n,inf_d,ok)) {
	inf_d *= den;
	ap_ppl_mpz2_to_scalar(r->inf,inf_n,inf_d);
      }
      else ap_scalar_set_infty(r->inf,-1);
    }
    return r;
  }
  CATCH_WITH_VAL(AP_FUNID_BOUND_LINEXPR,(ap_interval_set_top(r),r));
}

/* utility shared by XXX_bound_dimension & XXX_to_box (exact) */
static void ap_ppl_poly_bound_dim(ap_interval_t* r,const PPL_Poly* a,int dim)
{
  Coefficient sup_n,sup_d,inf_n,inf_d;
  Linear_Expression l = Variable(dim);
  bool ok;
  /* sup bound */
  if (a->p->maximize(l,sup_n,sup_d,ok)) 
    ap_ppl_mpz2_to_scalar(r->sup,sup_n,sup_d);
  else ap_scalar_set_infty(r->sup,1);
  /* inf bound */
  if (a->p->minimize(l,inf_n,inf_d,ok)) 
    ap_ppl_mpz2_to_scalar(r->inf,inf_n,inf_d);
  else ap_scalar_set_infty(r->inf,-1);
}

extern "C" 
ap_interval_t* ap_ppl_poly_bound_dimension(ap_manager_t* man,
					   const PPL_Poly* a, ap_dim_t dim)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  ap_interval_t* r = ap_interval_alloc();
  try {
    if (a->p->is_empty()) ap_interval_set_bottom(r); /* empty */
    else ap_ppl_poly_bound_dim(r,a,dim);         /* not empty */
    return r;
  }
  CATCH_WITH_VAL(AP_FUNID_BOUND_DIMENSION,(ap_interval_set_top(r),r));
}

extern "C" 
ap_lincons0_array_t ap_ppl_poly_to_lincons_array(ap_manager_t* man, 
						 const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  try {
    return ap_ppl_to_lincons_array(a->p->constraints());
  }
  CATCH_WITH_VAL(AP_FUNID_TO_LINCONS_ARRAY,ap_lincons0_array_make(0));
}

extern "C" 
ap_interval_t** ap_ppl_poly_to_box(ap_manager_t* man, const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  size_t dim = a->p->space_dimension();
  ap_interval_t** in = ap_interval_array_alloc(dim);
  try {
    if (a->p->is_empty())
      /* empty */
      for (size_t i=0;i<dim;i++)
	ap_interval_set_bottom(in[i]);
    else
      /* not empty */
      for (size_t i=0;i<dim;i++)
	ap_ppl_poly_bound_dim(in[i],a,i);
    return in;
  }
  CATCH_WITH_VAL(AP_FUNID_TO_BOX,ap_ppl_box_universe(in,dim));
}

extern "C" 
ap_generator0_array_t ap_ppl_poly_to_generator_array(ap_manager_t* man, 
						     const PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  try {
    bool inexact = false;
    ap_generator0_array_t r = 
      ap_ppl_to_generator_array(a->p->generators(),inexact);
    if (inexact) man->result.flag_exact = man->result.flag_best = tbool_top;
    return r;
  }
  CATCH_WITH_VAL(AP_FUNID_TO_GENERATOR_ARRAY,ap_ppl_generator_universe(a->p->space_dimension()));
}

extern "C" 
PPL_Poly* ap_ppl_poly_meet(ap_manager_t* man, bool destructive, 
			   PPL_Poly* a1, const PPL_Poly* a2)
{
  man->result.flag_exact = man->result.flag_best = 
    a1->intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = destructive ? a1 : new PPL_Poly(*a1);
    r->p->intersection_assign(*a2->p);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_MEET,a1);
}
 
extern "C" 
PPL_Poly* ap_ppl_poly_join(ap_manager_t* man, bool destructive, 
			   PPL_Poly* a1, const PPL_Poly* a2)
{
  man->result.flag_exact = tbool_top;
  man->result.flag_best = a1->intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = destructive ? a1 : new PPL_Poly(*a1);
    r->p->poly_hull_assign(*a2->p);
    return r; 
  }
  CATCH_WITH_POLY(AP_FUNID_JOIN,a1);
}

extern "C" 
PPL_Poly* ap_ppl_poly_meet_array(ap_manager_t* man, 
				 const PPL_Poly*const* tab, size_t size)
{
  assert(size>=1);
  man->result.flag_exact = man->result.flag_best = 
    tab[0]->intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = new PPL_Poly(*tab[0]);
    for (size_t i=1;i<size;i++)
      r->p->intersection_assign(*tab[i]->p);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_MEET_ARRAY,tab[0]);
}

extern "C" 
PPL_Poly* ap_ppl_poly_join_array(ap_manager_t* man, 
				 const PPL_Poly*const* tab, size_t size)
{  
  assert(size>=1);
  man->result.flag_exact = tbool_top;
  man->result.flag_best = tab[0]->intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = new PPL_Poly(*tab[0]);
    for (size_t i=1;i<size;i++)
      r->p->poly_hull_assign(*tab[i]->p);
  return r;
  }
  CATCH_WITH_POLY(AP_FUNID_JOIN_ARRAY,tab[0]);
}

extern "C" 
PPL_Poly* ap_ppl_poly_meet_lincons_array(ap_manager_t* man,
					 bool destructive,
					 PPL_Poly* a,
					 const ap_lincons0_array_t* array)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    Constraint_System c;
    /* when a->strict=false, c will not contain any strict constraint */
    if (ap_ppl_of_lincons_array(c,array,a->strict))
      man->result.flag_exact = man->result.flag_best = tbool_top;
    r->p->add_recycled_constraints(c);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_MEET_LINCONS_ARRAY,a);
}

extern "C" 
PPL_Poly* ap_ppl_poly_add_ray_array(ap_manager_t* man,
				    bool destructive,
				    PPL_Poly* a,
				    const ap_generator0_array_t* array)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    Generator_System c;
    ap_ppl_of_generator_array(c,array);
    r->p->add_recycled_generators(c);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_ADD_RAY_ARRAY,a);
}

extern "C" 
PPL_Poly* ap_ppl_poly_assign_linexpr(ap_manager_t* man,
				     bool destructive,
				     PPL_Poly* org,
				     ap_dim_t dim, const ap_linexpr0_t* expr,
				     const PPL_Poly* dest)
{
  man->result.flag_exact = man->result.flag_best = 
    org->intdim ? tbool_top : tbool_true;
  try {
    Linear_Expression e;
    mpz_class den;
    ap_ppl_of_linexpr(e,den,expr);
    PPL_Poly* r = destructive ? org : new PPL_Poly(*org);
    try {
      r->p->affine_image(Variable(dim),e,den);
    }
    catch (cannot_convert x) {
      /* defaults to forget */
      r->p->add_generator(Generator::line(Variable(dim)));
      man->result.flag_exact = man->result.flag_best = tbool_top;
    }
    if (dest) r->p->intersection_assign(*dest->p);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_ASSIGN_LINEXPR,org);
}

extern "C" 
PPL_Poly* ap_ppl_poly_substitute_linexpr(ap_manager_t* man,
					 bool destructive,
					 PPL_Poly* org,
					 ap_dim_t dim, 
					 const ap_linexpr0_t* expr,
					 const PPL_Poly* dest)
{
  man->result.flag_exact = man->result.flag_best = 
    org->intdim ? tbool_top : tbool_true;
  try {
    Linear_Expression e;
    mpz_class den;
    ap_ppl_of_linexpr(e,den,expr);
    PPL_Poly* r = destructive ? org : new PPL_Poly(*org);
    try {
      r->p->affine_preimage(Variable(dim),e,den);
    }
    catch (cannot_convert x) {
      /* defaults to forget */
      r->p->add_generator(Generator::line(Variable(dim)));
      man->result.flag_exact = man->result.flag_best = tbool_top;
    }
    if (dest) r->p->intersection_assign(*dest->p);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_SUBSTITUTE_LINEXPR,org);
}

extern "C" 
PPL_Poly* ap_ppl_poly_assign_linexpr_array(ap_manager_t* man,
					   bool destructive,
					   PPL_Poly* org,
					   const ap_dim_t* tdim,
					   const ap_linexpr0_t*const* texpr,
					   size_t size,
					   const PPL_Poly* dest)
{
  man->result.flag_exact = man->result.flag_best = 
    org->intdim ? tbool_top : tbool_true;
  try {
    int dim = org->p->space_dimension();
    Linear_Expression e;
    mpz_class den;
    PPL_Poly* r = destructive ? org : new PPL_Poly(*org);
    /* create temp */
    r->p->add_space_dimensions_and_embed(size);
    /* assign expression to temp */
    for (size_t i=0;i<size;i++) {
      try {
	ap_ppl_of_linexpr(e,den,texpr[i]);
	r->p->affine_image(Variable(dim+i),e,den);
      } 
      catch (cannot_convert x) {
	/* defaults to forget (nothing to do, actually) */
	man->result.flag_exact = man->result.flag_best = tbool_top;
      }
    }
    /* copy temp back to original */
    for (size_t i=0;i<size;i++)
      r->p->affine_image(Variable(tdim[i]),Linear_Expression(Variable(dim+i)));
    /* remove temp */
    r->p->remove_higher_space_dimensions(dim);
    /* intersect */
    if (dest) r->p->intersection_assign(*dest->p);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_ASSIGN_LINEXPR_ARRAY,org);
}

extern "C" 
PPL_Poly* ap_ppl_poly_substitute_linexpr_array(ap_manager_t* man,
					       bool destructive,
					       PPL_Poly* org,
					       const ap_dim_t* tdim,
					       const ap_linexpr0_t*const* texpr,
					       size_t size,
					       const PPL_Poly* dest)
{
  man->result.flag_exact = man->result.flag_best = 
    org->intdim ? tbool_top : tbool_true;
  try {
    int dim = org->p->space_dimension();
    Linear_Expression e;
    mpz_class den;
    PPL_Poly* r = destructive ? org : new PPL_Poly(*org);
    /* create temp */
    r->p->add_space_dimensions_and_embed(size);
    /* susbstitute org with temp variables */
    for (size_t i=0;i<size;i++)
      r->p->affine_preimage(Variable(tdim[i]),Linear_Expression(Variable(dim+i)));
    /* perform substitutions */
    for (size_t i=0;i<size;i++) {
      try {
	ap_ppl_of_linexpr(e,den,texpr[i]);
	r->p->affine_preimage(Variable(dim+i),e,den);
      }
      catch (cannot_convert x) {
	/* defaults to forget (nothing to do, actually) */
	man->result.flag_exact = man->result.flag_best = tbool_top;
      }
    }
    /* remove temp */
    r->p->remove_higher_space_dimensions(dim);
    /* intersect */
    if (dest) r->p->intersection_assign(*dest->p);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY,org);
}

  
extern "C" 
PPL_Poly* ap_ppl_poly_forget_array(ap_manager_t* man,
				   bool destructive,
				   PPL_Poly* a, 
				   const ap_dim_t* tdim, size_t size,
				   bool project)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    for (size_t i=0;i<size;i++)
      r->p->add_generator(Generator::line(Variable(tdim[i])));
    if (project) {
      for (size_t i=0;i<size;i++)
	r->p->add_constraint(Variable(tdim[i])==0);
    }
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_FORGET_ARRAY,a);
}

/* partial functions used in map_space_dimensions */
class ap_ppl_map {
 protected:
  size_t dom,codom,*tab;
  char* def;
 public:
  ap_ppl_map(size_t dom,size_t codom) : dom(dom), codom(codom)
  { 
    tab = new size_t [dom];
    def = new char [dom];
    memset(def,0,dom);
  }
  ~ap_ppl_map() { delete[] tab; delete[] def; }
  void set(dimension_type i,dimension_type j) { tab[i] = j; def[i] = 1; }
  bool has_empty_codomain() const { return false; }
  dimension_type max_in_codomain() const { return codom-1; }
  bool maps(dimension_type i,dimension_type& j) const { j = tab[i]; return def[i]; }
  void do_map(PPL_Poly* r) const { r->p->map_space_dimensions(*this); }
};

extern "C" 
PPL_Poly* ap_ppl_poly_add_dimensions(ap_manager_t* man,
				     bool destructive,
				     PPL_Poly* a,
				     ap_dimchange_t* dimchange,
				     bool project)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  size_t adddim = dimchange->intdim+dimchange->realdim;
  size_t olddim = a->p->space_dimension();
  try {
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    /* add dimensions */
    if (project) r->p->add_space_dimensions_and_project(adddim);
    else r->p->add_space_dimensions_and_embed(adddim);
    /* reorder dimensions */
    ap_ppl_map map = ap_ppl_map(olddim+adddim,olddim+adddim);
    size_t i,j=0;
    for (i=0;i<adddim;i++) {
      for (;j<dimchange->dim[i];j++)
	map.set(j,i+j);
      map.set(olddim+i,i+j);
    }
    for (;j<olddim;j++)
      map.set(j,i+j);
    map.do_map(r);
    r->intdim += dimchange->intdim;
    return r;
  }
  CATCH_WITH_DIM(AP_FUNID_ADD_DIMENSIONS,a->intdim+dimchange->intdim,olddim+dimchange->realdim-a->intdim,a->strict);
}

extern "C" 
PPL_Poly* ap_ppl_poly_remove_dimensions(ap_manager_t* man,
					bool destructive,
					PPL_Poly* a,
					ap_dimchange_t* dimchange)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  size_t deldim = dimchange->intdim+dimchange->realdim;
  size_t olddim = a->p->space_dimension();
  try {
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    /* reorder & drop dimensions */
    ap_ppl_map map = ap_ppl_map(olddim,olddim-deldim);
    for (size_t j=0,i=0;j<olddim;j++)
      if (i<deldim && j==dimchange->dim[i]) i++;
      else map.set(j,j-i);
    map.do_map(r);
    r->intdim -= dimchange->intdim;
    return r;
  }
  CATCH_WITH_DIM(AP_FUNID_REMOVE_DIMENSIONS,a->intdim-dimchange->intdim,olddim-dimchange->realdim-a->intdim,a->strict);
}

extern "C" 
PPL_Poly* ap_ppl_poly_permute_dimensions(ap_manager_t* man,
					 bool destructive,
					 PPL_Poly* a,
					 const ap_dimperm_t* perm)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    ap_ppl_map map = ap_ppl_map(perm->size,perm->size);
    for (size_t i=0;i<perm->size;i++)
      map.set(i,perm->dim[i]);
    map.do_map(r);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_PERMUTE_DIMENSIONS,a);
}

extern "C" 
PPL_Poly* ap_ppl_poly_expand(ap_manager_t* man,
			     bool destructive,
			     PPL_Poly* a,
			     ap_dim_t dim,
			     size_t n)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    size_t olddim = a->p->space_dimension();
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    r->p->expand_space_dimension(Variable(dim),n);
    if (dim<r->intdim) {
      /* expanded an integer dimension => needs some reordering */
      ap_ppl_map map = ap_ppl_map(olddim+n,olddim+n);
      size_t i;
      for (i=0;i<r->intdim;i++)
	map.set(i,i);
      for (i=0;i<n;i++)
	map.set(olddim+i,r->intdim+i);
      for (i=r->intdim;i<olddim;i++)
	map.set(i,i+n);
      map.do_map(r);
      r->intdim += n;
    }
    return r;
  }
  CATCH_WITH_DIM(AP_FUNID_EXPAND,a->intdim,a->p->space_dimension()-a->intdim+n,a->strict);
}

extern "C" 
PPL_Poly* ap_ppl_poly_fold(ap_manager_t* man,
			   bool destructive,
			   PPL_Poly* a,
			   const ap_dim_t* tdim,
			   size_t size)
{
  man->result.flag_exact = man->result.flag_best = 
    a->intdim ? tbool_top : tbool_true;
  try {
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    Variables_Set s;
    assert(size>0);
    for (size_t i=1;i<size;i++)
      s.insert(Variable(tdim[i]));
    r->p->fold_space_dimensions(s,Variable(tdim[0]));
    if (tdim[0]<a->intdim) r->intdim -= size-1;
    return r;
  }
  CATCH_WITH_DIM(AP_FUNID_FOLD,a->intdim,a->p->space_dimension()-a->intdim-size,a->strict);
}

extern "C" 
PPL_Poly* ap_ppl_poly_widening(ap_manager_t* man,
			       const PPL_Poly* a1, 
			       const PPL_Poly* a2)
{
  man->result.flag_exact = man->result.flag_best = tbool_top;
  int algo = man->option.funopt[AP_FUNID_WIDENING].algorithm;
  try {
    PPL_Poly* r = new PPL_Poly(*a2);
    if (algo>0) r->p->BHRZ03_widening_assign(*a1->p);
    else r->p->H79_widening_assign(*a1->p);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_WIDENING,a1);
}

extern "C" 
PPL_Poly* ap_ppl_poly_widening_threshold(ap_manager_t* man,
					 const PPL_Poly* a1, 
					 const PPL_Poly* a2,
					 ap_lincons0_array_t* array)
{
  man->result.flag_exact = man->result.flag_best = tbool_top;
  int algo = man->option.funopt[AP_FUNID_WIDENING].algorithm;
  try {
    Constraint_System c;
    /* when a1->strict=false, c will not contain any strict constraint */
    ap_ppl_of_lincons_array(c,array,a1->strict);
    PPL_Poly* r = new PPL_Poly(*a2);
    if (algo>2) r->p->bounded_BHRZ03_extrapolation_assign(*a1->p,c);
    else if (algo>1) r->p->limited_BHRZ03_extrapolation_assign(*a1->p,c);
    else if (algo>0) r->p->bounded_H79_extrapolation_assign(*a1->p,c);
    else r->p->limited_H79_extrapolation_assign(*a1->p,c);
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_WIDENING,a1);
}

ap_abstract0_t*
ap_abstract0_ppl_poly_widening_thresholds(ap_manager_t* man,
					  const ap_abstract0_t* a1,
					  const ap_abstract0_t* a2,
					  ap_lincons0_array_t* array)
{
  arg_assert(man->library==a1->man->library &&
             man->library==a2->man->library,
             return ap_ppl_invalid_abstract0(man,a1);,
	     AP_FUNID_WIDENING);
  return ap_ppl_make_abstract0(man,ap_ppl_poly_widening_threshold(man,(PPL_Poly*)a1->value,(PPL_Poly*)a2->value,array));
}

extern "C" 
PPL_Poly* ap_ppl_poly_closure(ap_manager_t* man, bool destructive, PPL_Poly* a)
{
  man->result.flag_exact = man->result.flag_best = tbool_true;
  try {
    PPL_Poly* r = destructive ? a : new PPL_Poly(*a);
    r->p->topological_closure_assign();
    return r;
  }
  CATCH_WITH_POLY(AP_FUNID_CLOSURE,a);
}




/* ********************************************************************** */
/* Managers */
/* ********************************************************************** */

extern "C" void ap_ppl_poly_internal_free(void* internal)
{
}

extern "C" ap_manager_t* ap_ppl_poly_manager_alloc(bool strict)
{
  int i;
  ap_manager_t* man;

  man = ap_manager_alloc("PPL::Polyhedron", PPL_VERSION, 
			 NULL, &ap_ppl_poly_internal_free);
  assert(man);

  man->funptr[AP_FUNID_COPY] = (void*)ap_ppl_poly_copy;
  man->funptr[AP_FUNID_FREE] = (void*)ap_ppl_poly_free;
  man->funptr[AP_FUNID_ASIZE] = (void*)ap_ppl_poly_size;
  man->funptr[AP_FUNID_MINIMIZE] = (void*)ap_ppl_poly_minimize;
  man->funptr[AP_FUNID_CANONICALIZE] = (void*)ap_ppl_poly_canonicalize;
  man->funptr[AP_FUNID_APPROXIMATE] = (void*)ap_ppl_poly_approximate;
  man->funptr[AP_FUNID_FPRINT] = (void*)ap_ppl_poly_fprint;
  man->funptr[AP_FUNID_FPRINTDIFF] = (void*)ap_ppl_poly_fprintdiff;
  man->funptr[AP_FUNID_FDUMP] = (void*)ap_ppl_poly_fdump;
  man->funptr[AP_FUNID_SERIALIZE_RAW] = (void*)ap_ppl_poly_serialize_raw;
  man->funptr[AP_FUNID_DESERIALIZE_RAW] = (void*)ap_ppl_poly_deserialize_raw;
  if (strict) {
    man->funptr[AP_FUNID_BOTTOM] = (void*)ap_ppl_nncpoly_bottom;
    man->funptr[AP_FUNID_TOP] = (void*)ap_ppl_nncpoly_top;
    man->funptr[AP_FUNID_OF_BOX] = (void*)ap_ppl_nncpoly_of_box;
    man->funptr[AP_FUNID_OF_LINCONS_ARRAY] = (void*)ap_ppl_nncpoly_of_lincons_array;
  }
  else {
    man->funptr[AP_FUNID_BOTTOM] = (void*)ap_ppl_cpoly_bottom;
    man->funptr[AP_FUNID_TOP] = (void*)ap_ppl_cpoly_top;
    man->funptr[AP_FUNID_OF_BOX] = (void*)ap_ppl_cpoly_of_box;
    man->funptr[AP_FUNID_OF_LINCONS_ARRAY] = (void*)ap_ppl_cpoly_of_lincons_array;
  }
  man->funptr[AP_FUNID_DIMENSION] = (void*)ap_ppl_poly_dimension;
  man->funptr[AP_FUNID_IS_BOTTOM] = (void*)ap_ppl_poly_is_bottom;
  man->funptr[AP_FUNID_IS_TOP] = (void*)ap_ppl_poly_is_top;
  man->funptr[AP_FUNID_IS_LEQ] = (void*)ap_ppl_poly_is_leq;
  man->funptr[AP_FUNID_IS_EQ] = (void*)ap_ppl_poly_is_eq;
  man->funptr[AP_FUNID_IS_DIMENSION_UNCONSTRAINED] = (void*)ap_ppl_poly_is_dimension_unconstrained;
  man->funptr[AP_FUNID_SAT_INTERVAL] = (void*)ap_ppl_poly_sat_interval;
  man->funptr[AP_FUNID_SAT_LINCONS] = (void*)ap_ppl_poly_sat_lincons;
  man->funptr[AP_FUNID_BOUND_DIMENSION] = (void*)ap_ppl_poly_bound_dimension;
  man->funptr[AP_FUNID_BOUND_LINEXPR] = (void*)ap_ppl_poly_bound_linexpr;
  man->funptr[AP_FUNID_TO_BOX] = (void*)ap_ppl_poly_to_box;
  man->funptr[AP_FUNID_TO_LINCONS_ARRAY] = (void*)ap_ppl_poly_to_lincons_array;
  man->funptr[AP_FUNID_TO_GENERATOR_ARRAY] = (void*)ap_ppl_poly_to_generator_array;
  man->funptr[AP_FUNID_MEET] = (void*)ap_ppl_poly_meet;
  man->funptr[AP_FUNID_MEET_ARRAY] = (void*)ap_ppl_poly_meet_array;
  man->funptr[AP_FUNID_MEET_LINCONS_ARRAY] = (void*)ap_ppl_poly_meet_lincons_array;
  man->funptr[AP_FUNID_JOIN] = (void*)ap_ppl_poly_join;
  man->funptr[AP_FUNID_JOIN_ARRAY] = (void*)ap_ppl_poly_join_array;
  man->funptr[AP_FUNID_ADD_RAY_ARRAY] = (void*)ap_ppl_poly_add_ray_array;
  man->funptr[AP_FUNID_ASSIGN_LINEXPR] = (void*)ap_ppl_poly_assign_linexpr;
  man->funptr[AP_FUNID_ASSIGN_LINEXPR_ARRAY] = (void*)ap_ppl_poly_assign_linexpr_array;
  man->funptr[AP_FUNID_SUBSTITUTE_LINEXPR] = (void*)ap_ppl_poly_substitute_linexpr;
  man->funptr[AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY] = (void*)ap_ppl_poly_substitute_linexpr_array;
  man->funptr[AP_FUNID_ADD_DIMENSIONS] = (void*)ap_ppl_poly_add_dimensions;
  man->funptr[AP_FUNID_REMOVE_DIMENSIONS] = (void*)ap_ppl_poly_remove_dimensions;
  man->funptr[AP_FUNID_PERMUTE_DIMENSIONS] = (void*)ap_ppl_poly_permute_dimensions;
  man->funptr[AP_FUNID_FORGET_ARRAY] = (void*)ap_ppl_poly_forget_array;
  man->funptr[AP_FUNID_EXPAND] = (void*)ap_ppl_poly_expand;
  man->funptr[AP_FUNID_FOLD] = (void*)ap_ppl_poly_fold;
  man->funptr[AP_FUNID_WIDENING] = (void*)ap_ppl_poly_widening;
  man->funptr[AP_FUNID_CLOSURE] = (void*)ap_ppl_poly_closure;

  for (i=0;i<AP_EXC_SIZE;i++)
    ap_manager_set_abort_if_exception(man,(ap_exc_t)i,false);

  return man;
}
