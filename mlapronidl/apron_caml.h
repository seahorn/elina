/* ********************************************************************** */
/* apron_caml.h */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.
   Please read the COPYING file packaged in the distribution  */

#ifndef _APRON_CAML_H_
#define _APRON_CAML_H_

#include <string.h>
#include "gmp_caml.h"

#include "ap_manager.h"
#include "ap_coeff.h"
#include "ap_expr0.h"
#include "ap_environment.h"
#include "ap_abstract0.h"
#include "ap_expr1.h"
#include "ap_abstract1.h"

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/custom.h>
#include <caml/bigarray.h>
#include <caml/intext.h>

struct ap_interval_array_t {
  struct ap_interval_t** p;
  size_t size;
};

struct ap_scalar_array_t {
  struct ap_scalar_t** p;
  size_t size;
};

typedef struct ap_interval_t* ap_interval_ptr;
typedef struct ap_scalar_t* ap_scalar_ptr;
typedef struct ap_linexpr0_t* ap_linexpr0_ptr;
typedef struct ap_manager_t* ap_manager_ptr;
typedef struct ap_manager_t* ap_manager_opt_ptr;
typedef struct ap_environment_t* ap_environment_ptr;

typedef ap_abstract0_t* ap_abstract0_ptr;

typedef struct ap_abstract1_ptr {
  ap_abstract0_ptr ap_abstract0_ptr;
  ap_environment_t* env;
} ap_abstract1_ptr;

/* ********************************************************************** */
/* dimchange */
/* ********************************************************************** */

void camlidl_apron_dimchange_ml2c(value v, ap_dimchange_t* dimchange);
value camlidl_apron_dimchange_c2ml(ap_dimchange_t* dimchange);

/* ********************************************************************** */
/* linexpr0 */
/* ********************************************************************** */

extern struct custom_operations camlidl_apron_custom_linexpr0_ptr;

static inline
void camlidl_apron_linexpr0_ptr_ml2c(value v, ap_linexpr0_ptr* p)
{
  *p = *((ap_linexpr0_ptr *) Data_custom_val(v));
}

static inline
value camlidl_apron_linexpr0_ptr_c2ml(ap_linexpr0_ptr* p)
{
  value v;
  v = alloc_custom(&camlidl_apron_custom_linexpr0_ptr, sizeof(ap_linexpr0_ptr), 0,1);
  *((ap_linexpr0_ptr *) Data_custom_val(v)) = *p;
  return v;
}

/* ********************************************************************** */
/* manager */
/* ********************************************************************** */

static inline
value camlidl_apron_manager_ptr_finalize(ap_manager_ptr* p){
  ap_manager_free(*p);
  return 0;
}

void ap_manager_set_deserialize(ap_manager_ptr man);
ap_manager_ptr ap_manager_get_deserialize(void);

/* ********************************************************************** */
/* abstract0 */
/* ********************************************************************** */

extern struct custom_operations camlidl_apron_custom_abstract0_ptr;
extern mlsize_t camlidl_apron_heap;

static inline
void camlidl_apron_abstract0_ptr_ml2c(value v, ap_abstract0_ptr* p)
{
  *p = *((ap_abstract0_ptr *) Data_custom_val(v));
}

static inline
value camlidl_apron_abstract0_ptr_c2ml(ap_abstract0_ptr* p)
{
  value v;
  v = alloc_custom(&camlidl_apron_custom_abstract0_ptr, sizeof(ap_abstract0_ptr), 
		   ap_abstract0_size((*p)->man,(*p)), 
		   camlidl_apron_heap);
  *((ap_abstract0_ptr *) Data_custom_val(v)) = *p;
  return v;
}

/* ********************************************************************** */
/* environment */
/* ********************************************************************** */

extern struct custom_operations camlidl_apron_custom_environment_ptr;

static inline
void camlidl_apron_environment_var_ml2c(value v, ap_var_t* p){
  *p = (ap_var_t)v;
}

static inline
value camlidl_apron_environment_var_c2ml(ap_var_t* p){
  return (value)(*p);
}

static inline
void camlidl_apron_environment_ptr_ml2c(value v, ap_environment_ptr* p)
{
  *p = *((ap_environment_ptr *) Data_custom_val(v));
}

value camlidl_apron_environment_ptr_c2ml(ap_environment_ptr* p);

/* ********************************************************************** */
/* init */
/* ********************************************************************** */
value camlidl_apron_init(value dummy);

#endif
