/* ********************************************************************** */
/* box_otherops.h */
/* ********************************************************************** */

#include "itv.h"

#ifdef __cplusplus
extern "C" {
#endif

box_t* box_forget_array(ap_manager_t* man,
			bool destructive,
			box_t* a,
			const ap_dim_t* tdim,
			size_t size,
			bool project);
box_t* box_expand(ap_manager_t* man,
		  bool destructive,
		  box_t* a,
		  ap_dim_t dim,
		  size_t n);
box_t* box_fold(ap_manager_t* man,
		bool destructive,
		box_t* a,
		const ap_dim_t* tdim,
		size_t size);

box_t* box_widening(ap_manager_t* man,
		    const box_t* a1, const box_t* a2);

box_t* box_closure(ap_manager_t* man, bool destructive, box_t* a);

#ifdef __cplusplus
}
#endif

