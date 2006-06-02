/* ********************************************************************** */
/* pk_closure.c: topological closure */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"
#include "pk_representation.h"
#include "pk_constructor.h"
#include "pk_closure.h"

/* Either po is just allocated with the right dimensions, or po==pa */

poly_t* poly_closure(ap_manager_t* man, bool destructive, poly_t* pa)
{
  matrix_t* C;
  bool change;
  size_t i;
  poly_t* po;

  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_CLOSURE);
  man->result.flag_best = man->result.flag_exact = tbool_true;
  if (!pk->strict){
    return destructive ? pa : poly_copy(man,pa);
  }
  if (pk->funopt->algorithm<0)
    poly_obtain_C(man,pa,"of the argument");
  else
    poly_chernikova(man,pa,"of the argument");
  
  po = destructive ? pa : poly_alloc(pa->intdim,pa->realdim);
  if (pk->exn){
    poly_set_top(pk,po);
    man->result.flag_best = man->result.flag_exact = tbool_false;
    return po;
  }
  if (!destructive){
     po->C = matrix_copy(pa->C);
  }
  
  C = po->C;
  change = false;
  for (i=0;i<C->nbrows; i++){
    if (numint_sgn(C->p[i][polka_eps])<0){
      change = true;
      numint_set_int(C->p[i][polka_eps],0);
    }
  }
  if (change){
    C->_sorted = false;
    if (destructive){
      if (po->F) matrix_free(po->F);
      if (po->satC) satmat_free(po->satC);
      if (po->satF) satmat_free(po->satF);
      po->F = NULL;
      po->satC = NULL;
      po->satF = NULL;
      po->status = poly_status_conseps;
      po->nbeq = 0;
      po->nbline = 0;
    }
  } else {
    if (!destructive){
      poly_set_save_C(po,pa);
    }
  }
  return po;
}
