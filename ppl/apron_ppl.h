/*
 * apron_ppl.h
 *
 * APRON Library / PPL library wrapper
 *
 * This is the (only) one .h to include to access all PPL domains.
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

/* 
 * This file is part of the APRON Library, released under LGPL license.
 * Please read the COPYING file packaged in the distribution.
 */

#ifndef __APRON_PPL_H
#define __APRON_PPL_H

#include "ap_global0.h"

#ifdef __cplusplus
extern "C" {
#endif

ap_manager_t* ap_ppl_poly_manager_alloc(bool strict);
  /* PPL::Polyhedron */

#ifdef __cplusplus
}
#endif

#endif /* __APRON_PPL_H */
