/* ************************************************************************* */
/* ap_texpr0_aux.h: tree expressions, submodule */
/* ************************************************************************* */

/* may be included several times, with different NUM_ defined */

#if !defined(NUM_MPQ) && !defined(NUM_DOUBLE) && !defined(NUM_LONGDOUBLE)
#error "Wrong NUM type"
#endif

#include "ap_manager.h"
#include "ap_expr0.h"
#include "ap_abstract0.h"
#include "ap_texpr0.h"
#include "itv_fun.h"

#ifdef __cplusplus
extern "C" {
#endif

ap_interval_t* 
ITVFUN(ap_texpr0_eval)(ap_manager_t* man,
		       ap_abstract0_t* abs,
		       ap_texpr0_t* expr,
		       bool* pexact);

ap_linexpr0_t* 
ITVFUN(ap_texpr0_linearize)(ap_manager_t* man,
			    ap_abstract0_t* abs,
			    ap_texpr0_t* expr,
			    bool quasilinearize,
			    bool* pexact);

#ifdef __cplusplus
}
#endif

