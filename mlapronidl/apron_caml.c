/* ********************************************************************** */
/* apron_caml.c */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.
   Please read the COPYING file packaged in the distribution  */

#include "apron_caml.h"

/* ********************************************************************** */
/* dimchange */
/* ********************************************************************** */

void camlidl_apron_dimchange_ml2c(const value v, ap_dimchange_t* dimchange)
{
  value vintdim,vrealdim,vdim,v2;
  size_t i,size;

  vintdim = Field(v,1);
  vrealdim = Field(v,2);
  ap_dimchange_init(dimchange,Int_val(vintdim),Int_val(vrealdim));
  vdim = Field(v,0);
  size = Wosize_val(vdim);
  if (dimchange->intdim+dimchange->realdim>size)
    caml_failwith("Invalid object of type Linexpr0.dimchange: size of the array smaller than intdim+realdim");
  for (i=0;i<size;i++){
    v2 = Field(vdim,i);
    dimchange->dim[i] = Int_val(v2);
  }
  return;
}

value camlidl_apron_dimchange_c2ml(ap_dimchange_t* dimchange)
{
  value vintdim,vrealdim,vdim,v;
  size_t i,size;

  size = dimchange->intdim+dimchange->realdim;
  vintdim = Val_int(dimchange->intdim);
  vrealdim = Val_int(dimchange->realdim);
  vdim = caml_alloc(size,0);
  Begin_root(vdim)
    for (i=0;i<size;i++){
      v = Val_int(dimchange->dim[i]);
      Store_field(vdim,i,v);
    }
  End_roots()
  v = caml_alloc_small(3,0);
  Field(v,1) = vintdim;
  Field(v,2) = vrealdim;
  Field(v,0) = vdim;
  return v;
}
/* ********************************************************************** */
/* linexpr0 */
/* ********************************************************************** */

static
void camlidl_apron_linexpr0_ptr_finalize(value v)
{
  ap_linexpr0_t* p = *(ap_linexpr0_ptr *) Data_custom_val(v);
  ap_linexpr0_free(p);
}

static
long camlidl_apron_linexpr0_ptr_hash(value v)
{
  ap_linexpr0_t* p = *(ap_linexpr0_ptr *) Data_custom_val(v);
  return ap_linexpr0_hash(p);
}
static
int camlidl_apron_linexpr0_ptr_compare(value v1, value v2)
{
  ap_linexpr0_t* p1 = *(ap_linexpr0_ptr *) Data_custom_val(v1);
  ap_linexpr0_t* p2 = *(ap_linexpr0_ptr *) Data_custom_val(v2);
  return ap_linexpr0_compare(p1,p2);
}

struct custom_operations camlidl_apron_custom_linexpr0_ptr = {
  NULL,
  camlidl_apron_linexpr0_ptr_finalize,
  camlidl_apron_linexpr0_ptr_compare,
  camlidl_apron_linexpr0_ptr_hash,
  custom_serialize_default,
  custom_deserialize_default
};

/* ********************************************************************** */
/* abstract0 */
/* ********************************************************************** */

mlsize_t camlidl_apron_heap = 1 << 20;

static
void camlidl_apron_abstract0_ptr_finalize(value v)
{
  ap_abstract0_ptr* p = (ap_abstract0_ptr *) Data_custom_val(v);
  const ap_abstract0_t* a = *p;
  ap_abstract0_free(a->man,a->value);
}

struct custom_operations camlidl_apron_custom_abstract0_ptr = {
  NULL,
  camlidl_apron_abstract0_ptr_finalize,
  custom_compare_default,
  custom_hash_default,
  custom_serialize_default,
  custom_deserialize_default
};

/* ********************************************************************** */
/* environment */
/* ********************************************************************** */

static value _v_var_hash_default = Val_unit;
static value _v_var_compare_default = Val_unit;
static value _v_var_to_string_default = Val_unit;

static long camlidl_apron_var_hash_default(void* v)
{
  value _v_v = (value)v;
  value _v_res;

  Begin_roots1(_v_v)
  _v_res = callback(_v_var_hash_default,_v_v);
  End_roots()
  return Int_val(_v_res);
}
static int camlidl_apron_var_compare_default(void* v1, void* v2)
{
  value _v_v1 = (value)v1;
  value _v_v2 = (value)v2;
  value _v_res;

  Begin_roots2(_v_v1,_v_v2)
  _v_res = callback2(_v_var_compare_default,_v_v1,_v_v2);
  End_roots()
  return Int_val(_v_res);
}
static void* camlidl_apron_var_copy_default(void* v)
{ return v; }
static void camlidl_apron_var_free_default(void* v)
{ return; }
static char* camlidl_apron_var_to_string_default(void* v)
{
  value _v_v = (value)v;
  value _v_res = Val_unit;
  char* str;
  char* str2;

  Begin_roots2(_v_v,_v_res)
    _v_res = callback(_v_var_to_string_default,_v_v);
    str = String_val(_v_res);
    str2 = malloc((strlen(str)+1)*sizeof(char));
    strcpy(str2,str);
  End_roots()
  return str2;
}

static 
struct ap_var_operations_t 
camlidl_apron_var_operations_default = {
  camlidl_apron_var_compare_default,
  camlidl_apron_var_copy_default,
  camlidl_apron_var_free_default,
  camlidl_apron_var_to_string_default
};

value camlidl_apron_set_var_operations(value v)
{
  CAMLparam1(v);
  register_global_root(&_v_var_hash_default);
  register_global_root(&_v_var_compare_default);
  register_global_root(&_v_var_to_string_default);
  _v_var_hash_default = *caml_named_value("ApronVar_hash");
  _v_var_compare_default = *caml_named_value("ApronVar_compare");
  _v_var_to_string_default = *caml_named_value("ApronVar_to_string");
  ap_var_operations = &camlidl_apron_var_operations_default;
  CAMLreturn(Val_unit);
}

static
void camlidl_apron_environment_ptr_finalize(value v)
{
  ap_environment_t* e = *(ap_environment_ptr *) Data_custom_val(v);
  ap_environment_free(e);
}

static
long camlidl_apron_environment_ptr_hash(value v)
{
  ap_environment_t* env = *(ap_environment_ptr *) Data_custom_val(v);
  int res;
  size_t size,i,dec;

  res = 1024*(2*env->intdim+3*env->realdim);
  size = env->intdim+env->realdim;
  dec = 0;
  for (i=0; i<size; i += (size+2)/3){
    res += camlidl_apron_var_hash_default(env->var_of_dim[i]) << dec;
    dec++;
  }
  return res;
}
static
int camlidl_apron_environment_ptr_compare(value v1, value v2)
{
  ap_environment_t* env1 = *(ap_environment_ptr *) Data_custom_val(v1);
  ap_environment_t* env2 = *(ap_environment_ptr *) Data_custom_val(v2);
  return ap_environment_compare(env1,env2);
}

struct custom_operations camlidl_apron_custom_environment_ptr = {
  NULL,
  camlidl_apron_environment_ptr_finalize,
  camlidl_apron_environment_ptr_compare,
  camlidl_apron_environment_ptr_hash,
  custom_serialize_default,
  custom_deserialize_default
};
