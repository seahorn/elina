m4_dnl
m4_define(`I0_CHECK_EXC',
`if ($1->result.exn!=AP_EXC_NONE){ \n\
  value v = camlidl_c2ml_manager_struct_ap_exclog_t($1->result.exclog,_ctx);\n\
  caml_raise_with_arg(*caml_named_value(\"apron exception\"),v);\n\
}\n\
')m4_dnl
m4_dnl
