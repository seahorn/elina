/*
 * oct_test.c
 *
 * Unit testing.
 *
 * APRON Library / Octagonal Domain
 *
 * Copyright (C) Antoine Mine' 2006
 *
 */

/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

long int lrand48(void);
void srand48(long int seedval);

#include "oct.h"
#include "oct_fun.h"
#include "oct_internal.h"

#include "pk.h"

ap_manager_t* mo; /* octagon */
ap_manager_t* mp; /* polyhedron */

oct_internal_t* pr;
pk_internal_t* pk;


#define N 60

char b1_[N+4]= " [";
char b2_[N+4];
int i_;
int error_ = 0;

typedef enum { none, best, exact } exactness;

exactness flag;

#define LOOP								\
  {									\
    memset(b1_+2,' ',N); b1_[N+2]=']'; b1_[N+3]=0;			\
    memset(b2_,8,N+3); b2_[N+3]=0;					\
    for (i_=0;i_<N;i_++) {						\
      printf("%s%s",b1_,b2_);						\
      fflush(stdout);							\
      flag = exact;							\
      
#define FLAG(m) \
if (m->result.flag_exact==tbool_true) ;		      \
 else if (m->result.flag_best==tbool_true)	      \
   { if (flag==exact) flag = best; else flag = none;} \
 else flag = none;


#define RESULT(c) b1_[i_+2]=c

#define ERROR(msg)					\
  do {							\
    fprintf(stderr,"\n%s\n",msg);			\
    RESULT('!');					\
    error_++;						\
  } while (0)

#define ENDLOOP	} } printf("%s%s\n",b1_,b2_)


char check(oct_t* o)
{
  if (!o->closed && !o->m) return 'o'; /* really empty */
  if (o->closed) {
    bound_t* cl;
    size_t i;
    if (num_incomplete || o->intdim) return '.'; /* incomplete */
    /* check that o->closed is really closed */
    if (!hmat_check_closed(o->closed,o->dim)) {
      ERROR("invalid closure");
      return '!';
    }
    if (!o->m) return 'c'; /* ok */
    /* now check that closure(o->m) = o->closed */
    cl = hmat_copy(pr,o->m,o->dim);
    hmat_close(cl,o->dim);
    for (i=0;i<matsize(o->dim);i++)
      if (bound_cmp(cl[i],o->closed[i])) {
	hmat_free(pr,cl,o->dim);
	return '#';
      }
    hmat_free(pr,cl,o->dim);
    return 'C';
  }
  return '.'; /* regular matrix */
}

void random_bound(bound_t b)
{
  num_t n;
  num_init(n);
  num_set_int2(n,lrand48()%20-2,lrand48()%4+1);
  bound_set_num(b,n);
  num_clear(n);
}

void random_hmat(bound_t* b, int dim, float frac)
{
  int i,j;
  num_t n;
  num_init(n);
  for (i=0;i<2*dim;i++)
    for (j=0;j<=(i|1);j++,b++) {
      if (i==j) continue;
      if (lrand48()%100>frac*100) continue;
      num_set_int2(n,lrand48()%20-2,lrand48()%4+1);
      bound_set_num(*b,n);
    }
  num_clear(n);
}

oct_t* random_oct(int dim,float frac)
{
  oct_t* o = oct_alloc_internal(pr,dim,0);
  o->m = hmat_alloc_top(pr,dim);
  random_hmat(o->m,dim,frac);
  return o;
}

typedef enum  {
  expr_unary,
  expr_oct,
  expr_lin,
  expr_qlin,
  expr_interv,
} exprmode;

const char* exprname[] = { "unary ", "octgonal ", "", "quasi", "interval " };

ap_linexpr0_t* random_linexpr(exprmode mode, int dim)
{
  ap_linexpr0_t* l = ap_linexpr0_alloc(AP_LINEXPR_DENSE,dim);
  int i,n1,n2,d;
  if (mode<=expr_oct) {
    if (lrand48()%10>2)
      ap_coeff_set_scalar_int(l->p.coeff+(lrand48()%dim),lrand48()%3-1);
    if (mode==expr_oct)
      if (lrand48()%10>2)
	ap_coeff_set_scalar_int(l->p.coeff+(lrand48()%dim),lrand48()%3-1);
  }
  else if (mode<=expr_qlin) {
    for (i=0;i<dim;i++)
      ap_coeff_set_scalar_frac(l->p.coeff+i,lrand48()%20-10,lrand48()%4+1);
  }
  else {
    for (i=0;i<dim;i++) {
      int n1 = lrand48()%20-10;
      int n2 = n1 + lrand48()%20;
      int d  = lrand48()%4+1;
      ap_coeff_set_interval_frac(l->p.coeff+i,n1,d,n2,d);
    }
  }
  if (mode<=expr_lin) {
    ap_coeff_set_scalar_frac(&l->cst,lrand48()%20-10,lrand48()%4+1);
  }
  else {
    int n1 = lrand48()%20-10;
    int n2 = n1 + lrand48()%20;
    int d  = lrand48()%4+1;
    ap_coeff_set_interval_frac(&l->cst,n1,d,n2,d);
  }
  return l;
}

/* chose one linexpr within an interval linexpr */
ap_linexpr0_t* random_from_linexpr(ap_linexpr0_t* a)
{
  size_t i;
  ap_linexpr0_t* l = ap_linexpr0_alloc(AP_LINEXPR_DENSE,a->size);
  assert(a->discr==AP_LINEXPR_DENSE);
  for (i=0;i<a->size;i++) {
    switch (a->p.coeff[i].discr) {
    case AP_COEFF_SCALAR:
      ap_coeff_set_scalar(l->p.coeff+i,a->p.coeff[i].val.scalar);
      break;
    case AP_COEFF_INTERVAL:
      if (lrand48()%2)
	ap_coeff_set_scalar(l->p.coeff+i,a->p.coeff[i].val.interval->inf);
      else 
	ap_coeff_set_scalar(l->p.coeff+i,a->p.coeff[i].val.interval->sup);
      break;
      
    }
  }
  switch (a->cst.discr) {
  case AP_COEFF_SCALAR:
    ap_coeff_set_scalar(&l->cst,a->cst.val.scalar);
    break;
  case AP_COEFF_INTERVAL:
    if (lrand48()%2) ap_coeff_set_scalar(&l->cst,a->cst.val.interval->inf);
    else ap_coeff_set_scalar(&l->cst,a->cst.val.interval->sup);
    break;
    
  }
 return l;
}

ap_lincons0_t random_from_lincons(ap_lincons0_t a)
{
  return ap_lincons0_make(a.constyp,random_from_linexpr(a.linexpr0),NULL);
}


ap_generator0_t random_generator(int dim, ap_gentyp_t g)
{
  ap_linexpr0_t* l = ap_linexpr0_alloc(AP_LINEXPR_DENSE,dim);
  int i;
  for (i=0;i<dim;i++)
    ap_coeff_set_scalar_frac(l->p.coeff+i,lrand48()%20-2,lrand48()%4+1);
  ap_coeff_set_scalar_int(&l->cst,0);
  return ap_generator0_make(g,l);
}

ap_abstract0_t* random_poly(int dim)
{
  ap_abstract0_t* p;
  int i;
  ap_interval_t** t = ap_interval_array_alloc(dim);
  ap_generator0_array_t ar = ap_generator0_array_make(dim);
  for (i=0;i<dim;i++)
    ap_interval_set_int(t[i],0,0);
  for (i=0;i<dim;i++)
    ar.p[i] = random_generator(dim,
			       (lrand48()%100>=90)?AP_GEN_RAY:AP_GEN_VERTEX);
  p = ap_abstract0_of_box(mp,0,dim,(const ap_interval_t**)t);
  ap_abstract0_add_ray_array(mp,true,p,&ar);
  /*ap_generator0_array_fprint(stderr,&ar,NULL);*/
  ap_generator0_array_clear(&ar);
  ap_interval_array_free(t,dim);
  return p;
}

oct_t* oct_of_poly(ap_abstract0_t* p)
{
  ap_generator0_array_t ar;
  ap_dimension_t d = ap_abstract0_dimension(mp,p);
  oct_t* o;
  ar = ap_abstract0_to_generator_array(mp,p);
  FLAG(mp);
  o = oct_of_generator_array(mo,d.intdim,d.realdim,&ar);
  FLAG(mo);
  /*ap_generator0_array_fprint(stderr,&ar,NULL);*/
  ap_generator0_array_clear(&ar);
  return o;
}

ap_abstract0_t* poly_of_oct(oct_t* o)
{
  ap_lincons0_array_t ar;
  ap_dimension_t d = oct_dimension(mo,o);
  ap_abstract0_t* p;
  ar = oct_to_lincons_array(mo,o);
  FLAG(mo);
  p = ap_abstract0_of_lincons_array(mp,d.intdim,d.realdim,&ar);
  FLAG(mp);
  /*ap_lincons0_array_fprint(stderr,&ar,NULL);*/
  ap_lincons0_array_clear(&ar);
  return p;
}

void print_oct(const char* msg, oct_t* o)
{
  fprintf(stderr,"%s = ",msg);
  oct_fprint(stderr,mo,o,NULL);
  oct_fdump(stderr,mo,o);
  fprintf(stderr,"\n");
}

void print_poly(const char* msg, ap_abstract0_t* p)
{
  fprintf(stderr,"%s = ",msg);
  ap_abstract0_fprint(stderr,mp,p,NULL);
  ap_abstract0_fdump(stderr,mp,p);
  fprintf(stderr,"\n");
}

void print_interval(const char*msg, ap_interval_t* i)
{
  fprintf(stderr,"%s = ",msg);
  ap_interval_fprint(stderr,i);
  fprintf(stderr,"\n");
}


/* ********************************* */
/*             infos                 */
/* ********************************* */



void info(void)
{
  printf("octagons:  %s (%s)\n",mo->library,mo->version);
  printf("polyhedra: %s (%s)\n",mp->library,mp->version);
  printf("nums:      %s (%s wto overflow,%s,%s double,%s mpq)\n",num_name,
	 num_safe ? "sound" : "unsound",
	 num_incomplete ? "incomplete" : "complete",
	 num_of_double_approx ? "inexact" : "exact",
	 num_of_mpq_approx ? "inexact" : "exact" );
}



/* ********************************* */
/*           various tests           */
/* ********************************* */

void test_misc(void)
{
  size_t D = 5;
  int i;
  oct_t* bot = oct_bottom(mo,0,D);
  oct_t* top = oct_top(mo,0,D);
  ap_abstract0_t* bot2 = poly_of_oct(bot);
  ap_abstract0_t* top2 = poly_of_oct(top);
  ap_dimension_t d1 = oct_dimension(mo,bot);
  ap_dimension_t d2 = oct_dimension(mo,top);  
  printf("\nperforming various tests\n");
  check(bot); check(top);
  if (d1.intdim || d1.realdim!=D) printf("oct_dimension failed #1\n");
  if (d2.intdim || d2.realdim!=D) printf("oct_dimension failed #2\n");
  if (oct_is_bottom(mo,bot)==tbool_false)  printf("oct_is_bottom failed #3\n");
  if (oct_is_bottom(mo,top)==tbool_true)   printf("oct_is_bottom failed #4\n");
  if (oct_is_top(mo,bot)==tbool_true)      printf("oct_is_top failed #5\n");
  if (oct_is_top(mo,top)==tbool_false)     printf("oct_is_top failed #6\n");
  if (oct_is_leq(mo,bot,top)==tbool_false) printf("oct_is_leq failed #7\n");
  if (oct_is_leq(mo,top,bot)==tbool_true)  printf("oct_is_leq failed #8\n");
  if (oct_is_eq(mo,bot,bot)==tbool_false)  printf("oct_is_eq failed #9\n");
  if (oct_is_eq(mo,top,top)==tbool_false)  printf("oct_is_eq failed #10\n");
  if (oct_is_eq(mo,bot,top)==tbool_true)   printf("oct_is_eq failed #11\n");
  if (oct_is_dimension_unconstrained(mo,bot,0)==tbool_true)
    printf("oct_is_dimension_unconstrained #12\n");
  if (oct_is_dimension_unconstrained(mo,top,0)==tbool_false)
    printf("oct_is_dimension_unconstrained #13\n");
  if (ap_abstract0_is_bottom(mp,bot2)==tbool_false) printf("ap_abstract0_is_bottom failed\n");
  if (ap_abstract0_is_top(mp,top2)==tbool_false)    printf("poly_is_top failed\n");
  for (i=0;i<N;i++) {
    oct_t* o = random_oct(D,.1);
    oct_t* c = oct_copy(mo,o);
    oct_t* l = oct_closure(mo,false,o);
    ap_dimension_t d = oct_dimension(mo,o);
    if (d.intdim || d.realdim!=D) printf("oct_dimension failed #14\n");
    if (oct_is_leq(mo,bot,o)==tbool_false)  printf("oct_is_leq failed #15\n");
    if (oct_is_leq(mo,o,top)==tbool_false)  printf("oct_is_leq failed #16\n");
    if (oct_is_eq(mo,o,c)==tbool_false)     printf("oct_is_eq failed #17\n");
    if (oct_is_eq(mo,o,l)==tbool_false)     printf("oct_is_eq failed #18\n");
    oct_size(mo,o);
    oct_close(pr,o);
    // not implemented
    //oct_minimize(mo,o);
    //oct_canonicalize(mo,o);
    //oct_approximate(mo,o,0);
    //oct_is_minimal(mo,o);
    //oct_is_canonical(mo,o);
    oct_free(mo,o); oct_free(mo,c); oct_free(mo,l);
  }
  oct_free(mo,bot); oct_free(mo,top);
  ap_abstract0_free(mp,bot2); ap_abstract0_free(mp,top2);
}



/* ********************************* */
/*           closure                 */
/* ********************************* */


void test_closure(void)
{
  printf("\nclosure (c,o expected)\n");
  LOOP {
    oct_t* o = random_oct(14,.1);
    oct_close(pr,o);
    RESULT(check(o));
    oct_free(mo,o);
  } ENDLOOP;
}

void test_incremental_closure(void)
{
  printf("\nincremental closure (C,o expected)\n");
  LOOP {
    size_t dim = 14;
    oct_t* o = random_oct(dim,.1);
    size_t i, v = lrand48() % dim;
    oct_close(pr,o);
    if (o->closed) {
      for (i=0;2*i<dim;i++) {
	if (lrand48()%10>8) random_bound(o->closed[matpos2(i,2*v)]);
	if (lrand48()%10>8) random_bound(o->closed[matpos2(i,2*v+1)]);
      }
      o->m = hmat_copy(pr,o->closed,dim);
      if (hmat_close_incremental(o->closed,dim,v)) RESULT('o');
      else RESULT(check(o));
    }
    else  RESULT('o');
    oct_free(mo,o);
  } ENDLOOP;
}



/* ********************************* */
/*            conversions            */
/* ********************************* */

void test_polyhedra_conversion(void)
{
  printf("\nconversion to polyhedra (* expected)\n");
  LOOP {
    oct_t *o, *o2;
    ap_abstract0_t *p, *p2;
    o = random_oct(7,.1);
    p = poly_of_oct(o);
    o2 = oct_of_poly(p);
    p2 = poly_of_oct(o2);
    RESULT(check(o)); check(o2);
    if (oct_is_leq(mo,o,o2)==tbool_false || ap_abstract0_is_leq(mp,p,p2)==tbool_false) {
      ERROR("not included in");
      print_oct("o",o); 
      print_poly("p",p); 
      print_oct("o2",o2); 
      print_poly("p2",p2); 
    }
    if (oct_is_eq(mo,o,o2)==tbool_true && ap_abstract0_is_eq(mp,p,p2)==tbool_true)
      RESULT('*');
    if (flag==exact && (oct_is_eq(mo,o,o2)==tbool_false || ap_abstract0_is_eq(mp,p,p2)==tbool_false)) ERROR("exact flag");
    oct_free(mo,o); oct_free(mo,o2); ap_abstract0_free(mp,p); ap_abstract0_free(mp,p2);
  } ENDLOOP;
  {
    oct_t *o;
    ap_abstract0_t *p;
    o = oct_bottom(mo,0,7);
    p = poly_of_oct(o);
    if (ap_abstract0_is_bottom(mp,p)!=tbool_true) printf("poly_is_bottom failed\n");
    oct_free(mo,o); ap_abstract0_free(mp,p);
  }
  {
    oct_t *o;
    ap_abstract0_t *p;
    o = oct_top(mo,0,7);
    p = poly_of_oct(o);
    if (ap_abstract0_is_top(mp,p)!=tbool_true) printf("poly_is_top failed\n");
    oct_free(mo,o); ap_abstract0_free(mp,p);
  }
}

void test_polyhedra_conversion2(void)
{
  printf("\nconversion from polyhedra (=from generator, * expected)\n");
  LOOP {
    ap_abstract0_t *p, *p2;
    oct_t *o, *o2;
    p = random_poly(7);
    o = oct_of_poly(p);
    p2 = poly_of_oct(o);
    o2 = oct_of_poly(p2);
    RESULT(check(o)); check(o2);
    if (oct_is_leq(mo,o,o2)==tbool_false || ap_abstract0_is_leq(mp,p,p2)==tbool_false) {
      ERROR("not included in"); 
      print_poly("p",p); 
      print_oct("o",o); 
      print_poly("p2",p2); 
      print_oct("o2",o2); 
    }
    if (oct_is_eq(mo,o,o2)==tbool_true) RESULT('*');
    if (flag==exact && oct_is_eq(mo,o,o2)==tbool_false) ERROR("exact flag");
    oct_free(mo,o); oct_free(mo,o2); ap_abstract0_free(mp,p); ap_abstract0_free(mp,p2);
  } ENDLOOP;
  {
    ap_abstract0_t *p;
    oct_t *o;
    p = ap_abstract0_bottom(mp,0,7);
    o = oct_of_poly(p);
    if (oct_is_bottom(mo,o)!=tbool_true) printf("oct_is_bottom failed\n");
    oct_free(mo,o); ap_abstract0_free(mp,p);
  }
  {
    ap_abstract0_t *p;
    oct_t *o;
    p = ap_abstract0_top(mp,0,7);
    o = oct_of_poly(p);
    if (oct_is_top(mo,o)!=tbool_true) printf("oct_is_top failed\n");
    oct_free(mo,o); ap_abstract0_free(mp,p);
  }
}

void test_lincons_conversion(exprmode mode)
{
  printf("\nconversion from %slincons (%s expected)\n",exprname[mode],
	 (mode<=expr_oct)?"*":"*,x,.");
  LOOP {
    int dim = 7, nb = 10, i;
    ap_abstract0_t *p, *p2;
    oct_t *o, *oo;
    ap_lincons0_array_t t = ap_lincons0_array_make(nb);
    ap_lincons0_array_t tt = ap_lincons0_array_make(nb);
    for (i=0;i<nb;i++) {
      t.p[i] = ap_lincons0_make((lrand48()%100>=90)?AP_CONS_EQ:
				(lrand48()%100>=90)?AP_CONS_SUP:
				AP_CONS_SUPEQ,
				random_linexpr(mode,dim),
				NULL);
      tt.p[i] = random_from_lincons(t.p[i]);
    }
    p  = ap_abstract0_of_lincons_array(mp,0,dim,&tt);
    o  = oct_of_lincons_array(mo,0,dim,&t); FLAG(mo);
    p2 = poly_of_oct(o);
    oo = oct_of_poly(p);
    RESULT(check(o));
    if (ap_abstract0_is_leq(mp,p,p2)==tbool_false) {
      ERROR("not included in"); 
      print_poly("p",p); 
      fprintf(stderr,"t = "); ap_lincons0_array_fprint(stderr,&t,NULL);
      print_oct("o",o); 
      print_poly("p2",p2); 
    }
    if (ap_abstract0_is_eq(mp,p,p2)==tbool_true) RESULT('*');
    else if (oct_is_eq(mo,o,oo)==tbool_true) RESULT('x');
    if (flag==best && oct_is_eq(mo,o,oo)==tbool_false) ERROR("best flag");
    if (flag==exact && ap_abstract0_is_eq(mp,p,p2)==tbool_false) 
      ERROR("exact flag");
    oct_free(mo,o); oct_free(mo,oo); 
    ap_abstract0_free(mp,p); ap_abstract0_free(mp,p2);
    ap_lincons0_array_clear(&t); ap_lincons0_array_clear(&tt);
  } ENDLOOP;
}

void test_generator_conversion(void)
{
  printf("\nconversion to generators (.,* expected)\n");
  LOOP {
    size_t dim = 7;
    oct_t *o, *o2;
    ap_generator0_array_t t;
    o  = random_oct(dim,.1);
    t  = oct_to_generator_array(mo,o); FLAG(mo);
    o2 = oct_of_generator_array(mo,0,dim,&t); FLAG(mo);
    RESULT(check(o2));
    if (oct_is_leq(mo,o,o2)==tbool_false) {
      ERROR("not included in"); 
      print_oct("o",o); 
      fprintf(stderr,"t = "); ap_generator0_array_fprint(stderr,&t,NULL);
      print_oct("o2",o2); 
    }
    if (oct_is_eq(mo,o,o2)==tbool_true) RESULT('*');
    if (flag==exact && oct_is_eq(mo,o,o2)==tbool_false) ERROR("exact flag");
    oct_free(mo,o); oct_free(mo,o2); ap_generator0_array_clear(&t);
  } ENDLOOP;
}

void test_box_conversion(void)
{
  printf("\nconversion to box (* expected)\n");
  LOOP {
    size_t dim = 7;
    int i;
    oct_t *o,*o2,*o3;
    ap_interval_t **b,**b2;
    o = random_oct(dim,.1);
    b = oct_to_box(mo,o); FLAG(mo);
    o2 = oct_of_box(mo,0,dim,(const ap_interval_t**)b); FLAG(mo);
    b2 = oct_to_box(mo,o2); FLAG(mo);
    o3 = oct_of_box(mo,0,dim,(const ap_interval_t**)b2); FLAG(mo);
    RESULT(check(o)); check(o2);
    if (oct_is_leq(mo,o,o2)==tbool_false || oct_is_leq(mo,o2,o3)==tbool_false) {
      ERROR("not included in"); 
      print_oct("o",o); 
      print_oct("o2",o2); 
      print_oct("o3",o3); 
    }
    if (oct_is_eq(mo,o2,o3)==tbool_true) RESULT('*');
    if (flag==exact && oct_is_eq(mo,o2,o3)==tbool_false) ERROR("exact flag");
    ap_interval_array_free(b,dim);
    ap_interval_array_free(b2,dim);
    oct_free(mo,o); oct_free(mo,o2); oct_free(mo,o3);
  } ENDLOOP;
}


/* ********************************* */
/*            serialization          */
/* ********************************* */


void test_serialize(void)
{
  printf("\nserialization (* expected)\n");
  LOOP {
    size_t sz;
    oct_t *o, *o2;
    ap_membuf_t b;
    o  = random_oct(10,.1);
    b  = oct_serialize_raw(mo,o); FLAG(mo);
    o2 = oct_deserialize_raw(mo,b.ptr,&sz); FLAG(mo);
    RESULT(check(o)); check(o2);
    if (oct_is_leq(mo,o,o2)==tbool_false) {
      ERROR("not included in");
      print_oct("o",o); 
      print_oct("o2",o2); 
    }
    if (oct_is_eq(mo,o,o2)==tbool_true) RESULT('*');
    if (flag==exact && oct_is_eq(mo,o,o2)==tbool_false) ERROR("exact flag");
    if (b.size!=sz) ERROR("different size");
    oct_free(mo,o); oct_free(mo,o2); free(b.ptr);
  } ENDLOOP;
}


/* ********************************* */
/*                bound              */
/* ********************************* */


void test_bound_dim(void)
{
  printf("\nbound dimension (* expected)\n");
  LOOP {
    int dim = 8;
    int v = lrand48() % dim;
    oct_t *o;
    ap_abstract0_t *p;
    ap_interval_t *io, *ip;
    o  = random_oct(dim,.2);
    p  = poly_of_oct(o);
    io = oct_bound_dimension(mo,o,v); FLAG(mo);
    ip = ap_abstract0_bound_dimension(mp,p,v); FLAG(mp);
    RESULT(check(o));
    if (oct_sat_interval(mo,o,v,io)==tbool_false) ERROR("not sat oct");
    FLAG(mo);
    if (ap_abstract0_sat_interval(mp,p,v,io)==tbool_false) {
      ERROR("not sat poly");
      print_oct("o",o);
      print_poly("p",p);
      fprintf(stderr,"dim=%i\n",v);
      print_interval("io",io);
      print_interval("ip",ip);
    }
    FLAG(mp);
    if (ap_interval_cmp(ip,io)==0 &&
	oct_sat_interval(mo,o,v,ip)==tbool_true) RESULT('*');
    else if (ap_interval_cmp(ip,io)==-1) RESULT('.');
    else ERROR("not included in");
    if (flag==exact && ap_interval_cmp(ip,io)!=0) ERROR("exact flag");
    if (oct_is_dimension_unconstrained(mo,o,v)==tbool_true &&
	ap_abstract0_is_dimension_unconstrained(mp,p,v)==tbool_false)
      ERROR("not unconstrained");
    oct_free(mo,o); ap_abstract0_free(mp,p);
    ap_interval_free(io); ap_interval_free(ip);
   } ENDLOOP;
}

void test_bound_linexpr(exprmode mode)
{
  printf("\nbound %slinexpr (%s expected)\n",exprname[mode],
	 (mode<=expr_oct)?"*":"*,.");
  LOOP {
    int dim = 8;
    oct_t *o;
    ap_abstract0_t *p;
    ap_linexpr0_t *e, *ee;
    ap_interval_t *io, *ip;
    o  = random_oct(dim,.2);
    p  = poly_of_oct(o);
    e  = random_linexpr(mode,dim);
    ee = random_from_linexpr(e);
    io = oct_bound_linexpr(mo,o,e); FLAG(mo);
    ip = ap_abstract0_bound_linexpr(mp,p,ee); FLAG(mp);
    if (ap_interval_cmp(ip,io)==0) RESULT('*');
    else if (ap_interval_cmp(ip,io)==-1) RESULT('.');
    else ERROR("not included in");
    if (flag==exact && ap_interval_cmp(ip,io)!=0) ERROR("exact flag");
    oct_free(mo,o); ap_abstract0_free(mp,p);
    ap_linexpr0_free(e); ap_linexpr0_free(ee);
    ap_interval_free(io); ap_interval_free(ip);
   } ENDLOOP;
}


/* ********************************* */
/*               meet                */
/* ********************************* */

void test_meet(void)
{
  printf("\nmeet (* expected)\n");
  LOOP {
    int dim = 7;
    oct_t *o1, *o2, *o;
    ap_abstract0_t *p1, *p2, *p, *pp;
    o1 = random_oct(dim,.2);
    o2 = random_oct(dim,.2);
    o  = oct_meet(mo,false,o1,o2); FLAG(mo);
    p1 = poly_of_oct(o1);
    p2 = poly_of_oct(o2);
    p  = ap_abstract0_meet(mp,false,p1,p2); FLAG(mp);
    pp = poly_of_oct(o);
    RESULT(check(o)); check(o1); check(o2);
    if (oct_is_leq(mo,o,o1)==tbool_false || oct_is_leq(mo,o,o2)==tbool_false) {
      ERROR("not lower bound");
      print_oct("o1",o1); print_oct("o2",o2); print_oct("o",o);
      print_poly("p",p);
    }
    if (ap_abstract0_is_leq(mp,p,pp)==tbool_false) {
      ERROR("not poly approx");
      print_oct("o1",o1); print_oct("o2",o2); print_oct("o",o);
      print_poly("p",p);
    }
    if (ap_abstract0_is_eq(mp,p,pp)==tbool_true) RESULT('*');
    if (flag==exact && ap_abstract0_is_eq(mp,p,pp)==tbool_false)
      ERROR("exact flag");
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2);
    ap_abstract0_free(mp,p); ap_abstract0_free(mp,pp); ap_abstract0_free(mp,p1); ap_abstract0_free(mp,p2);
  } ENDLOOP;
  printf("\nmeet top (* expected)\n");
  LOOP {
    int dim = 8;
    oct_t *o1, *o2, *o, *oo;
    o1 = random_oct(dim,.2);
    o2 = oct_top(mo,0,dim);
    o  = oct_meet(mo,false,o1,o2);
    oo = oct_meet(mo,false,o,o1);
    check(o1); check(o2); check(o);
    if (oct_is_eq(mo,o,o1)!=tbool_true) {
      ERROR("not eq #1");
      print_oct("o1",o1); print_oct("o",o);
    }
    else if (oct_is_eq(mo,o,oo)!=tbool_true) {
      ERROR("not eq #2");
      print_oct("o1",o1); print_oct("o",o); print_oct("oo",oo);
    }
    else RESULT('*');
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,oo);
  } ENDLOOP;
  printf("\nmeet bot (* expected)\n");
  LOOP {
    int dim = 8;
    oct_t *o1, *o2, *o;
    o1 = random_oct(dim,.2);
    o2 = oct_bottom(mo,0,dim);
    o  = oct_meet(mo,false,o1,o2);
    check(o1); check(o2); check(o);
    if (oct_is_bottom(mo,o)!=tbool_true) {
      ERROR("not bottom");
      print_oct("o1",o1); print_oct("o",o);
    }
    else RESULT('*');
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2);
  } ENDLOOP;
}

#define NB_MEET 5
void test_meet_array(void)
{
  printf("\nmeet array (* expected)\n");
  LOOP {
    int i, dim = 6;
    oct_t* o[NB_MEET], *oo;
    ap_abstract0_t* p[NB_MEET], *pp, *ppp;
    for (i=0;i<NB_MEET;i++) 
      { o[i] = random_oct(dim,.2); p[i] = poly_of_oct(o[i]); }
    oo = oct_meet_array(mo,o,NB_MEET); FLAG(mo);
    pp = poly_of_oct(oo);
    ppp = ap_abstract0_meet_array(mp,(const ap_abstract0_t**)p,NB_MEET); 
    FLAG(mp);
    RESULT(check(oo));
    for (i=0;i<NB_MEET;i++)
      if (oct_is_leq(mo,oo,o[i])==tbool_false) ERROR("not lower bound");
    if (ap_abstract0_is_leq(mp,ppp,pp)==tbool_false) ERROR("not poly approx");
    if (ap_abstract0_is_eq(mp,ppp,pp)==tbool_true) RESULT('*');
    if (flag==exact && ap_abstract0_is_eq(mp,ppp,pp)==tbool_false)
      ERROR("exact flag");
    for (i=0;i<NB_MEET;i++) { oct_free(mo,o[i]); ap_abstract0_free(mp,p[i]); }
    oct_free(mo,oo); ap_abstract0_free(mp,pp); ap_abstract0_free(mp,ppp);
  } ENDLOOP;
}

void test_add_lincons(exprmode mode)
{
  printf("\nadd %slincons (%s expected)\n",exprname[mode],
	 (mode<=expr_oct)?"*":"*,x,.");
  LOOP {
    size_t i, dim = 7, nb = 4;
    oct_t *o, *o1, *o2;
    ap_abstract0_t *p, *p1, *p2;
    ap_lincons0_array_t ar = ap_lincons0_array_make(nb);
    ap_lincons0_array_t arr = ap_lincons0_array_make(nb);
    o = random_oct(dim,.2);
    p = poly_of_oct(o);
     if (lrand48()%10>=8) oct_close(pr,o);
    for (i=0;i<nb;i++) {
      ar.p[i] = ap_lincons0_make((lrand48()%100>=90)?AP_CONS_EQ:
				 (lrand48()%100>=90)?AP_CONS_SUP:
				 AP_CONS_SUPEQ,
				 random_linexpr(mode,dim),
				 NULL);
      arr.p[i] = random_from_lincons(ar.p[i]);
    }
    o1 = oct_meet_lincons_array(mo,false,o,&ar); FLAG(mo);
    p1 = ap_abstract0_meet_lincons_array(mp,false,p,&arr); FLAG(mp);
    p2 = poly_of_oct(o1);
    o2 = oct_of_poly(p1);
    check(o); RESULT(check(o1));
    if (ap_abstract0_is_leq(mp,p1,p2)==tbool_false) {
      ERROR("not included in");
      ap_lincons0_array_fprint(stderr,&ar,NULL);
      print_poly("p",p); print_poly("p1",p1); print_poly("p2",p2);
    }
    if (ap_abstract0_is_eq(mp,p1,p2)==tbool_true) RESULT('*');
    else if (oct_is_eq(mo,o1,o2)==tbool_true) RESULT('x');
    if (flag==best && ap_abstract0_is_eq(mp,p1,p2)==tbool_false) 
      ERROR("best flag");
    if (flag==exact && oct_is_eq(mo,o1,o2)==tbool_false) 
      ERROR("exact flag");
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2);
    ap_abstract0_free(mp,p); ap_abstract0_free(mp,p1); ap_abstract0_free(mp,p2);
    ap_lincons0_array_clear(&ar); ap_lincons0_array_clear(&arr);
   } ENDLOOP;
}


/* ********************************* */
/*          saturation               */
/* ********************************* */

void test_sat_lincons(exprmode mode)
{
  printf("\nsaturate %slincons (%s expected)\n",exprname[mode],
	 (mode<=expr_oct)?"*":"*,.");
  LOOP {
    size_t dim = 7;
    oct_t *o, *o1;
    ap_abstract0_t* p;
    ap_lincons0_t l, ll;
    ap_lincons0_array_t ar = ap_lincons0_array_make(1);
    tbool_t ro,ro1,rp;
    o = random_oct(dim,.1);
    p = poly_of_oct(o);
    l = ap_lincons0_make((lrand48()%100>=90)?AP_CONS_EQ: AP_CONS_SUPEQ,
			 random_linexpr(mode,dim),
			 NULL);
    ll = random_from_lincons(l);
    ar.p[0] = l;
    o1 = oct_meet_lincons_array(mo,false,o,&ar); FLAG(mo);
    RESULT(check(o1));
    ro  = oct_sat_lincons(mo,o,&l); FLAG(mo);
    ro1 = oct_sat_lincons(mo,o1,&l); FLAG(mo);
    rp  = ap_abstract0_sat_lincons(mp,p,&ll); FLAG(mp);   
    if (ro1==tbool_false) {
      ERROR("not sat");
      ap_lincons0_fprint(stderr,&l,NULL);
      fprintf(stderr,"\n");
      print_oct("o1",o1);
    }
    else if ((ro==tbool_true  && rp==tbool_false) ||
	     (ro==tbool_false && rp==tbool_true) ||
	     (ro!=tbool_top   && rp==tbool_top)) {
      ERROR("sat oct =/=> sat poly");
      ap_lincons0_fprint(stderr,&l,NULL);
      fprintf(stderr,"\nsat oct = %s\nsat poly = %s\n",
	      ro==tbool_true ? "T" : ro==tbool_false ? "F" : "?",
	      rp==tbool_true ? "T" : rp==tbool_false ? "F" : "?");
      print_oct("o",o);
    }
    else if (ro==rp && ro==tbool_top) RESULT('?');
    else if (ro==rp) RESULT('*');
    if (flag==exact && ro!=rp) ERROR("exact flag");
    oct_free(mo,o); oct_free(mo,o1); ap_abstract0_free(mp,p);
    ap_lincons0_array_clear(&ar); ap_lincons0_clear(&ll);
  } ENDLOOP;
}



/* ********************************* */
/*               join                */
/* ********************************* */

void test_join(void)
{
  printf("\njoin (*,x expected)\n");
  LOOP {
    int dim = 5;
    oct_t *o1, *o2, *o3, *o;
    ap_abstract0_t *p1, *p2, *p, *pp;
    o1 = random_oct(dim,.1);
    o2 = random_oct(dim,.1);
    o  = oct_join(mo,false,o1,o2); FLAG(mo);
    p1 = poly_of_oct(o1);
    p2 = poly_of_oct(o2);
    p  = ap_abstract0_join(mp,false,p1,p2); FLAG(mp);
    pp = poly_of_oct(o);
    o3 = oct_of_poly(p);
    RESULT(check(o)); check(o1); check(o2);
    if (oct_is_leq(mo,o1,o)==tbool_false || oct_is_leq(mo,o2,o)==tbool_false) {
      ERROR("not upper bound");
      print_oct("o1",o1); print_oct("o2",o2); print_oct("o",o);
      print_poly("p",p);
    }
    if (ap_abstract0_is_leq(mp,p,pp)==tbool_false) {
      ERROR("not poly approx");
      print_oct("o1",o1); print_oct("o2",o2); print_oct("o",o);
      print_poly("p1",p1); print_poly("p2",p2); print_poly("p",p);
    }
    if (ap_abstract0_is_eq(mp,p,pp)==tbool_true) RESULT('*');
    else if (oct_is_eq(mo,o,o3)==tbool_true) RESULT('x');
    if (flag==exact && ap_abstract0_is_eq(mp,p,pp)==tbool_false) 
      ERROR("exact flag");
    if (flag==best && oct_is_eq(mo,o,o3)==tbool_false)
      ERROR("best flag");
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,o3);
    ap_abstract0_free(mp,p); ap_abstract0_free(mp,pp); ap_abstract0_free(mp,p1); ap_abstract0_free(mp,p2);
  } ENDLOOP;
  printf("\njoin bot (* expected)\n");
  LOOP {
    int dim = 8;
    oct_t *o1, *o2, *o, *oo;
    o1 = random_oct(dim,.2);
    o2 = oct_bottom(mo,0,dim);
    o  = oct_join(mo,false,o1,o2);
    oo = oct_join(mo,false,o,o1);
    RESULT(check(o1)); check(o2); check(o);
    if (oct_is_eq(mo,o,o1)!=tbool_true) {
      ERROR("not eq #1");
      print_oct("o1",o1); print_oct("o",o);
    }
    else if (oct_is_eq(mo,o,oo)!=tbool_true) {
      ERROR("not eq #2");
      print_oct("o1",o1); print_oct("o",o); print_oct("oo",oo);
    }
    else RESULT('*');
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,oo);
  } ENDLOOP;
  printf("\njoin top (* expected)\n");
  LOOP {
    int dim = 8;
    oct_t *o1, *o2, *o;
    o1 = random_oct(dim,.2);
    o2 = oct_top(mo,0,dim);
    o  = oct_join(mo,false,o1,o2);
    RESULT(check(o1)); check(o2); check(o);
    if (oct_is_top(mo,o)!=tbool_true) {
      ERROR("not top");
      print_oct("o1",o1); print_oct("o",o);
    }
    else RESULT('*');
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2);
  } ENDLOOP;
}

#define NB_JOIN 5
void test_join_array(void)
{
  printf("\njoin array (x,* expected)\n");
  LOOP {
    int i, dim = 6;
    oct_t* o[NB_JOIN], *oo, *ooo;
    ap_abstract0_t* p[NB_JOIN], *pp, *ppp, *ps;
    for (i=0;i<NB_JOIN;i++) 
      { o[i] = random_oct(dim,.1); p[i] = poly_of_oct(o[i]); }
    oo = oct_join_array(mo,o,NB_JOIN); FLAG(mo);
    pp = poly_of_oct(oo);
    ppp = ap_abstract0_join_array(mp,(const ap_abstract0_t**)p,NB_JOIN); 
    FLAG(mp);
    ps = ap_abstract0_join(mp,false,p[0],p[1]);
    for (i=2;i<NB_JOIN;i++) ps = ap_abstract0_join(mp,true,ps,p[i]);
    ooo = oct_of_poly(ps);
    RESULT(check(oo));
    for (i=0;i<NB_JOIN;i++)
      if (oct_is_leq(mo,o[i],oo)==tbool_false) ERROR("not upper bound");
    if (ap_abstract0_is_leq(mp,ppp,pp)==tbool_false) {
      ERROR("not poly approx");
      if (ap_abstract0_is_eq(mp,ppp,ps)!=tbool_true) fprintf(stderr,"BLHA\n");
       for (i=0;i<NB_JOIN;i++) 
	{ char n[3] = { 'o', '0'+i, 0 }; print_oct(n,o[i]); }
      for (i=0;i<NB_JOIN;i++) 
	{ char n[3] = { 'p', '0'+i, 0 }; print_poly(n,p[i]); }
      print_poly("pp",pp);
      print_poly("ppp",ppp);
    }
    if (ap_abstract0_is_eq(mp,ppp,pp)==tbool_true) RESULT('*');
    else if (oct_is_eq(mo,oo,ooo)==tbool_true) RESULT('x');
    if (flag==exact && ap_abstract0_is_eq(mp,ppp,pp)==tbool_false) 
      ERROR("exact flag");
    if (flag==best && oct_is_eq(mo,oo,ooo)==tbool_false)
      ERROR("best flag");
    if (ap_abstract0_is_eq(mp,ppp,ps)!=tbool_true) {
      ERROR("poly_join_array not equivalent to poly_join");
      for (i=0;i<NB_JOIN;i++) 
	{ char n[3] = { 'p', '0'+i, 0 }; print_poly(n,p[i]); }
      print_poly("ppp",ppp);
      print_poly("ps",ps);
    }
    for (i=0;i<NB_JOIN;i++) { oct_free(mo,o[i]); ap_abstract0_free(mp,p[i]); }
    oct_free(mo,oo); oct_free(mo,ooo);
    ap_abstract0_free(mp,pp); ap_abstract0_free(mp,ppp); ap_abstract0_free(mp,ps);
  } ENDLOOP;
}

void test_add_ray(void)
{
  printf("\nadd rays (*,x expected)\n");
  LOOP {
    size_t i, dim = 7, nb = 4;
    oct_t *o, *o1, *o2;
    ap_abstract0_t *p, *p1, *p2;
    ap_generator0_array_t ar = ap_generator0_array_make(nb);
    o = random_oct(dim,.1);
    p = poly_of_oct(o);
    if (lrand48()%10>=8) oct_close(pr,o);
    for (i=0;i<nb;i++)
      ar.p[i] = random_generator(dim,
				 (lrand48()%100>=80)?AP_GEN_LINE:AP_GEN_RAY);
    o1 = oct_add_ray_array(mo,false,o,&ar); FLAG(mo);
    p1 = ap_abstract0_add_ray_array(mp,false,p,&ar); FLAG(mp);
    p2 = poly_of_oct(o1);
    o2 = oct_of_poly(p1);
    check(o); RESULT(check(o1));
    if (ap_abstract0_is_leq(mp,p1,p2)==tbool_false) {
      ERROR("not included in");
      ap_generator0_array_fprint(stderr,&ar,NULL);
      print_poly("p",p); print_poly("p1",p1); print_poly("p2",p2);
    }
    if (ap_abstract0_is_eq(mp,p1,p2)==tbool_true) RESULT('*');
    else if (oct_is_eq(mo,o1,o2)==tbool_true) RESULT('x');
    if (flag==exact && ap_abstract0_is_eq(mp,p1,p2)==tbool_false)
      ERROR("exact flag");
    if (flag==best && oct_is_eq(mo,o1,o2)==tbool_false)
      ERROR("best flag");
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2); 
    ap_abstract0_free(mp,p); ap_abstract0_free(mp,p1); ap_abstract0_free(mp,p2);
    ap_generator0_array_clear(&ar);
   } ENDLOOP;
}


/* ********************************* */
/*     dimension manipulation        */
/* ********************************* */

void test_dimadd(void)
{
  printf("\nadd dimensions\n");
  LOOP {
    size_t i, dim = 15;
    oct_t *o1, *o2, *o3;
    ap_dimchange_t* a = ap_dimchange_alloc(0,3);
    ap_dimchange_t* r = ap_dimchange_alloc(0,a->realdim);
    o1 = random_oct(dim,.1);
    if (lrand48()%10>=8) oct_close(pr,o1);
    for (i=0;i<a->realdim;i++) {
      a->dim[i] = lrand48()%3;
      if (i) a->dim[i] += a->dim[i-1];
      r->dim[i] = a->dim[i]+i;
    }
    o2 = oct_add_dimensions(mo,false,o1,a,true);
    o3 = oct_remove_dimensions(mo,false,o2,r);
    RESULT(check(o1)); check(o2); check(o3);
    if (oct_is_eq(mo,o1,o3)!=tbool_true) {
      ERROR("not eq");
      ap_dimchange_fprint(stderr,a); ap_dimchange_fprint(stderr,r);
      print_oct("o1",o1); print_oct("o2",o2);print_oct("o3",o3);
    }
    oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,o3);
    ap_dimchange_free(a); ap_dimchange_free(r);
  } ENDLOOP;
}

void test_dimrem(void)
{
  printf("\nremove dimensions, forget\n");
  LOOP {
    size_t i, dim = 15;
    oct_t *o1, *o2, *o3 ,*o4;
    ap_dimchange_t* a = ap_dimchange_alloc(0,3);
    ap_dimchange_t* r = ap_dimchange_alloc(0,a->realdim);
    o1 = random_oct(dim,.1);
    if (lrand48()%10>=8) oct_close(pr,o1);
    for (i=0;i<r->realdim;i++) {
      r->dim[i] = lrand48()%3 + 1;
      if (i) r->dim[i] += r->dim[i-1];
      a->dim[i] = r->dim[i]-i;
    }
    o2 = oct_remove_dimensions(mo,false,o1,r);
    o3 = oct_add_dimensions(mo,false,o2,a,false);
    o4 = oct_forget_array(mo,false,o1,r->dim,r->realdim,false);
    RESULT(check(o1)); check(o2); check(o3); check(o4);
    if (oct_is_eq(mo,o3,o4)!=tbool_true) {
      ERROR("not eq");
      ap_dimchange_fprint(stderr,r); ap_dimchange_fprint(stderr,a);
      print_oct("o1",o1); print_oct("o2",o2);
      print_oct("o3",o3); print_oct("o4",o4);
    }

    oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,o3); oct_free(mo,o4);
    ap_dimchange_free(a); ap_dimchange_free(r);
  } ENDLOOP;
}


void test_permute(void)
{
  printf("\npermute dimensions\n");
  LOOP {
    size_t i, dim = 14;
    oct_t *o1, *o2, *o3;
    ap_dimperm_t* p = ap_dimperm_alloc(dim);
    ap_dimperm_t* q = ap_dimperm_alloc(dim);
    o1 = random_oct(dim,.1);
    if (lrand48()%10>=8) oct_close(pr,o1);

    /* random permutation */
    ap_dimperm_set_id(p);
    for (i=0;i<p->size-1;i++) {
      int j = i + (lrand48() % (p->size-i));
      int a = p->dim[j]; p->dim[j] = p->dim[i]; p->dim[i] = a;
    }
    /* inverse permutation */
    for (i=0;i<p->size;i++) q->dim[p->dim[i]] = i;

    o2 = oct_permute_dimensions(mo,false,o1,p);
    o3 = oct_permute_dimensions(mo,false,o2,q);
    RESULT(check(o1)); check(o2); check(o3);
    if (oct_is_eq(mo,o1,o3)!=tbool_true) {
      ERROR("not eq");
      ap_dimperm_fprint(stderr,p); ap_dimperm_fprint(stderr,q);
      print_oct("o1",o1); print_oct("o2",o2); print_oct("o3",o3);
    }
    oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,o3);
    ap_dimperm_free(p); ap_dimperm_free(q);
  } ENDLOOP;
}

void test_expand(void)
{
  printf("\nexpand dimensions\n");
  LOOP {
    size_t dim = 5;
    ap_dim_t d = lrand48() % dim;
    ap_dim_t dd[] = { d, dim, dim+1 };
    size_t n = (lrand48() % 2) + 1;
    oct_t *o1, *o2, *o3;
    ap_abstract0_t *p1, *p2, *p3;
    o1 = random_oct(dim,.1);
    o2 = oct_expand(mo,false,o1,d,n); FLAG(mo);
    o3 = oct_fold(mo,false,o2,dd,n+1); FLAG(mo);
    p1 = poly_of_oct(o1);
    p2 = ap_abstract0_expand(mp,false,p1,d,n); FLAG(mp);
    p3 = poly_of_oct(o2);
    RESULT(check(o1)); check(o2); check(o3); check(o3);
    if (oct_is_eq(mo,o1,o3)!=tbool_true) {
      ERROR("not eq");
      fprintf(stderr,"dim %i expanded %i times\n",(int)d,(int)n);
      print_oct("o1",o1); print_oct("o2",o2); print_oct("o3",o3);
    }
    if (ap_abstract0_is_leq(mp,p2,p3)==tbool_false) {
      ERROR("not leq");
      fprintf(stderr,"dim %i expanded %i times\n",(int)d,(int)n);
      print_oct("o1",o1); print_oct("o2",o2); 
      print_poly("p1",p1); print_poly("p2",p2); print_poly("p3",p3);
    }
    if (ap_abstract0_is_eq(mp,p2,p3)==tbool_true) RESULT('*');
    if (flag==exact && ap_abstract0_is_eq(mp,p2,p3)==tbool_false)
      ERROR("exact flag");
    oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,o3);
    ap_abstract0_free(mp,p1); ap_abstract0_free(mp,p2); ap_abstract0_free(mp,p3);
  } ENDLOOP;
}

void test_fold(void)
{
  printf("\nfold dimensions\n");
  LOOP {
    size_t dim = 6, i;
    ap_dim_t dd[3];
    oct_t *o1, *o2;
    ap_abstract0_t *p1, *p2, *p3;
    o1 = random_oct(dim,.1);
    p1 = poly_of_oct(o1);
    dd[0] = lrand48() % (dim-3);
    dd[1] = dd[0] + 1 + (lrand48() % (dim-2-dd[0]));
    dd[2] = dd[1] + 1 + (lrand48() % (dim-1-dd[1]));
    o2 = oct_fold(mo,false,o1,dd,3); FLAG(mo);
    p2 = ap_abstract0_fold(mp,false,p1,dd,3); FLAG(mp);
    p3 = poly_of_oct(o2);
    RESULT(check(o1)); check(o2);
    if (ap_abstract0_is_leq(mp,p2,p3)==tbool_false) {
      ERROR("not leq");
      fprintf(stderr,"fold %i,%i,%i\n",(int)dd[0],(int)dd[1],(int)dd[2]);
      print_oct("o1",o1); print_oct("o2",o2); 
      print_poly("p1",p1); print_poly("p2",p2); print_poly("p3",p3);
    }
    if (ap_abstract0_is_eq(mp,p2,p3)==tbool_true) RESULT('*');
    if (flag==exact && ap_abstract0_is_eq(mp,p2,p3)==tbool_false)
      ERROR("exact flag");
    oct_free(mo,o1); oct_free(mo,o2);
    ap_abstract0_free(mp,p1); ap_abstract0_free(mp,p2); ap_abstract0_free(mp,p3);
  } ENDLOOP;
}


/* ********************************* */
/*             widening              */
/* ********************************* */

void test_widening(void)
{
  printf("\nwidening\n");
  LOOP {
    int dim = 8;
    oct_t *o1, *o2, *o;
    o1 = random_oct(dim,.1);
    o2 = random_oct(dim,.1);
    o  = oct_widening(mo,o1,o2);
    RESULT(check(o));
    if (oct_is_leq(mo,o1,o)==tbool_false || oct_is_leq(mo,o2,o)==tbool_false) {
      ERROR("not upper bound");
      print_oct("o1",o1); print_oct("o2",o2); print_oct("o",o);
    }
    oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,o);
  } ENDLOOP;
  printf("\nwidening convergence\n");
  LOOP {
    int dim = 5, nb = 4*dim*dim;
    oct_t *o1 = random_oct(dim,.1);
    for (;nb>0;nb--) {
      oct_t* o2 = random_oct(dim,.1);
      oct_t* o = oct_widening(mo,o1,o2);
      oct_free(mo,o2);
      if (oct_is_leq(mo,o,o1)) { oct_free(mo,o); break; }
      oct_free(mo,o1); o1 = o;
    }
    RESULT(check(o1));
    oct_free(mo,o1);
    if (!nb) ERROR("not converging in reasonable time");
  } ENDLOOP;
}

void test_widening_thrs(void)
{
  printf("\nwidening with thresholds\n");
  LOOP {
    int dim = 8, n;
    oct_t *o1, *o2, *o;
    o1 = random_oct(dim,.1);
    o2 = random_oct(dim,.1);
    ap_scalar_t* t[10];
    for (n=0;n<10;n++) t[n]=ap_scalar_alloc_set_double((lrand48()%30-15)*0.25);
    o = oct_widening_thresholds(mo,o1,o2,(const ap_scalar_t**)t,10);
    RESULT(check(o));
    if (oct_is_leq(mo,o1,o)==tbool_false || oct_is_leq(mo,o2,o)==tbool_false) {
      ERROR("not upper bound");
      print_oct("o1",o1); print_oct("o2",o2); print_oct("o",o);
    }
    oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,o);
    for (n=0;n<10;n++) ap_scalar_free(t[n]);
  } ENDLOOP;
  printf("\nwidening with thresholds convergence\n");
  LOOP {
    int dim = 5, nb = 4*dim*dim*10, n;
    oct_t *o1 = random_oct(dim,.1);
    ap_scalar_t* t[10];
    for (n=0;n<10;n++) t[n]=ap_scalar_alloc_set_double((lrand48()%30-15)*0.25);
    for (;nb>0;nb--) {
      oct_t* o2 = random_oct(dim,.1);
      oct_t* o = oct_widening_thresholds(mo,o1,o2,(const ap_scalar_t**)t,10);
      oct_free(mo,o2);
      if (oct_is_leq(mo,o,o1)) { oct_free(mo,o); break; }
      oct_free(mo,o1); o1 = o;
    }
    RESULT(check(o1));
    oct_free(mo,o1);
    for (n=0;n<10;n++) ap_scalar_free(t[n]);
    if (!nb) ERROR("not converging in reasonable time");
  } ENDLOOP;
}


/* ********************************* */
/*            narrowing              */
/* ********************************* */

void test_narrowing(void)
{
  printf("\nnarrowing\n");
  LOOP {
    int dim = 8;
    oct_t *o1, *o2, *o, *oo;
    o1 = random_oct(dim,.1);
    o2 = random_oct(dim,.1);
    o  = oct_narrowing(mo,o1,o2);
    oo = oct_meet(mo,false,o1,o2);
    RESULT(check(o));
    if (oct_is_leq(mo,o,o1)==tbool_false || oct_is_leq(mo,oo,o)==tbool_false) {
      ERROR("not decreasing");
      print_oct("o1",o1); print_oct("o2",o2); 
      print_oct("o",o); print_oct("oo",oo);
    }
    oct_free(mo,o1); oct_free(mo,o2); oct_free(mo,o); oct_free(mo,oo);
  } ENDLOOP;
  printf("\nwidening narrowing\n");
  LOOP {
    int dim = 5, nb = 4*dim*dim;
    oct_t *o1 = random_oct(dim,.1);
    for (;nb>0;nb--) {
      oct_t* o2 = random_oct(dim,.1);
      oct_t* o = oct_narrowing(mo,o1,o2);
      oct_free(mo,o2);
      if (oct_is_leq(mo,o,o1)) { oct_free(mo,o); break; }
      oct_free(mo,o1); o1 = o;
    }
    RESULT(check(o1));
    oct_free(mo,o1);
    if (!nb) ERROR("not converging in reasonable time");
  } ENDLOOP;
}


/* ********************************* */
/*     assignments / substitutions   */
/* ********************************* */

void test_assign(int subst, exprmode mode)
{
  printf("\n%s %slinexpr%s\n", subst ? "subst" : "assign", exprname[mode],
	 mode==expr_unary ? " (* expected)" : "");
  LOOP {
    size_t dim = 6, d = lrand48()%dim;
    oct_t *o, *o1, *o2;
    ap_abstract0_t *p, *p1, *p2;
    ap_linexpr0_t* l = random_linexpr(mode,dim);
    ap_linexpr0_t* ll = random_from_linexpr(l);
    o = random_oct(dim,.1);
    p = poly_of_oct(o);
    if (lrand48()%10>=8) oct_close(pr,o);
    o1 = subst ? oct_substitute_linexpr(mo,false,o,d,l,NULL) : oct_assign_linexpr(mo,false,o,d,l,NULL);
    FLAG(mo);
    p1 = subst ? ap_abstract0_substitute_linexpr(mp,false,p,d,ll,NULL) : ap_abstract0_assign_linexpr(mp,false,p,d,ll,NULL);;
    FLAG(mp);
    p2 = poly_of_oct(o1);
    o2 = oct_of_poly(p1);
    RESULT(check(o1));
    if (ap_abstract0_is_leq(mp,p1,p2)!=tbool_true) {
      ERROR("not included in");
      fprintf(stderr,"x%i %s ",(int)d,subst?"->":"<-");
      ap_linexpr0_fprint(stderr,l,NULL);
      fprintf(stderr," / ");
      ap_linexpr0_fprint(stderr,ll,NULL);
      fprintf(stderr,"\n");
      print_poly("p",p);
      print_poly("p1",p1);
      print_poly("p2",p2);
    }
    if (ap_abstract0_is_eq(mp,p1,p2)==tbool_true) RESULT('*');
    else if (oct_is_eq(mo,o1,o2)==tbool_true) RESULT('x');
    if (flag==exact && ap_abstract0_is_eq(mp,p1,p2)==tbool_false)
      ERROR("exact flag");
    if (flag==best && oct_is_eq(mo,o1,o2)==tbool_false)
      ERROR("best flag");
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2); 
    ap_abstract0_free(mp,p); ap_abstract0_free(mp,p1); ap_abstract0_free(mp,p2);
    ap_linexpr0_free(l); ap_linexpr0_free(ll);
  } ENDLOOP;
}

#define NB_ASSIGN 3
void test_par_assign(int subst, exprmode mode)
{
  printf("\nparallel %s %slinexpr%s\n",subst ? "subst" : "assign",
	 exprname[mode],mode==expr_unary ? " (* expected)" : "");
  LOOP {
    size_t dim = 6, i, j, k;
    ap_dim_t d[NB_ASSIGN];
    oct_t *o, *o1, *o2;
    ap_abstract0_t *p, *p1, *p2;
    ap_linexpr0_t *l[NB_ASSIGN], *ll[NB_ASSIGN];
    o = random_oct(dim,.1);
    p = poly_of_oct(o);
    for (i=0;i<NB_ASSIGN;i++) {
      l[i]  = random_linexpr(mode,dim);
      ll[i] = random_from_linexpr(l[i]);
      if (!i) d[i] = lrand48()%(dim-NB_ASSIGN);
      else d[i] = d[i-1] + 1 + (lrand48()%(dim-NB_ASSIGN+i-d[i-1]));
    }
    if (lrand48()%10>=8) oct_close(pr,o);
    o1 = subst ? 
      oct_substitute_linexpr_array(mo,false,o,d,(const ap_linexpr0_t**)l,NB_ASSIGN,NULL) : 
      oct_assign_linexpr_array(mo,false,o,d,(const ap_linexpr0_t**)l,NB_ASSIGN,NULL);
    FLAG(mo);
    p1 = subst ? 
      ap_abstract0_substitute_linexpr_array(mp,false,p,d,(const ap_linexpr0_t**)ll,NB_ASSIGN,NULL) : 
      ap_abstract0_assign_linexpr_array(mp,false,p,d,(const ap_linexpr0_t**)ll,NB_ASSIGN,NULL);
    FLAG(mp);
    p2 = poly_of_oct(o1);
    o2 = oct_of_poly(p1);
    RESULT(check(o1));
    if (ap_abstract0_is_leq(mp,p1,p2)!=tbool_true) {
      ERROR("not included in");
      for (i=0;i<NB_ASSIGN;i++) {
	fprintf(stderr,"x%i %s ",(int)d[i],subst?"->":"<-");
	ap_linexpr0_fprint(stderr,l[i],NULL);
	fprintf(stderr," / ");
	ap_linexpr0_fprint(stderr,ll[i],NULL);
	fprintf(stderr,"\n");
      }
      print_poly("p",p);
      print_poly("p1",p1);
      print_poly("p2",p2);
    }
    else if (ap_abstract0_is_eq(mp,p1,p2)==tbool_true) RESULT('*');
    else if (oct_is_eq(mo,o1,o2)==tbool_true) RESULT('x');
    if (flag==exact && ap_abstract0_is_eq(mp,p1,p2)==tbool_false)
      ERROR("exact flag");
    if (flag==best && oct_is_eq(mo,o1,o2)==tbool_false)
      ERROR("best flag");
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2); 
    ap_abstract0_free(mp,p); ap_abstract0_free(mp,p1); ap_abstract0_free(mp,p2);
    for (i=0;i<NB_ASSIGN;i++) 
      { ap_linexpr0_free(l[i]); ap_linexpr0_free(ll[i]); }
  } ENDLOOP;
}

void test_par_assign2(int subst, exprmode mode)
{
  printf("\nparallel vs non-parallel %s %slinexpr\n",
	 subst ? "subst" : "assign",exprname[mode]);
  LOOP {
    size_t dim = 6;
    ap_dim_t d = lrand48()%dim;
    oct_t *o, *o1, *o2;
    ap_linexpr0_t* l = random_linexpr(mode,dim);
    o = random_oct(dim,.1);
    if (lrand48()%10>=8) oct_close(pr,o);
    o1 = subst ? oct_substitute_linexpr(mo,false,o,d,l,NULL) : oct_assign_linexpr(mo,false,o,d,l,NULL);
    FLAG(mo);
    o2 = subst ? 
      oct_substitute_linexpr_array(mo,false,o,&d,(const ap_linexpr0_t**)&l,1,NULL) :
      oct_assign_linexpr_array(mo,false,o,&d,(const ap_linexpr0_t**)&l,1,NULL);
    FLAG(mo);
    check(o1); check(o2);
    if (oct_is_eq(mo,o1,o2)==tbool_true) RESULT('=');
    else if (oct_is_leq(mo,o1,o2)==tbool_true) RESULT('<');
    else if (oct_is_leq(mo,o2,o1)==tbool_true) RESULT('>');
    else RESULT('.');
    if (flag==exact && oct_is_eq(mo,o1,o2)==tbool_false) ERROR("exact flag");
    oct_free(mo,o); oct_free(mo,o1); oct_free(mo,o2);
    ap_linexpr0_free(l);
  } ENDLOOP;
}


void tests(int algo)
{
  int i;
  for (i=0;i<AP_FUNID_SIZE;i++) {
    mo->option.funopt[i].algorithm = algo;
    mp->option.funopt[i].algorithm = algo;
  }
  printf("\nstarting tests with algo=%i\n",algo);
  /* tests */
  test_misc();
  test_serialize();
  test_closure();
  test_incremental_closure();
  test_polyhedra_conversion();
  //test_polyhedra_conversion2(); /* poly_check: F not normalized */
  test_lincons_conversion(expr_oct);
  test_lincons_conversion(expr_lin);
  test_lincons_conversion(expr_interv);
  test_generator_conversion();
  test_box_conversion();
  test_bound_dim();
  test_bound_linexpr(expr_oct);
  test_bound_linexpr(expr_lin);
  test_bound_linexpr(expr_interv);
  test_meet();
  test_meet_array();
  test_join();
  test_join_array();
  test_add_ray();
  test_add_lincons(expr_oct);
  test_add_lincons(expr_lin);
  test_add_lincons(expr_interv);
  test_sat_lincons(expr_oct);
  test_sat_lincons(expr_lin);
  test_sat_lincons(expr_interv);
  test_dimadd();
  test_dimrem();
  test_permute();
  test_expand();
  test_fold();
  test_widening();
  test_widening_thrs();
  test_narrowing();
  test_assign(0,expr_unary);
  test_assign(0,expr_lin);
  test_assign(0,expr_interv);
  test_par_assign(0,expr_unary);
  test_par_assign(0,expr_lin);
  test_par_assign(0,expr_interv);
  test_par_assign2(0,expr_unary);
  test_par_assign2(0,expr_lin);
  test_par_assign2(0,expr_interv);
  test_assign(1,expr_unary);
  test_assign(1,expr_lin);
  test_assign(1,expr_interv);
  test_par_assign(1,expr_unary);
  test_par_assign(1,expr_lin);
  test_par_assign(1,expr_interv);
  test_par_assign2(1,expr_unary);
  test_par_assign2(1,expr_lin);
  test_par_assign2(1,expr_interv);
}

int main(int argc, const char** argv)
{
  long int seed;
  int i;

  seed = time(NULL);
  if (argc==2){
    seed = atol(argv[1]);
  }
  printf("seed = %ld\n",seed);
  /* init */
  srand48(seed);
  mo = oct_manager_alloc();
  mp = pk_manager_alloc(false);
  for (i=0;i<AP_FUNID_SIZE;i++) {
    mo->option.funopt[i].flag_exact_wanted = true;
    mo->option.funopt[i].flag_best_wanted = true;
    mp->option.funopt[i].flag_exact_wanted = true;
    mp->option.funopt[i].flag_best_wanted = true;
  }
  for (i=0;i<AP_EXC_SIZE;i++){
    mo->option.abort_if_exception[i] = true;
    mp->option.abort_if_exception[i] = true;
  }
  mo->option.scalar_discr = mp->option.scalar_discr = AP_SCALAR_MPQ;
  pr = oct_init_from_manager(mo,0,0);
  pk = pk_manager_get_internal(mp);
  pk_set_max_coeff_size(pk,0);
  pk_set_approximate_max_coeff_size(pk,0);
  info();

  tests(0);

  /* quit */
  ap_manager_free(mo);
  ap_manager_free(mp);
  if (error_) printf("\n%i error(s)!\n",error_);
  else printf("\nall tests passed\n");
  return 0;
}
