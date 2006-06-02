/* ********************************************************************** */
/* pk_representation.h: General management of polyhedra  */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#ifndef _PK_REPRESENTATION_H_
#define _PK_REPRESENTATION_H_

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"

/* ********************************************************************** */
/* I. Memory */
/* ********************************************************************** */

/* Allocates a polyedron and fills its structure with null values, which
   corresponds to a bottom polyhedron. */
poly_t* poly_alloc(size_t intdim, size_t realdim);

/* Free all the members of the polyhedron structure (GMP semantics) */
void poly_clear(poly_t* po);

/*  Assignement with GMP semantics */
void poly_set(poly_t* pa, const poly_t* pb);
void poly_set_save_C(poly_t* pa, const poly_t* pb);

/* Duplicate (recursively) a polyhedron. */
poly_t* poly_copy(ap_manager_t* man, const poly_t* po);

/* Free a polyhedron (implies a call to poly_clear) */
void poly_free(ap_manager_t* man, poly_t* po);

/* Return the abstract size of a polydron, which is the number of
   coefficients of its current representation, possibly redundant. */
size_t poly_size(ap_manager_t* man, const poly_t* po);


/* ********************************************************************** */
/* II. Control of internal representation */
/* ********************************************************************** */

/* Minimization function, in the sense of minimized dual representation This
   function minimizes if not already done the given polyhedron.
   Raise an exception, but still transmit it (pk->exn not reseted).
*/
void poly_chernikova(ap_manager_t* man, const poly_t* poly, char* msg);

/* Same as poly_chernikova, but in addition ensure normalized epsilon
   constraints. */
void poly_chernikova2(ap_manager_t* man, const poly_t* poly, char* msg);

/* Same as poly_chernikova2, but in addition normalize matrices by Gauss
   elimination and sorting */
void poly_chernikova3(ap_manager_t* man, const poly_t* poly, char* msg);

/* Put the polyhedron with minimized constraints and frames.  If in addition
   the integer man->option->canonicalize.algorithm is strictly positive,
   normalize equalities and lines, and also strict constraints */
void poly_canonicalize(ap_manager_t* man, const poly_t* poly);

/* Minimize the size of the representation of the polyhedron */
void poly_minimize(ap_manager_t* man, const poly_t* poly);

/* Approximation:
   algorithm==0: do nothing

   algorithm==-1: underapproximation: normalize integer minimal constraints
   (induces a smaller polyhedron)
*/
void poly_approximate(ap_manager_t* man, poly_t* po, int algorithm);

/* Is the polyhedron in a minimal representation ? */
tbool_t poly_is_minimal(ap_manager_t* man, const poly_t* po);

/* Is the polyhedron in a canonical representation ? 
   (depends on the algorithm option of canonicalize) */
tbool_t poly_is_canonical(ap_manager_t* man, const poly_t* po);

/* Has the polyhedron normalized strict constraints ? */
static inline bool poly_is_conseps(pk_internal_t* pk, const poly_t* po);

/* Make available the matrix of constraints (resp. frames). The matrix will
   remain unavailable iff the polyhedron appears to be empty */
static inline void poly_obtain_C(ap_manager_t* man, const poly_t* po, char* msg);
static inline void poly_obtain_F(ap_manager_t* man, const poly_t* po, char* msg);

/* Assuming the the matrix of constraints (resp. frames) is available, sort it,
   and take care of the saturation matrices. */
void poly_obtain_sorted_C(pk_internal_t* pk, const poly_t* poly);
void poly_obtain_sorted_F(pk_internal_t* pk, const poly_t* poly);

/* Assuming one of the saturation matrix is available, make satC (resp. satF)
   available. */
static inline void poly_obtain_satC(const poly_t* poly);
static inline void poly_obtain_satF(const poly_t* poly);

/* Exchange C and F, sat C and satF, nbeq and nbline */
static inline void poly_dual(poly_t* po);

/* Like poly_chernikova, etc, but if usual is false, take into account the fact that
   poly_dual has been applied */ 
void poly_chernikova_dual(ap_manager_t* man,
			  const poly_t* poly,
			  char* msg,
			  bool usual);
static inline void poly_obtain_C_dual(ap_manager_t* man,const poly_t* po, char* msg, bool usual);
static inline void poly_obtain_F_dual(ap_manager_t* man,const poly_t* po, char* msg, bool usual);

/* ********************************************************************** */
/* III Printing */
/* ********************************************************************** */

void poly_print(FILE* stream, ap_manager_t* man, const poly_t* po,
		const char** name_of_dim);

void poly_printdiff(FILE* stream, ap_manager_t* man,
		    const poly_t* po1, const poly_t* po2,
		    const char** name_of_dim);

void poly_dump(FILE* stream, ap_manager_t* man, const poly_t* po);

/* ********************************************************************** */
/* IV. Serialization */
/* ********************************************************************** */

/* Not implemented ! */

struct ap_membuf_t poly_serialize_raw(ap_manager_t* man, const poly_t* a); 
poly_t* poly_deserialize_raw(ap_manager_t* man, void* ptr, size_t* size); 

/* ********************************************************************** */
/* V. Checking */
/* ********************************************************************** */

/* Perform rather detailed and costly checks on a polyhedron, to detect
   inconsistencies */
bool poly_check(pk_internal_t* pk, const poly_t* po);
/* Same as previous function, but usual==false mean that poly_dual has been
   applied */
bool poly_check_dual(pk_internal_t* pk, const poly_t* po, bool usual);

/* ********************************************************************** */
/* ********************************************************************** */
/* INLINE DEFINITIONS */
/* ********************************************************************** */

static inline bool poly_is_conseps(pk_internal_t* pk, const poly_t* po)
{
  return (!(pk->strict && po->realdim>0 && !(po->status & poly_status_conseps)));
}

static inline void poly_obtain_C(ap_manager_t* man, const poly_t* po, char* msg)
{
  if (!po->C) poly_chernikova(man,po,msg);
}
static inline void poly_obtain_F(ap_manager_t* man, const poly_t* po, char* msg)
{
  if (!po->F) poly_chernikova(man,po,msg);
}
static inline void poly_obtain_satF(const poly_t* poly)
{
  poly_t* po = (poly_t*)poly;
  if (!po->satF){
    assert(po->C && po->satC);
    po->satF = satmat_transpose(po->satC,po->C->nbrows);
  }
}
static inline void poly_obtain_satC(const poly_t* poly)
{
  poly_t* po = (poly_t*)poly;
  if (!po->satC){
    assert(po->F && po->satF);
    po->satC = satmat_transpose(po->satF,po->F->nbrows);
  }
}

static inline void poly_dual(poly_t* po)
{
  void* ptr;
  size_t nb;
  ptr = po->C; po->C = po->F; po->F = (matrix_t*)ptr;
  ptr = po->satC; po->satC = po->satF; po->satF = (satmat_t*)ptr;
  nb = po->nbeq; po->nbeq = po->nbline; po->nbline = nb;
}

static inline void poly_obtain_C_dual(ap_manager_t* man, const poly_t* po, char* msg, bool usual)
{
  if (!po->C) poly_chernikova_dual(man,po,msg,usual);
}
static inline void poly_obtain_F_dual(ap_manager_t* man, const poly_t* po, char* msg, bool usual)
{
  if (!po->F) poly_chernikova_dual(man,po,msg,usual);
}



#endif
