/* ********************************************************************** */
/* pk_representation.c: General management of polyhedra  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk_cherni.h"
#include "pk_int.h"
#include "pk_user.h"
#include "pk_representation.h"
#include "pk_extract.h"
#include "pk_constructor.h"

/* ********************************************************************** */
/* I. Memory */
/* ********************************************************************** */

/* This internal function allocates a polyhedron and fills its records
   with null values. */

poly_t* poly_alloc(size_t intdim, size_t realdim)
{
  poly_t* po = (poly_t*)malloc(sizeof(poly_t));
  po->C = NULL;
  po->F = NULL;
  po->satC = NULL;
  po->satF = NULL;
  po->intdim = intdim;
  po->realdim = realdim;
  po->nbeq = 0;
  po->nbline = 0;
  po->status = 0;
  return po;
}

/* Clearing a polyhedron */
void poly_clear(poly_t* po)
{
  if (po->C) matrix_free(po->C);
  if (po->F) matrix_free(po->F);
  if (po->satC) satmat_free(po->satC);
  if (po->satF) satmat_free(po->satF);
  po->C = NULL;
  po->F = NULL;
  po->satC = NULL;
  po->satF = NULL;
  po->status = 0;
  po->nbeq = 0;
  po->nbline = 0;
}

/* Assignement with GMP semantics */
void poly_set(poly_t* pa, const poly_t* pb)
{
  if (pa!=pb){
    poly_clear(pa);
    pa->intdim = pb->intdim;
    pa->realdim = pb->realdim;
    pa->C = pb->C ? matrix_copy(pb->C) : NULL;
    pa->F = pb->F ? matrix_copy(pb->F) : NULL;
    pa->satC = pb->satC ? satmat_copy(pb->satC) : NULL;
    pa->satF = pb->satF ? satmat_copy(pb->satF) : NULL;
    pa->status = pb->status;
    pa->nbeq = pb->nbeq;
    pa->nbline = pb->nbline;
    pa->status = pb->status;
  }
}

/* Duplicate (recursively) a polyhedron. */
poly_t* poly_copy(ap_manager_t* man, const poly_t* po)
{
  poly_t* npo = poly_alloc(po->intdim,po->realdim);
  npo->C = po->C ? matrix_copy(po->C) : 0;
  npo->F = po->F ? matrix_copy(po->F) : 0;
  npo->satC = po->satC ? satmat_copy(po->satC) : 0;
  npo->satF = po->satF ? satmat_copy(po->satF) : 0;
  npo->nbeq = po->nbeq;
  npo->nbline = po->nbline;
  npo->status = po->status;
  return npo;
}

/* Finalization function for polyhedra, which frees
   recursively the members of the structure. */
void poly_free(ap_manager_t* man, poly_t* po)
{
  poly_clear(po);
  free(po);
}

/* Return the abstract size of a polydron, which is the number of
   coefficients of its current representation, possibly redundant. */
size_t poly_size(ap_manager_t* man, const poly_t* po)
{
  size_t s1,s2;

  s1 = (po->C) ? po->C->nbrows : 0;
  s2 = (po->F) ? po->F->nbrows : 0;
  return (s1+s2)*(po->intdim + po->realdim);
}

/* ********************************************************************** */
/* II. Control of internal representation */
/* ********************************************************************** */

/* ====================================================================== */
/* II.1 Dual representation */
/* ====================================================================== */

/* Minimization function, in the sense of minimized dual representation
This function minimizes if not already done the given polyhedron.

Transmit exception
*/

void poly_chernikova(ap_manager_t* man,
		     const poly_t* poly,
		     char* msg)
{
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  poly_t* po = (poly_t*)poly;
  if ((po->C && po->F) || (!po->C && !po->F)){
    return;
  }
  else {
    if (po->C){
      if ( !poly_is_conseps(pk,po) ){
	matrix_normalize_constraint(pk,po->C,po->intdim,po->realdim);
      }
      matrix_sort_rows(pk,po->C);
      cherni_minimize(pk,true,po);
      if (pk->exn) goto poly_chernikova_exit0;
      po->status |=
	poly_status_conseps |
	poly_status_consgauss |
	poly_status_gengauss;
    }
    else {
      po->C = po->F; po->F = NULL;
      matrix_sort_rows(pk,po->C);
      cherni_minimize(pk,false,po);
      poly_dual(po);
      if (pk->exn) goto poly_chernikova_exit0;
      po->status |=
	poly_status_consgauss |
	poly_status_gengauss;
      po->status &= ~poly_status_conseps;
    }
    po->status &= ~poly_status_minimal;
  }
  return;
 poly_chernikova_exit0:
  po->status = 0;
  {
    char str[160];
    sprintf(str,"conversion from %s %s\n",
	    po->C ? "constraints to generators" : "generators to constraints",
	    msg);
    ap_manager_raise_exception(man,pk->exn,pk->funid,str);
  }
  return;
}


/* Same as poly_chernikova, but if usual is false meaning of constraints and
   matrices exchanged. */

void poly_chernikova_dual(ap_manager_t* man,
			  const poly_t* poly,
			  char* msg,
			  bool usual)
{
  if ( (!poly->C && !poly->F) || (poly->C && poly->F) )
    return;
  else {
    poly_t* po = (poly_t*)poly;
    if (!usual) poly_dual(po);
    poly_chernikova(man,po,msg);
    if (!usual) poly_dual(po);
  }
}
/* Same as poly_chernikova, but in addition ensure normalized epsilon
   constraints. */

void poly_chernikova2(ap_manager_t* man,
		      const poly_t* poly,
		      char* msg)
{
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  poly_t* po = (poly_t*)poly;

  poly_chernikova(man,po,msg);
  if (pk->exn)
    return;
  if (!po->C && !po->F)
    return;
  if (!poly_is_conseps(pk,po)){
    // Normalize strict constraints of C
    bool change = matrix_normalize_constraint(pk,po->C,po->intdim,po->realdim);
    po->status |= poly_status_conseps;
    // If there were some change, perform a new minimization from normalized C
    if (change){
      if (po->F){ matrix_free(po->F); po->F = NULL; }
      if (po->satC){ satmat_free(po->satC); po->satC = NULL; }
      if (po->satF){ satmat_free(po->satF); po->satF = NULL; }
      matrix_sort_rows(pk,po->C);
      cherni_minimize(pk, true, po);
    }
  }
}

/* Same as poly_chernikova2, but in addition normalize matrices by Gauss
   elimination and sorting */
void poly_chernikova3(ap_manager_t* man,
		      const poly_t* poly,
		      char* msg)
{
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  poly_t* po = (poly_t*)poly;
  poly_chernikova2(man,po,msg);
  if (pk->exn)
    return;

  if (po->C){
    size_t rank;
    if (! po->status & poly_status_consgauss){
      rank = cherni_gauss(pk,po->C,po->nbeq);
      assert(rank==po->nbeq);
      cherni_backsubstitute(pk,po->C,rank);
      po->C->_sorted = false;
    }
    if (! po->status & poly_status_gengauss){
      rank = cherni_gauss(pk,po->F,po->nbline);
      assert(rank==po->nbline);
      cherni_backsubstitute(pk,po->F,rank);
      po->F->_sorted = false;
    }
    poly_obtain_sorted_C(pk,po);
    poly_obtain_sorted_F(pk,po);
    po->status |=
      poly_status_conseps |
      poly_status_consgauss |
      poly_status_gengauss;
    assert(poly_check(pk,po));
  }
}

/* ====================================================================== */
/* II.2 Canonical form */
/* ====================================================================== */

/* Put the polyhedron with minimized constraints and frames.  If in addition
   the integer man->option->canonicalize.algorithm is strictly positive,
   normalize equalities and lines, and also strict constraints */

void poly_canonicalize(ap_manager_t* man, const poly_t* poly)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_CANONICALIZE);
  poly_t* po = (poly_t*)poly;

  if (poly_is_canonical(man,po))
    return;

  if (pk->funopt->algorithm >= 1)
    poly_chernikova3(man,po,NULL);
  else
    poly_chernikova(man,po,NULL);

  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_exact = man->result.flag_best = tbool_false;
    return;
  }
  man->result.flag_exact = man->result.flag_best =
    po->intdim>0 && (po->C || po->F) ? tbool_top : tbool_true;
}

/* Minimize the size of the representation of the polyhedron */
void poly_minimize(ap_manager_t* man, const poly_t* poly)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_MINIMIZE);
  poly_t* po = (poly_t*)poly;

  if (po->status != poly_status_minimal || po->C || po->F){
    poly_chernikova2(man,po,NULL);
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      man->result.flag_exact = man->result.flag_best = tbool_false;
      return;
    }
    if (po->C || po->F){
      if (po->satC) satmat_free(po->satC);
      if (po->satF) satmat_free(po->satF);
      po->satC = po->satF = NULL;
      if (po->C->nbrows > po->F->nbrows){
	matrix_free(po->C);
	po->C = NULL;
	matrix_minimize(po->F);
	po->status &= ~poly_status_consgauss;
      }
      else {
	matrix_free(po->F);
	po->F = NULL;
	matrix_minimize(po->C);
	po->status &= ~poly_status_gengauss;
      }
      po->status |= poly_status_minimal;
    }
  }
  man->result.flag_exact = man->result.flag_best =
    po->intdim>0 && (po->C || po->F) ? tbool_top : tbool_true;
}

/* ====================================================================== */
/* II.3 Approximation */
/* ====================================================================== */

/* Approximation by removing constraints with too big coefficients */
bool matrix_approximate_constraint_1(pk_internal_t* pk, matrix_t* C)
{
  size_t i,j;
  bool change;
  change = false;
  for (i=0;i<C->nbrows; i++){
    if (numint_sgn(C->p[i][0])){
      for (j=1; j<C->nbcolumns; j++){
	if (numint_size(C->p[i][j]) > pk->approximate_max_coeff_size){
	  change = true;
	  C->nbrows--;
	  matrix_exch_rows(C,i,C->nbrows);
	  break;
	}
      }
    }
  }
  if (change){
    /* Add for safety positivity and strictness that may not be implied any more */
    size_t nbrows = C->nbrows;
    matrix_realloc_lazy(C,nbrows+pk->dec-1);
    _matrix_fill_constraint_top(pk,C,nbrows);
    C->_sorted = false;
  }
  return change;
}


void poly_set_save_C(poly_t* po, const poly_t* pa)
{
  if (po != pa){
    po->F = pa->F ? matrix_copy(pa->F) : NULL;
    po->satC = pa->satC ? satmat_copy(pa->satC) : NULL;
    po->satF = pa->satF ? satmat_copy(pa->satF) : NULL;
    po->status = pa->status;
      po->nbeq = pa->nbeq;
      po->nbline = pa->nbline;
      po->status = pa->status;
  }
}

bool _poly_approximate_n1(ap_manager_t* man, poly_t* po, const poly_t* pa, int algorithm)
{
  if (po->intdim>0){
    pk_internal_t* pk = (pk_internal_t*)man->internal;
    bool change;

    poly_obtain_C(man,pa,NULL);
    if (!pa->C){
      poly_set(po,pa);
      return false;
    }
    if (po!=pa){
      po->C = matrix_copy(pa->C);
    }
    change = matrix_normalize_constraint_int(pk,po->C,po->intdim,po->realdim);
    if (change){
      if (po==pa){
	if (po->F){ matrix_free(po->F); po->F = NULL; }
	if (po->satC){ satmat_free(po->satC); po->satC = NULL; }
	if (po->satF){ satmat_free(po->satF); po->satF = NULL; }
      }
      po->status = 0;
      po->C->_sorted = false;
      man->result.flag_exact = tbool_false;
    } else {
      poly_set_save_C(po,pa);
    }
    return change;
  }
  else
    return false;
}

bool _poly_approximate_1(ap_manager_t* man, poly_t* po, const poly_t* pa)
{
  bool change;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  poly_obtain_C(man,pa,NULL);
  if (!pa->C){
    poly_set(po,pa);
    return false;
  }
  if (po!=pa){
    po->C = matrix_copy(pa->C);
  }
  change = matrix_approximate_constraint_1(pk,po->C);
  if (change){
    if (po==pa){
      if (po->F){ matrix_free(po->F); po->F = NULL; }
      if (po->satC){ satmat_free(po->satC); po->satC = NULL; }
      if (po->satF){ satmat_free(po->satF); po->satF = NULL; }
    }
    po->status = 0;
    man->result.flag_exact = tbool_false;
  } else {
    poly_set_save_C(po,pa);
  }
  return change;
}

void poly_approximate_123(ap_manager_t* man, poly_t* po, int algorithm)
{
  bool change;
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  
  if (algorithm==1){
    _poly_approximate_1(man,po,po);
  }
  else {
    size_t nbrows, nbrows2;
    ap_dim_t dim;
    ap_dim_t i,j;
    int sgn, sgny;
    ap_coeff_t* coeff = NULL;

    poly_t* pa = poly_alloc(po->intdim,po->realdim);

    change = _poly_approximate_1(man,pa,po);
    if (!change){
      poly_free(man,pa);
      return;
    }
    poly_obtain_F(man,po,NULL);
    if (!po->F){
      if (!po->C){
	poly_free(man,pa);
	poly_set_bottom(pk,po);
	man->result.flag_exact = tbool_true;
	return;
      }
      else {
	poly_clear(po);
	*po = *pa;
	free(pa);
      }
      return;
    }
    dim = pa->intdim + pa->realdim;
    nbrows = pa->C->nbrows;
    coeff = ap_coeff_alloc(AP_COEFF_SCALAR);
    ap_coeff_reinit(coeff,AP_COEFF_SCALAR,AP_SCALAR_MPQ);
    if (algorithm>=2){ /* Add interval constraints */
      nbrows2 = 2*dim;
      matrix_realloc_lazy(pa->C, nbrows + nbrows2);
      pa->C->_sorted = false;
      for (i=0; i<dim;i++){
	for (sgn=-1; sgn<=1; sgn += 2){
	  matrix_bound_dimension(pk,
				 coeff->val.scalar->val.mpq,
				 i,po->F,
				 sgn);
	  
	  if (!ap_scalar_infty(coeff->val.scalar)){
	    vector_set_dim_bound(pk, pa->C->p[nbrows], 
				 i, coeff->val.scalar->val.mpq,
				 po->intdim, po->realdim, sgn, false);
	    nbrows++;
	  }
	}
      }
      pa->C->nbrows = nbrows;
    }
    if (algorithm>=3){ /* Add octagonal constraints */
      vector_clear(pk->poly_numintp,po->F->nbcolumns);
      numint_set_int(pk->poly_numintp[0],1);
      for (i=0; i<dim;i++){
	numint_set_int(pk->poly_numintp[pk->dec+i],1);
	nbrows2 = 2*(dim-i-1);
	matrix_realloc_lazy(pa->C, nbrows + nbrows2);
	for (j=i+1; j<dim;j++){
	  for (sgny=-1; sgny<=1; sgny += 2){
	    numint_set_int(pk->poly_numintp[pk->dec+j],sgny);
	    for (sgn=-1; sgn<=1; sgn += 2){
	      matrix_bound_linexpr(pk,
				   coeff->val.scalar->val.mpq,
				   (const numint_t*)pk->poly_numintp,
				   po->F,
				   sgn);
	      if (!ap_scalar_infty(coeff->val.scalar)){
		vector_set_linexpr_bound(pk, pa->C->p[nbrows], 
					 (const numint_t*)pk->poly_numintp,
					 coeff->val.scalar->val.mpq,
					 po->intdim, po->realdim, sgn, false);
		nbrows++;
	      }
	    }
	  }
	  numint_set_int(pk->poly_numintp[pk->dec+j],0);
	}
	numint_set_int(pk->poly_numintp[pk->dec+i],0);
      }
      pa->C->nbrows = nbrows;
    }   
    ap_coeff_free(coeff);
    poly_clear(po);
    *po = *pa;
    free(pa);
    return;
  }
}

/*
Approximation:

- valgorithm==0: do nothing

- algorithm==-1: normalize integer minimal constraints (induces a smaller
		 polyhedron)

- algorithm==1: remove constraints with coefficients of size greater than
	       pk->max_coeff_size/(intdim+realdim)), if pk->max_coeff_size > 0

*/
void poly_approximate(ap_manager_t* man, poly_t* po, int algorithm)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_APPROXIMATE);

  man->result.flag_exact = tbool_true;
  man->result.flag_best = tbool_top;

  if (!po->C && !po->F)
    return;

  switch (algorithm){
  case -1:
    if (po->intdim>0)
      _poly_approximate_n1(man,po,po,algorithm);
    break;
  case 1:
  case 2:
  case 3:
    if (pk->approximate_max_coeff_size>0)
      poly_approximate_123(man,po,algorithm);
    break;
  }
}

/* ====================================================================== */
/* II.4 Minimal representation */
/* ====================================================================== */

/* Is the polyhedron in a minimal representation ? */
tbool_t poly_is_minimal(ap_manager_t* man, const poly_t* po)
{
  pk_init_from_manager(man,AP_FUNID_IS_MINIMAL);
  if ( (!po->C && !po->F) ||
       (po->status & poly_status_minimal) )
    return tbool_true;
  else if (po->C && po->F)
    return tbool_false;
  else
    return tbool_top;
}

/* Is the polyhedron in a canonical representation ?
   (depends on the algorithm option of canonicalize) */
tbool_t poly_is_canonical(ap_manager_t* man, const poly_t* po)
{
  tbool_t res;

  pk_init_from_manager(man,AP_FUNID_IS_CANONICAL);
  if (!po->C && !po->F)
    res = tbool_true;
  else if (!po->C || !po->F)
    res = tbool_false;
  else {
    if (man->option.funopt[AP_FUNID_CANONICALIZE].algorithm <= 0){
      res = tbool_true;
    }
    else {
      pk_internal_t* pk = (pk_internal_t*)man->internal;
      if (po->C->_sorted && po->F->_sorted &&
	  po->status & poly_status_consgauss &&
	  po->status & poly_status_gengauss &&
	  poly_is_conseps(pk,po))
	res = tbool_true;
      else
	res = tbool_top;
    }
  }
  return res;
}

void poly_obtain_sorted_F(pk_internal_t* pk, const poly_t* poly)
{
  poly_t* po = (poly_t*)poly;

  assert (po->F);

  if (!matrix_is_sorted(po->F)){
    if (po->satC){
      if (po->satF){ satmat_free(po->satF); po->satF = 0; }
      matrix_sort_rows_with_sat(pk,po->F,po->satC);
    }
    else if (po->satF){
      po->satC = satmat_transpose(po->satF,po->F->nbrows);
      satmat_free(po->satF); po->satF = 0;
      matrix_sort_rows_with_sat(pk,po->F,po->satC);
    }
    else {
      matrix_sort_rows(pk,po->F);
    }
  }
}

void poly_obtain_sorted_C(pk_internal_t* pk, const poly_t* poly)
{
  poly_t* po = (poly_t*)poly;

  assert (po->C);

  if (!matrix_is_sorted(po->C)){
    if (po->satF){
      if (po->satC){ satmat_free(po->satC); po->satC = 0; }
      matrix_sort_rows_with_sat(pk,po->C,po->satF);
    }
    else if (po->satC){
      po->satF = satmat_transpose(po->satC,po->C->nbrows);
      satmat_free(po->satC); po->satC = 0;
      matrix_sort_rows_with_sat(pk,po->C,po->satF);
    }
    else {
      matrix_sort_rows(pk,po->C);
    }
  }
}

/* ********************************************************************** */
/* III Printing */
/* ********************************************************************** */

void poly_fprint(FILE* stream, ap_manager_t* man, const poly_t* po,
		 char** name_of_dim)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_FPRINT);

  poly_chernikova(man,po,NULL);
  if (!po->C && !po->F){
    assert(pk->exn == AP_EXC_NONE);
    fprintf(stream,"empty polyhedron of dim (%lu,%lu)\n",
	    (unsigned long)po->intdim,(unsigned long)po->realdim);
  }
  else {
    fprintf(stream,"polyhedron of dim (%lu,%lu)\n",
	    (unsigned long)po->intdim,(unsigned long)po->realdim);
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      fprintf(stream,"cannot display due to exception\n");
    }
    else {
      ap_lincons0_array_t cons = poly_to_lincons_array(man,po);
      ap_lincons0_array_fprint(stream,&cons,name_of_dim);
      ap_lincons0_array_clear(&cons);
    }
  }
}

void poly_fprintdiff(FILE* stream, ap_manager_t* man,
		     const poly_t* po1, const poly_t* po2,
		     char** name_of_dim)
{
  pk_init_from_manager(man,AP_FUNID_FPRINTDIFF);
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_FPRINTDIFF,NULL);
}

/* Raw printing function. */
void poly_fdump(FILE* stream, ap_manager_t* man, const poly_t* po)
{
  pk_init_from_manager(man,AP_FUNID_FDUMP);
  if (!po->C && !po->F)
    fprintf(stream,"empty polyhedron of dim (%lu,%lu)\n",
	    (unsigned long)po->intdim,(unsigned long)po->realdim);
  else {
    fprintf(stream,"polyhedron of dim (%lu,%lu)\n",
	    (unsigned long)po->intdim,(unsigned long)po->realdim);
    if (po->C){
      fprintf(stream,"Constraints: ");
      matrix_fprint(stream, po->C);
    }
    if (po->F){
      fprintf(stream,"Frames: ");
      matrix_fprint(stream, po->F);
    }
    if (po->satC){
      fprintf(stream,"satC: ");
      satmat_fprint(stream, po->satC);
    }
    if (po->satF){
      fprintf(stream,"satF: ");
      satmat_fprint(stream, po->satF);
    }
  }
}

/* ********************************************************************** */
/* IV. Serialization */
/* ********************************************************************** */

ap_membuf_t poly_serialize_raw(ap_manager_t* man, const poly_t* a)
{
  ap_membuf_t membuf;
  pk_init_from_manager(man,AP_FUNID_SERIALIZE_RAW);
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_SERIALIZE_RAW,NULL);
  membuf.ptr = NULL;
  membuf.size = 0;
  return membuf;
}
poly_t* poly_deserialize_raw(ap_manager_t* man, void* ptr, size_t* size)
{
  pk_init_from_manager(man,AP_FUNID_DESERIALIZE_RAW);
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_DESERIALIZE_RAW,NULL);
  return NULL;
}

/* ********************************************************************** */
/* V. Checking */
/* ********************************************************************** */

static bool matrix_check1(pk_internal_t* pk, const matrix_t* mat)
{
  int i;
  bool res;
  res = false;
  for (i = 0; i<mat->nbrows; i++){
    if (numint_sgn(mat->p[i][pk->dec-1])>0){
      res = true;
      break;
    }
  }
  return res;
}

static bool matrix_check2(pk_internal_t* pk, const matrix_t* mat)
{
  int i;
  bool res;
  numint_t gcd;
  numint_init(gcd);

  res = true;
  for (i=0; i<mat->nbrows; i++){
    vector_gcd(pk, &mat->p[i][1], mat->nbcolumns-1, gcd);
    if (numint_cmp_int(gcd,1)>0){
      res = false;
      break;
    }
  }
  numint_clear(gcd);
  return res;
}

static bool matrix_check3(pk_internal_t* pk, const matrix_t* mat)
{
  int i;
  bool res;

  if (mat->_sorted==false) 
    return true;

  res = true;
  for (i=0; i<mat->nbrows-1; i++){
    if (vector_compare(pk,mat->p[i],mat->p[i+1],mat->nbcolumns)>0){
      res = false;
      break;
    }
  }
  return res;
}


bool poly_check(pk_internal_t* pk, const poly_t* poly)
{
  bool res;
  size_t nbdim,nbcols;

  poly_t* po = (poly_t*)poly;
  nbdim = po->intdim + po->realdim;
  if (po->nbline+po->nbeq>nbdim){
    fprintf(stderr,"poly_check: nbline+nbeq>intdim+realdim\n");
    return false;
  }
  if (!po->C && !po->F)
    return true;

  nbcols = po->C ? po->C->nbcolumns : po->F->nbcolumns;
  if (nbcols != pk->dec+nbdim){
    fprintf(stderr,"poly_check: pk->dec+intdim+realdim != nbcols\n");
    return false;
  }
  if (po->C){
    res = matrix_check1(pk,po->C);
    if (!res){ /* should not arise */
      fprintf(stderr,"poly_check: unvalid constraint system, does not imply the positivity constraint\n");
      return false;
    }
    res = matrix_check2(pk,po->C);
    if (!res){
      fprintf(stderr,"poly_check: C not normalized\n");
      return false;
    }
    res = matrix_check3(pk,po->C);
    if (!res){
      fprintf(stderr,"poly_check: C not sorted although _sorted=true\n");
      return false;
    }
  }
  if (po->F){
    res = matrix_check1(pk,po->F);
    if (!res){ /* should not arise */
      fprintf(stderr,"poly_check: unvalid generator system, does not imply the positivity constraint\n");
      return false;
    }
    res = matrix_check2(pk,po->F);
    if (!res){
      fprintf(stderr,"poly_check: F not normalized\n");
      return false;
    }
    res = matrix_check3(pk,po->F);
    if (!res){
      fprintf(stderr,"poly_check: C not sorted although _sorted=true\n");
      return false;
    }
  }
  if (!(po->C && po->F))
    return true;

  if (po->C->nbcolumns != nbcols || po->F->nbcolumns != nbcols){
    fprintf(stderr,"poly_check: po->C->nbcolumns==%lu, po->F->nbcolumns==%lu\n",
	    (unsigned long)po->C->nbcolumns, (unsigned long)po->F->nbcolumns);
    return false;
  }
  if (po->satC){
    if (po->satC->nbrows!=po->F->nbrows){
      fprintf(stderr,"poly_check: po->satC->nbrows==%lu, po->F->nbrows==%lu\n",
	    (unsigned long)po->satC->nbrows,(unsigned long)po->F->nbrows);
      return false;
    }
    if (!cherni_checksat(pk,true,
			 po->C,po->nbeq,
			 po->F,po->nbline,
			 po->satC)){
      fprintf(stderr,"poly_check: bad saturation numbers\n");
      return false;
    }
    res = cherni_checksatmat(pk,true,po->C,po->F,po->satC);
    if (!res){
      fprintf(stderr,"poly_check: bad satC\n");
      return false;
    }
  }
  if (po->satF){
    if (po->satF->nbrows!=po->C->nbrows){
      fprintf(stderr,"poly_check: po->satF->nbrows==%lu, po->C->nbrows==%lu\n",
	    (unsigned long)po->satF->nbrows, (unsigned long)po->C->nbrows);
      return false;
    }
    if (!cherni_checksat(pk,false,
			 po->F,po->nbline,
			 po->C,po->nbeq,
			 po->satF)){
      fprintf(stderr,"poly_check: bad saturation numbers\n");
      return false;
    }
    res = cherni_checksatmat(pk,false,po->F,po->C,po->satF);
    if (!res){
      fprintf(stderr,"poly_check: bad satF\n");
      return false;
    }
  }
  return true;
}

bool poly_check_dual(pk_internal_t* pk, const poly_t* poly, bool usual)
{
  bool res;
  poly_t* po = (poly_t*) poly;
  if (!usual) poly_dual(po);
  res = poly_check(pk,po);
  if (!usual) poly_dual(po);
  return res;
}
