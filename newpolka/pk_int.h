/* ********************************************************************** */
/* pk.h: Interface of the polka library  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

/* The invariant of the representation of a polyhedron is the following:
if the polyhedron is empty, then \verb-C=F=satC=satF=0-. Else, we have
\verb-(C || F) && (satC || satF || !(C && F))-. This means that a
polyhedron has a minimal representation minimal if and only if \verb-C && F-
if and only if \verb-satC || satF-. */

#ifndef __PK_INT_H__
#define __PK_INT_H__

#include "pk.h"

typedef enum poly_status_t {
  poly_status_conseps=0x1,
  poly_status_consgauss=0x2,
  poly_status_gengauss=0x4,
  poly_status_minimal=0x8
} poly_status_t;

struct poly_t {
  /* private data: do not use directly ! */
  struct matrix_t* C;
  struct matrix_t* F;
  struct satmat_t* satC;
  struct satmat_t* satF;
  size_t intdim;
  size_t realdim;
  size_t nbeq;
  size_t nbline;
  poly_status_t status;
};

#endif
