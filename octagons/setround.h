/*
 * setround.h
 *
 * Set floating-point rounding mode towoards +oo.
 *
 * APRON Library / Octagonal Domain
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#ifndef __SETROUND_H
#define __SETROUND_H

#include "ap_config.h"

#ifdef __cplusplus
extern "C" {
#endif

extern bool init_fpu(void);
/* returns true if succesfull, false if not */

#ifdef __cplusplus
}
#endif

#endif /* __SETROUND_H */
