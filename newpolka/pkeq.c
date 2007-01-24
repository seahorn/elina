/* ********************************************************************** */
/* pk_equality.c: linear equalities on top of NewPolka polyhedra */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"
#include "pk_user.h"
#include "pk_representation.h"
#include "pk_constructor.h"
#include "pk_assign.h"
#include "pk_resize.h"

#include "pkeq.h"


/* ********************************************************************** */
/* I. General management */
/* ********************************************************************** */

/* ============================================================ */
/* I.1 Memory */
/* ============================================================ */

/* Return the abstract size of a set of equalities, which is the number of
   equalities times the dimension */
size_t equality_size(ap_manager_t* man, const poly_t* po)
{
  size_t s;

  assert (po->C ? (po->C->nbrows >=1) : true);

  s = (po->C) ? (po->C->nbrows-1) : 0;
  return s*(po->intdim + po->realdim);
}

/* ============================================================ */
/* I.2 Control of internal representation */
/* ============================================================ */

bool equality_check(pk_internal_t* pk, poly_t* po)
{
  if (!poly_check(pk,po)) return false;
  if ((po->C && !po->F) || (!po->C && po->F)){
     fprintf(stderr,"equality_check: po->C && !po->F || !po->C && po->F: not in canonical form: \n");
     return false;
  }
  if (po->C && po->C->nbrows != po->nbeq+1){
      fprintf(stderr,"equality_check: po->C->nbrows != po->nbeq+1\n");
     return false;
  }
  if (po->F && po->F->nbrows != po->nbline+1){
      fprintf(stderr,"equality_check: po->F->nbrows != po->nbline+1\n");
     return false;
  }
  return true;
}


void matrix_reduce(matrix_t* mat)
{
  if (mat->nbrows + 4 <= mat->_maxrows){
    matrix_minimize(mat);
  }
}

void equality_reduce(ap_manager_t* man, poly_t* po)
{
  assert((!po->C && !po->F) || (po->C && po->F));
  if (po->C && po->F){
    if (po->C->nbrows > po->nbeq + 1){
      pk_internal_t* pk = (pk_internal_t*)man->internal;

      po->C->nbrows = po->nbeq + 1;
      _matrix_fill_constraint_top(pk,po->C,po->nbeq);
      matrix_reduce(po->C);
      matrix_free(po->F); po->F = NULL;
      if (po->satC){
	satmat_free(po->satC);
	po->satC = NULL;
      }
      if (po->satF){
	satmat_free(po->satF);
	po->satF = NULL;
      }
      poly_chernikova(man,po,"of result");
      if (pk->exn){
	pk->exn = AP_EXC_NONE;
	poly_set_top(pk,po);
      }
    }
  }
}
void equality_canonicalize(ap_manager_t* man, const poly_t* po)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_CANONICALIZE);
  pk->funopt->algorithm = 1;
  return poly_canonicalize(man,po);
}

void equality_approximate(ap_manager_t* man, poly_t* po, int algorithm)
{
  return;
}

/* ============================================================ */
/* I.4 Serialization */
/* ============================================================ */

ap_membuf_t equality_serialize_raw(ap_manager_t* man, const poly_t* a)
{
  ap_membuf_t membuf;
  pk_init_from_manager(man,AP_FUNID_SERIALIZE_RAW);
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_SERIALIZE_RAW,NULL);
  membuf.ptr = NULL;
  membuf.size = 0;
  return membuf;
}
poly_t* equality_deserialize_raw(ap_manager_t* man, void* ptr, size_t* size)
{
  pk_init_from_manager(man,AP_FUNID_DESERIALIZE_RAW);
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_DESERIALIZE_RAW,NULL);
  return NULL;
}

/* ********************************************************************** */
/* II. Constructor, accessors, tests and property extraction */
/* ********************************************************************** */

/* ============================================================ */
/* II.1 Basic constructors */
/* ============================================================ */

/* Abstract an hypercube defined by the array of intervals of size
   intdim+realdim.  */
poly_t* equality_of_box(ap_manager_t* man,
			size_t intdim, size_t realdim,
			const ap_interval_t** array)
{
  int i;
  size_t row,dim;
  poly_t* po;
  mpq_t mpq;
  bool exc;

  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_OF_BOX);
  pk_internal_realloc_lazy(pk,intdim+realdim);

  po = poly_alloc(intdim,realdim);
  po->status =
    poly_status_conseps |
    poly_status_minimal;

  dim = intdim + realdim;
  po->C = matrix_alloc(pk->dec-1 + dim, pk->dec + dim, false);

  /* constraints */
  row = 0;
  mpq_init(mpq);
  exc = false;
  for (i=0; i<dim; i++){
    const ap_interval_t* itv = array[i];
    if (ap_scalar_equal(itv->inf,itv->sup)){
      assert(ap_scalar_infty(itv->inf)==0);
      ap_mpq_set_scalar(mpq,itv->inf,0);
      exc = vector_set_dim_bound(pk,po->C->p[row],(ap_dim_t)i, mpq,
				 intdim,realdim,
				 0,
				 true);
      if (exc)
	break;
      row++;
    }
  }
  mpq_clear(mpq);
  if (exc){
    poly_free(man,po);
    ap_manager_raise_exception(man,AP_EXC_INVALID_ARGUMENT,AP_FUNID_OF_BOX,
			       "out of bound dimension in array of dimensions");
    poly_set_top(pk,po);
    man->result.flag_exact = man->result.flag_best = tbool_false;
    return po;
  }
  _matrix_fill_constraint_top(pk,po->C,row);
  po->C->nbrows = (size_t)row + 1;
  matrix_reduce(po->C);
  poly_canonicalize(man,po);
  man->result.flag_exact = man->result.flag_best = tbool_true;
  return po;
}

/*
Abstract a convex polyhedra defined by the array of linear constraints.
*/

poly_t* equality_of_lincons_array(ap_manager_t* man,
				  size_t intdim, size_t realdim,
				  const ap_lincons0_array_t* cons)
{
  int i;
  size_t row, dim;
  matrix_t* C;
  poly_t* po;

  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_OF_LINCONS_ARRAY);
  pk_internal_realloc_lazy(pk,intdim+realdim);

  man->result.flag_exact = man->result.flag_best =
    (intdim==0) ? tbool_true : tbool_top;

  if (pk->funopt->algorithm>=1){
    po = poly_of_lincons_array(man,intdim,realdim,cons);
    equality_reduce(man,po);
    return po;
  }
  else {
    /* initialization */
    po = poly_alloc(intdim,realdim);
    po->status =
      poly_status_conseps;
    dim = intdim + realdim;
    C = matrix_alloc(pk->dec-1 + cons->size, pk->dec + dim, false);
    po->C = C;

    /* constraints */
    row = 0;
    for (i=0; i<cons->size; i++){
      if (cons->p[i].constyp == AP_CONS_EQ){
	vector_set_lincons(pk, C->p[row], &cons->p[i], intdim, realdim, true);
	row++;
      }
    }
    _matrix_fill_constraint_top(pk,C,row);
    C->nbrows = row + 1;
    matrix_reduce(C);
  }
  poly_chernikova(man,po,"of the result");
  assert(equality_check(pk,po));
  return po;
}

/* ============================================================ */
/* II.3 Tests */
/* ============================================================ */

tbool_t equality_is_eq(ap_manager_t* man, const poly_t* pa, const poly_t* pb)
{
  pk_init_from_manager(man,AP_FUNID_IS_EQ);
  equality_canonicalize(man,(poly_t*)pa);
  equality_canonicalize(man,(poly_t*)pb);

  man->result.flag_exact = man->result.flag_best = tbool_true;

  if (pa->C){
    if (pb->C){
      if (pa->nbeq != pb->nbeq || pa->nbline != pb->nbline){
	return tbool_false;
      }
      else {
	int i,j;

	const matrix_t* mata = pa->C;
	const matrix_t* matb = pb->C;
	tbool_t res = tbool_true;
	for (i=0; i<mata->nbrows; i++){
	  for (j=0; j<matb->nbcolumns; j++){
	    if (numint_cmp(mata->p[i][j],matb->p[i][j])!=0){
	      res = tbool_false;
	      goto _equality_is_eq_exit;
	    }
	  }
	}
      _equality_is_eq_exit:
	return res;
      }
    }
    else {
      return tbool_false;
    }
  }
  else {
    return pb->C ? tbool_false : tbool_true;
  }
}

/* ********************************************************************** */
/* III. Operations */
/* ********************************************************************** */

/* ============================================================ */
/* III.1 Meet and Join */
/* ============================================================ */

poly_t* equality_meet(ap_manager_t* man, bool destructive, poly_t* polya, const poly_t* polyb)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_MEET);
  pk->funopt->algorithm = 1;
  po = poly_meet(man,destructive,polya,polyb);
  return po;
}

poly_t* equality_meet_array(ap_manager_t* man, const poly_t** po, size_t size)
{
  poly_t* poly;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_MEET);
  pk->funopt->algorithm = 1;
  poly = poly_meet_array(man,po,size);
  return poly;
}

poly_t* equality_meet_lincons_array(ap_manager_t* man, bool destructive, poly_t* pa, const ap_lincons0_array_t* array)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_MEET_LINCONS_ARRAY);
  pk->funopt->algorithm = 1;
  po = poly_meet_lincons_array(man,destructive,pa,array);
  equality_reduce(man,po);
  assert(equality_check(pk,po));
  return po;
}

poly_t* equality_join(ap_manager_t* man, bool destructive, poly_t* polya, const poly_t* polyb)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_JOIN);
  pk->funopt->algorithm = 1;
  po = poly_join(man,destructive,polya,polyb);
  equality_reduce(man,po);
  return po;
}

poly_t* equality_join_array(ap_manager_t* man, const poly_t** po, size_t size)
{
  poly_t* poly;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_JOIN);
  pk->funopt->algorithm = 1;
  poly = poly_join_array(man,po,size);
  equality_reduce(man,poly);
  return poly;
}

poly_t* equality_add_ray_array(ap_manager_t* man, bool destructive, poly_t* pa, const ap_generator0_array_t* array)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_ADD_RAY_ARRAY);
  pk->funopt->algorithm = 1;
  po = poly_add_ray_array(man,destructive,pa,array);
  equality_reduce(man,po);
  return po;
}

/* ============================================================ */
/* III.2 Assignement and Substitutions */
/* ============================================================ */

/* Assignement/Substitution by a linear expression */

poly_t* equality_asssub_linexpr(bool assign,
				ap_manager_t* man,
				bool destructive,
				poly_t* pa,
				ap_dim_t dim, const ap_linexpr0_t* linexpr)
{
  poly_t* po;
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  pk_internal_realloc_lazy(pk,pa->intdim+pa->realdim);
  
  /* Return empty if empty */
  if (!pa->C && !pa->F){
    man->result.flag_best = man->result.flag_exact = tbool_true;
    return destructive ? pa : poly_bottom(man,pa->intdim,pa->realdim);
  }
  /* Choose the right technique */
  switch (linexpr->cst.discr){
  case AP_COEFF_SCALAR:
    {
      po = poly_asssub_linear_linexpr(assign,man,destructive,pa,dim,linexpr);
      poly_chernikova(man,po,"of the result");
      if (pk->exn) goto _equality_asssub_linexpr_error;
      equality_reduce(man,po);
      if (pk->exn) goto _equality_asssub_linexpr_error;
      /* Is the result exact or best ? */
      if (pk->funopt->flag_best_wanted || pk->funopt->flag_exact_wanted){
	man->result.flag_best = man->result.flag_exact = 
	  (dim < pa->intdim || !ap_linexpr0_is_real(linexpr, pa->intdim)) ?
	  tbool_top :
	  tbool_true;
      }
      else {
	man->result.flag_best = man->result.flag_exact = 
	  pa->intdim>0 ? tbool_top : tbool_true;
      }
    }
    break;
  case AP_COEFF_INTERVAL:
    po = equality_forget_array(man,destructive,pa,&dim,1,false);
    break;
  default:
    abort();
  }
  return po;
 _equality_asssub_linexpr_error:
  pk->exn = AP_EXC_NONE;
  poly_set_top(pk,po);
  man->result.flag_best = man->result.flag_exact = tbool_false;
  return po;
}

poly_t* equality_assign_linexpr(ap_manager_t* man,
			    bool destructive, poly_t* pa, 
			    ap_dim_t dim, const ap_linexpr0_t* linexpr,
			    const poly_t* pb)
{
  pk_init_from_manager(man,AP_FUNID_ASSIGN_LINEXPR);
  poly_t* po;
  po = equality_asssub_linexpr(true,
			       man,destructive,pa,dim,linexpr);
  if (pb!=NULL){
    po = equality_meet(man,true,po,pb);
  }
  return po;
}

poly_t* equality_substitute_linexpr(ap_manager_t* man,
				    bool destructive, poly_t* pa, 
				    ap_dim_t dim, const ap_linexpr0_t* linexpr,
				    const poly_t* pb)
{
  pk_init_from_manager(man,AP_FUNID_SUBSTITUTE_LINEXPR);
  poly_t* po;
  po = equality_asssub_linexpr(false,
			       man,destructive,pa,dim,linexpr);
  if (pb!=NULL){
    po = equality_meet(man,true,po,pb);
  }
  return po;
}

/* Assignement/Substitution by an array of linear expressions */

poly_t* equality_asssub_linexpr_array(bool assign, 
				      ap_manager_t* man,
				      bool destructive,
				      poly_t* pa,
				      const ap_dim_t* tdim,
				      const ap_linexpr0_t** texpr,
				      size_t size)
{
  int i;
  poly_t* po;
  ap_dim_t* tdimp;
  const ap_linexpr0_t** texprp;
  size_t sizep;
  ap_dim_t* tdimforget;
  size_t sizeforget;

  pk_internal_t* pk = (pk_internal_t*)man->internal;

  /* Return empty if empty */
  if (!pa->C && !pa->F){
    man->result.flag_best = man->result.flag_exact = tbool_true;
    return destructive ? pa : poly_bottom(man,pa->intdim,pa->realdim);
  }
  /* Choose the right technique */
  tdimp = malloc(size*sizeof(ap_dim_t));
  texprp = malloc(size*sizeof(ap_linexpr0_t*));
  sizep = 0;
  tdimforget = malloc(size*sizeof(ap_dim_t));
  sizeforget = 0;
  for (i=0; i<size; i++){
    if (texpr[i]->cst.discr==AP_COEFF_SCALAR){
      tdimp[sizep] = tdim[i];
      texprp[sizep] = texpr[i];
      sizep++;
    } else {
      tdimforget[sizeforget] = tdim[i];
      sizeforget++;
    }
  }
  if (sizep>0){
    po = poly_asssub_linear_linexpr_array(assign,man,destructive,pa,tdimp,texprp,sizep);
    poly_chernikova(man,po,"of the result");
    if (pk->exn) goto _equality_asssub_linexpr_array_error;
    equality_reduce(man,po);
    if (pk->exn) goto _equality_asssub_linexpr_array_error;
    if (sizeforget){
      po = equality_forget_array(man,true,po,tdimforget,sizeforget,false);
    }
  }
  else {
    po = equality_forget_array(man,destructive,pa,tdimforget,sizeforget,false);
  }
  /* Is the result exact or best ? */
  if (pk->funopt->flag_best_wanted || pk->funopt->flag_exact_wanted){
    man->result.flag_best = tbool_true;
    for (i=0;i<size;i++){
      if (tdim[i] < pa->intdim || !ap_linexpr0_is_real(texpr[i], pa->intdim)){
	man->result.flag_best = tbool_top;
	break;
      }
    }
    man->result.flag_exact = man->result.flag_best;
  }
  else {
    man->result.flag_best = man->result.flag_exact = 
      pa->intdim>0 ? tbool_top : tbool_true;
  }
  free(tdimp);
  free(texprp); 
  free(tdimforget);
  return po;
 _equality_asssub_linexpr_array_error:
  free(tdimp);
  free(texprp); 
  free(tdimforget);
  pk->exn = AP_EXC_NONE;
  poly_set_top(pk,po);
  man->result.flag_best = man->result.flag_exact = tbool_false;
  return po;
}

poly_t* equality_assign_linexpr_array(ap_manager_t* man,
				      bool destructive, poly_t* pa,
				      const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
				      size_t size,
				      const poly_t* pb)
{
  pk_init_from_manager(man,AP_FUNID_ASSIGN_LINEXPR_ARRAY);
  poly_t* po;
  po = equality_asssub_linexpr_array(true,
				     man,destructive,pa,tdim,texpr,size);
  if (pb!=NULL){
    po = equality_meet(man,true,po,pb);
  }
  return po;
}

poly_t* equality_substitute_linexpr_array(ap_manager_t* man,
					  bool destructive, poly_t* pa,
					  const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
					  size_t size,
					  const poly_t* pb)
{
  pk_init_from_manager(man,AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY);
  poly_t* po;
  po = equality_asssub_linexpr_array(false,
				     man,destructive,pa,tdim,texpr,size);
  if (pb!=NULL){
    po = equality_meet(man,true,po,pb);
  }
  return po;
}

/* ============================================================ */
/* III.3 Projections */
/* ============================================================ */

poly_t* equality_forget_array(ap_manager_t* man,
			      bool destructive, poly_t* a,
			      const ap_dim_t* tdim, size_t size,
			      bool project)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_FORGET_ARRAY);
  pk->funopt->algorithm = 1;
  po = poly_forget_array(man,destructive,a,tdim,size,project);
  equality_reduce(man,po);
  return po;
}


/* ============================================================ */
/* III.4 Change and permutation of dimensions */
/* ============================================================ */

poly_t* equality_remove_dimensions(ap_manager_t* man,
			    bool destructive, poly_t* a,
			    const ap_dimchange_t* dimchange)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_REMOVE_DIMENSIONS);
  pk->funopt->algorithm = 1;
  po = poly_remove_dimensions(man,destructive,a,dimchange);
  equality_reduce(man,po);
  return po;
}

/* ============================================================ */
/* III.5 Expansion and folding of dimensions */
/* ============================================================ */
poly_t* equality_expand(ap_manager_t* man,
			bool destructive, poly_t* a,
			ap_dim_t dim,
			size_t n)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_EXPAND);
  pk->funopt->algorithm = 1;
  po = poly_expand(man,destructive,a,dim,n);
  equality_reduce(man,po);
  return po;
}
poly_t* equality_fold(ap_manager_t* man,
		      bool destructive, poly_t* a,
		      const ap_dim_t* tdim, size_t size)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_FOLD);
  pk->funopt->algorithm = 1;
  po = poly_fold(man,destructive,a,tdim,size);
  equality_reduce(man,po);
  return po;
}

/* ============================================================ */
/* III.6 Widening */
/* ============================================================ */

poly_t* equality_widening(ap_manager_t* man,
			  const poly_t* a1, const poly_t* a2)
{
  return poly_copy(man,a2);
}

/* ============================================================ */
/* III.7 Closure operation */
/* ============================================================ */

poly_t* equality_closure(ap_manager_t* man, bool destructive, poly_t* a)
{
  return destructive ? a : poly_copy(man,a);
}

pk_internal_t* pkeq_manager_get_internal(ap_manager_t* man)
{
  return pk_manager_get_internal(man);
}


ap_manager_t* pkeq_manager_alloc()
{
  ap_manager_t* man;
  void** funptr;

  man = pk_manager_alloc(false);
  man->library = "polkaeq";
  funptr = man->funptr;
  // funptr[AP_FUNID_COPY] = &poly_copy;
  // funptr[AP_FUNID_FREE] = &poly_free;
  funptr[AP_FUNID_ASIZE] = &equality_size;
  // funptr[AP_FUNID_MINIMIZE] = &poly_minimize;
  // funptr[AP_FUNID_CANONICALIZE] = &poly_canonicalize;
  funptr[AP_FUNID_APPROXIMATE] = &equality_approximate;
  // funptr[AP_FUNID_FPRINT] = &poly_fprint;
  // funptr[AP_FUNID_FPRINTDIFF] = &poly_fprintdiff;
  // funptr[AP_FUNID_FDUMP] = &poly_fdump;
  funptr[AP_FUNID_SERIALIZE_RAW] = &equality_serialize_raw;
  funptr[AP_FUNID_DESERIALIZE_RAW] = &equality_deserialize_raw;
  // funptr[AP_FUNID_BOTTOM] = &poly_bottom;
  // funptr[AP_FUNID_TOP] = &poly_top;
  funptr[AP_FUNID_OF_BOX] = &equality_of_box;
  funptr[AP_FUNID_OF_LINCONS_ARRAY] = &equality_of_lincons_array;
  // funptr[AP_FUNID_DIMENSION] = &poly_dimension;
  // funptr[AP_FUNID_IS_BOTTOM] = &poly_is_bottom;
  // funptr[AP_FUNID_IS_TOP] = &poly_is_top;
  // funptr[AP_FUNID_IS_LEQ] = &poly_is_leq;
  funptr[AP_FUNID_IS_EQ] = &equality_is_eq;
  // funptr[AP_FUNID_IS_DIMENSION_UNCONSTRAINED] = &poly_is_dimension_unconstrained;
  // funptr[AP_FUNID_SAT_INTERVAL] = &poly_sat_interval;
  // funptr[AP_FUNID_SAT_LINCONS] = &poly_sat_lincons;
  // funptr[AP_FUNID_BOUND_DIMENSION] = &poly_bound_dimension;
  // funptr[AP_FUNID_BOUND_LINEXPR] = &poly_bound_linexpr;
  // funptr[AP_FUNID_TO_BOX] = &poly_to_box;
  // funptr[AP_FUNID_TO_LINCONS_ARRAY] = &poly_to_lincons_array;
  // funptr[AP_FUNID_TO_GENERATOR_ARRAY] = &poly_to_generator_array;
  funptr[AP_FUNID_MEET] = &equality_meet;
  funptr[AP_FUNID_MEET_ARRAY] = &equality_meet_array;
  funptr[AP_FUNID_MEET_LINCONS_ARRAY] = &equality_meet_lincons_array;
  funptr[AP_FUNID_JOIN] = &equality_join;
  funptr[AP_FUNID_JOIN_ARRAY] = &equality_join_array;
  funptr[AP_FUNID_ADD_RAY_ARRAY] = &equality_add_ray_array;
  funptr[AP_FUNID_ASSIGN_LINEXPR] = &equality_assign_linexpr;
  funptr[AP_FUNID_SUBSTITUTE_LINEXPR] = &equality_substitute_linexpr;
  funptr[AP_FUNID_ASSIGN_LINEXPR_ARRAY] = &equality_assign_linexpr_array;
  funptr[AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY] = &equality_substitute_linexpr_array;
  //funptr[AP_FUNID_ADD_DIMENSIONS] = &poly_add_dimensions;
  funptr[AP_FUNID_REMOVE_DIMENSIONS] = &equality_remove_dimensions;
  //funptr[AP_FUNID_PERMUTE_DIMENSIONS] = &poly_permute_dimensions;
  funptr[AP_FUNID_FORGET_ARRAY] = &equality_forget_array;
  funptr[AP_FUNID_EXPAND] = &equality_expand;
  funptr[AP_FUNID_FOLD] = &equality_fold;
  funptr[AP_FUNID_WIDENING] = &equality_widening;
  funptr[AP_FUNID_CLOSURE] = &equality_closure;

  return man;
}
