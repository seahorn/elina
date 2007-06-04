/* ************************************************************************* */
/* ap_linearize_aux: auxiliary functions for (quasi)linearisation */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

/* Auxiliary module to ap_linearize, which contains functions depending of the
   number representation */

#include "ap_linearize.h"
#include "ap_linearize_aux.h"

static
bool quasilinearize_alloc(ap_manager_t* man, void* abs,
			  itv_internal_t** pintern,
			  itv_lincons_t* plincons,
			  itv_t** ptitv, size_t* psize)
{
  tbool_t (*is_bottom)(ap_manager_t*,...) = man->funptr[AP_FUNID_IS_BOTTOM];
  ap_interval_t** (*to_box)(ap_manager_t*,...) = man->funptr[AP_FUNID_TO_BOX];
  ap_dimension_t (*dimension)(ap_manager_t*,...) = man->funptr[AP_FUNID_DIMENSION];
  bool exact,exact2;
  ap_dimension_t dim;
  ap_interval_t** tinterval;

  assert(is_bottom(man,abs)!=tbool_true);
  exact = true;

  tinterval = to_box(man,abs);
  exact = (man->result.flag_exact == tbool_true) && exact;

  dim = dimension(man,abs);
  *psize = dim.intdim+dim.realdim;
  *pintern = itv_internal_alloc();
  exact = itv_array_set_ap_interval_array(*pintern,ptitv,tinterval,*psize)
    && exact;
  ap_interval_array_free(tinterval,*psize);
  itv_lincons_init(plincons);
  return exact;
}

static inline
void quasilinearize_free(itv_internal_t* intern, 
			 itv_lincons_t* lincons,
			 itv_t* titv, size_t size)
{
  itv_lincons_clear(lincons);
  itv_array_free(titv,size);
  itv_internal_free(intern);
}

/* ********************************************************************** */
/* Quasilinearization */
/* ********************************************************************** */

/* Evaluate a interval linear expression on the abstract
   value such as to transform it into a quasilinear expression.

   discr allows to choose the type of scalars used for computations and for the
   result.  pexact is a pointer to a Boolean, which is set to true if all
   the conversions and computations were exact.
*/


ap_linexpr0_t* ITVFUN(ap_quasilinearize_linexpr0)
  (ap_manager_t* man,
   void* abs,
   ap_linexpr0_t* linexpr0,
   bool* pexact)
{
  size_t nbdims;
  ap_linexpr0_t* rlinexpr0;
  itv_internal_t* intern;
  itv_lincons_t lincons;
  itv_t* titv;
  bool exact,exact2;

  exact = quasilinearize_alloc(man,abs,&intern,&lincons,&titv,&nbdims);
  exact = itv_linexpr_set_ap_linexpr0(intern,&lincons.linexpr,linexpr0)
    && exact;
  exact = itv_linexpr_quasilinearize(intern,&lincons.linexpr,titv)
    && exact;
  rlinexpr0 = NULL;
  ap_linexpr0_set_itv_linexpr(intern,&rlinexpr0,&lincons.linexpr);
  quasilinearize_free(intern,&lincons,titv,nbdims);
  *pexact = exact;
  return rlinexpr0;
}

/* Same for ap_lincons0_t */
ap_lincons0_t ITVFUN(ap_quasilinearize_lincons0)
  (ap_manager_t* man,
   void* abs,
   ap_lincons0_t* lincons0,
   bool* pexact)
{
  ap_linexpr0_t* rlinexpr0 = ITVFUN(ap_quasilinearize_linexpr0)(man,abs,
								lincons0->linexpr0,
								pexact);
  return ap_lincons0_make(lincons0->constyp,
			  rlinexpr0,
			  (lincons0->scalar ?
			   ap_scalar_alloc_set(lincons0->scalar) :
			   NULL)
			  );
}

/* Same for arrays of ap_linexpr0_t */
ap_linexpr0_t** ITVFUN(ap_quasilinearize_tlinexpr0)(ap_manager_t* man,
						   void* abs,
						   ap_linexpr0_t** texpr, size_t size,
						   bool* pexact)
{
  size_t nbdims;
  ap_linexpr0_t** tab;
  itv_internal_t* intern;
  itv_lincons_t lincons;
  itv_t* titv;
  bool exact,exact2;
  size_t i;

  exact = quasilinearize_alloc(man,abs,&intern,&lincons,&titv,&nbdims);
  tab = (ap_linexpr0_t**)malloc(size*sizeof(ap_linexpr0_t*));
  for (i=0; i<size; i++){
    exact = itv_linexpr_set_ap_linexpr0(intern,&lincons.linexpr,texpr[i])
      && exact;
    exact = itv_linexpr_quasilinearize(intern,&lincons.linexpr,titv)
      && exact;
    tab[i] = NULL;
    ap_linexpr0_set_itv_linexpr(intern,&tab[i],&lincons.linexpr);
  }
  quasilinearize_free(intern,&lincons,titv,nbdims);
  *pexact = exact;
  return tab;
}

/* Same for ap_lincons0_array_t */
ap_lincons0_array_t
ITVFUN(ap_quasilinearize_lincons0_array)(ap_manager_t* man,
					 void* abs,
					 ap_lincons0_array_t* array,
					 bool* pexact,
					 bool convert,
					 bool linearize)
{
  ap_lincons0_array_t res;
  size_t i,j,k;
  ap_dim_t dim;
  size_t nbdims;
  itv_internal_t* intern;
  itv_lincons_t lincons;
  itv_t* titv;
  ap_coeff_t* pcoeff;
  bool exact;
  ap_lincons0_t lincons0;

  exact = quasilinearize_alloc(man,abs,&intern,&lincons,&titv,&nbdims);
  lincons0 = ap_lincons0_make(AP_CONS_EQ,
			      ap_linexpr0_alloc(AP_LINEXPR_SPARSE,0),
			      NULL);
  res = ap_lincons0_array_make(array->size);
  j = 0;
  for (i=0; i<array->size; i++){
    if (linearize && j+2 > res.size){
      ap_lincons0_array_resize(&res,((j+2)*3)/2);
    }
    if (!convert &&
	(ap_linexpr0_is_linear(array->p[i].linexpr0) ||
	 (!linearize && ap_linexpr0_is_quasilinear(array->p[i].linexpr0)))){
      res.p[j] = ap_lincons0_copy(&array->p[i]);
      j++;
    }
    else {
      exact = itv_lincons_set_ap_lincons0(intern,&lincons,&array->p[i])
	&& exact;
      exact = itv_lincons_quasilinearize(intern,&lincons,titv)
	&& exact;
      ap_lincons0_set_itv_lincons(intern,&lincons0,&lincons);
      if (linearize){
	ap_linearize_quasilincons0(&res,&j,&lincons0);
      }
      else {
	res.p[j] = ap_lincons0_copy(&lincons0);
	j++;
      }
    }
  }
  *pexact = exact;
  if (linearize){
    if (j != res.size){
      ap_lincons0_array_resize(&res,j);
    }
  }
  ap_lincons0_clear(&lincons0);
  quasilinearize_free(intern,&lincons,titv,nbdims);
  return res;
}
