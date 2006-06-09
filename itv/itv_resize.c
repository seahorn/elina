/* ********************************************************************** */
/* itv_resize.c */
/* ********************************************************************** */

#include "itv_resize.h"
#include "itv_int.h"

itv_t* itv_add_dimensions(ap_manager_t* man,
			  bool destructive, itv_t* a,
			  const ap_dimchange_t* dimchange)
{
  itv_t* res;
  size_t size;
  size_t dimsup;
  int i,k;

  man->result.flag_best = tbool_true;  
  man->result.flag_exact = tbool_true;  
  res = destructive ? a : itv_copy(man,a);
  if (a->p==NULL){
    goto itv_add_dimensions_exit;
  }
  size = res->intdim+res->realdim;
  dimsup = dimchange->intdim+dimchange->realdim;
  res->p = realloc(res->p,(size+dimsup)*sizeof(itv_interval_t));
  for (i=size;i<size+dimsup;i++){
    itv_interval_init(res->p[i]);
  }
  k = dimsup;
  for (i=size; i>=0; i--){
    if (i<size){
      itv_interval_set(res->p[i+k],a->p[i]);
    }
    while (k>=1 && dimchange->dim[k-1]==i){
      k--;
      itv_interval_set_top(res->p[i+k]);
    }
  }  
 itv_add_dimensions_exit:
  res->intdim += dimchange->intdim;
  res->realdim += dimchange->realdim;
  return res;
}

itv_t* itv_remove_dimensions(ap_manager_t* man,
			     bool destructive, itv_t* a,
			     const ap_dimchange_t* dimchange)
{
  itv_t* res;
  size_t size;
  size_t dimsup;
  int i,k;
  
  man->result.flag_best = tbool_true;  
  man->result.flag_exact = tbool_true;  
  res = destructive ? a : itv_copy(man,a);
  if (a->p==NULL){
    goto itv_remove_dimensions_exit;
  }
  size = res->intdim+res->realdim;
  dimsup = dimchange->intdim+dimchange->realdim;
  k=0;
  for (i=0; i<size-dimsup; i++){
    while (k<dimsup && dimchange->dim[k]==i+k){
      k++;
    }
    itv_interval_set(res->p[i],a->p[i+k]);
  }
  for (i=size-dimsup;i<size;i++){
    itv_interval_clear(res->p[i]);
  }
  res->p = realloc(res->p,(size-dimsup)*sizeof(itv_interval_t));
 itv_remove_dimensions_exit:
  res->intdim -= dimchange->intdim;
  res->realdim -= dimchange->realdim;
  return res;
}

itv_t* itv_permute_dimensions(ap_manager_t* man,
			      bool destructive,
			      itv_t* a,
			      const ap_dimperm_t* perm)
{
  itv_t* res;
  size_t size;
  int i;
  
  man->result.flag_best = tbool_true;  
  man->result.flag_exact = tbool_true;  
  if (a->p==NULL){
    return destructive ? a : itv_copy(man,a);
  }
  res = itv_copy(man,a);
  size = res->intdim+res->realdim;
  for (i=0;i<size;i++){
    itv_interval_set(res->p[perm->dim[i]],a->p[i]);
  }
  if (destructive) itv_free(man,a);
  return res;
}
			   
