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
  "apl0",
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
  ap_abstract0_free(a->man,a);
}


/* global manager used for deserialization */
static ap_manager_ptr deserialize_man = NULL;

void ap_manager_set_deserialize(ap_manager_ptr man)
{
  deserialize_man = man;
}

ap_manager_ptr ap_manager_get_deserialize(void)
{
  return deserialize_man;
}

static
void camlidl_apron_abstract0_serialize(value v, unsigned long * w32, unsigned long * w64)
{
  ap_abstract0_ptr* p = (ap_abstract0_ptr *) Data_custom_val(v);
  const ap_abstract0_t* a = *p;
  ap_membuf_t buf = ap_abstract0_serialize_raw(a->man,a);
  serialize_int_8(buf.size);
  serialize_block_1(buf.ptr,buf.size);
  *w32 = 4;
  *w64 = 8;
}

static
unsigned long camlidl_apron_abstract0_deserialize(void * dst)
{
  if (deserialize_man) {
    size_t size = deserialize_uint_8(), realsize;
    void* data;
    ap_abstract0_t* a;
    data = malloc(size);
    assert(data);
    deserialize_block_1(data,size);
    *((ap_abstract0_ptr*)dst) =
      ap_abstract0_deserialize_raw(deserialize_man,data,&realsize);
    free(data);
  }
  else caml_failwith("you must call Apron.Manager.set_deserialize_manager before deserializing abstract elements.");
  return sizeof(ap_abstract0_ptr);
}

struct custom_operations camlidl_apron_custom_abstract0_ptr = {
  "apa0",
  camlidl_apron_abstract0_ptr_finalize,
  custom_compare_default,
  custom_hash_default,
  camlidl_apron_abstract0_serialize,
  camlidl_apron_abstract0_deserialize
};

/* ********************************************************************** */
/* variable and environment */
/* ********************************************************************** */

static value _v_var_dup = Val_unit;
static value _v_var_hash = Val_unit;
static value _v_var_compare = Val_unit;
static value _v_var_to_string = Val_unit;

static ap_var_t camlidl_apron_var_copy(ap_var_t p)
{ return p; }

static void camlidl_apron_var_free(ap_var_t p)
{ return; }

static char* camlidl_apron_var_to_string(void* v)
{
  CAMLparam1(v);
  CAMLlocal1(_v_res);
  char* str;
  char* str2;
  int l;

  _v_res = callback(_v_var_to_string,(value)v);
  l = string_length(_v_res);
  str = String_val(_v_res);
  str2 = malloc((l+1)*sizeof(char));
  memcpy(str2,str,l+1);
  CAMLreturn(str2);
}
static long camlidl_apron_var_hash(void* v)
{
  CAMLparam1(v);
  CAMLlocal1(_v_res);

  _v_res = callback(_v_var_hash,(value)v);
  CAMLreturn(Int_val(_v_res));
}
static int camlidl_apron_var_compare(void* v1, void* v2)
{
  CAMLparam2(v1,v2);
  CAMLlocal1(_v_res);

  _v_res = callback2(_v_var_compare,(value)v1,(value)v2);
  CAMLreturn(Int_val(_v_res));
}

static
struct ap_var_operations_t
camlidl_apron_var_operations = {
  camlidl_apron_var_compare,
  camlidl_apron_var_copy,
  camlidl_apron_var_free,
  camlidl_apron_var_to_string
};

value camlidl_apron_set_var_operations(value v)
{
  CAMLparam1(v);
  _v_var_dup = *caml_named_value("ApronVar_dup");
  register_global_root(&_v_var_dup);
  _v_var_hash = *caml_named_value("ApronVar_hash");
  register_global_root(&_v_var_hash);
  _v_var_compare = *caml_named_value("ApronVar_compare");
  register_global_root(&_v_var_compare);
  _v_var_to_string = *caml_named_value("ApronVar_to_string");
  register_global_root(&_v_var_to_string);
  ap_var_operations = &camlidl_apron_var_operations;
  CAMLreturn(Val_unit);
}

static
void camlidl_apron_environment_ptr_finalize(value v)
{
  size_t i;
  ap_environment_t* e = *(ap_environment_ptr *) Data_custom_val(v);
  if (e->intdim+e->realdim>=1 &&
      Is_block((value)(e->var_of_dim[0]))){
    for (i=0;i<e->intdim+e->realdim;i++){
      caml_remove_global_root((value*)&e->var_of_dim[i]);
    }
  }
  ap_environment_free(e);
  return;
}

static
long camlidl_apron_environment_ptr_hash(value v)
{
  CAMLparam1(v);
  ap_environment_t* env = *(ap_environment_ptr *) Data_custom_val(v);
  int res;
  size_t size,i,dec;

  res = 1024*(2*env->intdim+3*env->realdim);
  size = env->intdim+env->realdim;
  dec = 0;
  for (i=0; i<size; i += (size+2)/3){
    res += camlidl_apron_var_hash(env->var_of_dim[i]) << dec;
    dec++;
  }
  CAMLreturn(res);
}
static
int camlidl_apron_environment_ptr_compare(value v1, value v2)
{
  CAMLparam2(v1,v2);
  int res;
  ap_environment_t* env1 = *(ap_environment_ptr *) Data_custom_val(v1);
  ap_environment_t* env2 = *(ap_environment_ptr *) Data_custom_val(v2);
  res = ap_environment_compare(env1,env2);
  CAMLreturn(res);
}

struct custom_operations camlidl_apron_custom_environment_ptr = {
  "ape",
  camlidl_apron_environment_ptr_finalize,
  camlidl_apron_environment_ptr_compare,
  camlidl_apron_environment_ptr_hash,
  custom_serialize_default,
  custom_deserialize_default
};

value camlidl_apron_environment_ptr_c2ml(ap_environment_ptr* p)
{
  value v;
  ap_environment_t* env = *p;

  assert(env->count>=1);
  if (env->count==1 && 
      env->intdim+env->realdim>=1 &&
      Is_block(((value)(env->var_of_dim[0])))){
    size_t i;
    for (i=0; i<env->intdim+env->realdim;i++){
      caml_register_global_root((value*)&env->var_of_dim[i]);
    }
  }
   v = alloc_custom(&camlidl_apron_custom_environment_ptr, sizeof(ap_environment_ptr), 0,1);
  *((ap_environment_ptr *) Data_custom_val(v)) = *p;
  return v;
}

/* ********************************************************************** */
/* init */
/* ********************************************************************** */
value camlidl_apron_init(value dummy)
{
  register_custom_operations(&camlidl_apron_custom_abstract0_ptr);
  return Val_unit;
}
