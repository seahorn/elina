/* ********************************************************************** */
/* box_assign.c */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "itv.h"
#include "box_representation.h"
#include "box_constructor.h"
#include "box_meetjoin.h"
#include "box_assign.h"

box_t* box_assign_linexpr(ap_manager_t* man,
			  bool destructive,
			  box_t* a,
			  ap_dim_t dim, const ap_linexpr0_t* linexpr,
			  const box_t* dest)
{
  box_t* res;
  box_internal_t* intern = man->internal;

  man->result.flag_best = tbool_true;
  res = destructive ? a : box_copy(man,a);
  if (a->p==NULL || (dest && dest->p==NULL)){
    man->result.flag_exact = tbool_true;
    return res;
  }
  box_bound_linexpr_internal(intern,
			     res->p[dim],
			     a,linexpr);
  if (dest)
    res = box_meet(man,true,res,dest);
  man->result.flag_exact = tbool_top;
  return res;
}

box_t* box_assign_linexpr_array(ap_manager_t* man,
				bool destructive,
				box_t* a,
				const ap_dim_t* tdim, 
				const ap_linexpr0_t*const* texpr,
				size_t size,
				const box_t* dest)
{
  size_t i;
  box_t* res;
  box_internal_t* intern = man->internal;
  
  if (a->p==NULL || (dest && dest->p==NULL)){
    man->result.flag_best = tbool_true;
    man->result.flag_exact = tbool_top;
    return destructive ? a : box_copy(man,a);
  }
  res = box_copy(man,a);
  for (i=0;i<size;i++){
    box_bound_linexpr_internal(intern,
			       res->p[tdim[i]],
			       a,texpr[i]);
  }
  if (destructive) box_free(man,a);
  if (dest)
    res = box_meet(man,true,res,dest);
  man->result.flag_best = tbool_top;
  man->result.flag_exact = tbool_top;
  return res;
}



box_t* box_substitute_linexpr(ap_manager_t* man,
			      bool destructive,
			      box_t* a,
			      ap_dim_t dim, const ap_linexpr0_t* linexpr,
			      const box_t* dest)
{
  box_t* res;
  struct itv_t* itvinterval;
  struct itv_t* itvinterval2;
  box_lincons_t* itvlincons;
  size_t i;
  ap_dim_t dim2;
  num_t* pnum;
  box_internal_t* intern = man->internal;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  if (a->p==NULL || 
      (dest && dest->p==NULL)){
    res = destructive ? a : box_copy(man,a);
    return res;
  }
  else if (itv_is_top(a->p[dim])){
    res = 
      dest ?
      box_meet(man,destructive,a,dest) :
      (destructive ? a : box_copy(man,a));
  }
  itvinterval = intern->substitute_linexpr_itvinterval;
  itv_set_top(itvinterval);
  itv_swap(itvinterval,a->p[dim]);
  res =
    dest ?
    box_meet(man,destructive,a,dest) :
    (destructive ? a : box_copy(man,a))
    ;
  if (res->p==NULL){
    man->result.flag_exact = tbool_true;
    return res;
  }
  itvlincons = &intern->substitute_linexpr_itvlincons;
  itvinterval2 = intern->substitute_linexpr_itvinterval2;
  box_linexpr_set_linexpr(intern,
			  &itvlincons->itvlinexpr,
			  linexpr);
  itvlincons->constyp = AP_CONS_SUPEQ;
  itv_swap(itvlincons->itvlinexpr.cst,
	   itvinterval2);
  if (!bound_infty(itvinterval->inf)){
    itv_add_num(itvlincons->itvlinexpr.cst,
		itvinterval2,
		bound_numref(itvinterval->inf));
    box_meet_itvlincons(intern,res,itvlincons);
  }
  if (res->p==NULL){
    return res;
  }
  if (!bound_infty(itvinterval->sup)){
    box_linexpr_ForeachLinterm(&itvlincons->itvlinexpr,i,dim2,pnum){
      num_neg(*pnum,*pnum);
    }
    itv_add_num(itvlincons->itvlinexpr.cst,
		itvinterval2,
		bound_numref(itvinterval->sup));
    box_meet_itvlincons(intern,res,itvlincons);
  }
  man->result.flag_best = tbool_top;
  man->result.flag_exact = tbool_top;
  return res;
}


box_t* box_substitute_linexpr_array(ap_manager_t* man,
				    bool destructive,
				    box_t* a,
				    const ap_dim_t* tdim, 
				    const ap_linexpr0_t*const* texpr,
				    size_t size,
				    const box_t* dest)
{
  box_t* res;
  size_t i,j;
  ap_dim_t dim,dim2;
  num_t* pnum;
  struct itv_t* itvinterval;
  box_lincons_t* itvlincons;
  box_lincons_array_t array;
  box_internal_t* intern = man->internal;

  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  if (a->p==NULL || (dest && dest->p==NULL)){
    return destructive ? a : box_copy(man,a);
  }
  res = box_copy(man,a);

  for (i=0;i<size;i++){
    itv_set_top(res->p[tdim[i]]);
  }
  if (dest){
    res = box_meet(man,true,res,dest);
    if (res->p==NULL){
      goto box_substitute_linexpr_array_exit;
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
    goto box_substitute_linexpr_array_exit;
  }
  itvinterval = intern->substitute_linexpr_itvinterval;
  itvlincons = &intern->substitute_linexpr_itvlincons;
  array = box_lincons_array_make(j);
  j = 0;
  for (i=0;i<size;i++){
    dim = tdim[i];
    bool inf = !bound_infty(res->p[dim]->inf);
    bool sup = !bound_infty(res->p[dim]->sup);
    if (inf || sup){
      box_linexpr_set_linexpr(intern,
			      &itvlincons->itvlinexpr,
			      texpr[i]);
      itvlincons->constyp = AP_CONS_SUPEQ;
      itv_swap(itvlincons->itvlinexpr.cst,
	       itvinterval);
      if (inf){
	itv_add_num(itvlincons->itvlinexpr.cst,
		    itvinterval,
		    bound_numref(a->p[dim]->inf));
	box_lincons_set(&array.p[j],itvlincons);
	j++;
      }
      if (sup){
	box_linexpr_ForeachLinterm(&itvlincons->itvlinexpr,i,dim2,pnum){
	  num_neg(*pnum,*pnum);
	}
	itv_add_num(itvlincons->itvlinexpr.cst,
		    itvinterval,
		    bound_numref(a->p[dim]->sup));
	box_lincons_set(&array.p[j],itvlincons);
	j++;
      }
    }
  }
  box_meet_box_lincons_array(intern,res,&array,2);
  box_lincons_array_clear(&array);
  man->result.flag_best = tbool_top;
  man->result.flag_exact = tbool_top;
 box_substitute_linexpr_array_exit:
  if (destructive) box_free(man,a);
  return res;
}


