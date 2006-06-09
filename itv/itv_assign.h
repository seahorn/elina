/* ********************************************************************** */
/* itv_assign.h */
/* ********************************************************************** */

#include "itv.h"
#include "itv_internal.h"

itv_t* itv_assign_linexpr(ap_manager_t* man,
			  bool destructive,
			  itv_t* a,
			  ap_dim_t dim, const ap_linexpr0_t* linexpr,
			  const itv_t* dest);
itv_t* itv_assign_linexpr_array(ap_manager_t* man,
				bool destructive,
				itv_t* a,
				const ap_dim_t* tdim, 
				const ap_linexpr0_t*const* texpr,
				size_t size,
				const itv_t* dest);
itv_t* itv_substitute_linexpr(ap_manager_t* man,
			      bool destructive,
			      itv_t* a,
			      ap_dim_t dim, const ap_linexpr0_t* linexpr,
			      const itv_t* dest);
itv_t* itv_substitute_linexpr_array(ap_manager_t* man,
				    bool destructive,
				    itv_t* a,
				    const ap_dim_t* tdim, 
				    const ap_linexpr0_t*const* texpr,
				    size_t size,
				    const itv_t* dest);


