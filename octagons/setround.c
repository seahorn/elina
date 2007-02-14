/*
 * setround.c
 *
 * Set floating-point rounding mode towoards +oo.
 *
 * APRON Library / Octagonal Domain
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution 
*/

#include "ap_config.h"

#if defined(__linux)
#include <fenv.h>
bool init_fpu(void) 
{ 
  return fesetround(FE_UPWARD) ? false : true;
}

#elif defined(__FreeBSD__) || defined(sun)
#include <ieeefp.h>
bool init_fpu(void) 
{ 
  fpsetround(FP_RP); 
  return true;
}

#elif defined(__ppc__)
bool init_fpu(void) 
{ 
  __asm volatile ("mtfsfi 7,2");
  return true;
}

#else
bool init_fpu(void)
{
  return false;
}

#endif
