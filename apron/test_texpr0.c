/*
 * Testing expression tree support in APRON.
 *
 * Antoine Mine' 2007
 */

/* Compile with:
   gcc test_texpr0.c -g  -L. -lapron_debug -L../itv -litv -lgmp -lmpfr -lm
*/

#include <stdio.h>
#include <stdlib.h>

#include "ap_expr0.h"
#include "ap_abstract0.h"

#define V(i)   ap_texpr0_dim(i)
#define C(i)   ap_texpr0_cst_scalar_int(i)
#define I(a,b) ap_texpr0_cst_interval_double(a,b)
#define ADD(a,b) ap_texpr0_binop(AP_TEXPR_ADD,AP_RTYPE_REAL,AP_RDIR_EXACT,a,b)
#define NEG(a)   ap_texpr0_unop(AP_TEXPR_NEG,AP_RTYPE_REAL,AP_RDIR_EXACT,a)
#define SQRT(a)  ap_texpr0_unop(AP_TEXPR_SQRT,AP_RTYPE_REAL,AP_RDIR_EXACT,a)
#define MUL(a,b) ap_texpr0_binop(AP_TEXPR_MUL,AP_RTYPE_REAL,AP_RDIR_EXACT,a,b)
#define DIV(a,b) ap_texpr0_binop(AP_TEXPR_DIV,AP_RTYPE_REAL,AP_RDIR_EXACT,a,b)
#define ADDF(a,b) ap_texpr0_binop(AP_TEXPR_ADD,AP_RTYPE_SINGLE,AP_RDIR_NEAREST,a,b)

void print_info(ap_texpr0_t* a)
{
  int i, m = ap_texpr0_max_dim(a);
  ap_dim_t* dd = ap_texpr0_dimlist(a);
  ap_texpr0_fprint(stdout,a,NULL);
  /* sizes, hashes */
  printf("\ndepth=%i, size=%i, max_dim=%i, hash=%i, dims=[",
	 ap_texpr0_depth(a),ap_texpr0_size(a),m, (int)ap_texpr0_hash(a));
  /* dims */
  for (i=0;dd[i]!=AP_DIM_MAX;i++) printf("%s%i",i?",":"",dd[i]);
  printf("]\n");
  for (i=0;dd[i]!=AP_DIM_MAX;i++) assert(ap_texpr0_has_dim(a,dd[i]));
  assert(!i || dd[i-1]==m);
  assert(!i || !ap_texpr0_has_dim(a,dd[i-1]+1));
  /* classification */
  printf("%scst, %slinear, %spolynomial, %spolyfrac, %sscalar\n\n",
	 ap_texpr0_is_interval_cst(a)?"":"non-",
	 ap_texpr0_is_interval_linear(a)?"":"non-",
	 ap_texpr0_is_interval_polynomial(a)?"":"non-",
	 ap_texpr0_is_interval_polyfrac(a)?"":"non-",
	 ap_texpr0_is_scalar(a)?"":"non-");
  free(dd);
}

int main()
{
  /* creation, copy, comparison */
  ap_texpr0_t* x = ADD(V(0),MUL(NEG(ADD(V(4),I(1,2.5))),C(3)));
  ap_texpr0_t* y = ap_texpr0_copy(x);
  print_info(y);
  assert(ap_texpr0_equal(x,y));
  ap_texpr0_free(x);
  ap_texpr0_free(y);

  x = ADDF(V(1),MUL(NEG(ADD(V(4),I(1,1))),C(3)));
  y = ADD(V(1),MUL(NEG(ADD(V(4),I(1,1))),V(3)));
  print_info(x);
  print_info(y);
  assert(!ap_texpr0_equal(x,y));
  ap_texpr0_free(x);   
  ap_texpr0_free(y);   

  x = DIV(V(1),MUL(NEG(ADD(V(4),I(1,1))),C(3)));
  print_info(x);
  ap_texpr0_free(x);   

  /* from linexpr0 */
  ap_linexpr0_t* l = ap_linexpr0_alloc(AP_LINEXPR_SPARSE,12);
  ap_linexpr0_fprint(stdout,l,NULL); printf("\n");
  x = ap_texpr0_from_linexpr0(l);
  print_info(x);
  ap_texpr0_free(x);
  ap_linexpr0_set_list(l,AP_CST_S_INT,12,AP_COEFF_I_INT,1,2,42,AP_COEFF_I_INT,-1,1,3,AP_END);
  ap_linexpr0_fprint(stdout,l,NULL); printf("\n");
  x = ap_texpr0_from_linexpr0(l);
  print_info(x);
  ap_texpr0_free(x);
  ap_linexpr0_free(l);

  /* substitutions */
  x = DIV(V(1),MUL(SQRT(ADD(V(4),I(1,1))),C(3)));
  y = ADD(C(1),V(1));
  print_info(x);
  ap_texpr0_subst_with(x, 1, y);
  ap_texpr0_subst_with(x, 1, y);
  print_info(x);
  ap_texpr0_free(x);   
  ap_texpr0_free(y);

  /* dimchanges */
  ap_dim_t dt[] = { 0,1,2,2,4 };
  ap_dimchange_t d = { dt,3,1 };
  x = DIV(MUL(V(0),V(1)),MUL(NEG(ADD(V(2),C(3))),V(3)));
  print_info(x);
  ap_dimchange_fprint(stdout, &d);
  y = ap_texpr0_add_dimensions(x, &d);
  print_info(y);
  ap_texpr0_free(x);
  ap_dimchange_add_invert(&d);
  ap_dimchange_fprint(stdout, &d);
  x = ap_texpr0_remove_dimensions(y, &d);
  print_info(x);
  ap_texpr0_free(x);
  ap_texpr0_free(y);

  /* linearization */
  x = ADD(V(0),ADD(MUL(C(2),V(1)),C(3)));
  /* TODO */
  ap_texpr0_free(x);
}
