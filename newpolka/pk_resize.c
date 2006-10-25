/* ********************************************************************** */
/* pk_resize.c: change and permutation of dimensions  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_bit.h"
#include "pk_satmat.h"
#include "pk_matrix.h"

#include "pk_user.h"
#include "pk_representation.h"
#include "pk_constructor.h"
#include "pk_project.h"
#include "pk_resize.h"

/* ********************************************************************** */
/*  I. Vectors */
/* ********************************************************************** */

/*
If diff is positive, adds diff dimensions at the end of
q1 and puts the result in q2; If diff is negative,
deletes the -diff last dimensions of q1 and puts the
result in q2. q2 is supposed to have a sufficient size.
*/

void vector_resize_dimensions(numint_t* q2,
			      const numint_t* q1, size_t size,
			      int diff)
{
  int i;
  if (q2 != (numint_t*)q1){
    if (diff > 0){
      vector_copy(q2,q1,size);
      for (i=size; i<size+diff; i++){
	numint_set_int(q2[i],0);
      }
    }
    else {
      vector_copy(q2,q1,size+diff);
    }
  }
  else {
    if (diff>0){
      for (i=size; i<size+diff; i++){
	numint_set_int(q2[i],0);
      }
    }
  }
}

/*
  Apply the
  permutation permutation. The result is stored in newq. The array
  permutation is supposed to be of size size-pk->dec.
  
  BE CAUTIOUS: value 0 in the permutation means columns pk->dec.
*/

void vector_permute_dimensions(pk_internal_t* pk,
			       numint_t* newq, const numint_t* q, size_t size,
			       const ap_dim_t* permut)
{
  bool destructive;
  int j,newj;
  numint_t* nq;
  
  destructive = (newq==(numint_t*)q);
  
  /* Where to write ?
     If destructive, we write in a temporary vector
     otherwise we write in the destination
  */
  nq = destructive ? pk->vector_numintp : newq;

  /* Fill the non-permuted fields */
  for (j=0; j<pk->dec && j<size; j++){
    numint_set(nq[j],q[j]);
  }
  /* Permutation itself */
  for (j=pk->dec; j<size; j++){
    newj = permut[j - pk->dec] + pk->dec;
    numint_set(nq[newj],q[j]);
  }
  if (destructive){
    for(j=0; j<size; j++){
      numint_set(newq[j],nq[j]);
    }
  }
  return;
}

void vector_add_dimensions(pk_internal_t* pk,
			   numint_t* newq, 
			   const numint_t* q, size_t size,
			   const ap_dimchange_t* dimchange)
{
  int i,k,dimsup;

  if (newq!=(numint_t*)q){ 
    for (i=0;i<pk->dec && i<size; i++) 
      numint_set(newq[i],q[i]);
  }
  dimsup = dimchange->intdim+dimchange->realdim;
  k = dimsup;
  for (i=size-pk->dec; i>=0; i--){
    if (i<size-pk->dec){
      numint_set(newq[pk->dec+i+k],q[pk->dec+i]);
    }
    while (k>=1 && dimchange->dim[k-1]==i){
      k--;
      numint_set_int(newq[pk->dec+i+k],0);
    }
  }
}

void vector_remove_dimensions(pk_internal_t* pk,
			   numint_t* newq, 
			   const numint_t* q, size_t size,
			   const ap_dimchange_t* dimchange)
{
  size_t i,k,dimsup;
  
  if (newq!=(numint_t*)q){ 
    for (i=0;i<pk->dec && i<size; i++) 
      numint_set(newq[i],q[i]);
  }
  dimsup = dimchange->intdim+dimchange->realdim;
  k=0;
  for (i=0; i<size-dimsup-pk->dec; i++){
    while (k<dimsup && dimchange->dim[k]==i+k){
      k++;
    }
    numint_set(newq[pk->dec+i],q[pk->dec+i+k]);
  }
  vector_normalize(pk,newq,size-dimsup);
}

/* ====================================================================== */
/* II. Matrices */
/* ====================================================================== */

/* Modifications of the number of columns in-place */
void matrix_resize(matrix_t* mat, int diff)
{
  if (diff != 0){
    int i;
    for(i=0; i<mat->_maxrows; i++){
      vector_realloc(&mat->p[i],
		     mat->nbcolumns,
		     mat->nbcolumns+diff);
    }
    mat->nbcolumns += diff;
  }
}

matrix_t* matrix_add_dimensions(pk_internal_t* pk,
				bool destructive,
				matrix_t* mat,
				const ap_dimchange_t* dimchange)
{
  matrix_t* nmat;
  size_t i,dimsup;

  dimsup = dimchange->intdim+dimchange->realdim;
  if (destructive){
    nmat = (matrix_t*)mat;
    matrix_resize(nmat,(int)dimsup);
  }
  else {
    nmat = matrix_alloc(mat->nbrows,mat->nbcolumns+dimsup,mat->_sorted);
  }
  for (i=0; i<mat->nbrows; i++){
    vector_add_dimensions(pk,nmat->p[i],(const numint_t*)mat->p[i],nmat->nbcolumns-dimsup,dimchange);
  }
  return nmat;
}

matrix_t* matrix_remove_dimensions(pk_internal_t* pk,
				   bool destructive,
				   matrix_t* mat,
				   const ap_dimchange_t* dimchange)
{
  matrix_t* nmat;
  size_t i,dimsup;

  dimsup = dimchange->intdim + dimchange->realdim;
  nmat = 
    destructive ? 
    mat : 
    matrix_alloc(mat->nbrows, mat->nbcolumns-dimsup, false);
  for (i=0; i<mat->nbrows; i++){
    vector_remove_dimensions(pk,
			     nmat->p[i],
			     (const numint_t*)mat->p[i],
			     mat->nbcolumns,
			     dimchange);
  }
  if (destructive){
    matrix_resize(nmat, -(int)dimsup);
  }
  return nmat;
}

matrix_t* matrix_permute_dimensions(pk_internal_t* pk,
				    bool destructive,
				    matrix_t* mat,
				    const ap_dim_t* permutation)
{
  matrix_t* nmat;
  size_t i;

  nmat = destructive ? (matrix_t*)mat : matrix_alloc(mat->nbrows,mat->nbcolumns,false);
  for (i=0; i<mat->nbrows; i++){
    vector_permute_dimensions(pk,nmat->p[i],(const numint_t*)mat->p[i],mat->nbcolumns,permutation);
  }
  nmat->_sorted = false;
  return nmat;
}

/* ********************************************************************** */
/* III. Factorized forms */
/* ********************************************************************** */

/* Standard case of the following functions: addition of
   dimensions, and embedding the polyhedron in the new space.

   Dual version, with new dimensions sets at 0: exchanging
   constraints and generators.
*/

poly_t* cherni_add_dimensions(pk_internal_t* pk,
			      bool destructive,
			      poly_t* pa,
			      const ap_dimchange_t* dimchange)
{
  poly_t* po;
  int i,k;
  size_t dimsup;

  dimsup = dimchange->intdim + dimchange->realdim;
  if (destructive){
    po = pa;
    po->intdim += dimchange->intdim;
    po->realdim += dimchange->realdim;
  }
  else {
    po = poly_alloc(pa->intdim + dimchange->intdim,
		    pa->realdim + dimchange->realdim);
    po->status = pa->status;
  }
  
  if (pa->C || pa->F){
    /* Get pa->satC if possible */
    if (pa->satF && pa->satC==NULL){
      ((poly_t*)pa)->satC = satmat_transpose(pa->satF,pa->F->nbrows);
    }
    /* Extend constraints */
    if (pa->C){
      po->C = matrix_add_dimensions(pk,destructive,pa->C,dimchange);
      po->C->_sorted = false;
    }
    /* Extend generators and add new lines */
    if (pa->F){
      size_t nbrows = pa->F->nbrows;
      po->F = matrix_add_dimensions(pk,destructive,pa->F,dimchange);
      matrix_realloc(po->F,po->F->nbrows+dimsup);
      /* translate rows [0,oldF->nbrows-1] to [dimsup,oldF->nbrows+dimsup-1] */
      matrix_move_rows(po->F,dimsup,0,nbrows);
      /* addition of new lines at the beginning of the matrix */
      k=dimsup-1;
      for (i=po->intdim+po->realdim - dimsup; i>=0; i--){
	while (k>=0 && dimchange->dim[k]==i){
	  numint_set_int(po->F->p[dimsup-1-k][pk->dec+i+k], 1);
	  k--;
	}
	if (k<0) break;
      }
      po->F->_sorted =
	pa->F->_sorted && 
	(vector_compare(pk,
		       (const numint_t*)po->F->p[dimsup-1],
		       (const numint_t*)po->F->p[dimsup],po->F->nbcolumns)
	 < 0);
    }
    if (pa->satC){
      if (destructive){
	satmat_realloc(po->satC,po->satC->nbrows+dimsup);
	/* translate rows [0,oldF->nbrows-1] to [dimsup,oldF->nbrows+dimsup-1] */
	satmat_move_rows(po->satC,dimsup,0,po->F->nbrows-dimsup);
	/* the first rows, corresponding to new lines, to zero */
	for (i=0; i<dimsup; i++){
	  bitstring_clear(po->satC->p[i],po->satC->nbcolumns);
	}
      }
      else {
	po->satC = satmat_alloc(pa->satC->nbrows+dimsup, pa->satC->nbcolumns);
	/* the first rows, corresponding to new lines, are already zero */
	for (i=0; i<pa->satC->nbrows; i++){
	  bitstring_copy(po->satC->p[dimsup+i],pa->satC->p[i],pa->satC->nbcolumns);
	}
      }
    }
    /* New saturation matrix: we cannot work with satF */
    if (destructive && dimsup>0 && po->satF){
      satmat_free(po->satF);
      po->satF=NULL;
    }
    if (pa->C && pa->F){
      po->nbeq = pa->nbeq;
      po->nbline = pa->nbline+dimsup;
    }
    else {
      po->nbeq = 0;
      po->nbline = 0;
    }
  }
  return po;
}


/* ====================================================================== */
/* ====================================================================== */

/* ********************************************************************** */
/* IV. Exported functions */
/* ********************************************************************** */

poly_t* poly_add_dimensions(ap_manager_t* man,
			    bool destructive,
			    poly_t* pa,
			    const ap_dimchange_t* dimchange,
			    bool project)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_ADD_DIMENSIONS);
  pk_internal_realloc_lazy(pk,pa->intdim+pa->realdim+dimchange->intdim+dimchange->realdim);

  if (pk->funopt->algorithm>0){
    /* Minimize the argument */
    poly_chernikova(man,pa,"of the argument");
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      /* we can still operate on the available matrix */
    }
  }
  /* Return empty if empty */
  if (!pa->C && !pa->F){
    man->result.flag_best = man->result.flag_exact = tbool_true;
    if (destructive){
      pa->intdim += dimchange->intdim;
      pa->realdim += dimchange->realdim;
      return pa;
    }
    else {
      return poly_bottom(man,
			 pa->intdim + dimchange->intdim,
			 pa->realdim + dimchange->realdim);
    }
  }
  if (project){
      poly_dual(pa);
      po = cherni_add_dimensions(pk, destructive, pa, dimchange);
      poly_dual(po);
      if (!destructive)
	poly_dual(pa);
  }
  else {
    po = cherni_add_dimensions(pk, destructive, pa, dimchange);
  }
  assert(poly_check(pk,po));
  return po;
}

poly_t* poly_remove_dimensions(ap_manager_t* man,
			       bool destructive,
			       poly_t* pa,
			       const ap_dimchange_t* dimchange)
{
  poly_t* po;
  size_t dimsup;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_REMOVE_DIMENSIONS);

   if (pk->funopt->algorithm<=0){
    poly_obtain_F(man,pa,"of the argument");
  } else {
    poly_chernikova(man,pa,"of the argument");
  }
  if (destructive){
    po = pa;
    po->intdim -= dimchange->intdim;
    po->realdim -= dimchange->realdim;
    po->status = 0;
    po->nbeq = po->nbline = 0;
  }
  else {
    po = poly_alloc(pa->intdim - dimchange->intdim,
		    pa->realdim - dimchange->realdim);
  }
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    if (!pa->F){
      man->result.flag_best = man->result.flag_exact = tbool_false;
      poly_set_top(pk,po);
      return po;
    }
    /* We can still work with pa->F */
  }
  /* Return empty if empty */
  if (!pa->C && !pa->F){
    man->result.flag_best = man->result.flag_exact = tbool_true;
    if (!destructive){
      poly_set_bottom(pk,po);
    }
    return po;
  }
  if (po->C){ matrix_free(po->C); po->C = NULL; }
  if (po->satC){ satmat_free(po->satC); po->satC = NULL; }
  if (po->satF){ satmat_free(po->satF); po->satF = NULL; }
  dimsup = dimchange->intdim+dimchange->realdim;
  po->F = matrix_remove_dimensions(pk,destructive,pa->F,dimchange);
  man->result.flag_best = man->result.flag_exact =
    dimchange->intdim>0 ? tbool_top : tbool_true;
  assert(poly_check(pk,po));
  return po;
}

poly_t* poly_permute_dimensions(ap_manager_t* man,
				bool destructive,
				poly_t* pa,
				const ap_dimperm_t* permutation)
{
  poly_t* po;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_PERMUTE_DIMENSIONS);

  if (pk->funopt->algorithm>=0){
    /* Minimize the argument */
    poly_chernikova(man,pa,"of the argument");
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      /* we can still operate on the available matrix */
    }
  }
  man->result.flag_best = man->result.flag_exact = tbool_true;
  po = destructive ? pa : poly_alloc(pa->intdim,pa->realdim);
  if (pa->C){
    po->C = matrix_permute_dimensions(pk,destructive,pa->C,permutation->dim);
  }
  if (pa->F){
    po->F = matrix_permute_dimensions(pk,destructive,pa->F,permutation->dim);
  }
  if (!destructive){
    po->satC = pa->satC ? satmat_copy(pa->satC) : NULL;
    po->satF = pa->satF ? satmat_copy(pa->satF) : NULL;
    po->nbline = pa->nbline;
    po->nbeq = pa->nbeq;
    po->status = pa->status;
  }
  assert(poly_check(pk,po));
  return po;
}
