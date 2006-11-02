/* ********************************************************************** */
/* pk_widening.c: widening  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

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

typedef struct satmat_row_t {
  bitstring_t* p; 
    /* pointing somewhere in the field pinit of an underlying satmat_t object */
  int index;
} satmat_row_t;


static satmat_row_t* esatmat_of_satmat(satmat_t* sat)
{
  size_t i;
  satmat_row_t* tab;

  tab = malloc(sat->nbrows * sizeof(satmat_row_t));
  for (i=0; i<sat->nbrows; i++){
    tab[i].p = sat->p[i];
    tab[i].index = i;
  }
  return tab;
}

/* Row sorting.

We use here the insertion sort. 
The array tab is supposed to be of size sat->nbrows.
*/
static void esatmat_isort_rows(satmat_row_t* tab, const satmat_t* sat)
{
  int i,j;

  for (i=1; i<sat->nbrows; i++){
    satmat_row_t row = tab[i];
    j = i;
    while (j > 0 && bitstring_cmp(tab[j-1].p, row.p, sat->nbcolumns) > 0){
      tab[j] = tab[j-1];
      j--;
    }
    tab[j] = row;
  }
}

/* Row sorting.

We use here the quick sort. */
static size_t qsort_size = 0;
static int qsort_rows_compar(const void* p1, const void* p2)
{
  return (bitstring_cmp( ((satmat_row_t*)p1)->p,
			 ((satmat_row_t*)p2)->p,
			 qsort_size));
}

static void esatmat_sort_rows(satmat_row_t* tab, const satmat_t* sat)
{
  if (sat->nbrows>=6){
    qsort_size = sat->nbcolumns;
    qsort(tab,
	  (size_t)sat->nbrows, sizeof(satmat_row_t),
	  qsort_rows_compar);
  }
  else {
    esatmat_isort_rows(tab,sat);
  }
}

/* Membership test.

The following function tests if the given row belongs to the sorted saturation
matrix. If it is the case, it returns its rank in the saturation
matrix. Otherwise, it returns -1 */

static const bitstring_t* index_satline = NULL;
static satmat_row_t* index_tab = NULL;
static size_t index_size = 0;

static bool index2(size_t low, size_t high)
{
  if (high - low <= 4){
    int i;
    int res=-1;
    for (i=low; i<high; i++){
      int cmp = bitstring_cmp(index_tab[i].p,index_satline,index_size);
      if (cmp==0){
	res=i; break;
      }
      else if (cmp>0) break;
    }
    return res;
  }
  else {
    size_t mid = low+(high-low)/2;
    int cmp = bitstring_cmp(index_tab[mid].p,index_satline,index_size);
    if (cmp<0)
      return (index2(mid+1,high));
    else if (cmp>0)
      return (index2(low,mid));
    else
      return mid;
  }
}

static
int esatmat_index_in_sorted_rows(const bitstring_t* const satline, 
				 satmat_row_t* tab, 
				 const satmat_t* const sat)
{
  index_satline = satline;
  index_tab = tab;
  index_size = sat->nbcolumns;
  return (index2(0,sat->nbrows));
}

/* This function defines the standard widening operator.  The resulting
   polyhedron has no frame matrix, unless pa is empty. */
poly_t* poly_widening(ap_manager_t* man, const poly_t* pa, const poly_t* pb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_WIDENING);
  bool widening_affine = pk->funopt->algorithm<=0;
  poly_chernikova2(man,pa,"of the first argument");
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_best = man->result.flag_exact = tbool_false;
    return poly_top(man,pa->intdim,pa->realdim);
  }
  poly_chernikova2(man,pb,"of the second argument");
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
    poly_t* po;
    bitstring_t* bitstringp;
    satmat_row_t* tab;
 
    /* copy saturation pa->satF, and sort it */
    poly_obtain_satF(pa);
    tab = esatmat_of_satmat(pa->satF);
    esatmat_sort_rows(tab,pa->satF);
    sat_nbcols = pa->satF->nbcolumns;

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
      index = esatmat_index_in_sorted_rows(bitstringp,tab,pa->satF);
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
    free(tab);
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
