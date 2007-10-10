/* ************************************************************************* */
/* reducedproduct.h: generic reduced product of N domains */
/* ************************************************************************* */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _AP_REDUCEDPRODUCT_H_
#define _AP_REDUCEDPRODUCT_H_

#include <stdlib.h>
#include <stdio.h>

#include "ap_global0.h"

#ifdef __cplusplus
extern "C" {
#endif

/* (internal) abstract value for a product */
typedef struct ap_reducedproduct_t {
  bool reduced; /* is the product reduced ? */
  void* p[0];   /* array of internal abstract values, 
		   the size of which is specified in the associated 
		   ap_reducedproduct_internal_t */
} ap_reducedproduct_t;

/* internal field of manager */
typedef struct ap_reducedproduct_internal_t {
  void (*reduce)(ap_manager_t*, ap_reducedproduct_t*);             
    /* reduce function */
  void (*approximate)(ap_manager_t*, ap_reducedproduct_t*, int n); 
    /* approximate function */
  char* library;               /* (constructed) library name  */
  char* version;               /* (constructed) library version */
  size_t size;                 /* size of the product */
  ap_manager_t* tmanagers[0];  /* of size size */
} ap_reducedproduct_internal_t;

ap_manager_t* ap_reducedproduct_manager_alloc
(
 char* library, /* library name */
 ap_manager_t** tab,  /* Array of managers */
 size_t size,         /* size of array */
 void (*reduce)(ap_manager_t*, ap_reducedproduct_t*),            
   /* reduce function */
 void (*approximate)(ap_manager_t*, ap_reducedproduct_t*, int n) 
   /* approximate function */
);
  
#ifdef __cplusplus
}
#endif

#endif
