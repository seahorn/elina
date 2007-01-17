/* ************************************************************************* */
/* ap_var.c: generic variables */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include <string.h>
#include <limits.h>
#include "ap_manager.h"
#include "ap_var.h"

static int var_compare_default(void* v1, void* v2){
  return (v1==v2) ? 0 : strcmp((char*)v1,(char*)v2);
}
static void* var_copy_default(void* v){
  return strdup((char*)v);
}
static void var_free_default(void* v){
  free(v);
}
static char* var_to_string_default(void* v){
  return strdup((char*)v);
}

struct ap_var_operations_t ap_var_operations_default = {
  var_compare_default,
  var_copy_default,
  var_free_default,
  var_to_string_default
};

ap_var_operations_t* ap_var_operations = &ap_var_operations_default;
