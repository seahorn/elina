/* ********************************************************************** */
/* box_constructor.h: constructors */
/* ********************************************************************** */

#include "box_config.h"
#include "box_int.h"

#ifdef __cplusplus
extern "C" {
#endif

void box_bound_linexpr_internal(box_internal_t* intern,
				itv_t itvinterval, 
				const box_t* a, const ap_linexpr0_t* expr);
void box_bound_itvlinexpr(box_internal_t* intern,
			  itv_t itvinterval, 
			  const box_t* a, const box_linexpr_t* expr);

box_t* box_bottom(ap_manager_t* man, size_t intdim, size_t realdim);
box_t* box_top(ap_manager_t* man, size_t intdim, size_t realdim);
box_t* box_of_box(ap_manager_t* man,
		  size_t intdim, size_t realdim,
		  const ap_interval_t*const* tinterval);
box_t* box_of_lincons_array(ap_manager_t* man,
			    size_t intdim, size_t realdim,
			    const ap_lincons0_array_t* array);
ap_dimension_t box_dimension(ap_manager_t* man, const box_t* a);
tbool_t box_is_bottom(ap_manager_t* man, const box_t* a);
tbool_t box_is_top(ap_manager_t* man, const box_t* a);
tbool_t box_is_leq(ap_manager_t* man, const box_t* a, const box_t* b);
tbool_t box_is_eq(ap_manager_t* man, const box_t* a, const box_t* b);
tbool_t box_is_dimension_unconstrained(ap_manager_t* man, const box_t* a, const ap_dim_t dim);
tbool_t box_sat_interval(ap_manager_t* man, 
			 const box_t* a,
			 ap_dim_t dim, const ap_interval_t* interval);
tbool_t box_sat_lincons(ap_manager_t* man, 
			const box_t* a, const ap_lincons0_t* cons);
ap_interval_t* box_bound_dimension(ap_manager_t* man,
				const box_t* a, ap_dim_t dim);
ap_interval_t* box_bound_linexpr(ap_manager_t* man,
			      const box_t* a, const ap_linexpr0_t* expr);
ap_lincons0_array_t box_to_lincons_array(ap_manager_t* man, const box_t* a);
ap_generator0_array_t box_to_generator_array(ap_manager_t* man, const box_t* a);
ap_interval_t** box_to_box(ap_manager_t* man, const box_t* a);

#ifdef __cplusplus
}
#endif

