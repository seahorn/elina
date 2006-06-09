/* ********************************************************************** */
/* itv_assign.c */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "itv.h"
#include "itv_representation.h"
#include "itv_constructor.h"
#include "itv_meetjoin.h"
#include "itv_assign.h"

itv_t* itv_assign_linexpr(ap_manager_t* man,
			  bool destructive,
			  itv_t* a,
			  ap_dim_t dim, const ap_linexpr0_t* linexpr,
			  const itv_t* dest)
{
  itv_t* res;
  itv_internal_t* intern = man->internal;

  man->result.flag_best = tbool_true;
  res = destructive ? a : itv_copy(man,a);
  if (a->p==NULL || (dest && dest->p==NULL)){
    man->result.flag_exact = tbool_true;
    return res;
  }
  itv_bound_linexpr_internal(intern,
			     res->p[dim],
			     a,linexpr);
  if (dest)
    res = itv_meet(man,true,res,dest);
  man->result.flag_exact = tbool_top;
  return res;
}

itv_t* itv_assign_linexpr_array(ap_manager_t* man,
				bool destructive,
				itv_t* a,
				const ap_dim_t* tdim, 
				const ap_linexpr0_t*const* texpr,
				size_t size,
				const itv_t* dest)
{
  size_t i;
  itv_t* res;
  itv_internal_t* intern = man->internal;
  
  if (a->p==NULL || (dest && dest->p==NULL)){
    man->result.flag_best = tbool_true;
    man->result.flag_exact = tbool_top;
    return destructive ? a : itv_copy(man,a);
  }
  res = itv_copy(man,a);
  for (i=0;i<size;i++){
    itv_bound_linexpr_internal(intern,
			       res->p[tdim[i]],
			       a,texpr[i]);
  }
  if (destructive) itv_free(man,a);
  if (dest)
    res = itv_meet(man,true,res,dest);
  man->result.flag_best = tbool_top;
  man->result.flag_exact = tbool_top;
  return res;
}



itv_t* itv_substitute_linexpr(ap_manager_t* man,
			      bool destructive,
			      itv_t* a,
			      ap_dim_t dim, const ap_linexpr0_t* linexpr,
			      const itv_t* dest)
{
  itv_t* res;
  struct itv_interval_t* itvinterval;
  struct itv_interval_t* itvinterval2;
  itv_lincons_t* itvlincons;
  size_t i;
  ap_dim_t dim2;
  num_t* pnum;
  itv_internal_t* intern = man->internal;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  if (a->p==NULL || 
      (dest && dest->p==NULL)){
    res = destructive ? a : itv_copy(man,a);
    return res;
  }
  else if (itv_interval_is_top(a->p[dim])){
    res = 
      dest ?
      itv_meet(man,destructive,a,dest) :
      (destructive ? a : itv_copy(man,a));
  }
  itvinterval = intern->substitute_linexpr_itvinterval;
  itv_interval_set_top(itvinterval);
  itv_interval_swap(itvinterval,a->p[dim]);
  res =
    dest ?
    itv_meet(man,destructive,a,dest) :
    (destructive ? a : itv_copy(man,a))
    ;
  if (res->p==NULL){
    man->result.flag_exact = tbool_true;
    return res;
  }
  itvlincons = &intern->substitute_linexpr_itvlincons;
  itvinterval2 = intern->substitute_linexpr_itvinterval2;
  itv_linexpr_set_linexpr(intern,
			  &itvlincons->itvlinexpr,
			  linexpr);
  itvlincons->constyp = AP_CONS_SUPEQ;
  itv_interval_swap(itvlincons->itvlinexpr.cst,
		    itvinterval2);
  if (!bound_infty(itvinterval->inf)){
    itv_interval_add_num(itvlincons->itvlinexpr.cst,
			 itvinterval2,
			 bound_numref(itvinterval->inf));
    itv_meet_itvlincons(intern,res,itvlincons);
  }
  if (res->p==NULL){
    return res;
  }
  if (!bound_infty(itvinterval->sup)){
    itv_linexpr_ForeachLinterm(&itvlincons->itvlinexpr,i,dim2,pnum){
      num_neg(*pnum,*pnum);
    }
    itv_interval_add_num(itvlincons->itvlinexpr.cst,
			 itvinterval2,
			 bound_numref(itvinterval->sup));
    itv_meet_itvlincons(intern,res,itvlincons);
  }
  man->result.flag_best = tbool_top;
  man->result.flag_exact = tbool_top;
  return res;
}


itv_t* itv_substitute_linexpr_array(ap_manager_t* man,
				    bool destructive,
				    itv_t* a,
				    const ap_dim_t* tdim, 
				    const ap_linexpr0_t*const* texpr,
				    size_t size,
				    const itv_t* dest)
{
  itv_t* res;
  size_t i,j;
  ap_dim_t dim,dim2;
  num_t* pnum;
  struct itv_interval_t* itvinterval;
  itv_lincons_t* itvlincons;
  itv_lincons_array_t array;
  itv_internal_t* intern = man->internal;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  if (a->p==NULL || (dest && dest->p==NULL)){
    return destructive ? a : itv_copy(man,a);
  }
  res = itv_copy(man,a);

  for (i=0;i<size;i++){
    itv_interval_set_top(res->p[tdim[i]]);
  }
  if (dest){
    res = itv_meet(man,true,res,dest);
    if (res->p==NULL){
      goto itv_substitute_linexpr_array_exit;
    }
  }
  j = 0;
  for (i=0;i<size;i++){
    dim = tdim[i];
    if (!bound_infty(a->p[dim]->inf))
      j++;
    if (!bound_infty(a->p[dim]->sup))
      j++;
  }
  if (j==0){
      goto itv_substitute_linexpr_array_exit;
  }
  itvinterval = intern->substitute_linexpr_itvinterval;
  itvlincons = &intern->substitute_linexpr_itvlincons;
  array = itv_lincons_array_make(j);
  j = 0;
  for (i=0;i<size;i++){
    dim = tdim[i];
    bool inf = !bound_infty(res->p[dim]->inf);
    bool sup = !bound_infty(res->p[dim]->sup);
    if (inf || sup){
      itv_linexpr_set_linexpr(intern,
			     &itvlincons->itvlinexpr,
			     texpr[i]);
      itvlincons->constyp = AP_CONS_SUPEQ;
      itv_interval_swap(itvlincons->itvlinexpr.cst,
		       itvinterval);
      if (inf){
	itv_interval_add_num(itvlincons->itvlinexpr.cst,
			    itvinterval,
			    bound_numref(a->p[dim]->inf));
	itv_lincons_set(&array.p[j],itvlincons);
	j++;
      }
      if (sup){
	itv_linexpr_ForeachLinterm(&itvlincons->itvlinexpr,i,dim2,pnum){
	  num_neg(*pnum,*pnum);
	}
	itv_interval_add_num(itvlincons->itvlinexpr.cst,
			    itvinterval,
			    bound_numref(a->p[dim]->sup));
	itv_lincons_set(&array.p[j],itvlincons);
	j++;
      }
    }
  }
  itv_meet_itv_lincons_array(intern,res,&array,2);
  itv_lincons_array_clear(&array);
  man->result.flag_best = tbool_top;
  man->result.flag_exact = tbool_top;
 itv_substitute_linexpr_array_exit:
  if (destructive) itv_free(man,a);
  return res;
}


