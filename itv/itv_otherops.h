/* ********************************************************************** */
/* itv_otherops.h */
/* ********************************************************************** */

#include "itv.h"

itv_t* itv_forget_array(ap_manager_t* man,
			bool destructive,
			itv_t* a,
			const ap_dim_t* tdim,
			size_t size,
			bool project);
itv_t* itv_expand(ap_manager_t* man,
		  bool destructive,
		  itv_t* a,
		  ap_dim_t dim,
		  size_t n);
itv_t* itv_fold(ap_manager_t* man,
		bool destructive,
		itv_t* a,
		const ap_dim_t* tdim,
		size_t size);

itv_t* itv_widening(ap_manager_t* man,
		    const itv_t* a1, const itv_t* a2);

itv_t* itv_closure(ap_manager_t* man, bool destructive, itv_t* a);
