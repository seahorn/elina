/* ********************************************************************** */
/* util.c */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include <string.h>

#include "pk_config.h"
#include "pk_util.h"
#include "pk.h"

bool manager_check_dim_dim(manager_t* man,
			   const poly_t* a, dim_t dim,
			   funid_t funid)
{
  if (dim>a->intdim+a->realdim){
    char str[160];
    char* str2;

    snprintf(str,160,"\
incompatible dimensions for the two arguments:\n\
abstract:  (%3d,%3d)\n\
dimension: %3d\n",
	     a->intdim,
	     a->realdim,dim);
    str2 = strdup(str);
    manager_raise_exception(man,
			    EXC_INVALID_ARGUMENT,
			    funid,str2);
    return true;
  } else {
    return false;
  }
}

bool manager_check_dim_bin(manager_t* man,
			   const poly_t* pa, const poly_t* pb,
			   funid_t funid)
{
  if(pa->intdim != pb->intdim ||
     pa->realdim != pb->realdim){
    char str[160];
    char* str2;

    snprintf(str,160,"\
incompatible dimensions for the two arguments:\n\
first abstract:  (%3d,%3d)\n\
second abstract: (%3d,%3d)",
	     pa->intdim,
	     pa->realdim,
	     pb->intdim,
	     pb->realdim);
    str2 = strdup(str);
    manager_raise_exception(man,
			    EXC_INVALID_ARGUMENT,
			    funid,str2);
    return true;
  } else {
    return false;
  }
}

bool manager_check_dim_array(manager_t* man,
			     const poly_t** po, size_t size, funid_t funid)
{
  bool res;

  if (size==0)
    res = true;
  else {
    size_t intdim = po[0]->intdim;
    size_t realdim = po[0]->realdim;
    size_t i;
    
    res = false;
    for (i=1; i<size; i++){
      if (po[i]->intdim != intdim ||
	  po[i]->realdim != realdim){
	res = true;
	break;
      }
    }
  }
  if (res){
    char* str2;
    if (size==0){
      char str[160];
      snprintf(str,160,"array of size 0");
      str2 = strdup(str);
    }
    else {
      char* str = malloc(160+80*size);
      int i,count;

      count = sprintf(str,"incompatible dimensions for the array of polyhedra\n");
      for (i=0; i<size; i++){
	count += sprintf(str+count,"abstract %i: (%3d,%3d)\n",
			 i,
			 po[i]->intdim,
			 po[i]->realdim);
      }
      str2 = strdup(str);
      free(str);
    }
    manager_raise_exception(man,
			    EXC_INVALID_ARGUMENT,
			    funid,str2);
  }
  return res;
}

bool manager_check_dim_linexpr(manager_t* man,
			       const poly_t* a,
			       const linexpr0_t* expr,
			       funid_t funid)
{
  bool res;
  size_t nbdims;
  dim_t dim;
  int i;

  nbdims = a->intdim+a->realdim;
  res = false;
  dim = UINT_MAX;
  switch (expr->discr){
  case LINEXPR_DENSE:
    if (expr->size > nbdims){
      res = true;
      dim = expr->size-1;
    }
    break;
  case LINEXPR_SPARSE:
    for (i=0; i<expr->size; i++){
      if (expr->p.linterm[i].dim >= nbdims){
	res = true;
	dim = expr->p.linterm[i].dim;
	break;
      }
    }
    break;
  default:
    
    abort();
  }
  if (res){
    int count;
    char str[160];
    char* str2;
    count = sprintf(str,"incompatible dimension in the linear expression\n");
    count += sprintf(str+count,"poly value: (%3d,%3d), dimension %d\n",
		     a->intdim,
		     a->realdim,
		     dim);
    str2 = strdup(str);
    manager_raise_exception(man,
			    EXC_INVALID_ARGUMENT,
			    funid,str2);
  }
  return res;
}
