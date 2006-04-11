/* ********************************************************************** */
/* test_environment.c: testing of environment */
/* ********************************************************************** */

#include "dimension.h"
#include "expr0.h"
#include "environment.h"
#include "expr1.h"
#include "abstract1.h"
#include "pk.h"

void test_environment()
{
  size_t i;
  environment_t* e1;
  environment_t* e2;
  environment_t* e3;
  environment_t* e4;
  environment_t* e5;
  dimchange_t* change1;
  dimchange_t* change2;

  var_t name_of_intdim1[3] = {
    "a30","a10","a20"
  };
  var_t name_of_realdim1[3] = {
    "a11","a31","a21"
  };
  var_t name_of_intdim2[4] = {
    "a2","a0","a22","a24"
  };
  var_t name_of_realdim2[4] = {
    "a27","a23","a33","a37"
  };
  var_t name_of_intdim5[4] = {
    "a2","a10","a22","a20"
  };
  var_t name_of_realdim5[4] = {
    "a27","a31","a11","a37"
  };
  e1 = environment_make_empty();
  printf("env 1\n");
  environment_fdump(stdout,e1);
  environment_free(e1);
  e1 = environment_make(name_of_intdim1,3,
			name_of_realdim1,3);
  printf("env 1\n");
  environment_fdump(stdout,e1);
  e2 = environment_make(name_of_intdim2,4,
			name_of_realdim2,4);
  printf("env 2\n");
  environment_fdump(stdout,e2);
  e5 = environment_make(name_of_intdim5,4,
			name_of_realdim5,4);
  printf("env 5\n");
  environment_fdump(stdout,e5);
  
  e3 = environment_add(e1,name_of_intdim2,4,
			name_of_realdim2,4);
  printf("env 3\n");
  environment_fdump(stdout,e3);
  e4 = environment_add(e2,name_of_intdim1,3,
		       name_of_realdim1,3);
  printf("env 4\n");
  environment_fdump(stdout,e4);
  for(i=0; i<e4->intdim+e4->realdim; i++){
    var_t var = environment_var_of_dim(e4,i);
    dim_t dim = environment_dim_of_var(e4,var);
    char* name = environment_var_operations->to_string(var);
    printf("i=%d, var=%s, dim=%d\n",i,name,dim);
    free(name);
  }
  environment_free(e4);
  environment_free(e3);

  e4 = environment_remove(e5,name_of_intdim2,4,
			name_of_realdim2,4);
  assert(e4==NULL);

  e3 = environment_lce(e1,e2,&change1,&change2);
  printf("env 3\n");
  environment_fdump(stdout,e3);
  dimchange_fprint(stdout,change1);
  dimchange_fprint(stdout,change2);
  dimchange_free(change1);
  dimchange_free(change2);

  e4 = environment_lce(e3,e2,&change1,&change2);
  assert(e3==e4);
  printf("env 4\n");
  environment_fdump(stdout,e4);
  assert(change1==NULL);
  dimchange_fprint(stdout,change2);
  dimchange_free(change2);
  environment_free(e3);
  environment_free(e4);

  e3 = environment_lce(e1,e5,&change1,&change2);
  printf("env 3\n");
  environment_fdump(stdout,e3);
  dimchange_fprint(stdout,change1);
  dimchange_fprint(stdout,change2);
  dimchange_free(change1);
  dimchange_free(change2);
  environment_free(e3);
  environment_free(e1);
  environment_free(e2);
  environment_free(e5);
}


/* ********************************************************************** */
/* Polyhedra 1 */
/* ********************************************************************** */

void absval1(manager_t* man, abstract1_t* ppo1, abstract1_t* ppo2)
{
  /* Creation du polyèdre 
     1/2x+2/3y=1, [1,2]<=z+2w<=4, -2<=1/3z-w<=3,
     u non contraint */
  var_t name_of_realdim1[6] = {
    "x","y","z","u","w","v"
  };
  var_t name_of_realdimXY[2] = {
    "x","y"
  };
  var_t name_of_realdimZW[2] = {
    "z","w"
  };
  lincons1_t cons;
  lincons1_array_t array;
  generator1_array_t garray;
  abstract1_t poly;
  abstract1_t poly2;
  abstract1_t poly3;
  box1_t box;
  interval_t* itv;
  size_t i;
  linexpr1_t expr;
  tbool_t tb;
  environment_t* env;
  environment_t* envXY;
  environment_t* envZW;
  environment_t* envXYZW;
  bool b;

  env = environment_make(NULL,0,name_of_realdim1,6);
  envXY = environment_make(NULL,0,name_of_realdimXY,2);
  envZW = environment_make(NULL,0,name_of_realdimZW,2);
  envXYZW = environment_add(envXY,NULL,0,name_of_realdimZW,2);
  
  array = lincons1_array_make(envXYZW,5);

  /* 1. Constraint system */
  /* 1/2x+2/3y=1 */
  expr = linexpr1_make(envXY,LINEXPR_SPARSE,3);
  linexpr1_set_cst_scalar_int(&expr,1);
  assert(! linexpr1_set_coeff_scalar_frac(&expr,"x",1,2) );
  assert(! linexpr1_set_coeff_scalar_frac(&expr,"y",2,3) );
  cons = lincons1_make(CONS_EQ,&expr);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  lincons1_extend_environment_with(&cons,envXYZW);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  assert(! lincons1_array_set(&array,0,&cons) );
  /* [1,2]<=z+2w */
  expr = linexpr1_make(envZW,LINEXPR_SPARSE,1);
  b = linexpr1_set_format(&expr,"@ii%si%si",
			  -2,-1,
			  "z",1,"w",2);
  assert(!b);
  cons = lincons1_make(CONS_SUPEQ,&expr);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  lincons1_extend_environment_with(&cons,envXYZW);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  assert(! lincons1_array_set(&array,1,&cons) );
  /* z+2w<=4 */
  expr = linexpr1_make(envZW,LINEXPR_SPARSE,0);
  b = linexpr1_set_format(&expr,"@si%si%si",
			  4,
			  "z",-1,"w",-2);
  assert(!b);
  cons = lincons1_make(CONS_SUPEQ,&expr);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  lincons1_extend_environment_with(&cons,envXYZW);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  assert(! lincons1_array_set(&array,2,&cons) );
  /* -2<=1/3z-w */
  expr = linexpr1_make(envZW,LINEXPR_SPARSE,2);
  cons = lincons1_make(CONS_SUPEQ,&expr);
  b = lincons1_set_format(&cons,"@si%sf%si",
			  2,
			  "z",1,3,"w",-1);
  assert(!b);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  lincons1_extend_environment_with(&cons,envXYZW);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  assert(! lincons1_array_set(&array,3,&cons) );
  /* 1/3z-w<=3 */
  expr = linexpr1_make(envZW,LINEXPR_SPARSE,1);
  linexpr1_set_cst_scalar_int(&expr,3);
  assert(! linexpr1_set_coeff_scalar_frac(&expr,"z", -1,3) );
  assert(! linexpr1_set_coeff_scalar_int(&expr,"w", 1) );
  cons = lincons1_make(CONS_SUPEQ,&expr);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  lincons1_extend_environment_with(&cons,envXYZW);
  lincons1_fprint(stdout,&cons);fprintf(stdout,"\n");
  assert(! lincons1_array_set(&array,4,&cons) );
  
  /* Creation */
  lincons1_array_fprint(stdout,&array);
  poly = abstract1_of_lincons_array(man,env,&array);
  abstract1_fprint(stdout,man,&poly);
  abstract1_canonicalize(man,&poly);
  abstract1_fprint(stdout,man,&poly);  
  garray = abstract1_to_generator_array(man,&poly);
  generator1_array_fprint(stdout,&garray);
  generator1_array_clear(&garray);
  abstract1_fdump(stdout,man,&poly);  
  /* 2. Constraint system */
  /* Conversion (to_lincons_array already tested with print) */
  /* Should be
     x: [-oo,+oo]
     y: [-oo,+oo]
     z: [-6/5,6]
     w: [-7/5,2]
     u: [-oo,+oo]
     v: [-oo,+oo]
  */
  box = abstract1_to_box(man,&poly);
  box1_fprint(stdout,&box);
  box1_clear(&box);

  /* Extraction (we first extract values for existing constraints, then for
     dimensions) */
  /* existing constraints */
  for (i=0; i<5; i++){
    cons = lincons1_array_get(&array,i);
    expr = lincons1_linexpr1ref(&cons);
    itv = abstract1_bound_linexpr(man,&poly,&expr);
    fprintf(stdout,"Bound of ");
    linexpr1_fprint(stdout,&expr);
    fprintf(stdout,": ");
    interval_fprint(stdout,itv);
    fprintf(stdout,"\n");
    interval_free(itv);
  }
  /* dimensions */
  for (i=0; i<env->intdim+env->realdim; i++){
    expr = linexpr1_make(env,LINEXPR_SPARSE,1);
    linexpr1_set_cst_scalar_double(&expr,0.0);
    linexpr1_set_coeff_scalar_int(&expr,environment_var_of_dim(env,i),1);
    itv = abstract1_bound_linexpr(man,&poly,&expr);
    fprintf(stdout,"Bound of ");
    linexpr1_fprint(stdout,&expr);
    fprintf(stdout,": ");
    interval_fprint(stdout,itv);
    fprintf(stdout,"\n");
    interval_free(itv);
    linexpr1_clear(&expr);
  }
  /* 3. of box z: [-6/5,6], w: [-7/5,2] */
  {
    var_t name[2] = { "z", "w" };
    interval_t* itv[2];

    itv[0] = interval_alloc();
    itv[1] = interval_alloc();
    interval_set_infsup_frac(itv[0],-6,5,6,1);
    interval_set_infsup_frac(itv[1],-7,5,2,1);
    poly2 = abstract1_of_box(man,env,name,itv,2);
    abstract1_fprint(stdout,man,&poly2);
    abstract1_canonicalize(man,&poly2);
    abstract1_fprint(stdout,man,&poly2);  
    abstract1_fdump(stdout,man,&poly2);
    interval_free(itv[0]);
    interval_free(itv[1]);
  }

  /* 4. Tests top and bottom */
  poly3 = abstract1_bottom(man,env);
  tb = abstract1_is_bottom(man,&poly3);
  fprintf(stdout,"abstract1_is_bottom(poly3)=%d\n",tb);
  tb = abstract1_is_top(man,&poly3);
  fprintf(stdout,"abstract1_is_top(poly3)=%d\n",tb);
  abstract1_clear(man,&poly3);

  poly3 = abstract1_top(man,env);
  tb = abstract1_is_bottom(man,&poly3);
  fprintf(stdout,"abstract1_is_bottom(poly3)=%d\n",tb);
  tb = abstract1_is_top(man,&poly3);
  fprintf(stdout,"abstract1_is_top(poly3)=%d\n",tb);
  abstract1_clear(man,&poly3);

  abstract1_minimize(man,&poly2);
  abstract1_fprint(stdout,man,&poly2);  
  tb = abstract1_is_bottom(man,&poly2);
  fprintf(stdout,"abstract1_is_bottom(poly2)=%d\n",tb);
   tb = abstract1_is_top(man,&poly2);
  fprintf(stdout,"abstract1_is_top(poly2)=%d\n",tb);

  /* 5. Tests leq */
  tb = abstract1_is_leq(man,&poly,&poly2);
  fprintf(stdout,"abstract1_is_leq(poly,&poly2)=%d\n",tb);
  tb = abstract1_is_leq(man,&poly2,&poly);
  fprintf(stdout,"abstract1_is_leq(poly,&poly2)=%d\n",tb);
  
  /* 6. Tests sat_interval */
  itv = interval_alloc();
  interval_set_infsup_double(itv,-6.0,6.0);
  tb = abstract1_sat_interval(man,&poly,"z",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,z)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);

  tb = abstract1_sat_interval(man,&poly,"w",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,w)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);

  tb = abstract1_sat_interval(man,&poly,"u",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,u)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);

  interval_set_infsup_double(itv,-2.5,2.5);
  tb = abstract1_sat_interval(man,&poly,"z",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,z)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);
  tb = abstract1_sat_interval(man,&poly,"w",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,w)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);
  tb = abstract1_sat_interval(man,&poly,"u",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,u)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);

  interval_set_infsup_double(itv,-1.4,2.0);
  tb = abstract1_sat_interval(man,&poly,"z",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,z)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);
  tb = abstract1_sat_interval(man,&poly,"w",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,w)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);
  tb = abstract1_sat_interval(man,&poly,"u",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,u)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);

  scalar_set_frac(itv->inf,-14,10);
  scalar_set_double(itv->sup,2.0);
  tb = abstract1_sat_interval(man,&poly,"z",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,z)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);
  tb = abstract1_sat_interval(man,&poly,"w",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,w)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);
  tb = abstract1_sat_interval(man,&poly,"u",itv);
  fprintf(stdout,"abstract1_sat_interval(poly,u)");
  interval_fprint(stdout,itv);
  fprintf(stdout," = %d\n",tb);

  interval_free(itv);

  /* 7. Tests sat_lincons with -3x-4y+z-w */
  expr = linexpr1_make(env,LINEXPR_DENSE,0);
  linexpr1_set_cst_scalar_int(&expr,0);
  linexpr1_set_coeff_scalar_int(&expr,"x",-3);
  linexpr1_set_coeff_scalar_int(&expr,"y",-4);
  linexpr1_set_coeff_scalar_int(&expr,"z",1);
  linexpr1_set_coeff_scalar_int(&expr,"w",-1);
  cons = lincons1_make(CONS_SUPEQ,&expr);

  itv = abstract1_bound_linexpr(man,&poly,&expr);
  fprintf(stdout,"Bound of ");
  linexpr1_fprint(stdout,&expr);
  fprintf(stdout,": ");
  interval_fprint(stdout,itv);
  fprintf(stdout,"\n");
  interval_free(itv);
  
  for (i=0; i<6; i++){
    linexpr1_set_cst_scalar_frac(&expr,-26 + (int)i*10, 5);
    *(lincons1_constypref(&cons)) = CONS_SUPEQ;
    tb = abstract1_sat_lincons(man,&poly,&cons);
    fprintf(stdout,"abstract1_sat_lincons(poly)");
    lincons1_fprint(stdout,&cons);
    fprintf(stdout,": %d\n",tb);
    *(lincons1_constypref(&cons)) = CONS_SUP;
    tb = abstract1_sat_lincons(man,&poly,&cons);
    fprintf(stdout,"abstract1_sat_lincons(poly)");
    lincons1_fprint(stdout,&cons);
    fprintf(stdout,": %d\n",tb);
  }
  
  linexpr1_set_cst_scalar_int(&expr,0);
  linexpr1_set_coeff_scalar_frac(&expr,"z",-2,3);
  linexpr1_set_coeff_scalar_double(&expr,"w",2.0);

  itv = abstract1_bound_linexpr(man,&poly,&expr);
  fprintf(stdout,"Bound of ");
  linexpr1_fprint(stdout,&expr);
  fprintf(stdout,": ");
  interval_fprint(stdout,itv);
  fprintf(stdout,"\n");
  interval_free(itv);
  
  for (i=0; i<6; i++){
    linexpr1_set_cst_scalar_int(&expr,-1+(int)i);
    *(lincons1_constypref(&cons)) = CONS_SUPEQ;
    tb = abstract1_sat_lincons(man,&poly,&cons);
    fprintf(stdout,"abstract1_sat_lincons(poly)");
    lincons1_fprint(stdout,&cons);
    fprintf(stdout,": %d\n",tb);
    *(lincons1_constypref(&cons)) = CONS_SUP;
    tb = abstract1_sat_lincons(man,&poly,&cons);
    fprintf(stdout,"abstract1_sat_lincons(poly)");
    lincons1_fprint(stdout,&cons);
    fprintf(stdout,": %d\n",tb);
  }
  linexpr1_clear(&expr);
  
  lincons1_array_clear(&array);
  environment_free(env);
  environment_free(envXY);
  environment_free(envZW);
  environment_free(envXYZW);
  *ppo1 = poly;
  *ppo2 = poly2;
}

int main(int argc, char**argv)
{
  manager_t* man;
  abstract1_t a1,a2;

  test_environment();
  man = pk_manager_alloc(true);
  absval1(man,&a1,&a2);
  abstract1_clear(man,&a1);
  abstract1_clear(man,&a2);
  manager_free(man);
}
