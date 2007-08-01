/* ************************************************************************* */
/* ap_generic: generic functions for library implementors */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "ap_generic.h"
#include "ap_scalar.h"

/* These functions are dedicated to implementors of domains. They offer generic
   default implementations for some of the operations required by the APRON
   API, when there is no more specific and efficient implementation for the
   domain being implemented.

   To use them, the function allocating manager, which is specific to the domain,
   should put the corresponding pointers in the virtual table to them.

   They manipulated "unboxed" abstract values, which are native to the
   underlying library: they are not yet boxed with the manager in the type
   ap_abstract0_t.
*/

/* ********************************************************************** */
/* I. Constructors */
/* ********************************************************************** */

tbool_t ap_generic_sat_tcons(ap_manager_t* man, void* abs, ap_tcons0_t* cons,
			     ap_scalar_discr_t discr, 
			     bool quasilinearize)
{
  tbool_t (*is_bottom)(ap_manager_t*,...) = man->funptr[AP_FUNID_IS_BOTTOM];
  tbool_t (*sat_lincons)(ap_manager_t*,...) = man->funptr[AP_FUNID_SAT_LINCONS];
  bool exact;
  ap_lincons0_t lincons0;
  tbool_t res;
  ap_abstract0_t a0;

  if (is_bottom(man,abs)==tbool_true){
  man->result.flag_exact = man->result.flag_best = tbool_true;
    return tbool_true;
  }

  a0.value = abs;
  a0.man = man;
  lincons0 = ap_intlinearize_tcons0(man,&a0,cons,&exact,discr,quasilinearize,false);
  res = sat_lincons(man,abs,&lincons0);
  ap_lincons0_clear(&lincons0);
  if (!exact){
    man->result.flag_exact = man->result.flag_best = tbool_top;
  }
  return res;
}

/*
   This function implements a generic bound_texpr operation using linearisation
   and bound_linexpr
*/
ap_interval_t* ap_generic_bound_texpr(ap_manager_t* man, void* abs, ap_texpr0_t* expr,
				      ap_scalar_discr_t discr, bool quasilinearize)
{
  tbool_t (*is_bottom)(ap_manager_t*,...) = man->funptr[AP_FUNID_IS_BOTTOM];
  ap_interval_t* (*bound_linexpr)(ap_manager_t*,...) = man->funptr[AP_FUNID_BOUND_LINEXPR];
  bool exact;
  ap_linexpr0_t* linexpr0;
  ap_interval_t* res;
  ap_abstract0_t a0;

  if (is_bottom(man,abs)==tbool_true){
    res = ap_interval_alloc();
    ap_interval_set_bottom(res);
    return res;
  }
  
  a0.value = abs;
  a0.man = man;
  linexpr0 = ap_intlinearize_texpr0(man,&a0,expr,&exact,discr,quasilinearize);
  res = bound_linexpr(man,abs,linexpr0);
  ap_linexpr0_free(linexpr0);
  if (!exact){
    man->result.flag_exact = man->result.flag_best = tbool_top;
  }
  return res;
}

ap_tcons0_array_t ap_generic_to_tcons_array(ap_manager_t* man,
					    void* abs)
{
  ap_lincons0_array_t (*to_lincons_array)(ap_manager_t*,...) = man->funptr[AP_FUNID_TO_LINCONS_ARRAY];
  ap_lincons0_array_t array = to_lincons_array(man,abs);
  ap_tcons0_array_t res = ap_tcons0_array_make(array.size);
  size_t i;
  for (i=0; i<array.size; i++){
    res.p[i] = ap_tcons0_from_lincons0(&array.p[i]);
  }
  ap_lincons0_array_clear(&array);
  return res;
}


/* ********************************************************************** */
/* II. Operations */
/* ********************************************************************** */

/* ============================================================ */
/*  Meet/Join on arrays of abstract values */
/* ============================================================ */

/*
   This function implements a generic meet/join_array operation using copy and meet
   operations.
*/
void* ap_generic_meetjoin_array(bool meet,
				ap_manager_t* man,
				void** tab, size_t size)
{
  void* (*copy)(ap_manager_t*,...) = man->funptr[AP_FUNID_COPY];
  void* (*meetjoin)(ap_manager_t*,...) = man->funptr[meet ? AP_FUNID_MEET : AP_FUNID_JOIN];
  size_t i;
  void* res;
  tbool_t exact,best;
  if (size==1){
    return copy(man,tab[0]);
  }
  else {
    res = meetjoin(man,false,tab[0],tab[1]);
    exact = man->result.flag_exact;
    best =  man->result.flag_best;
    for (i=2; i<size; i++){
      res = meetjoin(man,true,res,tab[i]);
      exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
      best =  (best==tbool_true ? man->result.flag_best : tbool_top);
    }
    man->result.flag_exact = exact;
    man->result.flag_best = best;
    return res;
  }
}

/* ============================================================ */
/*  Meet with array of constraints */
/* ============================================================ */

void*
ap_generic_meet_quasilinearize_lincons_array(ap_manager_t* man,
					     bool destructive,
					     void* abs,
					     ap_lincons0_array_t* array,
					     ap_scalar_discr_t discr,
					     bool linearize,
					     void* (*meet_lincons_array)(ap_manager_t*,
									 bool, void*,
									 ap_lincons0_array_t*))
{
  bool exact;
  ap_lincons0_array_t array2;
  void* res;
  void* (*copy)(ap_manager_t*,...) = man->funptr[AP_FUNID_COPY];
  tbool_t (*is_bottom)(ap_manager_t*,...) = man->funptr[AP_FUNID_IS_BOTTOM];

  man->result.flag_exact = man->result.flag_best = tbool_true;

  if (is_bottom(man,abs)==tbool_true || array->size==0){
    res = destructive ? abs : copy(abs);
  }
  else {
    array2 = ap_quasilinearize_lincons0_array(man,abs,array,&exact,
					      discr,linearize,true);
    res = meet_lincons_array(man,destructive,abs,&array2);
    if (!exact){
      man->result.flag_exact = man->result.flag_best = tbool_top;
    }
    if (array2.p!=array->p){
      ap_lincons0_array_clear(&array2);
    }
  }
  return res;
}

void*
ap_generic_meet_intlinearize_tcons_array(ap_manager_t* man,
					 bool destructive,
					 void* abs,
					 ap_tcons0_array_t* array,
					 ap_scalar_discr_t discr,
					 ap_linexpr_type_t linearize,
					 void* (*meet_lincons_array)(ap_manager_t*,
								     bool, void*,
								     ap_lincons0_array_t*))
{
  bool exact;
  ap_lincons0_array_t array2;
  void* res;
  void* (*copy)(ap_manager_t*,...) = man->funptr[AP_FUNID_COPY];
  tbool_t (*is_bottom)(ap_manager_t*,...) = man->funptr[AP_FUNID_IS_BOTTOM];
  ap_abstract0_t a0;

  man->result.flag_exact = man->result.flag_best = tbool_true;

  if (is_bottom(man,abs)==tbool_true || array->size==0){
    res = destructive ? abs : copy(man,abs);
  }
  else {
    a0.value = abs;
    a0.man = man;
    array2 = ap_intlinearize_tcons0_array(man,&a0,array,&exact,discr,linearize,true,true,2,false);
    res = meet_lincons_array(man,destructive,abs,&array2);
    if (!exact){
      man->result.flag_exact = man->result.flag_best = tbool_top;
    }
    ap_lincons0_array_clear(&array2);
  }
  return res;
}

/* ============================================================ */
/*  Assignemens/Substitutions */
/* ============================================================ */

/*
   This function implements generic parallel assignement/substitution
   operations by:
   1. introducing primed dimensions
   2. transforming linear expressions into equality constraints relating the
      assigned primed dimension and the linear expression
   If dest!=NULL
     3. introducing primed dimensions in dest
     4. exchanging primed and unprimed dimensions in dest
     5. intersecting the abstract value with the modified dest
   6. intersecting the obtained abstract value with the constraints
   7. exchanging primed and unprimed dimensions
   8. removing the introduced (primed) dimensions

   It relies on: is_bottom, copy, dimension, add_dimensions,
   permute_dimensions, remove_dimensions, meet_lincons_array, meet and free
   abstract operations.

   Meaning of parameters:
   - assign selects the operation: true means assignement, false substitution
   - The other parameters have the meaning they have for parallel
     assignement/substitution
*/

void* ap_generic_asssub_linexpr_array(bool assign,
				      ap_manager_t* man,
				      bool destructive, void* abs, ap_dim_t* tdim, ap_linexpr0_t** texpr, size_t size,
				      void* dest)
{
  tbool_t (*is_bottom)(ap_manager_t*,...) = man->funptr[AP_FUNID_IS_BOTTOM];
  void* (*copy)(ap_manager_t*,...) = man->funptr[AP_FUNID_COPY];
  void* (*add_dimensions)(ap_manager_t*,...) = man->funptr[AP_FUNID_ADD_DIMENSIONS];
  void* (*permute_dimensions)(ap_manager_t*,...) = man->funptr[AP_FUNID_PERMUTE_DIMENSIONS];
  void* (*remove_dimensions)(ap_manager_t*,...) = man->funptr[AP_FUNID_REMOVE_DIMENSIONS];
  void* (*meet_lincons_array)(ap_manager_t*,...) = man->funptr[AP_FUNID_MEET_LINCONS_ARRAY];
  void* (*meet)(ap_manager_t*,...) = man->funptr[AP_FUNID_MEET];
  void (*ap_free)(ap_manager_t*,...) = man->funptr[AP_FUNID_FREE];
  ap_dimension_t (*dimension)(ap_manager_t*,...) = man->funptr[AP_FUNID_DIMENSION];
  size_t i;
  ap_dimension_t d, dsup;
  ap_dimchange_t dimchange;
  ap_dimperm_t permutation;
  ap_lincons0_array_t array;
  void* abs2;
  tbool_t exact,best;

  if (is_bottom(man,abs)){
    man->result.flag_exact = man->result.flag_best = tbool_true;
    return destructive ? abs : copy(man,abs);
  }
  /* 1. Compute the number of integer and real dimensions assigned */
  d = dimension(man,abs);
  dsup.intdim = 0;
  dsup.realdim = 0;
  for (i=0; i<size; i++){
    if (tdim[i]<d.intdim)
      dsup.intdim++;
    else
      dsup.realdim++;
  }
  /* 2. Build dimchange (for addition of primed dimensions) */
  ap_dimchange_init(&dimchange,dsup.intdim,dsup.realdim);
  for (i=0;i<dsup.intdim;i++)
    dimchange.dim[i]=d.intdim;
  for (i=dsup.intdim;i<dsup.intdim+dsup.realdim;i++)
    dimchange.dim[i]=d.intdim+d.realdim;

  /* 3. Build permutation (exchanging primed and unprimed dimensions) */
  ap_dimperm_init(&permutation,d.intdim+d.realdim+dsup.intdim+dsup.realdim);
  ap_dimperm_set_id(&permutation);
  for (i=0; i<size; i++){
    ap_dim_t dim = tdim[i];
    ap_dim_t dimp = dim<d.intdim ? d.intdim+i : d.intdim+d.realdim+i;
    permutation.dim[dim] = dimp;
    permutation.dim[dimp] = dim;
  }

  /* 4. Add primed dimensions to abstract value */
  abs2 = add_dimensions(man,destructive,abs,&dimchange,false);
  exact = man->result.flag_exact;
  best =  man->result.flag_best;
  /* From now, work by side-effect on abs2 */

  /* 5. Build constraints system
     An assignement x'_i := a_ij x_j + b_i becomes
     an equality constraint -x'_i + a_ij x_j + b_i = 0
     Primed and unprimed dimensiosn permuted if dest!=NULL
  */
  array = ap_lincons0_array_make(size);
  for (i=0; i<size; i++){
    ap_dim_t dim = tdim[i];
    ap_dim_t dimp = permutation.dim[dim];
    ap_linexpr0_t* expr = ap_linexpr0_add_dimensions(texpr[i],&dimchange);
    ap_linexpr0_set_coeff_scalar_double(expr,dimp,-1.0);
    ap_lincons0_t cons = ap_lincons0_make(AP_CONS_EQ,expr,NULL);
    array.p[i] = cons;
  }
  /* 6. Permute unprimed and primed dimensions if !assign */
  if (!assign){
    abs2 = permute_dimensions(man,true,abs2,&permutation);
    exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
    best =  (best==tbool_true ? man->result.flag_best : tbool_top);
  }
  /* 7. If dest!=NULL, perform intersection */
  if (dest!=NULL){
    void* dest2 = add_dimensions(man,false,dest,&dimchange,false);
    exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
    best =  (best==tbool_true ? man->result.flag_best : tbool_top);

    if (assign){
      dest2 = permute_dimensions(man,true,dest2,&permutation);
      exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
      best =  (best==tbool_true ? man->result.flag_best : tbool_top);
    }
    abs2 = meet(man,true,abs2,dest2);
    exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
    best =  (best==tbool_true ? man->result.flag_best : tbool_top);

    ap_free(man,dest2);
  }
  /* 8. Perform meet of abs2 with constraints */
  abs2 = meet_lincons_array(man,true,abs2,&array);
  exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
  best =  (best==tbool_true ? man->result.flag_best : tbool_top);

  /* 9. Permute unprimed and primed dimensions if assign */
  if (assign){
    abs2 = permute_dimensions(man,true,abs2,&permutation);
    exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
    best =  (best==tbool_true ? man->result.flag_best : tbool_top);
  }
  /* 10. Remove extra dimensions */
  ap_dimchange_add_invert(&dimchange);
  abs2 = remove_dimensions(man,true,abs2,&dimchange);
  exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
  best =  (best==tbool_true ? man->result.flag_best : tbool_top);

  /* 11. Free allocated objects */
  ap_dimperm_clear(&permutation);
  ap_dimchange_clear(&dimchange);
  ap_lincons0_array_clear(&array);

  man->result.flag_exact = exact;
  man->result.flag_best = best;
  return abs2;
}

void* ap_generic_asssub_texpr_array(bool assign,
				    ap_manager_t* man,
				    bool destructive, void* abs, ap_dim_t* tdim, ap_texpr0_t** texpr, size_t size,
				    void* dest)
{
  tbool_t (*is_bottom)(ap_manager_t*,...) = man->funptr[AP_FUNID_IS_BOTTOM];
  void* (*copy)(ap_manager_t*,...) = man->funptr[AP_FUNID_COPY];
  void* (*add_dimensions)(ap_manager_t*,...) = man->funptr[AP_FUNID_ADD_DIMENSIONS];
  void* (*permute_dimensions)(ap_manager_t*,...) = man->funptr[AP_FUNID_PERMUTE_DIMENSIONS];
  void* (*remove_dimensions)(ap_manager_t*,...) = man->funptr[AP_FUNID_REMOVE_DIMENSIONS];
  void* (*meet_tcons_array)(ap_manager_t*,...) = man->funptr[AP_FUNID_MEET_TCONS_ARRAY];
  void* (*meet)(ap_manager_t*,...) = man->funptr[AP_FUNID_MEET];
  void (*ap_free)(ap_manager_t*,...) = man->funptr[AP_FUNID_FREE];
  ap_dimension_t (*dimension)(ap_manager_t*,...) = man->funptr[AP_FUNID_DIMENSION];
  size_t i;
  ap_dimension_t d, dsup;
  ap_dimchange_t dimchange;
  ap_dimperm_t permutation;
  ap_tcons0_array_t array;
  void* abs2;
  tbool_t exact,best;

  if (is_bottom(man,abs)){
    man->result.flag_exact = man->result.flag_best = tbool_true;
    return destructive ? abs : copy(man,abs);
  }
  /* 1. Compute the number of integer and real dimensions assigned */
  d = dimension(man,abs);
  dsup.intdim = 0;
  dsup.realdim = 0;
  for (i=0; i<size; i++){
    if (tdim[i]<d.intdim)
      dsup.intdim++;
    else
      dsup.realdim++;
  }
  /* 2. Build dimchange (for addition of primed dimensions) */
  ap_dimchange_init(&dimchange,dsup.intdim,dsup.realdim);
  for (i=0;i<dsup.intdim;i++)
    dimchange.dim[i]=d.intdim;
  for (i=dsup.intdim;i<dsup.intdim+dsup.realdim;i++)
    dimchange.dim[i]=d.intdim+d.realdim;

  /* 3. Build permutation (exchanging primed and unprimed dimensions) */
  ap_dimperm_init(&permutation,d.intdim+d.realdim+dsup.intdim+dsup.realdim);
  ap_dimperm_set_id(&permutation);
  for (i=0; i<size; i++){
    ap_dim_t dim = tdim[i];
    ap_dim_t dimp = dim<d.intdim ? d.intdim+i : d.intdim+d.realdim+i;
    permutation.dim[dim] = dimp;
    permutation.dim[dimp] = dim;
  }

  /* 4. Add primed dimensions to abstract value */
  abs2 = add_dimensions(man,destructive,abs,&dimchange,false);
  exact = man->result.flag_exact;
  best =  man->result.flag_best;
  /* From now, work by side-effect on abs2 */

  /* 5. Build constraints system
     An assignement x'_i := a_ij x_j + b_i becomes
     an equality constraint -x'_i + a_ij x_j + b_i = 0
     Primed and unprimed dimensiosn permuted if dest!=NULL
  */
  array = ap_tcons0_array_make(size);
  for (i=0; i<size; i++){
    ap_dim_t dim = tdim[i];
    ap_dim_t dimp = permutation.dim[dim];
    ap_texpr0_t* expr = ap_texpr0_add_dimensions(texpr[i],&dimchange);
    expr = ap_texpr0_binop(AP_TEXPR_SUB, expr,ap_texpr0_dim(dimp), 
			   AP_RTYPE_REAL, AP_RDIR_RND);
    ap_tcons0_t cons = ap_tcons0_make(AP_CONS_EQ,expr,NULL);
    array.p[i] = cons;
  }
  /* 6. Permute unprimed and primed dimensions if !assign */
  if (!assign){
    abs2 = permute_dimensions(man,true,abs2,&permutation);
    exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
    best =  (best==tbool_true ? man->result.flag_best : tbool_top);
  }
  /* 7. If dest!=NULL, perform intersection */
  if (dest!=NULL){
    void* dest2 = add_dimensions(man,false,dest,&dimchange,false);
    exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
    best =  (best==tbool_true ? man->result.flag_best : tbool_top);

    if (assign){
      dest2 = permute_dimensions(man,true,dest2,&permutation);
      exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
      best =  (best==tbool_true ? man->result.flag_best : tbool_top);
    }
    abs2 = meet(man,true,abs2,dest2);
    exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
    best =  (best==tbool_true ? man->result.flag_best : tbool_top);

    ap_free(man,dest2);
  }
  /* 8. Perform meet of abs2 with constraints */
  abs2 = meet_tcons_array(man,true,abs2,&array);
  exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
  best =  (best==tbool_true ? man->result.flag_best : tbool_top);

  /* 9. Permute unprimed and primed dimensions if assign */
  if (assign){
    abs2 = permute_dimensions(man,true,abs2,&permutation);
    exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
    best =  (best==tbool_true ? man->result.flag_best : tbool_top);
  }
  /* 10. Remove extra dimensions */
  ap_dimchange_add_invert(&dimchange);
  abs2 = remove_dimensions(man,true,abs2,&dimchange);
  exact = (exact==tbool_true ? man->result.flag_exact : tbool_top);
  best =  (best==tbool_true ? man->result.flag_best : tbool_top);

  /* 11. Free allocated objects */
  ap_dimperm_clear(&permutation);
  ap_dimchange_clear(&dimchange);
  ap_tcons0_array_clear(&array);

  man->result.flag_exact = exact;
  man->result.flag_best = best;
  return abs2;
}

