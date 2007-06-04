/* ************************************************************************* */
/* ap_linearize: generic functions for (quasi)linearisation ofinterval expressions */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "ap_linearize.h"

/* ********************************************************************** */
/* Prototypes */
/* ********************************************************************** */

ap_linexpr0_t* ap_quasilinearize_linexpr0_mpq(ap_manager_t* man,
					      void* abs,
					      ap_linexpr0_t* linexpr0,
					      bool* pexact);
ap_lincons0_t ap_quasilinearize_lincons0_mpq(ap_manager_t* man,
					     void* abs,
					     ap_lincons0_t* lincons0,
					     bool* pexact);
ap_linexpr0_t** ap_quasilinearize_tlinexpr0_mpq(ap_manager_t* man,
						void* abs,
						ap_linexpr0_t** texpr, size_t size,
						bool* pexact);
ap_lincons0_array_t ap_quasilinearize_lincons0_array_mpq(ap_manager_t* man,
							 void* abs,
							 ap_lincons0_array_t* array,
							 bool* pexact,
							 bool convert,
							 bool linearize);
ap_linexpr0_t* ap_quasilinearize_linexpr0_dbl(ap_manager_t* man,
					      void* abs,
					      ap_linexpr0_t* linexpr0,
					      bool* pexact);
ap_lincons0_t ap_quasilinearize_lincons0_dbl(ap_manager_t* man,
					     void* abs,
					     ap_lincons0_t* lincons0,
					     bool* pexact);
ap_linexpr0_t** ap_quasilinearize_tlinexpr0_dbl(ap_manager_t* man,
						void* abs,
						ap_linexpr0_t** texpr, size_t size,
						bool* pexact);
ap_lincons0_array_t ap_quasilinearize_lincons0_array_dbl(ap_manager_t* man,
							 void* abs,
							 ap_lincons0_array_t* array,
							 bool* pexact,
							 bool convert,
							 bool linearize);

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
  switch (discr){
  case AP_SCALAR_MPQ:
    return ap_quasilinearize_linexpr0_mpq(man,abs,linexpr0,pexact);
  case AP_SCALAR_DOUBLE:
    return ap_quasilinearize_linexpr0_dbl(man,abs,linexpr0,pexact);
  default:
    abort();
  }
}

/* Same for ap_lincons0_t */
ap_lincons0_t ap_quasilinearize_lincons0(ap_manager_t* man,
					 void* abs,
					 ap_lincons0_t* lincons0,
					 ap_scalar_discr_t discr,
					 bool* pexact)
{
  switch (discr){
  case AP_SCALAR_MPQ:
    return ap_quasilinearize_lincons0_mpq(man,abs,lincons0,pexact);
  case AP_SCALAR_DOUBLE:
    return ap_quasilinearize_lincons0_dbl(man,abs,lincons0,pexact);
  default:
    abort();
  }
}

/* Same for arrays of ap_linexpr0_t */
ap_linexpr0_t** ap_quasilinearize_tlinexpr0(ap_manager_t* man,
					    void* abs,
					    ap_linexpr0_t** texpr, size_t size,
					    ap_scalar_discr_t discr,
					    bool* pexact)
{
  switch (discr){
  case AP_SCALAR_MPQ:
    return ap_quasilinearize_tlinexpr0_mpq(man,abs,texpr,size,pexact);
  case AP_SCALAR_DOUBLE:
    return ap_quasilinearize_tlinexpr0_dbl(man,abs,texpr,size,pexact);
  default:
    abort();
  }
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
  switch (discr){
  case AP_SCALAR_MPQ:
    return ap_quasilinearize_lincons0_array_mpq(man,abs,array,pexact,convert,linearize);
  case AP_SCALAR_DOUBLE:
    return ap_quasilinearize_lincons0_array_dbl(man,abs,array,pexact,convert,linearize);
  default:
    abort();
  }
}

/* ********************************************************************** */
/* Linearization of array of constraints*/
/* ********************************************************************** */

/* Transform a quasilinear constraint as follows:
   e.x + [a,b] >= 0 ==> e.x + b >= 0
   e.x + [a,b] > 0  ==> e.x + b > 0
   e.x + [a,b] = 0  ==> e.x + b >= 0 and e.x + a <= 0
   e.x + [a,b] = 0 mod k ==> unchanged
   
   Also removes trivially true constraints e.x + oo >= 0

   Resulting constraints are written in the array of constraint, with pindex a
   pointer to the current index, which is incremented accordingly.

   The initial constraint remains unchanged (and unfreed).
*/

void ap_linearize_quasilincons0(ap_lincons0_array_t* array, size_t* pindex, 
				ap_lincons0_t* cons)
{
  ap_linexpr0_t* linexpr0; 
  ap_coeff_t* pcst;
  size_t index;
  size_t k;
  ap_dim_t dim;
  ap_coeff_t* pcoeff;
  
  index = *pindex;
  linexpr0 = cons->linexpr0;  
  pcst = ap_linexpr0_cstref(linexpr0);
  switch (cons->constyp){
  case AP_CONS_EQ:
    switch (pcst->discr){
    case AP_COEFF_INTERVAL:
      {
	if (!ap_scalar_infty(pcst->val.interval->inf)){
	  ap_linexpr0_t* expr = ap_linexpr0_copy(linexpr0);
	  ap_coeff_set_scalar(ap_linexpr0_cstref(expr),
			      pcst->val.interval->inf);
	  ap_coeff_neg(ap_linexpr0_cstref(expr),ap_linexpr0_cstref(expr));
	  ap_linexpr0_ForeachLinterm(expr,k,dim,pcoeff){
	    ap_coeff_neg(pcoeff,pcoeff);
	  }
	  array->p[index].linexpr0 = expr;
	  array->p[index].constyp = AP_CONS_SUPEQ;
	  array->p[index].scalar = NULL;
	  index++;
	}
	if (!ap_scalar_infty(pcst->val.interval->sup)){
	  ap_linexpr0_t* expr = ap_linexpr0_copy(linexpr0);
	  ap_coeff_set_scalar(ap_linexpr0_cstref(expr),
			      pcst->val.interval->sup);
	  array->p[index].linexpr0 = expr;
	  array->p[index].constyp = AP_CONS_SUPEQ;
	  array->p[index].scalar = NULL;
	  index++;
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
	ap_linexpr0_t* expr = ap_linexpr0_copy(linexpr0);
	ap_coeff_set_scalar(ap_linexpr0_cstref(expr),
			      pcst->val.interval->sup);
	array->p[index].linexpr0 = expr;
	array->p[index].constyp = cons->constyp;
	array->p[index].scalar = NULL;
	index++;
      }
      break;
    case AP_COEFF_SCALAR:
      goto _ap_quasilinearize_lincons0_array_std;
      break;
    }
  default:
  _ap_quasilinearize_lincons0_array_std:
    array->p[index] = ap_lincons0_copy(cons);
    break;
  }
  *pindex = index;
}

			  




