/* ********************************************************************** */
/* itv_resize.h */
/* ********************************************************************** */

#include "itv.h"

itv_t* itv_add_dimensions(ap_manager_t* man,
			  bool destructive, itv_t* a,
			  const ap_dimchange_t* dimchange);

itv_t* itv_remove_dimensions(ap_manager_t* man,
                            bool destructive, itv_t* a,
                            const ap_dimchange_t* dimchange);
itv_t* itv_permute_dimensions(ap_manager_t* man,
			      bool destructive,
			      itv_t* a,
			      const ap_dimperm_t* permutation);
