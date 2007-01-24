/* File generated from box.idl */

#include <stddef.h>
#include <string.h>
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/callback.h>
#ifdef Custom_tag
#include <caml/custom.h>
#include <caml/bigarray.h>
#endif
#include <caml/camlidlruntime.h>

#include "box.h"
#include "ap_manager.h"
#include "apron_caml.h"
extern int camlidl_ml2c_manager_enum_tbool_t(value);
extern value camlidl_c2ml_manager_enum_tbool_t(int);

extern int camlidl_transl_table_manager_enum_tbool_t[];

extern int camlidl_ml2c_manager_enum_ap_funid_t(value);
extern value camlidl_c2ml_manager_enum_ap_funid_t(int);

extern int camlidl_transl_table_manager_enum_ap_funid_t[];

extern int camlidl_ml2c_manager_enum_ap_exc_t(value);
extern value camlidl_c2ml_manager_enum_ap_exc_t(int);

extern int camlidl_transl_table_manager_enum_ap_exc_t[];

extern void camlidl_ml2c_manager_struct_ap_funopt_t(value, struct ap_funopt_t *, camlidl_ctx _ctx);
extern value camlidl_c2ml_manager_struct_ap_funopt_t(struct ap_funopt_t *, camlidl_ctx _ctx);

extern void camlidl_ml2c_manager_struct_ap_exclog_t(value, struct ap_exclog_t *, camlidl_ctx _ctx);
extern value camlidl_c2ml_manager_struct_ap_exclog_t(struct ap_exclog_t *, camlidl_ctx _ctx);

extern void camlidl_ml2c_manager_ap_manager_ptr(value, ap_manager_ptr *, camlidl_ctx _ctx);
extern value camlidl_c2ml_manager_ap_manager_ptr(ap_manager_ptr *, camlidl_ctx _ctx);

value camlidl_box_box_manager_alloc(value _unit)
{
  ap_manager_ptr _res;
  value _vres;

  struct camlidl_ctx_struct _ctxs = { CAMLIDL_TRANSIENT, NULL };
  camlidl_ctx _ctx = &_ctxs;
  _res = box_manager_alloc();
  _vres = camlidl_c2ml_manager_ap_manager_ptr(&_res, _ctx);
  camlidl_free(_ctx);
  return _vres;
}

