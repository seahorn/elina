/* ********************************************************************** */
/* itv.c: abstract lattice of intervals */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>
#include <gmp.h>


#include "manager.h"
#include "expr0.h"

#include "num.h"
#include "bound.h"
#include "itv_util.h"
#include "itvint.h"
#include "itv.h"

/* ********************************************************************** */
/* I. General management */
/* ********************************************************************** */

/* ============================================================ */
/* I.1 Memory */
/* ============================================================ */
/* Return a copy of an abstract value, on
   which destructive update does not affect the initial value. */
itv_t* itv_copy(manager_t* man, itv_t* a)
{
  int i;
  size_t nbdims = a->intdim+a->realdim;

  itv_t* b = _itv_alloc(a->intdim,a->realdim);
  if (a->p){
    b->p = malloc(nbdims*sizeof(itv_interval_t));
    for (i=0; i<nbdims; i++){
      itv_interval_init_set(b->p[i],a->p[i]);
    }
  }
  return b;
}

/* Free all the memory used by the abstract value */
void itv_free(manager_t* man, itv_t* a)
{
  if (a->p){
    itv_interval_array_free(a->p,a->intdim+a->realdim);
    a->p = NULL;
  }
  free(a);
}

/* Return the abstract size of an abstract value (see manager_t) */
size_t itv_size(manager_t* man, itv_t* a)
{
  return 2*(a->intdim+a->realdim);
}


/* ============================================================ */
/* I.2 Control of internal representation */
/* ============================================================ */

/* Minimize the size of the representation of a.
   This may result in a later recomputation of internal information.
*/
void itv_minimize(manager_t* man, itv_t* a)
{
  return;
}

/* Put the abstract value in canonical form. (not yet clear definition) */
void itv_canonicalize(manager_t* man, itv_t* a)
{
  if (a->p && a->intdim>0){
    /* Check integer dimensions */
    int i;
    bool exc;

    for (i=0;i<a->intdim+a->realdim;i++){
      exc = itv_interval_canonicalize(a->p[i],i < a->intdim);
      if (exc){
	_itv_fill_bottom(a);
	break;
      }
    }
  }
}

/* Perform some transformation on the abstract value, guided by the
   field algorithm.

   The transformation may lose information.  The argument "algorithm"
   overrides the field algorithm of the structure of type foption_t
   associated to itv_approximate (commodity feature). */
void itv_approximate(manager_t* man, itv_t* a, int algorithm)
{
  return;
}

tbool_t itv_is_minimal(manager_t* man, itv_t* a)
{
  return tbool_true;
}
tbool_t itv_is_canonical(manager_t* man, itv_t* a)
{
  return tbool_true;
}

/* ============================================================ */
/* I.3 Printing */
/* ============================================================ */

/* Print the abstract value in a pretty way, using function
   name_of_dim to name dimensions */
void itv_print(FILE* stream,
	       manager_t* man,
	       itv_t* a,
	       char*(*name_of_dim)(dim_t dim))
{
  int i;
  size_t nbdims = a->intdim + a->realdim;

  fprintf(stream,"interval of dim (%d,%d):",
	  a->intdim,a->intdim);
  if (a->p){
    fprintf(stream,"\n");
    for(i=0; i<nbdims; i++){
      fprintf(stream,"%8s in ",name_of_dim((dim_t)i));
      itv_interval_fprint(stream,a->p[i]);
      fprintf(stream,"\n");
    }
  }
  else {
    fprintf(stream,nbdims>0 ? " bottom\n" : "top\n");
  }
}

/* Dump the internal representation of an abstract value,
   for debugging purposes */
void itv_dump(FILE* stream,
	      manager_t* man,
	      itv_t* a)
{
  int i;
  size_t nbdims = a->intdim + a->realdim;

  fprintf(stream,"interval of dim (%d,%d):",
	  a->intdim,a->intdim);
  if (a->p){
    fprintf(stream,"\n");
    for(i=0; i<nbdims; i++){
      fprintf(stream,"dim %3d in ",i);
      itv_interval_fprint(stream,a->p[i]);
      fprintf(stream,"\n");
    }
  }
  else {
    fprintf(stream, nbdims>0 ? " bottom\n" : "top\n");
  }
}

/* Print the difference between a1 (old value) and a2 (new value),
   using function name_of_dim to name dimensions.
   The meaning of difference is library dependent. */
void itv_printdiff(FILE* stream,
		   manager_t* man,
		   itv_t* a, itv_t* b,
		   char*(*name_of_dim)(dim_t dim))
{
  int i;
  size_t nbdims;
  char* str;

  if (manager_check_dim_bin(man,a,b,"itv_printdiff"))
    return;

  nbdims = a->intdim + a->realdim;

  fprintf(stream,"diff of 2 intervals of dim (%d,%d)",
	  a->intdim,b->intdim);
  if (itv_is_eq(man,a,b)){
    fprintf(stream,": none\n");
  }
  else {
    /* we are sure that nbdims>0 */
    if (a->p==0){
      fprintf(stream,"\nbottom =>\n");
      itv_print(stream,man,b,name_of_dim);
    }
    else {
      fprintf(stream,"\n");
      if (b->p==0){
	itv_print(stream,man,a,name_of_dim);
	fprintf(stream,"=> bottom\n");
      }
      else {
	for(i=0; i<nbdims; i++){
	int sgn1 = bound_cmp(a->p[i]->inf, b->p[i]->inf);
	int sgn2 = bound_cmp(a->p[i]->sup, b->p[i]->sup);

	if (sgn1!=0 || sgn2!=0){
	  fprintf(stream,"%8s in ",name_of_dim((dim_t)i));
	  itv_interval_fprint(stream,a->p[i]);
	  fprintf(stream," => ");
	  str =
	    sgn1>0 ?
	    "+[" :
	    ( sgn1<0 ?
	      "-[" :
	      "[=, "  );
	  fprintf(stream,str);
	  if (sgn1!=0){
	    bound_fprint(stream,b->p[i]->inf);
	    fprintf(stream,", ");
	  }
	  if (sgn2!=0){
	    bound_fprint(stream,b->p[i]->sup);
	  }
	  str =
	    sgn2>0 ?
	    "]+" :
	    ( sgn2<0 ?
	      "]-" :
	      "=]"  );
	  fprintf(stream,"%s\n",str);
	}
	else {
	  fprintf(stream,"%8s in [=,=]\n",name_of_dim((dim_t)i));
	}
	}
      }
    }
  }
}


/* ============================================================ */
/* I.4 Serialization */
/* ============================================================ */

/* Allocate a memory buffer (with malloc), output the abstract value in raw
   binary format to it and return a pointer on the memory buffer and the size
   of bytes written.  It is the user responsability to free the memory
   afterwards (with free). */
membuf_t itv_serialize_raw(manager_t* man, itv_t* a)
{
  membuf_t buf;
  manager_raise_exception(man,EXC_NOT_IMPLEMENTED,"itv_serialize_raw");
  buf.ptr = NULL;
  buf.size = 0;
  return buf;
}

/* Return the abstract value read in raw binary format from the input stream
   and store in size the number of bytes read */
itv_t* itv_deserialize_raw(manager_t* man, void* ptr)
{
  manager_raise_exception(man,EXC_NOT_IMPLEMENTED,"itv_deserialize_raw");
  return NULL;
}

/* ********************************************************************** */
/* II. Constructor, accessors, tests and property extraction */
/* ********************************************************************** */

/* ============================================================ */
/* II.1 Basic constructors */
/* ============================================================ */

/* We assume that dimensions [0..intdim-1] correspond to integer variables, and
   dimensions [intdim..intdim+realdim-1] to real variables */

/* Create a bottom (empty) value */
itv_t* itv_bottom(manager_t* man, size_t intdim, size_t realdim)
{
  return _itv_alloc(intdim,realdim);
}

/* Create a top (universe) value */
itv_t* itv_top(manager_t* man, size_t intdim, size_t realdim)
{
  int i;
  itv_t* a = _itv_alloc(intdim,realdim);
  _itv_init(a);
  for(i=0;i<a->intdim+a->realdim; i++){
    itv_interval_set_top(a->p[i]);
  }
  return a;
}

/* Abstract an hypercube defined by the array of intervals
   of size intdim+realdim */
itv_t* itv_of_box(manager_t* man,
		  size_t intdim, size_t realdim,
		  interval_t* tinterval)
{
  int i;
  bool exc;
  coeff_t coeff;

  itv_t* a = _itv_alloc(intdim,realdim);
  if (intdim+realdim!=0){
    _itv_init(a);
    coeff_init_set(coeff,tinterval[0]->inf);
    for(i=0;i<intdim+realdim; i++){
      itv_interval_set_interval(a->p[i],tinterval[i],coeff);
      exc = itv_interval_canonicalize(a->p[i],i<intdim);
      if (exc) { _itv_fill_bottom(a); break; }
    }
    coeff_clear(coeff);
  }
  return a;
}

/* Abstract a convex polyhedra defined by the array of linear constraints
   of size size */
itv_t* itv_of_lincons_array(manager_t* man,
			    size_t intdim, size_t realdim,
			    lincons_array_t array)
{
  manager_raise_exception(man,EXC_NOT_IMPLEMENTED,"abstract_of_lincons_array");
  return itv_top(man,intdim,realdim);
}

/* ============================================================ */
/* II.2 Accessors */
/* ============================================================ */

/* Return the number of integer dimensions of the abstract values */
size_t itv_integer_dimension(manager_t* man, itv_t* a)
{ return a->intdim; }

/* Return the number of real dimensions of the abstract values */
size_t itv_real_dimension(manager_t* man, itv_t* a)
{ return a->realdim; }

/* Return the total number of dimensions of the abstract values */
size_t itv_dimension(manager_t* man, itv_t* a)
{ return a->intdim + a->realdim; }

/* ============================================================ */
/* II.3 Tests */
/* ============================================================ */

/* If any of the following functions returns tbool_top, this means that
   an exception has occured, or that the exact computation was
   considered too expensive to be performed (according to the options).
   The flag exact and best should be cleared in such a case. */

tbool_t itv_is_bottom(manager_t* man, itv_t* a)
{
  return tbool_of_bool(a->p==NULL && a->intdim + a->realdim>0);
}

tbool_t itv_is_top(manager_t* man, itv_t* a)
{
  int i;
  tbool_t res;
  size_t nbdims = a->intdim + a->realdim;
  if (nbdims==0)
    return tbool_true;

  if (a->p==NULL)
    return tbool_false;

  res = tbool_true;
  for (i=0;i<nbdims;i++){
    if (! itv_interval_is_top(a->p[i])){
      res = tbool_false;
      break;
    }
  }
  return res;
}

/* inclusion check */
tbool_t itv_is_leq(manager_t* man, itv_t* a, itv_t* b)
{
  int i;
  tbool_t res;
  size_t nbdims;

  if (manager_check_dim_bin(man,a,b,"itv_is_leq"))
    return tbool_top;

  nbdims = a->intdim + a->realdim;
  if (a->p==NULL)
    return tbool_true;

  if (b->p==NULL)
    return tbool_false;

  res = tbool_true;
  for (i=0;i<nbdims;i++){
    if (! itv_interval_is_leq(a->p[i],b->p[i])){
      res = tbool_false;
      break;
    }
  }
  return res;
}

/* equality check */
tbool_t itv_is_eq(manager_t* man, itv_t* a, itv_t* b)
{
  int i;
  tbool_t res;
  size_t nbdims;

  if (manager_check_dim_bin(man,a,b,"itv_is_eq"))
    return tbool_top;

  nbdims = a->intdim + a->realdim;
  if (a->p==NULL)
    return tbool_of_bool(b->p==NULL);
  else if (b->p==NULL)
    return tbool_false;

  res = tbool_true;
  for (i=0;i<nbdims;i++){
    if (! itv_interval_is_eq(a->p[i],b->p[i])){
      res = tbool_false;
      break;
    }
  }
  return res;
}

/* does the abstract value satisfy the linear constraint ? */
tbool_t itv_sat_lincons(manager_t* man, itv_t* a, lincons_t lincons)
{
  tbool_t res;
  internal_t* intern = (internal_t*)man->internal;

  if (a->p==NULL)
    return tbool_true;

  _itv_bound_linexpr(man, intern->sat_itvint, a, lincons.expr);
  switch (lincons.constyp){
  case CONS_EQ:
    res =
      (bound_sgn(intern->sat_itvint->inf)==0 &&
       bound_sgn(intern->sat_itvint->sup)==0) ?
      tbool_true :
      tbool_top;
    break;
  case CONS_SUPEQ:
    if (bound_sgn(intern->sat_itvint->inf)>=0)
      res = tbool_true;
    else if (bound_sgn(intern->sat_itvint->sup)<0)
      res = tbool_false;
    else
      res = tbool_top;
    break;
  case CONS_SUP:
     if (bound_sgn(intern->sat_itvint->inf)>0)
      res = tbool_true;
    else if (bound_sgn(intern->sat_itvint->sup)<=0)
      res = tbool_false;
    else
      res = tbool_top;
     break;
  default:
    assert(0);
    res = tbool_top;
  }
  return res;
}

/* is the dimension included in the interval in the abstract value ? */
tbool_t itv_sat_interval(manager_t* man, itv_t* a,
			 dim_t dim, interval_t interval)
{
  if (manager_check_dim_dim(man,a,dim,"itv_sat_interval"))
    return tbool_top;

  if (a->p==NULL)
    return tbool_true;

  return tbool_of_bool(itv_interval_is_leq_interval(a->p[dim],interval));
}

/* ============================================================ */
/* II.4 Extraction of properties */
/* ============================================================ */

struct interval_t itv_bound_dimension(manager_t* man,
			       itv_t* a, dim_t dim)
{
  interval_t interval;

  interval_init(interval, COEFF_MPQ);
  if (manager_check_dim_dim(man,a,dim,"itv_bound_dimension")){
    interval_set_top(interval);
  }
  else if (a->p==NULL){
    interval_set_bottom(interval);
  }
  else {
    interval_set_itvinterval(interval,a->p[dim]);
  }
  return *interval;
}

/* Returns the interval taken by a linear expression
   over the abstract value */
struct interval_t itv_bound_linexpr(manager_t* man,
			     itv_t* a, linexpr_t* expr)
{
  interval_t interval;
  internal_t* intern = (internal_t*)man->internal;

  interval_init(interval,COEFF_MPQ);
  if (manager_check_dim_linexpr(man,a,expr,"itv_bound_linexpr")){
    interval_set_top(interval);
  }
  else if (a->p==NULL){
    interval_set_bottom(interval);
  }
  else {
    _itv_bound_linexpr(man,intern->bound2_itvint,a,expr);
    interval_set_itvinterval(interval,intern->bound2_itvint);
  }
  return *interval;
}

/* Converts an abstract value to a polyhedra
   (conjunction of linear constraints).
   The size of the returned array is stored in size. */
lincons_array_t itv_to_lincons_array(manager_t* man, itv_t* a)
{
  int i;
  lincons_array_t array;
  size_t nbdims = a->intdim + a->realdim;

  if (nbdims==0){
    array = lincons_array_alloc(0);
  }
  else if (a->p==NULL){
    array = lincons_array_alloc(1);
    array.p[0] = lincons_make_unsat();
  }
  else {
    size_t size;
    linexpr_t* expr;
    
    size = 0;
    for (i=0;i<nbdims;i++){
      if (!bound_infty(a->p[i]->inf)) size++;
      if (!bound_infty(a->p[i]->sup)) size++;
    }
    array = lincons_array_alloc(size);
    size = 0;
    for (i=0;i<nbdims;i++){
      if (!bound_infty(a->p[i]->inf)){
	expr = linexpr_alloc(CST_COEFF,LINEXPR_SPARSE,1);
	expr->p.linterm[0].dim = i;
	coeff_set_int(expr->p.linterm[0].coeff, 1);
	

	coeff_set_num(cst_coeffref(expr->cst),bound_numref(a->p[i]->inf),+1);
	coeff_neg(cst_coeffref(expr->cst),
		  cst_coeffref(expr->cst));
	array.p[size].constyp = CONS_SUPEQ;
	array.p[size].expr = expr;
	size++;
      }
      if (!bound_infty(a->p[i]->sup)){
	expr = linexpr_alloc(CST_COEFF,LINEXPR_SPARSE,1);
	expr->p.linterm[0].dim = i;
	coeff_set_int(expr->p.linterm[0].coeff, -1);
	coeff_set_num(cst_coeffref(expr->cst),a->p[i]->sup,+1);
	array.p[size].constyp = CONS_SUPEQ;
	array.p[size].expr = expr;
	size++;
      }
    }
  }
  return array;
}

/* Converts an abstract value to an interval/hypercube.
   The size of the resulting array is itv_dimension(man,a).  This
   function can be reimplemented by using itv_bound_linexpr */
interval_t* itv_to_box(manager_t* man, itv_t* a)
{
  int i;
  interval_t* interval;
  size_t nbdims;

  nbdims = a->intdim+a->realdim;
  if (nbdims==0){
    interval = NULL;
  }
  else {
    interval = malloc(nbdims*sizeof(interval_t));
     for (i=0; i<nbdims; i++){
       interval_init(interval[i],COEFF_MPQ);
       if (a->p==NULL){
	 interval_set_top(interval[i]);
       } else {
	 interval_set_itvinterval(interval[i],a->p[i]);
       }
     }
  }
  return interval;
}

/* ********************************************************************** */
/* III. Operations: functional version */
/* ********************************************************************** */

/* ============================================================ */
/* III.1 Meet and Join */
/* ============================================================ */
itv_t* itv_meet(manager_t* man, itv_t* a1, itv_t* a2)
{
  itv_t* res = _itv_alloc(a1->intdim,a1->realdim);
  _itv_meet("meet",man,res,a1,a2);
  return res;
}

itv_t* itv_join(manager_t* man, itv_t* a1, itv_t* a2)
{
  itv_t* res = _itv_alloc(a1->intdim,a1->realdim);
  _itv_meet("meet",man,res,a1,a2);
  return res;
}

/* Meet and Join of an array of abstract values
   Raises an [[exc_invalid_argument]] exception if [[size==0]]
   (no way to define the dimensionality of the result in such a case */
itv_t* itv_meet_array(manager_t* man, itv_t** tab, size_t size)
{
  size_t i,j,nbdims;
  bool exc;
  itv_t* res;

  if (manager_check_dim_array(man,tab,size,"itv_meet_array")){
    if (size==0){
      res = _itv_alloc(0,1);
    } else {
      res = _itv_alloc(tab[0]->intdim,tab[0]->realdim);
    }
    _itv_fill_top(res);
    return res;
  }

  res = _itv_alloc(tab[0]->intdim,tab[0]->realdim);
  /* if one is bottom, exit */
  for (i=0; i<size; i++){
    if (tab[i]->p==NULL){
      _itv_fill_bottom(res);
      return res;
    }
  }
  /* None is bottom */
  nbdims = tab[0]->intdim + tab[0]->realdim;
  _itv_copy(res,tab[0]);
  for (i=1; i<size; i++){
    /* meet(res,res,tab[i]) */
    for (j=0;j<nbdims; j++){
      exc = itv_interval_meet(res->p[j],res->p[j],tab[i]->p[j]);
      if (exc){
	_itv_fill_bottom(res);
	break;
      }
    }
  }
  return res;
}

itv_t* itv_join_array(manager_t* man, itv_t** tab, size_t size)
{
  size_t i,j,nbdims;
  itv_t* res;

  if (manager_check_dim_array(man,tab,size,"itv_join_array")){
    if (size==0){
      res = _itv_alloc(0,1);
    } else {
      res = _itv_alloc(tab[0]->intdim,tab[0]->realdim);
    }
    _itv_fill_top(res);
    return res;
  }

  res = _itv_alloc(tab[0]->intdim,tab[0]->realdim);
  nbdims = tab[0]->intdim + tab[0]->realdim;
  _itv_copy(res,tab[0]);
  for (i=1; i<size; i++){
    /* join(res,res,tab[i]) */
    if (res->p==NULL){
      _itv_copy(res,tab[i]);
    }
    else if (tab[i]->p!=NULL){
      for (j=0;j<nbdims; j++){
	itv_interval_join(res->p[j],res->p[j],tab[i]->p[j]);
      }
    }
  }
  return res;
}

itv_t* itv_meet_lincons(manager_t* man,
			itv_t* a, lincons_t cons)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  _itv_meet_lincons("meet_lincons",man,res,a,cons);
  return res;
 }
/* Meet of an abstract value with a set of constraints
   (generalize itv_of_lincons_array) */
itv_t* itv_meet_lincons_array(manager_t* man,
			      itv_t* a,
			      lincons_array_t array)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  _itv_meet_lincons_array("meet_lincons_array",man,res,a,array);
  return res;
}

/* Generalized time elapse operator */
itv_t* itv_add_ray_array(manager_t* man,
			  itv_t* a,
			  generator_array_t array)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  _itv_add_ray_array("itv_add_ray__array",man,res,a,array);
  return res;
}

/* ============================================================ */
/* III.2 Assignement and Substitutions */
/* ============================================================ */
/* Assignement and Substitution of a single dimension by resp.
   a linear expression and a interval linear expression */
itv_t* itv_assign_linexpr(manager_t* man,
			  itv_t* a,
			  dim_t dim, linexpr_t* expr)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  _itv_assign_linexpr("abstract_assign_linexpr",man,res,a,dim,expr);
  return res;
}
itv_t* itv_substitute_linexpr(manager_t* man,
					itv_t* a,
					dim_t dim, linexpr_t* expr)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  _itv_substitute_linexpr("abstract_substitute_linexpr",man,res,a,dim,expr);
  return res;
}
/* Parallel Assignement and Substitution of several dimensions by
   linear expressons. */
itv_t* itv_assign_linexpr_array(manager_t* man,
				itv_t* a,
				dim_t* tdim,
				linexpr_t** texpr,
				size_t size)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  _itv_assign_linexpr_array("abstract_assign_linexpr_array",
			    man,res,a,tdim, texpr, size);
  return res;
}
itv_t* itv_substitute_linexpr_array(manager_t* man,
				    itv_t* a,
				    dim_t* tdim,
				    linexpr_t** texpr,
				    size_t size)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  manager_raise_exception(man,EXC_NOT_IMPLEMENTED,"abstract_substitute_linexpr_array");
  _itv_fill_top(res);
  return res;
}

/* ============================================================ */
/* III.3 Projections */
/* ============================================================ */

itv_t* itv_project(manager_t* man, itv_t* a, dim_t dim)
{
  dim_t tdim[1];
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  tdim[0] = dim;
  _itv_projectforget_array("abstract_project",true,
			    man, res,a, tdim, 1);
  return res;
}
itv_t* itv_forget(manager_t* man, itv_t* a, dim_t dim)
{
  dim_t tdim[1];
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  tdim[0] = dim;
  _itv_projectforget_array("abstract_forget",false,
			   man, res,a, tdim, 1);
  return res;
}
itv_t* itv_project_array(manager_t* man,
			 itv_t* a, dim_t* tdim, size_t size)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  _itv_projectforget_array("abstract_forget",true,
			   man, res, a, tdim, size);
  return res;
}
itv_t* itv_forget_array(manager_t* man,
			itv_t* a, dim_t tdim, size_t size)
{
  itv_t* res = _itv_alloc(a->intdim,a->realdim);
  _itv_projectforget_array("abstract_forget",false,
			   man, res, a, tdim, size);
  return res;
}


/* ============================================================ */
/* III.4 Change and permutation of dimensions */
/* ============================================================ */

/* ------------------------------------------------------------ */
/* III.4a Add at the end */
/* ------------------------------------------------------------ */

itv_t* itv_add_dimensions_and_project(manager_t* man,
						itv_t* a,
						size_t intdim, size_t realdim);
itv_t* itv_add_dimensions_and_forget(manager_t* man,
					       itv_t* a,
					       size_t intdim, size_t realdim);
  /* Adds intdim (resp. realdim) dimensions at the end of integer (resp. real)
     variables range */

itv_t* itv_remove_dimensions(manager_t* man,
				       itv_t* a, size_t intdim, size_t realdim);
  /* Removes intdim (resp. realdim) dimensions at the end of integer
     (resp. real) variables range. Implicitly project (or forget)
     the removed dimensions. */

/* ------------------------------------------------------------ */
/* III.4b Add anywhere and permute */
/* ------------------------------------------------------------ */

/* Provides necessary stuff for level 1 to automatically adjust abstract values
   defined on different variables, e.g. A1(x,y,z) inter A2(z,y,w) */

itv_t* itv_add_permute_dim_and_forget(manager_t* man,
						itv_t* a,
						size_t intdim, size_t realdim,
						int* permutation);
itv_t* itv_add_permute_dim_and_project(manager_t* man,
						 itv_t* a,
						 size_t intdim, size_t realdim,
						 int* permutation);
  /* Adds intdim (resp. realdim) dimensions at the end of integer (resp. real)
     variables range, and then permutes dimensions with i -> permutation[i].
     Assumption:
     1. permutation is of size the dimension on the resulting abstract value
     2. integer dimensions should remain before real dimensions.

     Example: if we have resp. a vertex  [b0,b1,b2,b3] and a constraint
     a0 x0 + a1 x1 + a2 x2 + a3 x3 >= 0 in an abstract value a, to which
     is applied

     itv_add_permute_dim_and_project(man,a, 0, 3, [0,4,1,2,5,6,3]),

     the vertecs becomes [b0,0,b1,b2,0,0,b3] and the constraint becomes
     a0 x0 + a1 x2 + a2 x3 + a3 x7 >= 0.

     Conversely, if we apply

     itv_permute_remove_dim(man,a, 0, 2, [2,0,3,1]),

     the vertecs becomes [b1,b3].
  */

itv_t* itv_permute_remove_dim(manager_t* man,
					itv_t* a,
					size_t intdim, size_t realdim,
					int* permutation);
  /* Permutes the dimensions with i -> permutation[i], and then
     remove the last dimensions */

/* ============================================================ */
/* III.5 Expansion and folding of dimensions */
/* ============================================================ */

itv_t* itv_expand_dim(manager_t* man,
				itv_t* a,
				dim_t dim,
				size_t n);
  /* Expand the dimension dim into itself + n additional dimensions.
     It results in (n+1) unrelated dimensions having same
     relations with other dimensions. The (n+1) dimensions are put as follows:

     - original dimension dim

     - if the dimension is integer, the n additional dimensions are put at the
       end of integer dimensions; if it is real, at the end of the real
       dimensions.
  */

itv_t* itv_fold_dim(manager_t* man,
			      itv_t* a,
			      dim_t* tdim,
			      size_t size);
  /* Fold the dimensions in the array tdim of size n>=1 and put the result
     in the first dimension in the array. The other dimensions of the array
     are then removed (using itv_permute_remove_dimensions). */

/* ============================================================ */
/* III.6 Widening */
/* ============================================================ */

/* Widening with threshold */
itv_t* itv_widening(manager_t* man,
		    itv_t* a1, itv_t* a2,
		    lincons_array_t array);

/* ============================================================ */
/* III.7 Closure operation */
/* ============================================================ */

/* Returns the topological closure of a possibly opened abstract value */

itv_t* itv_closure(manager_t* man, itv_t* a);
