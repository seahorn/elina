/* ********************************************************************** */
/* pk_constructor.c: constructors and accessors */
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

/* ********************************************************************** */
/* I. Constructors */
/* ********************************************************************** */

/* ====================================================================== */
/* Empty polyhedron */
/* ====================================================================== */

void poly_set_bottom(pk_internal_t* pk, poly_t* po)
{
  if (po->C) matrix_free(po->C);
  if (po->F) matrix_free(po->F);
  if (po->satC) satmat_free(po->satC);
  if (po->satF) satmat_free(po->satF);
  po->C = po->F = NULL;
  po->satC = po->satF = NULL;
  po->status = poly_status_minimal;
  po->nbeq = po->nbline = 0;
}

/*
The empty polyhedron is just defined by the absence of both
constraints matrix and frames matrix.
*/

poly_t* poly_bottom(ap_manager_t* man, size_t intdim, size_t realdim)
{
  poly_t* po = poly_alloc(intdim,realdim);
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_BOTTOM);
  pk_internal_realloc_lazy(pk,intdim+realdim);
  po->status = poly_status_minimal;
  man->result.flag_exact = man->result.flag_best = tbool_true;
  return po;
}

/* ====================================================================== */
/* Universe polyhedron */
/* ====================================================================== */

void _matrix_fill_constraint_top(pk_internal_t* pk, matrix_t* C, size_t start)
{
  if (pk->strict){
    /* constraints epsilon and xi-epsilon*/
    vector_clear(C->p[start+0],C->nbcolumns);
    vector_clear(C->p[start+1],C->nbcolumns);
    numint_set_int(C->p[start+0][0],1);
    numint_set_int(C->p[start+0][polka_eps],1);
    numint_set_int(C->p[start+1][0],1);
    numint_set_int(C->p[start+1][polka_cst],1);
    numint_set_int(C->p[start+1][polka_eps],(-1));
  }
  else {
    /* constraint \xi \geq 0 */
    vector_clear(C->p[start+0],C->nbcolumns);
    numint_set_int(C->p[start+0][0],1);
    numint_set_int(C->p[start+0][polka_cst],1);
  }
}

void poly_set_top(pk_internal_t* pk, poly_t* po)
{
  int i;
  size_t dim;

  if (po->C) matrix_free(po->C);
  if (po->F) matrix_free(po->F);
  if (po->satC) satmat_free(po->satC);
  if (po->satF) satmat_free(po->satF);

  po->status =
    poly_status_conseps |
    poly_status_consgauss |
    poly_status_gengauss;

  dim = po->intdim + po->realdim;

  po->C = matrix_alloc(pk->dec-1, pk->dec+dim,true);
  po->F = matrix_alloc(pk->dec+dim-1,pk->dec+dim,true);
  /* We have to ensure that the matrices are really sorted */
  po->satC = satmat_alloc(pk->dec+dim-1,bitindex_size(pk->dec-1));
  po->satF = 0;
  po->nbeq = 0;
  po->nbline = dim;

  /* constraints */
  _matrix_fill_constraint_top(pk,po->C,0);

  /* generators */
  /* lines $x_i$ */
  for(i=0; i<dim; i++){
    numint_set_int(po->F->p[i][pk->dec+dim-1-i],1);
  }
  if (pk->strict){
    /* rays xi and xi+epsilon */
    numint_set_int(po->F->p[dim][0],1);
    numint_set_int(po->F->p[dim][polka_cst],1);
    numint_set_int(po->F->p[dim+1][0],1);
    numint_set_int(po->F->p[dim+1][polka_cst],1);
    numint_set_int(po->F->p[dim+1][polka_eps],1);
    /* saturation matrix */
    po->satC->p[dim][0] = bitstring_msb >> 1;
    po->satC->p[dim+1][0] = bitstring_msb;
  }
  else {
    /* ray xi */
    numint_set_int(po->F->p[dim][0],1);
    numint_set_int(po->F->p[dim][polka_cst],1);
    /* saturation matrix */
    po->satC->p[dim][0] = bitstring_msb;
  }
}

poly_t* poly_top(ap_manager_t* man, size_t intdim, size_t realdim)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_TOP);
  pk_internal_realloc_lazy(pk,intdim+realdim);

  po = poly_alloc(intdim,realdim);
  poly_set_top(pk,po);
  man->result.flag_exact = man->result.flag_best = tbool_true;
  assert(poly_check(pk,po));
  return po;
}

/* ====================================================================== */
/* Hypercube polyhedron */
/* ====================================================================== */

/* Fills the vector with the constraint:
   dim <= bound if sgn>0,
   dim = bound if sgn=0
   dim >= bound if sgn<0

   Normally returns 0,
   Returns 1 if equality of an integer dimension with a non-integer numbers
*/

bool vector_set_dim_bound(pk_internal_t* pk,
			  numint_t* vec,
			  ap_dim_t dim,
			  mpq_t mpq,
			  size_t intdim, size_t realdim,
			  int sgn,
			  bool integer)
{
  size_t size;
  mpq_t bound;

  assert (mpz_sgn(mpq_denref(mpq)));

  size = pk->dec+intdim+realdim;

  mpq_init(bound);
  if (integer && dim<intdim){
    if (sgn>0){
      mpz_fdiv_q(mpq_numref(bound),mpq_numref(mpq),mpq_denref(mpq));
      mpz_set_ui(mpq_denref(bound),1);
    }
    else if (sgn<0){
      mpz_cdiv_q(mpq_numref(bound),mpq_numref(mpq),mpq_denref(mpq));
      mpz_set_ui(mpq_denref(bound),1);
    }
    else {
      if (mpz_cmp_ui(mpq_denref(mpq),1)!=0){
	mpq_clear(bound);
	return 1;
      }
    }
  } else {
    mpq_set(bound,mpq);
  }
  /* Write the constraint num + den*x >= 0 */
  vector_clear(vec,size);
  numint_set_int(vec[0], (sgn ? 1 : 0));
  numint_set_mpz(vec[polka_cst],mpq_numref(bound));
  numint_set_mpz(vec[pk->dec+dim],mpq_denref(bound));
  mpq_clear(bound);
  /* put the right sign now */
  if (sgn>0){
    numint_neg(vec[pk->dec+dim],vec[pk->dec+dim]);
  } else {
    numint_neg(vec[polka_cst],vec[polka_cst]);
  }
  return 0;
}


/* The matrix is supposed to be big enough */

int _matrix_fill_constraint_box(pk_internal_t* pk,
				matrix_t* C, size_t start,
				const ap_interval_t** box,
				size_t intdim, size_t realdim,
				bool integer)
{
  size_t k;
  ap_dim_t i;
  bool exc;
  mpq_t mpq;

  k = start;
  mpq_init(mpq);
  for (i=0; i<intdim+realdim; i++){
    const ap_interval_t* itv;

    itv = box[i];
    if (ap_scalar_equal(itv->inf,itv->sup)){
      assert(!ap_scalar_infty(itv->inf));
      ap_mpq_set_scalar(mpq,itv->inf,0);
      exc = vector_set_dim_bound(pk,C->p[k],(ap_dim_t)i, mpq,
				 intdim,realdim,
				 0,
				 integer);
      if (exc){
	mpq_clear(mpq);
	return -1;
      }
      k++;
    }
    else {
      /* inferior bound */
      if (!ap_scalar_infty(itv->inf)){
	ap_mpq_set_scalar(mpq,itv->inf,-1);
	vector_set_dim_bound(pk,C->p[k],(ap_dim_t)i,mpq,
			     intdim,realdim,
			     -1,
			     integer);
	k++;
      }
      /* superior bound */
      if (!ap_scalar_infty(itv->sup)){
	ap_mpq_set_scalar(mpq,itv->sup,+1);
	vector_set_dim_bound(pk,C->p[k],(ap_dim_t)i,mpq,
			     intdim,realdim,
			     1,
			     integer);
	k++;
      }
    }
  }
  mpq_clear(mpq);
  return (int)k;
}

/* Abstract an hypercube defined by the array of intervals of size
   intdim+realdim.  */

poly_t* poly_of_box(ap_manager_t* man,
		    size_t intdim, size_t realdim,
		    const ap_interval_t** array)
{
  int k;
  size_t dim;
  poly_t* po;

  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_OF_BOX);
  pk_internal_realloc_lazy(pk,intdim+realdim);

  po = poly_alloc(intdim,realdim);
  po->status =
    poly_status_conseps |
    poly_status_consgauss |
    poly_status_minimal;

  dim = intdim + realdim;
  po->C = matrix_alloc(pk->dec-1 + 2*dim, pk->dec + dim, false);

  /* constraints */
  _matrix_fill_constraint_top(pk,po->C,0);
  k = _matrix_fill_constraint_box(pk,po->C,pk->dec-1,array,intdim,realdim,true);
  if (k==-1){
    matrix_free(po->C);
    po->C = NULL;
    return po;
  }
  po->C->nbrows = (size_t)k;
  assert(poly_check(pk,po));
  man->result.flag_exact = man->result.flag_best = tbool_true;
  return po;
}

/* ====================================================================== */
/* Polyhedron from a set of (interval) linear constraints */
/* ====================================================================== */

/* Fills the vector with the constraint:
   expr <= bound if sgn>0,
   expr = bound if sgn=0
   expr >= bound if sgn<0

   Normally returns 0,
   Returns 1 if equality with a non-integer numbers (like 2x=1).
*/

bool vector_set_linexpr_bound(pk_internal_t* pk,
			      numint_t* vec,
			      const numint_t* vec2,
			      mpq_t mpq,
			      size_t intdim, size_t realdim,
			      int sgn,
			      bool integer)
{
  size_t i;
  size_t size;
  numint_t numint,cst;

  assert (mpz_sgn(mpq_denref(mpq)));

  size = pk->dec+intdim+realdim;

  if (vec!=(numint_t*)vec2){
    vector_copy(vec,vec2,size);
  }
  numint_init(cst);
  numint_set_mpz(cst,mpq_numref(mpq));
  numint_mul(cst,cst,vec[0]);
  if (mpz_cmp_ui(mpq_denref(mpq),1) != 0){
    numint_init(numint);
    numint_set_mpz(numint,mpq_denref(mpq));
    for (i=0; i<size; i++){
      numint_mul(vec[i],vec[i],numint);
    }
    numint_clear(numint);
  }
  numint_sub(vec[polka_cst],vec[polka_cst],cst);
  numint_clear(cst);
  if (sgn>0){
    for (i=1; i<size; i++){
      numint_neg(vec[i],vec[i]);
    }
  }
  numint_set_int(vec[0], sgn ? 1 : 0);
  vector_normalize(pk,vec,size);
  if (integer) vector_normalize_constraint_int(pk,vec,intdim,realdim);

  return false;
}

/*
Abstract a convex polyhedra defined by the array of linear constraints.
*/

poly_t* poly_of_lincons_array(ap_manager_t* man,
			      size_t intdim, size_t realdim,
			      const ap_lincons0_array_t* cons)
{
  int i;
  size_t row, dim;
  matrix_t* C;
  poly_t* po;

  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_OF_LINCONS_ARRAY);
  pk_internal_realloc_lazy(pk,intdim+realdim);

  /* initialization */
  po = poly_alloc(intdim,realdim);
  po->status =
    poly_status_conseps;
  dim = intdim + realdim;
  C = matrix_alloc(pk->dec-1 + cons->size, pk->dec + dim, false);
  po->C = C;

  /* constraints */
  _matrix_fill_constraint_top(pk,C,0);
  row = pk->dec - 1;
  dim = intdim + realdim;
  for (i=0; i<cons->size; i++){
    vector_set_lincons(pk, C->p[row+i], &cons->p[i], intdim, realdim, true);
  }
  assert(poly_check(pk,po));
  man->result.flag_exact = man->result.flag_best = tbool_true;
  return po;
}

/* ********************************************************************** */
/* II. Accessors */
/* ********************************************************************** */

/* Return the dimensions of the polyhedra */
ap_dimension_t poly_dimension(ap_manager_t* man, const  poly_t* po){
  ap_dimension_t res;
  res.intdim = po->intdim;
  res.realdim = po->realdim;
  return res;
}

