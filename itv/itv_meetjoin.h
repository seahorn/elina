/* ********************************************************************** */
/* itv_meetjoin.h */
/* ********************************************************************** */

#include "itv_config.h"
#include "itv.h"

bool itv_meet_itvlincons(itv_internal_t* intern,
			 itv_t* a, itv_lincons_t* cons);
void itv_meet_itv_lincons_array(itv_internal_t* intern,
			       itv_t* a, itv_lincons_array_t* array,
			       size_t kmax);
void itv_add_ray(itv_internal_t* intern,
		 itv_t* a, const ap_generator0_t* gen);

itv_t* itv_meet(ap_manager_t* man, bool destructive, itv_t* a1, const itv_t* a2);
itv_t* itv_join(ap_manager_t* man, bool destructive, itv_t* a1, const itv_t* a2);
itv_t* itv_meet_array(ap_manager_t* man, const itv_t*const* tab, size_t size);
itv_t* itv_join_array(ap_manager_t* man, const itv_t*const * tab, size_t size);
itv_t* itv_meet_lincons_array(ap_manager_t* man,
			      bool destructive,
			      itv_t* a,
			      const ap_lincons0_array_t* array);
itv_t* itv_add_ray_array(ap_manager_t* man,
			 bool destructive,
			 itv_t* a, const ap_generator0_array_t* array);
