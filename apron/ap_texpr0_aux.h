/* ************************************************************************* */
/* ap_texpr0_aux.h: tree expressions, submodule */
/* ************************************************************************* */

#ifndef _AP_TEXPR0_AUX_H_
#define _AP_TEXPR0_AUX_H_

#include "ap_manager.h"
#include "ap_expr0.h"
#include "ap_abstract0.h"
#include "ap_texpr0.h"

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

ap_linexpr0_t* ITVFUN(ap_texpr0_intlinearize)
  (ap_interval_t** tinterval, size_t size,
   ap_texpr0_t* expr0,
   bool quasilinearize,
   bool* pexact);

#ifdef __cplusplus
}
#endif

#endif
