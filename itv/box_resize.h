/* ********************************************************************** */
/* box_resize.h */
/* ********************************************************************** */

#ifndef _BOX_RESIZE_H_
#define _BOX_RESIZE_H_

#include "box.h"

#ifdef __cplusplus
extern "C" {
#endif

box_t* box_add_dimensions(ap_manager_t* man,
			  bool destructive, box_t* a,
			  const ap_dimchange_t* dimchange);

box_t* box_remove_dimensions(ap_manager_t* man,
			     bool destructive, box_t* a,
			     const ap_dimchange_t* dimchange);

box_t* box_permute_dimensions(ap_manager_t* man,
			      bool destructive,
			      box_t* a,
			      const ap_dimperm_t* permutation);

#ifdef __cplusplus
}
#endif

#endif
