/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

/* Auxiliary module to ap_linearize, which contains functions depending of the
   number representation */

#ifndef _AP_LINEARIZE_AUX_H_
#define _AP_LINEARIZE_AUX_H_

#include "ap_manager.h"
#include "ap_expr0.h"
#include "ap_abstract0.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !defined(NUM_MPQ) && !defined(NUM_DOUBLE)
#error "Wrong NUM type"
#endif

#include "num.h"
#include "bound.h"
#include "itv.h"
#include "itv_linexpr.h"

ap_linexpr0_t* ITVFUN(ap_quasilinearize_linexpr0)(ap_manager_t* man,
						  void* abs,
						  ap_linexpr0_t* linexpr0,
						  bool* pexact);
ap_lincons0_t ITVFUN(ap_quasilinearize_lincons0)(ap_manager_t* man,
						 void* abs,
						 ap_lincons0_t* lincons0,
						 bool* pexact);
ap_linexpr0_t** ITVFUN(ap_quasilinearize_tlinexpr0)(ap_manager_t* man,
						    void* abs,
						    ap_linexpr0_t** texpr, size_t size,
						    bool* pexact);
ap_lincons0_array_t ITVFUN(ap_quasilinearize_lincons0_array)(ap_manager_t* man,
							     void* abs,
							     ap_lincons0_array_t* array,
							     bool* pexact,
							     bool convert,
							     bool linearize);

#ifdef __cplusplus
}
#endif

#endif
