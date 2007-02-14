/* ********************************************************************** */
/* box_assign.c */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "box_internal.h"
#include "box_constructor.h"
#include "box_assign.h"

#include "ap_generic.h"
#include "itv_linexpr.h"

box_t* box_assign_linexpr(ap_manager_t* man,
			  bool destructive,
			  box_t* a,
			  ap_dim_t dim, const ap_linexpr0_t* linexpr,
			  const box_t* dest)
{
  bool exact;
  box_t* res;
  box_internal_t* intern = man->internal;

  man->result.flag_best = tbool_true;
  res = destructive ? a : box_copy(man,a);
  if (a->p==NULL || (dest!=NULL && dest->p==NULL)){
    man->result.flag_exact = tbool_true;
    return res;
  }
  exact = itv_eval_ap_linexpr0(intern->itv,
			       res->p[dim],
			       (const itv_t*)a->p,linexpr);
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
    itv_eval_ap_linexpr0(intern->itv,
			 res->p[tdim[i]],
			 (const itv_t*)a->p,texpr[i]);
  }
  if (destructive) box_free(man,a);
  if (dest)
    res = box_meet(man,true,res,dest);
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
  return (box_t*)ap_generic_substitute_linexpr_array(man,destructive,a,
						     tdim,texpr,size,
						     dest);
}

box_t* box_substitute_linexpr(ap_manager_t* man,
			      bool destructive,
			      box_t* a,
			      ap_dim_t dim, const ap_linexpr0_t* linexpr,
			      const box_t* dest)
{
  return (box_t*)ap_generic_substitute_linexpr_array(man,destructive,a,
						     &dim,&linexpr,1,
						     dest);
}

