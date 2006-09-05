/* ********************************************************************** */
/* itv_representation.c: general management */
/* ********************************************************************** */

#include "itv_config.h"
#include "itv.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Internal functions */
itv_t* itv_alloc(size_t intdim, size_t realdim);
void itv_init(itv_t* a);
void itv_set_bottom(itv_t* a);
void itv_set_top(itv_t* a);
void itv_set(itv_t* a, const itv_t* b);

/* 1. Memory */
itv_t* itv_copy(ap_manager_t* man, const itv_t* a);
void itv_free(ap_manager_t* man, itv_t* a);
size_t itv_size(ap_manager_t* man, const itv_t* a);

/* 2. Control of internal representation */
void itv_minimize(ap_manager_t* man, const itv_t* a);
void itv_canonicalize(ap_manager_t* man, const itv_t* a);
void itv_approximate(ap_manager_t* man, itv_t* a, int algorithm);
tbool_t itv_is_minimal(ap_manager_t* man, const itv_t* a);
tbool_t itv_is_canonical(ap_manager_t* man, const itv_t* a);


/* 3. Printing */
void itv_print(FILE* stream,
	       ap_manager_t* man,const itv_t* a,char** name_of_dim);
void itv_dump(FILE* stream, ap_manager_t* man, const itv_t* a);
void itv_printdiff(FILE* stream,
		   ap_manager_t* man, const itv_t* a, const itv_t* b,
		   char** name_of_dim);

/* 4. Serialization */
ap_membuf_t itv_serialize_raw(ap_manager_t* man, const itv_t* a);
itv_t* itv_deserialize_raw(ap_manager_t* man, void* ptr);

#ifdef __cplusplus
}
#endif
