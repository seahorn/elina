/* ********************************************************************** */
/* pk_meetjoin.c: Meet and join operations */
/* ********************************************************************** */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"
#include "pk_user.h"
#include "pk_representation.h"
#include "pk_constructor.h"
#include "pk_test.h"
#include "pk_cherni.h"
#include "pk_meetjoin.h"

/* ********************************************************************** */
/* I. Meet/Join */
/* ********************************************************************** */

/* In all the following functions, for the structure poly_t:

   - If meet is true, standard meaning of everything
   - If meet is false,
     - matrices C and F, satC and satF have been exchanged,
     - nbeq and nbline have been exchanged
     - in status, nothing has changed
*/

/* ====================================================================== */
/* I.1 Meet/Join of a (prepared) polyhedron with a sorted matrix of
       constraints/generators */
/* ====================================================================== */

/* The polyhedron is supposed:
   - to have constraints,
   - if lazy is false, to be minimized, and to have satC
   - possible emptiness not detected,

   Matrix of constraints is supposed:
   - to be sorted
   - to be canonical too.

   Return true if exception
 */

bool _poly_meet_matrix(bool meet,
		       bool lazy,
		       ap_manager_t* man,
		       poly_t* po,
		       const poly_t* polya, matrix_t* mat)
{
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  poly_t* pa = (poly_t*)polya;

  assert(mat->_sorted);

  man->result.flag_best = meet ? tbool_true : (pa->intdim>0 ? tbool_top : tbool_true);
  man->result.flag_exact = meet ? tbool_true : tbool_top;

  if (lazy){
    poly_obtain_sorted_C(pk,pa);
    if (po != pa){
      po->C = matrix_merge_sort(pk,pa->C,mat);
    }
    else {
       matrix_merge_sort_with(pk,pa->C,mat);
       if (pa->F){ matrix_free(pa->F); pa->F=NULL; }
       if (pa->satC){ satmat_free(pa->satC); pa->satC=NULL; }
       if (pa->satF){ satmat_free(pa->satF); pa->satF=NULL; }
       pa->nbeq = pa->nbline = 0;
    }
    po->status =
      meet ?
      poly_status_conseps :
      0
      ;
  }
  /* strict behaviour */
  else {
    size_t start = pa->C->nbrows;
    assert(pa->satC);
    if (po != pa){
      po->C = matrix_append(pa->C,mat);
      po->F = matrix_copy(pa->F);
      po->satC = satmat_copy_extend_columns(pa->satC,
					    bitindex_size(po->C->nbrows));
      po->nbline = pa->nbline;
      po->nbeq = pa->nbeq;
    }
    else {
      matrix_append_with(pa->C,mat);
      satmat_extend_columns(pa->satC,
			    bitindex_size(pa->C->nbrows));
    }
    cherni_add_and_minimize(pk,meet,po,start);
    if (pk->exn) goto _poly_meet_matrix_exit0;
    po->status = 
      meet ?
      ( poly_status_conseps | 
	poly_status_consgauss |
	(po->status & poly_status_gengauss) ) :
      ( (po->status & poly_status_consgauss) |
	poly_status_gengauss ) ;
    assert( poly_check_dual(pk,po,meet) );
  }
  return false;
 _poly_meet_matrix_exit0:
  pk->exn = AP_EXC_NONE;
  po->status = 0;
  {
    char str[160];
    sprintf(str,"conversion from %s of the (intermediate) result\n",
	    (meet ? po->C : po->F) ? "constraints to generators" : "generators to constraints");
    ap_manager_raise_exception(man,pk->exn,pk->funid,str);
  }
  return true;
}

/* ====================================================================== */
/* I.2 Meet/Join of two polyhedra, functional and side-effect versions */
/* ====================================================================== */

/* ---------------------------------------------------------------------- */
/* Factorized form */
/* ---------------------------------------------------------------------- */

bool _poly_meet_particularcases(bool meet, bool lazy,
				ap_manager_t* man,
				poly_t* po, const poly_t* pa, const poly_t* pb)
{
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  man->result.flag_exact = tbool_true;
  if (meet){
    /* if one is bottom, return bottom */
    if ( (!pa->C && !pa->F) || (!pb->C && !pb->F) ){
      poly_set_bottom(pk,po);
      return true;
    }
  }
  else {
    /* if one is bottom, return a copy of the other */
    if (!pa->C && !pa->F){
      if (!lazy){
	poly_chernikova_dual(man,pb,"of the second argument",false);
	pk->exn = AP_EXC_NONE;
      }
      poly_set(po,pb);
      return true;
    }
    if (!pb->C && !pb->F){
      if (!lazy){
	poly_chernikova_dual(man,pa,"of the first argument",false);
	pk->exn = AP_EXC_NONE;
      }
      poly_set(po,pa);
      return true;
    }
    /* if one want information about exactness, also test inclusion */
    if (pk->funopt->flag_exact_wanted){
      if (poly_is_leq(man,pa,pb)==tbool_true){
	poly_set(po,pb);
	return true;
      }
      else if (poly_is_leq(man,pb,pa)==tbool_true){
	poly_set(po,pa);
	return true;
      }
    }
  }
  return false;
}


void _poly_meet(bool meet,
		bool lazy,
		ap_manager_t* man,
		poly_t* po, const poly_t* pa, const poly_t* pb)
{
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  man->result.flag_best = tbool_true;

  if (pa==pb){
    if (!lazy) poly_chernikova_dual(man,pa,"of the first argument", meet);
    pk->exn = AP_EXC_NONE;
    poly_set(po,pa);
    man->result.flag_exact = tbool_true;
    return;
  }   
  
  /* Set the dimensions of po */
  if (po!=pa){
    po->intdim = pa->intdim;
    po->realdim = pa->realdim;
    assert(!po->C && !po->F && !po->satC && !po->satF);
  }
  /* Particular cases */
  if (_poly_meet_particularcases(meet,lazy,man,po,pa,pb))
    return;

  /* Get the constraint system of pa */
  poly_obtain_C_dual(man,pa,"of the first argument",meet);
  if (pk->exn){
    assert(!pa->C);
    pk->exn = AP_EXC_NONE;
    man->result.flag_best = man->result.flag_exact = tbool_false;
    if (meet) poly_set(po,pb);
    else { poly_set_top(pk,po); poly_dual(po); }
    return;
  }
  /* Get the constraint system of pb */
  poly_obtain_C_dual(man,pb,"of the second argument",meet);
  if (pk->exn){
    assert(!pb->C);
    pk->exn = AP_EXC_NONE;
    man->result.flag_best = man->result.flag_exact = tbool_false;
    if (meet) poly_set(po,pa);
    else { poly_set_top(pk,po); poly_dual(po); }
    return;
  }
  /* Particular cases again */
  if (_poly_meet_particularcases(meet,lazy,man,po,pa,pb))
    return;
  
  /* lazy behaviour */
  if (lazy){
  _poly_meet_entry0:
    poly_obtain_sorted_C(pk,pa);
    poly_obtain_sorted_C(pk,pb);
    _poly_meet_matrix(meet,lazy,man,po,pa,pb->C);
  }
  /* strict behaviour */
  else {
    if (po != pa){
      int start = 0;

      /* choice of the starting polyhedron */
      if (pa->F && !pb->F) /* only pa is minimized */
	start=1;
      else if (pb->F && !pa->F) /* only pb is minimized */
	start=2;
      else { /* either both or none are minimized */
	if (!pa->F && !pb->F){
	  /* ensure minimization */
	  poly_chernikova_dual(man,pa,"of the first argument",meet);
	  if (pk->exn){
	    assert(pa->C);
	    pk->exn = AP_EXC_NONE;
	    start = 2;
	  }
	  poly_chernikova_dual(man,pb,"of the second argument",meet);
	  if (pk->exn){
	    assert(pb->C);
	    pk->exn = AP_EXC_NONE;
	    if (start==2){
	      goto _poly_meet_entry0;
	    }
	    else {
	      start = 1;
	    }
	  }
	  /* Particular cases */
	  if (_poly_meet_particularcases(meet,lazy,man,po,pa,pb))
	    return;
	}
	/* Perform the choice */
	if (pa->F && pb->F){
	  if ( pa->nbeq > pb->nbeq
	       || (pa->nbeq == pb->nbeq &&
		   (pa->nbline < pb->nbline || pa->C->nbrows >= pb->C->nbrows) ) )
	    start = 1;
	  else
	    start = 2;
	}
      }
      if (start==2){
	const poly_t* p = pa; pa=pb; pb=p;
      }
    }
    /* Now, pa is the start polyhedron */
    poly_obtain_satC(pa);
    poly_obtain_sorted_C(pk,pb);
    _poly_meet_matrix(meet,lazy,man,po,pa,pb->C);
  }
  assert(poly_check_dual(pk,po,meet));
}

/* ====================================================================== */
/* I.3 Meet/Join array */
/* ====================================================================== */

poly_t* _poly_meet_array(bool meet,
			 bool lazy,
			 ap_manager_t* man,
			 const poly_t** po, size_t size)
{
  size_t intdim,realdim;
  poly_t* poly;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  man->result.flag_best = tbool_true;

  /* 1. Special cases */
  if (size==0){
    ap_manager_raise_exception(man,
			    AP_EXC_INVALID_ARGUMENT,
			    pk->funid, "empty array");
    man->result.flag_best = man->result.flag_exact = tbool_false;
    poly = poly_top(man,0,1);
    if (!meet) poly_dual(poly);
    return poly;    
  }
  intdim = po[0]->intdim;
  realdim = po[0]->realdim;
  poly = poly_alloc(intdim,realdim);
  if (size==1){
    if (!lazy){
      poly_chernikova_dual(man,po[0],"of the single argument",meet);
      pk->exn = AP_EXC_NONE;
    }
    poly_set(poly,po[0]);
    return poly;
  }
  else if (size==2){
    _poly_meet(meet,lazy,man,poly,po[0],po[1]);
    return poly;
  }
  /* 2. General case */
  else {
    matrix_t* C;
    size_t nbrows;
    int i,j;

    man->result.flag_best = tbool_true;
    man->result.flag_exact = meet ? tbool_true : tbool_top;

    /* Count the total number of constraints */
    nbrows = 0;
    for (i=0; i<size; i++){
      char str[80];
      sprintf(str,"of the %d argument",i);
      if (lazy)
	poly_obtain_C_dual(man,po[i],str,meet);
      else 
	poly_chernikova_dual(man,po[i],str,meet);
      if (pk->exn){
	pk->exn = AP_EXC_NONE;
	if (!po[i]->C){ 
	  man->result.flag_best = man->result.flag_exact = tbool_false;
	  poly_set_top(pk, poly);
	  if (!meet) poly_dual(poly);
	  return poly;
	} 
      }
      if (!po[i]->C){
	/* one polyhedron is empty */
	if (meet){
	  /* We return with bottom */
	  poly_set_bottom(pk,poly);
	  return poly;
	} else {
	  /* We exchange po[i] and po[size-1] */
	  do {
	    size--;
	    if (i<size){
	       poly_t* tmp = (poly_t*)po[i]; po[i] = po[size]; po[size] = tmp;
	    }
	    else {
	      goto _poly_meet_array_label0;
	    }
	  } 
	  while (!po[i]->C);
	}
      }
      nbrows += po[i]->C ? po[i]->C->nbrows : 0;
    }
  _poly_meet_array_label0:
    /* if size has been decreased */
    if (size<=2){
      assert(!meet);
      if (size==0){ 
	man->result.flag_exact = tbool_true;
	poly_set_bottom(pk,poly);
      } 
      else if (size==1){
	man->result.flag_exact = tbool_true;
	poly_set(poly,po[0]);
      }
      else if (size==2){
	_poly_meet(meet,lazy,man,poly,po[0],po[1]);
      }
      return poly;
    }
    /* 2.1. lazy behaviour */
    if (lazy){
      C = matrix_alloc(nbrows,pk->dec+intdim+realdim,true);
      C->nbrows = 0;
      C->_sorted = true;
      for (i=0; i<size; i++){
	if (po[i]->C){
	  poly_obtain_sorted_C(pk,po[i]);
	  matrix_merge_sort_with(pk,C,po[i]->C);
	}
      }
      poly->C = C;
      poly->status = 0;
    }
    /* 2.2 strict hehaviour */
    else {
      /* Minimizing and selecting the start polyhedron */
      j = 0; /* The selected start polyhedron */
      for (i=0; i<size; i++){
	if (po[i]->nbeq > po[j]->nbeq ||
	    (po[i]->nbeq == po[j]->nbeq &&
	     (po[i]->nbline < po[j]->nbline ||
	      po[i]->C->nbrows > po[j]->C->nbrows)))
	  j=i;
      }
      /* Add the other polyehdra to the polyhedra of index j */
      C = matrix_alloc(nbrows, pk->dec+intdim+realdim,true);
      C->nbrows = 0;
      C->_sorted = true;
      for (i=0; i<size; i++){
	if (i!=j){
	  poly_obtain_sorted_C(pk,po[i]);
	  matrix_merge_sort_with(pk,C,po[i]->C);
	}
      }
      matrix_revappend_with(C,po[j]->C);
      C->_sorted = false;
      poly->C = C;
      poly->F = matrix_copy(po[j]->F);
      poly_obtain_satC(po[j]);
      poly->satC = satmat_copy_extend_columns(po[j]->satC,
					      bitindex_size(C->nbrows));
      cherni_add_and_minimize(pk,meet,poly,po[j]->C->nbrows);
      if (pk->exn) goto _poly_meet_array_exit0;
      poly->status = 
	meet ?
	( poly_status_conseps | 
	  poly_status_consgauss |
	  (poly->status & poly_status_gengauss) ) :
	( (poly->status & poly_status_consgauss) |
	  poly_status_gengauss ) ;
    }
    return poly;
  _poly_meet_array_exit0:
    poly->status = 0;
    {
      char str[160];
      sprintf(str,"conversion from %s of the (intermediate) result\n",
	      (meet ? poly->C : poly->F) ? "constraints to generators" : "generators to constraints");
      ap_manager_raise_exception(man,pk->exn,pk->funid,str);
    }
    pk->exn = AP_EXC_NONE;
    return poly;
  }
}

/* ********************************************************************** */
/* II. Meet */
/* ********************************************************************** */

/* ********************************************************************** */
/* II.1 Meet of two or more polyhedra */
/* ********************************************************************** */

poly_t* poly_meet(ap_manager_t* man, 
		  bool destructive, poly_t* pa, const poly_t* pb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_MEET);
  poly_t* po = destructive ? pa : poly_alloc(pa->intdim,pa->realdim);
  _poly_meet(true, pk->funopt->algorithm < 0,
	     man,po,pa,pb);
  return po;
}

poly_t* poly_meet_array(ap_manager_t* man,
			 const poly_t** po, size_t size)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_MEET_ARRAY);
  return _poly_meet_array(true, pk->funopt->algorithm < 0,
			  man,po,size);
}

/* ====================================================================== */
/* II.2 Meet with (array of) linear constraint(s) */
/* ====================================================================== */

/* ---------------------------------------------------------------------- */
/* Factorized version */

void _poly_meet_lincons_array(bool lazy,
			      ap_manager_t* man,
			      poly_t* po, const poly_t* pa, const ap_lincons0_array_t* array)
{
  matrix_t* mat;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  man->result.flag_best = man->result.flag_exact = tbool_true;

  /* Get the constraint systems */
  if (lazy){
    poly_obtain_C(man,pa,"of the argument");
  } else {
    poly_chernikova(man,pa,"of the argument");
  }
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    if (!pa->C){
      poly_t* po2 = poly_of_lincons_array(man,pa->intdim,pa->realdim,array);
      man->result.flag_best = man->result.flag_exact = tbool_false;
      poly_set(po,po2);
      free(po2);
      return;
    }
  }
  /* if pa is bottom, return bottom */
  if ( !pa->C && !pa->F){
    poly_set(po,pa);
    return;
  }
  mat = matrix_of_lincons_array(pk,array,pa->intdim,pa->realdim, true);
  matrix_sort_rows(pk,mat);
  if (!lazy) poly_obtain_satC(pa);
  _poly_meet_matrix(true,lazy,man,po,pa,mat);
  matrix_free(mat);
}

poly_t* poly_meet_lincons_array(ap_manager_t* man, bool destructive, poly_t* pa, const ap_lincons0_array_t* array)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_MEET_LINCONS_ARRAY);
  poly_t* po = destructive ? pa : poly_alloc(pa->intdim,pa->realdim);
  _poly_meet_lincons_array(pk->funopt->algorithm<0,
			   man,po,pa,array);
  return po;
}

/* ********************************************************************** */
/* III. Join */
/* ********************************************************************** */

/* ====================================================================== */
/* III.1 Join of two or more polyhedra, functional and side-effect versions */
/* ====================================================================== */

poly_t* poly_join(ap_manager_t* man, bool destructive, poly_t* polya, const poly_t* polyb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_JOIN);
  poly_t* pa = (poly_t*)polya;
  poly_t* pb = (poly_t*)polyb;
  poly_t* po = destructive ? polya : poly_alloc(pa->intdim,pa->realdim);

  poly_dual(pa);
  if (pb!=pa) poly_dual(pb); /* We take care of possible alias */
  _poly_meet(false,pk->funopt->algorithm<0,
	     man,po,pa,pb);
  poly_dual(pa);
  if (pb!=pa) poly_dual(pb); /* We take care of possible alias */
  if (po!=pa) poly_dual(po);
  return po;
}

static int poly_cmp(const void* a, const void* b)
{
  poly_t* pa = *((poly_t**)a);
  poly_t* pb = *((poly_t**)b);
  return (pa>pb ? 1 : (pa==pb ? 0 : -1));
}

poly_t* poly_join_array(ap_manager_t* man, const poly_t** po, size_t size)
{
  poly_t** tpoly;
  poly_t* poly;
  int i;
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_JOIN_ARRAY);

  /* We have to take care of possible aliases in the array of polyhedra */
  tpoly = malloc(size*sizeof(poly_t*));
  memcpy(tpoly, po, size*sizeof(poly_t*));
  qsort(tpoly,size,sizeof(poly_t*),poly_cmp);
 
  /* remove doublons */
  for(i=0;i<size-1;i++){
    if (tpoly[i]==tpoly[i+1]){
      if (i<size-2){
	memmove(&tpoly[i+1],&tpoly[i+2],(size-i-2)*sizeof(poly_t*));
      }
      size--;
    }
  }
  /* dual */
  for (i=0;i<size;i++)
    poly_dual(tpoly[i]);

  poly =_poly_meet_array(false,pk->funopt->algorithm<0,
			 man,(const poly_t**)tpoly,size);
  for(i=0;i<size;i++){
    poly_dual(tpoly[i]);
  }
  free(tpoly);
  poly_dual(poly);
  return poly;
}

/* ====================================================================== */
/* III.2 Join with array of rays */
/* ====================================================================== */

/* ---------------------------------------------------------------------- */
/* Factorized version */

void _poly_add_ray_array(bool lazy, 
			  ap_manager_t* man,
			  poly_t* po, const poly_t* pa, const ap_generator0_array_t* array)
{
  matrix_t* mat;

  pk_internal_t* pk = (pk_internal_t*)man->internal;

  man->result.flag_best = man->result.flag_exact = tbool_true;

  /* Get the generator systems */
  if (lazy){
    poly_obtain_F(man,pa,"of the argument");
  } else {
    poly_chernikova(man,pa,"of the argument");
  }
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    if (!pa->F){
      man->result.flag_best = man->result.flag_exact = tbool_false;
      poly_set_top(pk,po);
      return;
    }
  }
  /* if pa is bottom, return bottom */
  if ( !pa->C && !pa->F){
    poly_set(po,pa);
    return;
  }
  mat = matrix_of_generator_array(pk,array,pa->intdim,pa->realdim);
  matrix_sort_rows(pk,mat);

  if (!lazy) poly_obtain_satF(pa);
  poly_dual(po);
  if (po!=pa) poly_dual((poly_t*)pa);
  _poly_meet_matrix(false,lazy,man,po,pa,mat);
  poly_dual(po);
  if (po!=pa) poly_dual((poly_t*)pa);
  matrix_free(mat);
  man->result.flag_exact = tbool_true;
}

poly_t* poly_add_ray_array(ap_manager_t* man, bool destructive, poly_t* pa, const ap_generator0_array_t* array)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_ADD_RAY_ARRAY);
  poly_t* po = destructive ? pa : poly_alloc(pa->intdim,pa->realdim);
  _poly_add_ray_array(pk->funopt->algorithm<0,
		      man,po,pa,array);
  return po;
}

