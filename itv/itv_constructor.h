/* ********************************************************************** */
/* itv_constructor.h: constructors */
/* ********************************************************************** */

#include "itv_config.h"
#include "itv_int.h"

void itv_bound_linexpr_internal(itv_internal_t* intern,
				itv_interval_t itvinterval, 
				const itv_t* a, const ap_linexpr0_t* expr);
void itv_bound_itvlinexpr(itv_internal_t* intern,
			  itv_interval_t itvinterval, 
			  const itv_t* a, const itv_linexpr_t* expr);

itv_t* itv_bottom(ap_manager_t* man, size_t intdim, size_t realdim);
itv_t* itv_top(ap_manager_t* man, size_t intdim, size_t realdim);
itv_t* itv_of_box(ap_manager_t* man,
		  size_t intdim, size_t realdim,
		  const ap_interval_t*const* tinterval);
itv_t* itv_of_lincons_array(ap_manager_t* man,
			    size_t intdim, size_t realdim,
			    const ap_lincons0_array_t* array);
ap_dimension_t itv_dimension(ap_manager_t* man, const itv_t* a);
tbool_t itv_is_bottom(ap_manager_t* man, const itv_t* a);
tbool_t itv_is_top(ap_manager_t* man, const itv_t* a);
tbool_t itv_is_leq(ap_manager_t* man, const itv_t* a, const itv_t* b);
tbool_t itv_is_eq(ap_manager_t* man, const itv_t* a, const itv_t* b);
tbool_t itv_is_dimension_unconstrained(ap_manager_t* man, const itv_t* a, const ap_dim_t dim);
tbool_t itv_sat_interval(ap_manager_t* man, 
			 const itv_t* a,
			 ap_dim_t dim, const ap_interval_t* interval);
tbool_t itv_sat_lincons(ap_manager_t* man, 
			const itv_t* a, const ap_lincons0_t* cons);
ap_interval_t* itv_bound_dimension(ap_manager_t* man,
				const itv_t* a, ap_dim_t dim);
ap_interval_t* itv_bound_linexpr(ap_manager_t* man,
			      const itv_t* a, const ap_linexpr0_t* expr);
ap_lincons0_array_t itv_to_lincons_array(ap_manager_t* man, const itv_t* a);
ap_generator0_array_t itv_to_generator_array(ap_manager_t* man, const itv_t* a);
ap_interval_t** itv_to_box(ap_manager_t* man, const itv_t* a);

