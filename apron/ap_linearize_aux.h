/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

/* Auxiliary module to ap_linearize, which contains functions depending of the
   number representation */

/* may be included several times, with different NUM_ defined */

#if !defined(NUM_MPQ) && !defined(NUM_DOUBLE) && !defined(NUM_LONGDOUBLE)
#error "Wrong NUM type"
#endif

#include "ap_manager.h"
#include "ap_expr0.h"
#include "ap_abstract0.h"
#include "itv_fun.h"

#ifdef __cplusplus
extern "C" {
#endif

ap_interval_t* 
ITVFUN(ap_linexpr0_eval)(ap_manager_t* man,
			 ap_abstract0_t* abs,
			 ap_linexpr0_t* expr,
			 bool* pexact);


ap_linexpr0_t* 
ITVFUN(ap_quasilinearize_linexpr0)(ap_manager_t* man,
				   ap_abstract0_t* abs,
				   ap_linexpr0_t* linexpr0,
				   bool* pexact);
  
ap_lincons0_t 
ITVFUN(ap_quasilinearize_lincons0)(ap_manager_t* man,
				   ap_abstract0_t* abs,
				   ap_lincons0_t* lincons0,
				   bool* pexact);
  
ap_linexpr0_t** 
ITVFUN(ap_quasilinearize_linexpr0_array)(ap_manager_t* man,
					 ap_abstract0_t* abs,
					 ap_linexpr0_t** texpr, size_t size,
					 bool* pexact);

ap_lincons0_array_t 
ITVFUN(ap_quasilinearize_lincons0_array)(ap_manager_t* man,
					 ap_abstract0_t* abs,
					 ap_lincons0_array_t* array,
					 bool* pexact,
					 bool convert,
					 bool linearize);


#ifdef __cplusplus
}
#endif
