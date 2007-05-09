/* ************************************************************************* */
/* ap_linearize: generic functions for (quasi)linearisation ofinterval expressions */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "ap_linearize.h"


/* ********************************************************************** */
/* Prototypes */
/* ********************************************************************** */

void* itvmpq_internal_alloc(void);
bool itvmpq_ap_linexpr0_set_ap_linexpr0(void* intern,
					ap_linexpr0_t* rlinexpr0,
					void* titv,
					ap_linexpr0_t* linexpr0);
bool itvmpq_array_set_ap_interval_array(void* intern,
					void** ptitv,
					ap_interval_t** tinterval,
					size_t size);
void itvmpq_array_free(void* titv, size_t size);
void itvmpq_internal_free(void* intern);

void* itvdbl_internal_alloc(void);
bool itvdbl_ap_linexpr0_set_ap_linexpr0(void* intern,
					ap_linexpr0_t* rlinexpr0,
					void* titv,
					ap_linexpr0_t* linexpr0);
bool itvdbl_array_set_ap_interval_array(void* intern,
					void** ptitv,
					ap_interval_t** tinterval,
					size_t size);
void itvdbl_array_free(void* titv, size_t size);
void itvdbl_internal_free(void* intern);

static
bool quasilinearize_alloc(ap_manager_t* man, void* abs,
			  void** intern, void** titv, size_t* size,
			  ap_scalar_discr_t discr)
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
  exact2 = (man->result.flag_exact == tbool_true); exact = exact && exact2;

  dim = dimension(man,abs);
  *size = dim.intdim+dim.realdim;
  switch (discr){
  case AP_SCALAR_MPQ:
    *intern = itvmpq_internal_alloc();
    exact2 = itvmpq_array_set_ap_interval_array(*intern,titv,tinterval,*size);
    break;
  case AP_SCALAR_DOUBLE:
    *intern = itvdbl_internal_alloc();
    exact2 = itvdbl_array_set_ap_interval_array(*intern,titv,tinterval,*size);
    break;
  }
  exact = exact && exact2;
  ap_interval_array_free(tinterval,*size);
  return exact;
}
static
void quasilinearize_free(void* intern, void* titv, size_t size,
			 ap_scalar_discr_t discr)
{
  switch (discr){
  case AP_SCALAR_MPQ:
    itvmpq_array_free(titv,size);
    itvmpq_internal_free(intern);
    break;
  case AP_SCALAR_DOUBLE:
    itvdbl_array_free(titv,size);
    itvdbl_internal_free(intern);
    break;
  }
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


ap_linexpr0_t* ap_quasilinearize_linexpr0(ap_manager_t* man,
					  void* abs,
					  ap_linexpr0_t* linexpr0,
					  ap_scalar_discr_t discr,
					  bool* pexact)
{
  size_t nbdims;
  ap_linexpr0_t* rlinexpr0;
  void* intern;
  void* titv;
  bool exact,exact2;

  exact = quasilinearize_alloc(man,abs,&intern,&titv,&nbdims,discr);

  rlinexpr0 = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,0);
  switch (discr){
  case AP_SCALAR_MPQ:
    exact2 = itvmpq_ap_linexpr0_set_ap_linexpr0(intern,rlinexpr0,titv,linexpr0);
    break;
  case AP_SCALAR_DOUBLE:
    exact2 = itvdbl_ap_linexpr0_set_ap_linexpr0(intern,rlinexpr0,titv,linexpr0);
    break;
  default:
    abort();
  }
  exact = exact && exact2;
  *pexact = exact;

  quasilinearize_free(intern,titv,nbdims,discr);
  return rlinexpr0;
}

/* Same for ap_lincons0_t */
ap_lincons0_t ap_quasilinearize_lincons0(ap_manager_t* man,
					 void* abs,
					 ap_lincons0_t* lincons0,
					 ap_scalar_discr_t discr,
					 bool* pexact)
{
  ap_linexpr0_t* rlinexpr0 = ap_quasilinearize_linexpr0(man,abs,lincons0->linexpr0,
							discr,pexact);
  return ap_lincons0_make(lincons0->constyp,
			  rlinexpr0,
			  (lincons0->scalar ? 
			   ap_scalar_alloc_set(lincons0->scalar) : 
			   NULL)
			  );
}

/* Same for arrays of ap_linexpr0_t */
ap_linexpr0_t** ap_quasilinearize_tlinexpr0(ap_manager_t* man,
					    void* abs,
					    ap_linexpr0_t** texpr, size_t size,
					    ap_scalar_discr_t discr,
					    bool* pexact)
{
  size_t nbdims;
  ap_linexpr0_t** tab;
  void* intern;
  void* titv;
  bool exact,exact2;
  size_t i;

  exact = quasilinearize_alloc(man,abs,&intern,&titv,&nbdims,discr);

  tab = (ap_linexpr0_t**)malloc(size*sizeof(ap_linexpr0_t*));
  switch (discr){
  case AP_SCALAR_MPQ:
    for (i=0; i<size; i++){
      tab[i] = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,0);
      exact2 = itvmpq_ap_linexpr0_set_ap_linexpr0(intern,tab[i],titv,texpr[i]);
      exact = exact && exact2;
    }
    break;
  case AP_SCALAR_DOUBLE:
    for (i=0; i<size; i++){
      tab[i] = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,0);
      exact2 = itvdbl_ap_linexpr0_set_ap_linexpr0(intern,tab[i],titv,texpr[i]);
      exact = exact && exact2;
    }
    break;
  }
  *pexact = exact;
  quasilinearize_free(intern,titv,nbdims,discr);
  return tab;
}

/* Same for ap_lincons0_array_t */
ap_lincons0_array_t 
ap_quasilinearize_lincons0_array(ap_manager_t* man,
				 void* abs,
				 ap_lincons0_array_t* array,
				 ap_scalar_discr_t discr,
				 bool* pexact,
				 bool convert,
				 bool linearize)
{
  ap_lincons0_array_t res;
  size_t i,j,k;
  ap_dim_t dim;
  size_t nbdims;
  void* intern;
  void* titv;
  ap_coeff_t* pcoeff;
  bool exact,exact2;

  exact = quasilinearize_alloc(man,abs,&intern,&titv,&nbdims,discr);
  res = ap_lincons0_array_make(array->size);
  j = 0;
  for (i=0; i<array->size; i++){
    if (j+2 > res.size){
      ap_lincons0_array_resize(&res,((j+2)*3)/2);
    }
    if (!convert &&
	(ap_linexpr0_is_linear(array->p[i].linexpr0) ||
	 (!linearize && ap_linexpr0_is_quasilinear(array->p[i].linexpr0)))){
      goto _ap_quasilinearize_lincons0_array_std;
    }
    ap_linexpr0_t* linexpr0 = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,0);
    switch (discr){
    case AP_SCALAR_MPQ:
      exact2 = itvmpq_ap_linexpr0_set_ap_linexpr0(intern,linexpr0,
						  titv,array->p[i].linexpr0);
      break;
    case AP_SCALAR_DOUBLE:
      exact2 = itvdbl_ap_linexpr0_set_ap_linexpr0(intern,linexpr0,
						  titv,array->p[i].linexpr0);
      break;
    default:
      abort();
    }
    exact = exact && exact2;
    if (linearize){
      /* Transform sets of constraints as follows:
	 e.x + [a,b] >= 0 ==> e.x + b >= 0
	 e.x + [a,b] > 0  ==> e.x + b > 0
	 e.x + [a,b] = 0  ==> e.x + b >= 0 and e.x + a <= 0
	 e.x + [a,b] = 0 mod k ==> unchanged
	 
	 Also removes trivially true constraints e.x + oo >= 0
      */
      ap_scalar_t scalar;
      ap_scalar_init(&scalar,AP_SCALAR_DOUBLE);
      ap_coeff_t* pcst = ap_linexpr0_cstref(linexpr0);
      switch (array->p[i].constyp){
      case AP_CONS_EQ:
	switch (pcst->discr){
	case AP_COEFF_INTERVAL:
	  {
	    int supinfty = ap_scalar_infty(pcst->val.interval->sup);
	    if (!ap_scalar_infty(pcst->val.interval->inf)){
	      ap_linexpr0_t* expr = 
		supinfty ?
		linexpr0 :
		ap_linexpr0_copy(linexpr0)
		;
	      ap_scalar_neg(&scalar, pcst->val.interval->inf);
	      ap_coeff_set_scalar(ap_linexpr0_cstref(expr),
				  &scalar);
	      ap_linexpr0_ForeachLinterm(expr,k,dim,pcoeff){
		ap_coeff_neg(pcoeff,pcoeff);
	      }
	      res.p[j].linexpr0 = expr;
	      res.p[j].constyp = AP_CONS_SUPEQ;
	      res.p[j].scalar = NULL;
	      j++;
	    }
	    if (!supinfty){
	      ap_scalar_set(&scalar, pcst->val.interval->sup);
	      ap_coeff_set_scalar(ap_linexpr0_cstref(linexpr0),
				  &scalar);
	      res.p[j].linexpr0 = linexpr0;
	      res.p[j].constyp = AP_CONS_SUPEQ;
	      res.p[j].scalar = NULL;
	      j++;
	    }
	  }
	  break;
	case AP_COEFF_SCALAR:
	  goto _ap_quasilinearize_lincons0_array_std;
	  break;
	}
	break;
      case AP_CONS_SUPEQ:
      case AP_CONS_SUP:
	switch (pcst->discr){
	case AP_COEFF_INTERVAL:
	  if (!ap_scalar_infty(pcst->val.interval->sup)){
	    ap_scalar_set(&scalar, pcst->val.interval->sup);
	    ap_coeff_set_scalar(ap_linexpr0_cstref(linexpr0),
				&scalar);
	    res.p[j].linexpr0 = linexpr0;
	    res.p[j].constyp = array->p[i].constyp;
	    res.p[j].scalar = NULL;
	    j++;
	  }
	  break;
	case AP_COEFF_SCALAR:
	  goto _ap_quasilinearize_lincons0_array_std;
	  break;
	}
      default:
	goto _ap_quasilinearize_lincons0_array_std;
	break;
      }
      ap_scalar_clear(&scalar);
    }
    else {
    _ap_quasilinearize_lincons0_array_std:
      res.p[j] = ap_lincons0_copy(&array->p[i]);
      j++;
    }
  }
  *pexact = exact;
  if (linearize){
    if (j != res.size){
      ap_lincons0_array_resize(&res,j);
    }
  }
  quasilinearize_free(intern,titv,nbdims,discr);
  return res;
}
