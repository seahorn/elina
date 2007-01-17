/* ************************************************************************* */
/* ap_config.h */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _AP_CONFIG_H_
#define _AP_CONFIG_H_

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
#define HAS_BOOL
#endif

#ifndef HAS_BOOL
#define HAS_BOOL
typedef enum bool {
  false=0,
  true=1
} bool;
#endif

#if !(defined __USE_SVID || defined __USE_BSD || defined __USE_XOPEN_EXTENDED)

static inline char* strdup(const char* s){
  char* s2;

  s2 = malloc(strlen(s)+1);
  strcpy(s2,s);
  return s2;
}
#endif

#endif
