/* ********************************************************************** */
/* example1.c: using APRON library  */
/* ********************************************************************** */

/* We illustrate here the use of the APRON interface, at level 1 */

#include "ap_global0.h"
#include "ap_global1.h"

#include "pk.h"

void ex1(ap_manager_t* man)
{
  ap_var_t name_of_dim[6] = {    
    "x","y","z","u","w","v"
  };
  ap_environment_t* env = ap_environment_alloc(NULL,0,name_of_dim,6);

  /* =================================================================== */
  /* Creation of polyhedra 
     1/2x+2/3y=1, [1,2]<=z+2w<=4 */
  /* =================================================================== */

  /* 0. Create the array */
  ap_lincons1_array_t array = ap_lincons1_array_make(env,3);

  /* 1.a Creation of an equality constraint 1/2x+2/3y=1 */
  ap_linexpr1_t expr = ap_linexpr1_make(env,AP_LINEXPR_SPARSE,2);
  ap_lincons1_t cons = ap_lincons1_make(AP_CONS_EQ,&expr);
    /* Now expr is memory-managed by cons */ 

  /* 1.b Fill the constraint */ 
  ap_lincons1_set_list(&cons,
		       AP_COEFF_S_FRAC,1,2,"x",
		       AP_COEFF_S_FRAC,2,3,"y",
		       AP_CST_S_INT,1,
		       AP_END);
  /* 1.c Put in the array */
  ap_lincons1_array_set(&array,0,&cons);
    /* Now cons is memory-managed by array */ 

  /* 2.a Creation of an inequality constraint [1,2]<=z+2w */
  expr = ap_linexpr1_make(env,AP_LINEXPR_SPARSE,2);
  cons = ap_lincons1_make(AP_CONS_SUPEQ,&expr);
    /* The old cons is not lost, because it is stored in the array.
       It would be an error to clear it (same for expr). */
  /* 2.b Fill the constraint */ 
  ap_lincons1_set_list(&cons,
		       AP_COEFF_S_INT,1,"z",
		       AP_COEFF_S_DOUBLE,2.0,"w",
		       AP_CST_I_INT,-2,-1,
		       AP_END);
  /* 2.c Put in the array */
  ap_lincons1_array_set(&array,1,&cons);
  
  /* 2.a Creation of an inequality constraint */
  expr = ap_linexpr1_make(env,AP_LINEXPR_SPARSE,2);
  cons = ap_lincons1_make(AP_CONS_SUPEQ,&expr);
    /* The old cons is not lost, because it is stored in the array.
       It would be an error to clear it (same for expr). */
  /* 2.b Fill the constraint */ 
  ap_lincons1_set_list(&cons,
		       AP_COEFF_S_INT,1,"z",
		       AP_COEFF_S_DOUBLE,2.0,"w",
		       AP_CST_I_INT,-2,-1,
		       AP_END);
  /* 2.c Put in the array */
  ap_lincons1_array_set(&array,1,&cons);
  
  /* 3.a Creation of an inequality constraint by duplication and
       modification z+2w<=4 */
  cons = ap_lincons1_copy(&cons);
  /* 3.b Fill the constraint (by negating the existing coefficients) */ 
  expr = ap_lincons1_linexpr1ref(&cons);
  {
    ap_coeff_t* pcoeff;
    ap_var_t var;
    size_t i;
    ap_linexpr1_ForeachLinterm1(&expr,i,var,pcoeff){
      ap_coeff_neg(pcoeff,pcoeff);
    }
  }
  ap_linexpr1_set_cst_scalar_int(&expr,4);
  /* 3.c Put in the array */
  ap_lincons1_array_set(&array,2,&cons);

  /* 4. Creation of an abstract value */
  ap_abstract1_t abs = ap_abstract1_of_lincons_array(man,env,&array);
  
  fprintf(stdout,"Abstract value:\n");
  ap_abstract1_fprint(stdout,man,&abs);

  /* deallocation */
  ap_lincons1_array_clear(&array);
  
  /* =================================================================== */
  /* Creation of polyhedra 
     x+y+z+w+u=0, 0<= w <= 5, -5<=u<=0, x+y-z-w>=0, */
  /* =================================================================== */
  array = ap_lincons1_array_make(env,6);
  expr = ap_linexpr1_make(env,AP_LINEXPR_DENSE,0);
  cons = ap_lincons1_make(AP_CONS_EQ,&expr);
  ap_linexpr1_set_list(&expr,
		       AP_COEFF_S_INT,1,"x",
		       AP_COEFF_S_INT,1,"y",
		       AP_COEFF_S_INT,1,"z",
		       AP_COEFF_S_INT,1,"w",
		       AP_COEFF_S_INT,1,"u",
		       AP_END);
  ap_lincons1_array_set(&array,0,&cons);
  expr = ap_linexpr1_make(env,AP_LINEXPR_SPARSE,0);
  cons = ap_lincons1_make(AP_CONS_SUPEQ,&expr);
  ap_linexpr1_set_list(&expr, AP_COEFF_S_INT,1,"w",AP_END);
  ap_lincons1_array_set(&array,1,&cons);
  expr = ap_linexpr1_make(env,AP_LINEXPR_SPARSE,0);
  cons = ap_lincons1_make(AP_CONS_SUPEQ,&expr);
  ap_linexpr1_set_list(&expr, AP_COEFF_S_INT,-1,"w",AP_CST_S_INT,5, AP_END);
  ap_lincons1_array_set(&array,2,&cons);
  expr = ap_linexpr1_make(env,AP_LINEXPR_SPARSE,0);
  cons = ap_lincons1_make(AP_CONS_SUPEQ,&expr);
  ap_linexpr1_set_list(&expr, AP_COEFF_S_INT,1,"u",AP_CST_S_INT,5, AP_END);
  ap_lincons1_array_set(&array,3,&cons);
  expr = ap_linexpr1_make(env,AP_LINEXPR_SPARSE,0);
  cons = ap_lincons1_make(AP_CONS_SUPEQ,&expr);
  ap_linexpr1_set_list(&expr, AP_COEFF_S_INT,-1,"u",AP_END);
  ap_lincons1_array_set(&array,4,&cons);
  expr = ap_linexpr1_make(env,AP_LINEXPR_DENSE,0);
  cons = ap_lincons1_make(AP_CONS_EQ,&expr);
  ap_linexpr1_set_list(&expr,
		       AP_COEFF_S_INT,1,"x",
		       AP_COEFF_S_INT,1,"y",
		       AP_COEFF_S_INT,-1,"z",
		       AP_COEFF_S_INT,-1,"w",
		       AP_END);
  ap_lincons1_array_set(&array,5,&cons);

  ap_abstract1_t abs2 = ap_abstract1_of_lincons_array(man,env,&array);
  
  fprintf(stdout,"Abstract value 2:\n");
  ap_abstract1_fprint(stdout,man,&abs2);

  ap_lincons1_array_clear(&array);

  /* =================================================================== */
  /* Meet and Join */
  /* =================================================================== */
 
  ap_abstract1_t res = ap_abstract1_meet(man,false,&abs,&abs2);
  fprintf(stdout,"Meet:\n");
  ap_abstract1_fprint(stdout,man,&res);
  ap_abstract1_clear(man,&res);

  res = ap_abstract1_join(man,false,&abs,&abs2);
  fprintf(stdout,"Join:\n");
  ap_abstract1_fprint(stdout,man,&res);
  ap_abstract1_clear(man,&res);

  /* =================================================================== */
  /* Assignement */
  /* =================================================================== */
 
  expr = ap_linexpr1_make(env,AP_LINEXPR_SPARSE,0);
  ap_linexpr1_set_list(&expr,
		       AP_COEFF_S_INT,-3,"w",
		       AP_COEFF_S_DOUBLE,2.625,"z",
		       AP_CST_I_INT,0,3,
		       AP_END);
  fprintf(stdout,"Assignement (side-effect) in abstract value of u by expression:\n");
  ap_linexpr1_fprint(stdout,&expr);

  abs = ap_abstract1_assign_linexpr(man,true,&abs,"u",&expr,NULL);
  fprintf(stdout,"\n");
  ap_abstract1_fprint(stdout,man,&abs);
  ap_linexpr1_clear(&expr);

  /* deallocation */
  ap_abstract1_clear(man,&abs);
}

void main()
{
  ap_manager_t* man = pk_manager_alloc(true);
  ex1(man);
}
