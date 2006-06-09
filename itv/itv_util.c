/* ********************************************************************** */
/* util.c */
/* ********************************************************************** */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "manager.h"
#include "expr0.h"

#include "itv_util.h"

static inline char* strdup(const char* s)
{
  char* s2;

  s2 = malloc(strlen(s)+1);
  strcpy(s2,s);
  return s2;
}

bool manager_check_dim_dim(manager_t* man,
			     itv_t* a, dim_t dim,
			     char* prefix)
{
  if (dim>itv_dimension(man,a)){
    char str[160];
    char* str2;

    snprintf(str,160,"\
%s: incompatible dimensions for the two arguments:\n\
interval:  (%3d,%3d)\n\
dimension: %3d\n",
	     prefix,
	     itv_integer_dimension(man,a),
	     itv_real_dimension(man,a),dim);
    str2 = strdup(str);
    manager_raise_exception(man,
			    EXC_INVALID_ARGUMENT,
			    str2);
    return true;
  } else {
    return false;
  }
}
bool manager_check_dim_bin(manager_t* man,
			     itv_t* pa, itv_t* pb,
			     char* prefix)
{
  if(itv_integer_dimension(man,pa) != itv_integer_dimension(man,pb) ||
     itv_real_dimension(man,pa) != itv_real_dimension(man,pb)){
    char str[160];
    char* str2;

    snprintf(str,160,"\
%s: incompatible dimensions for the two arguments:\n\
first interval:  (%3d,%3d)\n\
second interval: (%3d,%3d)",
	     prefix,itv_integer_dimension(man,pa),
	     itv_real_dimension(man,pa),
	     itv_integer_dimension(man,pb),
	     itv_real_dimension(man,pb));
    str2 = strdup(str);
    manager_raise_exception(man,
			    EXC_INVALID_ARGUMENT,
			    str2);
    return true;
  } else {
    return false;
  }
}

bool manager_check_dim_array(manager_t* man,
			       itv_t** po, size_t size, char* prefix)
{
  bool res;

  if (size==0)
    res = true;
  else {
    size_t intdim = itv_integer_dimension(man,po[0]);
    size_t realdim = itv_real_dimension(man,po[0]);
    size_t i;

    res = false;
    for (i=1; i<size; i++){
      if (itv_integer_dimension(man,po[i]) != intdim ||
	  itv_real_dimension(man,po[i]) != realdim){
	res = true;
	break;
      }
    }
  }
  if (res){
    char* str2;
    if (size==0){
      char str[160];
      snprintf(str,160,"%s: array of size 0",prefix);
      str2 = strdup(str);
    }
    else {
      char* str = malloc(160+80*size);
      int i,count;

      count = sprintf(str,"%s: incompatible dimensions for the array of polyhedra\n",prefix);
      for (i=0; i<size; i++){
	count += sprintf(str+count,"interval %i: (%3d,%3d)\n",
			 i,
			 itv_integer_dimension(man,po[i]),
			 itv_real_dimension(man,po[i]));
      }
      str2 = strdup(str);
      free(str);
    }
    manager_raise_exception(man,
			    EXC_INVALID_ARGUMENT,
			    str2);
  }
  return res;
}

bool manager_check_dim_linexpr(manager_t* man,
				 itv_t* a, linexpr_t* expr,
				 char* prefix)
{
  bool res;
  size_t nbdims;
  dim_t dim;
  int i;

  nbdims = itv_dimension(man,a);
  res = false;
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
    assert(0);
    res = true;
    dim = 0;
  }
  if (res){
    int count;
    char str[160];
    char* str2;
    count = sprintf(str,"%s: incompatible dimension in the linear expression\n",prefix);
    count += sprintf(str+count,"itv value: (%3d,%3d), dimension %d\n",
		     itv_integer_dimension(man,a),
		     itv_real_dimension(man,a),
		     dim);
    str2 = strdup(str);
    manager_raise_exception(man,
			    EXC_INVALID_ARGUMENT,
			    str2);
  }
  return res;
}
