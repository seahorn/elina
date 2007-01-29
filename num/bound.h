/* ********************************************************************** */
/* bound.h: numbers used for bounds */
/* ********************************************************************** */

#ifndef _BOUND_H_
#define _BOUND_H_

#ifndef _NUM_H_
#error "File bound.h requires num.h to be first included"
#endif

#include <stdio.h>

#include "bound_def.h"

static inline bool bound_infty(const bound_t a);
/*
Macro:
static inline num_t bound_numref(bound_t a); 
*/

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */
static inline void bound_set(bound_t a, const bound_t b);
static inline void bound_set_array(bound_t* a, const bound_t* b, size_t size);
static inline void bound_set_int(bound_t a, long int i);
static inline void bound_set_num(bound_t a, num_t b);
static inline void bound_set_infty(bound_t a);
static inline void bound_swap(bound_t a, bound_t b);

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void bound_init(bound_t a);
static inline void bound_init_array(bound_t* a, size_t size);

static inline void bound_init_set(bound_t a, const bound_t b);
static inline void bound_init_set_int(bound_t a, long int i);
static inline void bound_init_set_infty(bound_t a);

static inline void bound_clear(bound_t a);
static inline void bound_clear_array(bound_t* a, size_t size);

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

static inline void bound_neg(bound_t a, const bound_t b);
static inline void bound_abs(bound_t a, const bound_t b);
static inline void bound_add(bound_t a, const bound_t b, const bound_t c);
static inline void bound_add_uint(bound_t a, const bound_t b, unsigned long int c);
static inline void bound_add_num(bound_t a, const bound_t b, const num_t c);
static inline void bound_sub(bound_t a, const bound_t b, const bound_t c);
static inline void bound_sub_uint(bound_t a, const bound_t b, unsigned long int c);
static inline void bound_sub_num(bound_t a, const bound_t b, const num_t c);
static inline void bound_mul(bound_t a, const bound_t b, const bound_t c);
static inline void bound_mul_num(bound_t a, const bound_t b, const num_t c);
static inline void bound_mul_2(bound_t a, const bound_t b);
static inline void bound_div(bound_t a, const bound_t b, const bound_t c);
static inline void bound_div_num(bound_t a, const bound_t b, const num_t c);
static inline void bound_div_2(bound_t a, const bound_t b);
static inline void bound_min(bound_t a, const bound_t b, const bound_t c);
static inline void bound_max(bound_t a, const bound_t b, const bound_t c);
static inline void bound_floor(bound_t a, const bound_t b);
static inline void bound_ceil(bound_t a, const bound_t b);

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

static inline int bound_sgn(const bound_t a);
static inline int bound_cmp(const bound_t a, const bound_t b);
static inline int bound_cmp_int(const bound_t a, long int b);
static inline int bound_cmp_num(const bound_t a, const num_t b);
static inline int bound_equal(const bound_t a, const bound_t b);

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void bound_print(const bound_t a);
static inline void bound_fprint(FILE* stream, const bound_t a);
static inline int bound_snprint(char* s, size_t size, const bound_t a);

/* ====================================================================== */
/* Conversions */
/* ====================================================================== */

/* ====================================================================== */
/* Serialization */
/* ====================================================================== */

/* Note: call _init before _deserialize */

static inline size_t bound_serialize(void* dst, const bound_t src);
static inline size_t bound_deserialize(bound_t dst, const void* src);
static inline size_t bound_serialized_size(const bound_t a);

static inline size_t bound_serialize_array(void* dst, const bound_t* src, size_t size);
static inline size_t bound_deserialize_array(bound_t* dst, const void* src, size_t size);
static inline size_t bound_serialized_size_array(const bound_t* src, size_t size);

#endif
