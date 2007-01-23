/* ********************************************************************** */
/* box_assign.h */
/* ********************************************************************** */

#ifndef _BOX_ASSIGN_H_
#define _BOX_ASSIGN_H_

#include "box.h"

#ifdef __cplusplus
extern "C" {
#endif

box_t* box_assign_linexpr(ap_manager_t* man,
			  bool destructive,
			  box_t* a,
			  ap_dim_t dim, const ap_linexpr0_t* linexpr,
			  const box_t* dest);
box_t* box_assign_linexpr_array(ap_manager_t* man,
				bool destructive,
				box_t* a,
				const ap_dim_t* tdim, 
				const ap_linexpr0_t*const* texpr,
				size_t size,
				const box_t* dest);
box_t* box_substitute_linexpr(ap_manager_t* man,
			      bool destructive,
			      box_t* a,
			      ap_dim_t dim, const ap_linexpr0_t* linexpr,
			      const box_t* dest);
box_t* box_substitute_linexpr_array(ap_manager_t* man,
				    bool destructive,
				    box_t* a,
				    const ap_dim_t* tdim, 
				    const ap_linexpr0_t*const* texpr,
				    size_t size,
				    const box_t* dest);


#ifdef __cplusplus
}
#endif

#endif
