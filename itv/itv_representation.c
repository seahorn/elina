/* ********************************************************************** */
/* itv_representation.c: general management */
/* ********************************************************************** */

#include <string.h>
#include <stdio.h>

#include "itv_config.h"
#include "itv_int.h"
#include "itv_representation.h"

/* ********************************************************************** */
/* Internal functions */
/* ********************************************************************** */
itv_t* itv_alloc(size_t intdim, size_t realdim)
{
  itv_t* itv = malloc(sizeof(itv_t));
  itv->p = NULL;
  itv->intdim = intdim;
  itv->realdim = realdim;
  return itv;
}

void itv_init(itv_t* a)
{
  int i;
  size_t nbdims = a->intdim + a->realdim;
  if (nbdims>0){
    assert(a->p==NULL);
    a->p = malloc( nbdims*sizeof(itv_interval_t) );
    for(i=0;i<nbdims; i++){
      itv_interval_init(a->p[i]);
    }
  }
}

void itv_set_bottom(itv_t* a)
{
  if (a->p){
    itv_interval_array_free(a->p,a->intdim+a->realdim);
    a->p = NULL;
  }
}

void itv_set_top(itv_t* a)
{
  int i;
  size_t nbdims;
  
  nbdims = a->intdim + a->realdim;
  if (nbdims>0){
    if (a->p==NULL){
      itv_init(a);
    };
    for (i=0; i<nbdims; i++){
      itv_interval_set_top(a->p[i]);
    }
  }
}

void itv_set(itv_t* a, const itv_t* b)
{
  int i;
  size_t nbdims;
  
  if (b->p==NULL)
    return;

  nbdims = b->intdim + b->realdim;
  if (a->p==NULL){
    itv_init(a);
  };
  for (i=0; i<nbdims; i++){
    itv_interval_set(a->p[i],b->p[i]);
  }
}

/* ********************************************************************** */
/* 1. Memory */
/* ********************************************************************** */

/* Return a copy of an abstract value, on
   which destructive update does not affect the initial value. */
itv_t* itv_copy(ap_manager_t* man, const itv_t* a)
{
  int i;
  size_t nbdims = a->intdim+a->realdim;

  itv_t* b = itv_alloc(a->intdim,a->realdim);
  if (a->p){
    b->p = malloc(nbdims*sizeof(itv_interval_t));
    for (i=0; i<nbdims; i++){
      itv_interval_init_set(b->p[i],a->p[i]);
    }
  }
  man->result.flag_best = tbool_true;
  man->result.flag_exact = tbool_true;
  return b;
}

/* Free all the memory used by the abstract value */
void itv_free(ap_manager_t* man, itv_t* a)
{
  if (a->p){
    itv_interval_array_free(a->p,a->intdim+a->realdim);
    a->p = NULL;
  }
  free(a);
}

/* Return the abstract size of an abstract value (see ap_manager_t) */
size_t itv_size(ap_manager_t* man, const itv_t* a)
{
  return 2*(a->intdim+a->realdim);
}

/* ********************************************************************** */
/* 2. Control of internal representation */
/* ********************************************************************** */

/* Minimize the size of the representation of a.
   This may result in a later recomputation of internal information.
*/
void itv_minimize(ap_manager_t* man, const itv_t* a)
{
  return;
}

/* Put the abstract value in canonical form. (not yet clear definition) */
void itv_canonicalize(ap_manager_t* man, const itv_t* a)
{
  return;
}

/* Perform some transformation on the abstract value, guided by the
   field algorithm.

   The transformation may lose information.  The argument "algorithm"
   overrides the field algorithm of the structure of type foption_t
   associated to itv_approximate (commodity feature). */
void itv_approximate(ap_manager_t* man, itv_t* a, int algorithm)
{
  return;
}

tbool_t itv_is_minimal(ap_manager_t* man, const itv_t* a)
{
  man->result.flag_exact = tbool_true;
  return tbool_true;
}
tbool_t itv_is_canonical(ap_manager_t* man, const itv_t* a)
{
  man->result.flag_exact = tbool_true;
  return tbool_true;
}

/* ********************************************************************** */
/* 3. Printing */
/* ********************************************************************** */

/* Print the abstract value in a pretty way, using function
   name_of_dim to name dimensions */
void itv_fprint(FILE* stream,
	       ap_manager_t* man,
	       const itv_t* a,
	       char** name_of_dim)
{
  int i;
  size_t nbdims = a->intdim + a->realdim;

  fprintf(stream,"interval of dim (%d,%d):",
	  a->intdim,a->intdim);
  if (a->p){
    fprintf(stream,"\n");
    for(i=0; i<nbdims; i++){
      if (name_of_dim){
	fprintf(stream,"%8s in ", name_of_dim[i]);
      } else {
	fprintf(stream,"x%d in ", i);
      }
      itv_interval_fprint(stream,a->p[i]);
      fprintf(stream,"\n");
    }
  }
  else {
    fprintf(stream,nbdims>0 ? " bottom\n" : "top\n");
  }
}

/* Dump the internal representation of an abstract value,
   for debugging purposes */
void itv_fdump(FILE* stream,
	      ap_manager_t* man,
	      const itv_t* a)
{
  int i;
  size_t nbdims = a->intdim + a->realdim;

  fprintf(stream,"interval of dim (%d,%d):",
	  a->intdim,a->intdim);
  if (a->p){
    fprintf(stream,"\n");
    for(i=0; i<nbdims; i++){
      fprintf(stream,"dim %3d in ",i);
      itv_interval_fprint(stream,a->p[i]);
      fprintf(stream,"\n");
    }
  }
  else {
    fprintf(stream, nbdims>0 ? " bottom\n" : "top\n");
  }
}

/* Print the difference between a1 (old value) and a2 (new value),
   using function name_of_dim to name dimensions.
   The meaning of difference is library dependent. */
void itv_fprintdiff(FILE* stream,
		   ap_manager_t* man,
		   const itv_t* a, const itv_t* b,
		   char** name_of_dim)
{
  int i;
  size_t nbdims;
  char* str;

  nbdims = a->intdim + a->realdim;

  fprintf(stream,"diff of 2 intervals of dim (%d,%d)",
	  a->intdim,b->intdim);
  if (itv_is_eq(man,a,b)){
    fprintf(stream," : none\n");
  }
  else {
    /* we are sure that nbdims>0 */
    if (a->p==0){
      fprintf(stream,"\nbottom =>\n");
      itv_fprint(stream,man,b,name_of_dim);
    }
    else if (b->p==0){
      fprintf(stream,"\n");
      itv_fprint(stream,man,a,name_of_dim);
      fprintf(stream,"=> bottom\n");
    }
    else {
      bound_t bound;
      bound_init(bound);
      for(i=0; i<nbdims; i++){
	int sgn1 = bound_cmp(a->p[i]->inf, b->p[i]->inf);
	int sgn2 = bound_cmp(a->p[i]->sup, b->p[i]->sup);
	
	if (sgn1!=0 || sgn2!=0){
	  fprintf(stream,"%8s in ",name_of_dim[i]);
	  itv_interval_fprint(stream,a->p[i]);
	  fprintf(stream," => ");
	  str =
	    sgn1>0 ?
	    "-[" :
	    ( sgn1<0 ?
	      "+[" :
	      "[=, "  );
	  fprintf(stream,str);
	  if (sgn1!=0){
	    bound_neg(bound,b->p[i]->inf);
	    bound_fprint(stream,bound);
	    fprintf(stream,", ");
	  }
	  if (sgn2!=0){
	    bound_fprint(stream,b->p[i]->sup);
	  }
	  str =
	    sgn2>0 ?
	    "]+" :
	    ( sgn2<0 ?
	      "]-" :
	      "=]"  );
	  fprintf(stream,"%s\n",str);
	}
	else {
	  fprintf(stream,"%8s in [=,=]\n",name_of_dim[i]);
	}
      }
      bound_clear(bound);
    }
  }
}


/* ********************************************************************** */
/* 4. Serialization */
/* ********************************************************************** */

/* Allocate a memory buffer (with malloc), output the abstract value in raw
   binary format to it and return a pointer on the memory buffer and the size
   of bytes written.  It is the user responsability to free the memory
   afterwards (with free). */
ap_membuf_t itv_serialize_raw(ap_manager_t* man, const itv_t* a)
{
  ap_membuf_t buf;
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_SERIALIZE_RAW,"");
  buf.ptr = NULL;
  buf.size = 0;
  return buf;
}

/* Return the abstract value read in raw binary format from the input stream
   and store in size the number of bytes read */
itv_t* itv_deserialize_raw(ap_manager_t* man, void* ptr)
{
  ap_manager_raise_exception(man,AP_EXC_NOT_IMPLEMENTED,AP_FUNID_DESERIALIZE_RAW,"");
  return NULL;
}

