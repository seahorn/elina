/* ************************************************************************* */
/* ap_config.h */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _AP_CONFIG_H_

#define _AP_CONFIG_H_

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

#endif
