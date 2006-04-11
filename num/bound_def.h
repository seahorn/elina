/* ********************************************************************** */
/* bound_def.h: numbers used for bounds */
/* ********************************************************************** */

#ifndef _BOUND_DEF_H_
#define _BOUND_DEF_H_

#ifndef _BOUND_H_
#error "File bound_def.h should not be included directly, only via bound.h"
#endif

#include <stdio.h>

#if defined(NUM_MAX) || defined(NUM_NUMRAT)

typedef num_t bound_t;
#define bound_numref(a) a
#define _bound_inf(a)

#else

typedef struct _bound_t {
  num_t num; /* always allocated, even if inf=1 */
  char inf;  /* 1 => +oo; 0 => <+oo */
} bound_t[1];
#define bound_numref(a) a->num
#define _bound_inf(a) a->inf = 0

#endif


/* ---------------------------------------------------------------------- */

static inline bool bound_infty(const bound_t a)

#if defined(NUM_MAX) && defined(NUM_NUMFLT)
{ return numflt_infty(a); }

#elif defined(NUM_MAX)
{ return (*a>=NUM_MAX || *a<=-NUM_MAX); }

#elif defined(NUM_NUMRAT) && defined(NUMINT_MACHINE)
{ return a->den==0; }

#elif defined(NUM_NUMRAT)
{ return numint_sgn(numrat_denref(a))==0; } 

#else
{ return (bool)a->inf; } 

#endif

/* ---------------------------------------------------------------------- */
static inline void bound_set_infty(bound_t a)
#if defined(NUM_MAX)
{ *a = NUM_MAX; }
#elif defined(NUM_NUMRAT)
{ 
  numint_set_int(numrat_numref(a),1); /* for bound_sgn and memory efficiency */
  numint_set_int(numrat_denref(a),0); 
}
#else
{
  num_set_int(a->num,1); /* for bound_sgn and memory efficiency */
  a->inf = 1;
}
#endif

/* ---------------------------------------------------------------------- */
static inline void bound_init_set_infty(bound_t a)
{
#if !defined(NUM_NATIVE)
  num_init(bound_numref(a));
#endif
  bound_set_infty(a);
}
static inline void bound_swap(bound_t a, bound_t b)
{ bound_t t; *t = *a; *a=*b; *b=*t; }

/* ====================================================================== */
/* Assignement */
/* ====================================================================== */

#if defined(NUM_MAX) || defined(NUM_NUMRAT)
static inline void bound_set(bound_t a, const bound_t b)
{ num_set(a,b); }
static inline void bound_set_array(bound_t* a, const bound_t* b, size_t size)
{ num_set_array(a,b,size); }

#else
static inline void bound_set(bound_t a, const bound_t b)
{ 
#if defined(NUM_NATIVE)
  *a = *b;
#else
  num_set(a->num,b->num);
  a->inf = b->inf;
#endif
}
static inline void bound_set_array(bound_t* a, const bound_t* b, size_t size)
{ 
#if defined(NUM_NATIVE)
  memcpy(a,b,size*sizeof(bound_t));
#else
  int i;
  for (i=0; i<size; i++) bound_set(a[i],b[i]);
#endif
}

#endif

static inline void bound_set_int(bound_t a, long int i)
{ 
  num_set_int(bound_numref(a),i);
  _bound_inf(a);
}

static inline void bound_set_num(bound_t a, num_t b)
{ num_set(bound_numref(a),b); _bound_inf(a); }

/* ====================================================================== */
/* Constructors and Destructors */
/* ====================================================================== */

static inline void bound_init(bound_t a)
{ 
  num_init(bound_numref(a)); 
  _bound_inf(a); 
}
static inline void bound_init_set_int(bound_t a, long int i)
{ 
  num_init_set_int(bound_numref(a),i);
  _bound_inf(a);
}
static inline void bound_clear(bound_t a)
{ num_clear(bound_numref(a)); }

#if defined(NUM_MAX) || defined(NUM_NUMRAT)

static inline void bound_init_array(bound_t* a, size_t size)
{ num_init_array(a,size); }
static inline void bound_init_set(bound_t a, const bound_t b)
{ num_init_set(a,b); }
static inline void bound_clear_array(bound_t* a, size_t size)
{ num_clear_array(a,size); }

#else

static inline void bound_init_array(bound_t* a, size_t size)
{ 
  int i;
  for (i=0;i<size;i++) bound_init(a[i]);
}
static inline void bound_init_set(bound_t a, const bound_t b)
{
  if (bound_infty(b)){
    bound_init_set_infty(a);
  } else {
    num_init_set(bound_numref(a),bound_numref(b));
    _bound_inf(a);
  }
}
static inline void bound_clear_array(bound_t* a, size_t size) 
{ 
#if !defined(NUM_NATIVE)
  int i;
  for (i=0;i<size;i++) bound_clear(a[i]);
#endif
}

#endif

/* ====================================================================== */
/* Arithmetic Operations */
/* ====================================================================== */

#if defined (NUM_MAX) && defined(NUM_NUMFLT)
static inline void bound_neg(bound_t a, const bound_t b)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else num_neg(a,b); 
}
static inline void bound_abs(bound_t a, const bound_t b)
{ num_abs(a,b); }
static inline void bound_add(bound_t a, const bound_t b, const bound_t c)
{ num_add(a,b,c); }
static inline void bound_add_uint(bound_t a, const bound_t b, unsigned long int c)
{ num_add_uint(a,b,c); }
static inline void bound_add_num(bound_t a, const bound_t b, const num_t c)
{ num_add(a,b,c); }
static inline void bound_sub(bound_t a, const bound_t b, const bound_t c)
{ 
  if (bound_infty(c)) bound_set_infty(a);
  else num_sub(a,b,c); 
}
static inline void bound_sub_uint(bound_t a, const bound_t b, unsigned long int c)
{ num_sub_uint(a,b,c); }
static inline void bound_sub_num(bound_t a, const bound_t b, const num_t c)
{ num_sub(a,b,c); }

 et pour zero ?
static inline void bound_mul(bound_t a, const bound_t b, const bound_t c)
{ num_mul(a,b,c); }
static inline void bound_mul_num(bound_t a, const bound_t b, const num_t c)
{ num_mul(a,b,c); }
static inline void bound_mul_2(bound_t a, const bound_t b)
{ num_mul_2(a,b); }
static inline void bound_div(bound_t a, const bound_t b, const bound_t c)
{ num_div(a,b,c); }
static inline void bound_div_num(bound_t a, const bound_t b, const num_t c)
{ num_div(a,b,c); }
static inline void bound_div_2(bound_t a, const bound_t b)
{ num_div_2(a,b); }

#else

static inline void bound_neg(bound_t a, const bound_t b)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_neg(bound_numref(a),bound_numref(b)); _bound_inf(a); } 
}
static inline void bound_abs(bound_t a, const bound_t b)
{ num_abs(bound_numref(a),bound_numref(b)); }
static inline void bound_add(bound_t a, const bound_t b, const bound_t c)
{ 
  if (bound_infty(b) || bound_infty(c)) bound_set_infty(a);
  else { num_add(bound_numref(a),bound_numref(b),bound_numref(c)); _bound_inf(a); } 
}
static inline void bound_add_uint(bound_t a, const bound_t b, unsigned long int c)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_add_uint(bound_numref(a),bound_numref(b),c); _bound_inf(a); } 
}
static inline void bound_add_num(bound_t a, const bound_t b, const num_t c)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_add(bound_numref(a),bound_numref(b),c); _bound_inf(a); } 
}
static inline void bound_sub(bound_t a, const bound_t b, const bound_t c)
{ 
  if (bound_infty(b) || bound_infty(c)) bound_set_infty(a);
  else { num_sub(bound_numref(a),bound_numref(b),bound_numref(c)); _bound_inf(a); } 
}
static inline void bound_sub_uint(bound_t a, const bound_t b, unsigned long int c)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_sub_uint(bound_numref(a),bound_numref(b),c); _bound_inf(a); } 
}
static inline void bound_sub_num(bound_t a, const bound_t b, const num_t c)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_sub(bound_numref(a),bound_numref(b),c); _bound_inf(a); } 
}

/* et pour zero ? */
static inline void bound_mul(bound_t a, const bound_t b, const bound_t c)
{ 
  if (bound_infty(b) || bound_infty(c)) bound_set_infty(a);
  else { num_mul(bound_numref(a),bound_numref(b),bound_numref(c)); _bound_inf(a); } 
}
static inline void bound_mul_num(bound_t a, const bound_t b, const num_t c)
{ 
  if (num_sgn(c)==0){
    num_set(bound_numref(a),0); _bound_inf(a);
  } 
  else if (bound_infty(b))
    bound_set_infty(a);
  else {
    num_mul(bound_numref(a),bound_numref(b),c); _bound_inf(a);
  }
}
static inline void bound_mul_2(bound_t a, const bound_t b)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_mul_2(bound_numref(a),bound_numref(b)); _bound_inf(a); } 
}
static inline void bound_div(bound_t a, const bound_t b, const bound_t c)
{ 
  if (bound_infty(b) || bound_infty(c)) bound_set_infty(a);
  else { num_div(bound_numref(a),bound_numref(b),bound_numref(c)); _bound_inf(a); } 
}
static inline void bound_div_num(bound_t a, const bound_t b, const num_t c)
{ 
  if (num_sgn(c)==0){
    bound_set_infty(a);
  } 
  else if (bound_infty(b))
    bound_set_infty(a);
  else {
    num_div(bound_numref(a),bound_numref(b),c); _bound_inf(a);
  }
}
static inline void bound_div_2(bound_t a, const bound_t b)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_div_2(bound_numref(a),bound_numref(b)); _bound_inf(a); } 
}
#endif


#if defined (NUM_MAX)

static inline void bound_min(bound_t a, const bound_t b, const bound_t c)
{ num_min(a,b,c); }
static inline void bound_max(bound_t a, const bound_t b, const bound_t c)
{ num_max(a,b,c); }

#else

static inline void bound_min(bound_t a, const bound_t b, const bound_t c)
{
  if (bound_infty(b)) bound_set(a,c);
  else if (bound_infty(c)) bound_set(a,b);
  else { num_min(bound_numref(a),bound_numref(b),bound_numref(c)); _bound_inf(a); }
}
static inline void bound_max(bound_t a, const bound_t b, const bound_t c)
{
  if (bound_infty(b) || bound_infty(c)) bound_set_infty(a);
  else { num_max(bound_numref(a),bound_numref(b),bound_numref(c)); _bound_inf(a); }
}

#endif

static inline void bound_floor(bound_t a, const bound_t b)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_floor(bound_numref(a),bound_numref(b)); _bound_inf(a); } 
}
static inline void bound_ceil(bound_t a, const bound_t b)
{ 
  if (bound_infty(b)) bound_set_infty(a);
  else { num_ceil(bound_numref(a),bound_numref(b)); _bound_inf(a); } 
}

/* ====================================================================== */
/* Arithmetic Tests */
/* ====================================================================== */

#if defined(NUM_MAX)
static inline int bound_sgn(const bound_t a)
         { return num_sgn(a); }
static inline int bound_cmp(const bound_t a, const bound_t b)
      { return num_cmp(a,b); }
static inline int bound_cmp_int(const bound_t a, long int b)
      { return num_cmp_int(a,b); }
static inline int bound_cmp_num(const bound_t a, const num_t b)
      { return num_cmp(a,b); }
static inline int bound_equal(const bound_t a, const bound_t b)
      { return num_equal(a,b); }

#else

static inline int bound_sgn(const bound_t a)
{ return bound_infty(a) ? 1 : num_sgn(bound_numref(a)); }
static inline int bound_cmp(const bound_t a, const bound_t b)
{
  if (bound_infty(a)){
    if (bound_infty(b)) return 0;
    else return 1;
  } else {
    if (bound_infty(b)) return -1;
    else return num_cmp(bound_numref(a),bound_numref(b));
  }
}
static inline int bound_cmp_int(const bound_t a, long int b)
{
  if (bound_infty(a)) return 1;
  else return num_cmp_int(bound_numref(a),b);
}
static inline int bound_cmp_num(const bound_t a, const num_t b)
{
  if (bound_infty(a)) return 1;
  else return num_cmp(bound_numref(a),b);
}
#if defined(NUM_NUMRAT)
static inline int bound_equal(const bound_t a, const bound_t b)
{
  if (bound_infty(a)){
    return bound_infty(b);
  } else {
    if (bound_infty(b)) return false;
    else return num_equal(bound_numref(a),bound_numref(b));
  }
}
#else
static inline int bound_equal(const bound_t a, const bound_t b)
{
  if (a->inf==b->inf)
    return a->inf || num_equal(a->num,b->num);
  else
    return false;
}
#endif

#endif

/* ====================================================================== */
/* Printing */
/* ====================================================================== */

static inline void bound_fprint(FILE* stream, const bound_t a)
{
  if (bound_infty(a)) fprintf(stream,"+oo");
  else num_fprint(stream,bound_numref(a));
}
static inline void bound_print(const bound_t a)
{
  bound_fprint(stdout,a);
}
static inline int bound_snprint(char* s, size_t size, const bound_t a)
{
  if (bound_infty(a)) return snprintf(s,size,"+oo");
  else return num_snprint(s,size,bound_numref(a));
}

#endif
