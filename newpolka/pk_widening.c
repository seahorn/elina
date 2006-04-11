/* ********************************************************************** */
/* pk_widening.c: widening  */
/* ********************************************************************** */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_bit.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk_cherni.h"

#include "pk_user.h"
#include "pk_representation.h"
#include "pk_constructor.h"
#include "pk_test.h"
#include "pk_widening.h"

/* This function defines the standard widening operator.  The resulting
   polyhedron has no frame matrix, unless pa is empty. */

poly_t* poly_widening(ap_manager_t* man, const poly_t* pa, const poly_t* pb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_WIDENING);
  bool widening_affine = pk->funopt->algorithm<=0;
  poly_chernikova(man,pa,"of the first argument");
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_best = man->result.flag_exact = tbool_false;
    return poly_top(man,pa->intdim,pa->realdim);
  }
  poly_chernikova(man,pb,"of the second argument");
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_best = man->result.flag_exact = tbool_false;
    return poly_top(man,pa->intdim,pa->realdim);
  }
  if (!pa->C && !pa->F) /* pa is empty */
    return poly_copy(man,pb);
  else {
    size_t sat_nbcols;
    size_t nbrows,i;
    int index;
    satmat_t* sat;
    poly_t* po;
    bitstring_t* bitstringp;

    /* copy saturation pa->satF, and sort it */
    poly_obtain_satF(pa);
    sat = satmat_copy(pa->satF);
    satmat_sort_rows(sat);
    sat_nbcols = sat->nbcolumns;

    po = poly_alloc(pa->intdim,pa->realdim);

    po->C = matrix_alloc(pk->dec-1+pb->C->nbrows, pb->C->nbcolumns, false);
    _matrix_fill_constraint_top(pk,po->C,0);
    nbrows = pk->dec-1;

    /* Adding constraints of pb mutually redundant with some of pa, except if
     it is mutually redundant with the positivity constraint of pa only. */
    bitstringp = bitstring_alloc(sat_nbcols);
    for (i=0; i<pb->C->nbrows; i++){
      bitstring_clear(bitstringp,sat_nbcols);
      cherni_buildsatline(pk, pa->F, (const numint_t*)pb->C->p[i], bitstringp);
      index = satmat_index_in_sorted_rows(bitstringp,sat);
      if ( index>=0 &&
	   (!widening_affine ||
	    !vector_is_positivity_constraint(pk, 
					     (const numint_t*)pa->C->p[index],
					     pa->C->nbcolumns)) ){
	/* Belongs to saturation matrix, and does not correspond to the
	   positivity constraint. */
	vector_copy(po->C->p[nbrows],(const numint_t*)pb->C->p[i], 
		    pa->C->nbcolumns);
	  nbrows++;
      }
    }
    satmat_free(sat);
    bitstring_free(bitstringp);
    po->C->nbrows = nbrows;
    man->result.flag_best = man->result.flag_exact = tbool_top;
    return po;
  }
}

/* This second one implements a version parametrized by a set of constraints:
when a constraint of this set is verified by both polyhedra, it is kept in
the result. */
poly_t* poly_widening_threshold(ap_manager_t* man, 
			       const poly_t* pa, const poly_t* pb, 
			       const ap_lincons0_array_t* array)
{
  poly_t* po;
  int i,nbrows;
  matrix_t* mat;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  po = poly_widening(man,pa,pb);
  if (!po->C && !po->F)
    return po;

  mat = matrix_of_lincons_array(pk,array,pa->intdim,pa->realdim,true);
  
  /* We assume that both pa and pb are minimized, and that po->F==NULL */
  nbrows = po->C->nbrows;
  matrix_realloc_lazy(po->C, nbrows + mat->nbrows);
  for (i=0; i<mat->nbrows; i++){
    if (do_generators_sat_constraint(pk,pb->F,
				     (const numint_t*)mat->p[i],
				     pk->strict && 
				     numint_sgn(mat->p[i][polka_eps])<0))
    {
      /* if the constraint is satisfied by pb, add it */
      vector_copy(po->C->p[nbrows],(const numint_t*)mat->p[i],mat->nbcolumns);
      nbrows++;
    }
  }
  matrix_free(mat);
  po->C->nbrows = nbrows;
  matrix_minimize(po->C);
  return po;
}
