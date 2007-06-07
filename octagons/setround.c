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

#include <stdio.h>
#include <math.h>


/* simple run-time test that fpu behaves correctly */
static bool test_fpu(void)
{
  int i;
  long double d = 1., dd;
  /* find the minimal long double, as the fixpoint of x -> x/2 with rounding
     towards +oo;
     the max iteration value should be enough for 128-bit floating-point */
  for (i=0;i<5000000;i++) {
    dd = d;
    d /= 2;
    if (d==dd || d==0.) break;
  }
  /* fails if flush to 0 */
  if (d!=dd) { fprintf(stderr,"test_fpu failed test #1 after %i iterations\n",i); return false; }
  /* fails if long double rounding is not towards +oo */
  if (d*0.25!=dd) { fprintf(stderr,"test_fpu failed test #2\n"); return false; }
  /* fails if double rounding is not towards +oo */
  if ((double)d<dd) { fprintf(stderr,"test_fpu failed test #3\n"); return false; }
  /* fails if float rounding is not towards +oo */
  if ((float)d<dd) { fprintf(stderr,"test_fpu failed test #4\n"); return false; }
  return true;
}

#if defined(__linux)
#include <fenv.h>
bool init_fpu(void) 
{ 
  if (!fesetround(FE_UPWARD)) return test_fpu();
  fprintf(stderr,"could not set fpu rounding mode: fesetround failed\n");
  return false;
}

#elif defined(__FreeBSD__) || defined(sun)
#include <ieeefp.h>
bool init_fpu(void) 
{ 
  fpsetround(FP_RP); 
  return test_fpu();
}

#elif defined(__ppc__)
bool init_fpu(void) 
{ 
  __asm volatile ("mtfsfi 7,2");
  return test_fpu();
}

#else
bool init_fpu(void)
{
  fprintf(stderr,"could not set fpu rounding mode: platform not supported\n");
  return false;
}

#endif
