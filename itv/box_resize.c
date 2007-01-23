/* ********************************************************************** */
/* box_resize.c */
/* ********************************************************************** */

#include "box_internal.h"
#include "box_resize.h"

box_t* box_add_dimensions(ap_manager_t* man,
			  bool destructive, box_t* a,
			  const ap_dimchange_t* dimchange)
{
  box_t* res;
  size_t size;
  size_t dimsup;
  int i,k;

  man->result.flag_best = tbool_true;  
  man->result.flag_exact = tbool_true;  
  res = destructive ? a : box_copy(man,a);
  if (a->p==NULL){
    goto box_add_dimensions_exit;
  }
  size = res->intdim+res->realdim;
  dimsup = dimchange->intdim+dimchange->realdim;
  res->p = realloc(res->p,(size+dimsup)*sizeof(itv_t));
  for (i=size;i<size+dimsup;i++){
    itv_init(res->p[i]);
  }
  k = dimsup;
  for (i=size; i>=0; i--){
    if (i<size){
      itv_set(res->p[i+k],a->p[i]);
    }
    while (k>=1 && dimchange->dim[k-1]==i){
      k--;
      itv_set_top(res->p[i+k]);
    }
  }  
 box_add_dimensions_exit:
  res->intdim += dimchange->intdim;
  res->realdim += dimchange->realdim;
  return res;
}

box_t* box_remove_dimensions(ap_manager_t* man,
			     bool destructive, box_t* a,
			     const ap_dimchange_t* dimchange)
{
  box_t* res;
  size_t size;
  size_t dimsup;
  int i,k;
  
  man->result.flag_best = tbool_true;  
  man->result.flag_exact = tbool_true;  
  res = destructive ? a : box_copy(man,a);
  if (a->p==NULL){
    goto box_remove_dimensions_exit;
  }
  size = res->intdim+res->realdim;
  dimsup = dimchange->intdim+dimchange->realdim;
  k=0;
  for (i=0; i<size-dimsup; i++){
    while (k<dimsup && dimchange->dim[k]==i+k){
      k++;
    }
    itv_set(res->p[i],a->p[i+k]);
  }
  for (i=size-dimsup;i<size;i++){
    itv_clear(res->p[i]);
  }
  res->p = realloc(res->p,(size-dimsup)*sizeof(itv_t));
 box_remove_dimensions_exit:
  res->intdim -= dimchange->intdim;
  res->realdim -= dimchange->realdim;
  return res;
}

box_t* box_permute_dimensions(ap_manager_t* man,
			      bool destructive,
			      box_t* a,
			      const ap_dimperm_t* perm)
{
  box_t* res;
  size_t size;
  int i;
  
  man->result.flag_best = tbool_true;  
  man->result.flag_exact = tbool_true;  
  if (a->p==NULL){
    return destructive ? a : box_copy(man,a);
  }
  res = box_copy(man,a);
  size = res->intdim+res->realdim;
  for (i=0;i<size;i++){
    itv_set(res->p[perm->dim[i]],a->p[i]);
  }
  if (destructive) box_free(man,a);
  return res;
}
			   
