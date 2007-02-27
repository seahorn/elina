/* ********************************************************************** */
/* pk_user.c: conversions with interface datatypes */
/* ********************************************************************** */

/* This file is part of the APRON Library, released under LGPL license.  Please
   read the COPYING file packaged in the distribution */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_matrix.h"
#include "pk_internal.h"

/* ********************************************************************** */
/* */
/* ********************************************************************** */

void ap_coeff_set_scalar_numint(ap_coeff_t* coeff, numint_t num){
  ap_coeff_reinit(coeff,AP_COEFF_SCALAR,AP_SCALAR_MPQ);
  mpq_set_numint(coeff->val.scalar->val.mpq,num);
}

/* Fills the vector with the quasi-linear expression expr. Coefficients in the expression
   are supposed to have been converted to rationals. */

void vector_set_linexpr(pk_internal_t* pk,
			numint_t* vec,
			ap_linexpr0_t* expr,
			size_t dim,
			int mode)
{
  size_t i;
  ap_dim_t j;
  mpq_t* mpq;
  ap_coeff_t* coeff;

  mpq = pk->vector_mpqp;
  /* Fill the array mpq */
  switch(expr->cst.discr){
  case AP_COEFF_SCALAR:
    ap_mpq_set_scalar(mpq[0],expr->cst.val.scalar,0);
    break;
  case AP_COEFF_INTERVAL:
    if (mode<=0){
      ap_mpq_set_scalar(mpq[0],
		     expr->cst.val.interval->inf,
		     0);
    }
    else {
      ap_mpq_set_scalar(mpq[0],
		     expr->cst.val.interval->sup,
		     0);
    }
    break;
  }
  for (i=0;i<dim; i++){
    mpq_set_si(mpq[i+1],0,1);
  }
  ap_linexpr0_ForeachLinterm(expr,i,j,coeff){
    assert(j<dim);
    assert(coeff->discr == AP_COEFF_SCALAR);
    ap_mpq_set_scalar(mpq[j+1],coeff->val.scalar,0);
  }
  
  /* compute lcm of denominators, in vec[0] */
  mpz_set(vec[0],mpq_denref(mpq[0]));
  for (i=0; i<dim; i++){
    mpz_lcm(vec[0],vec[0],mpq_denref(mpq[i+1]));
  }

  /* fill the vector */
  if (pk->strict) numint_set_int(vec[polka_eps],0);

  for (i=0; i<dim+1; i++){
    size_t index = i==0 ? polka_cst : pk->dec + i - 1;
    mpz_divexact(vec[index],vec[0],mpq_denref(mpq[i]));
    mpz_mul(vec[index],vec[index],mpq_numref(mpq[i]));
  }
  return;
}

/* Fills the vector with the constraint cons. */

void vector_set_lincons(pk_internal_t* pk,
			numint_t* vec,
			ap_lincons0_t* cons,
			size_t intdim, size_t realdim,
			bool integer)
{
  assert(cons->constyp == AP_CONS_EQ || 
	 cons->constyp == AP_CONS_SUPEQ ||
	 cons->constyp == AP_CONS_SUP);
  vector_set_linexpr(pk, vec, cons->linexpr0, intdim+realdim, -1);
  vector_normalize(pk,vec,pk->dec+intdim+realdim);
  if (cons->constyp == AP_CONS_EQ)
    numint_set_int(vec[0],0);
  else
    numint_set_int(vec[0],1);
  if (cons->constyp == AP_CONS_SUP){
    if (pk->strict){
      numint_set_int(vec[polka_eps],-1);
    }
    else if (integer && vector_is_integer(pk, vec, intdim, realdim)){
      numint_sub_uint(vec[polka_cst], vec[polka_cst], 1);
    }
  }
  if (integer)
    vector_normalize_constraint_int(pk,vec,intdim,realdim);
    
  return;
}


/* Fills the vector with the generator gen. */

void vector_set_generator(pk_internal_t* pk,
			  numint_t* vec,
			  ap_generator0_t* gen,
			  size_t intdim, size_t realdim)
{
  vector_set_linexpr(pk, vec, gen->linexpr0, intdim+realdim, -1);

  if (pk->strict)
    numint_set_int(vec[polka_eps],0);

  numint_set_int(vec[0], 
		 ( ((gen->gentyp == AP_GEN_LINE) || (gen->gentyp == AP_GEN_LINEMOD)) ?
		   0 : 
		   1 ));
  if (gen->gentyp != AP_GEN_VERTEX){
    numint_set_int(vec[polka_cst],0);
    vector_normalize(pk,vec, pk->dec + intdim + realdim);
  }
  return;
}

ap_lincons0_t lincons_of_vector(pk_internal_t* pk,
			     numint_t* q,
			     size_t size)
{
  ap_lincons0_t lincons;
  ap_linexpr0_t* linexpr;
  size_t i;

  linexpr = ap_linexpr0_alloc(AP_LINEXPR_DENSE, size - pk->dec);

  ap_coeff_set_scalar_numint(&linexpr->cst, q[polka_cst]);
  for (i=pk->dec; i<size; i++){
    ap_dim_t dim = i - pk->dec;
    ap_coeff_set_scalar_numint(&linexpr->p.coeff[dim], q[i]);
  }
  if (numint_sgn(q[0])){
    if (pk->strict && numint_sgn(q[polka_eps])<0)
      lincons.constyp = AP_CONS_SUP;
    else
      lincons.constyp = AP_CONS_SUPEQ;
  }
  else {
    lincons.constyp = AP_CONS_EQ;
  }
  lincons.linexpr0 = linexpr;
  lincons.scalar = NULL;
  return lincons;
}

ap_generator0_t generator_of_vector(pk_internal_t* pk,
				numint_t* q,
				size_t size)
{
  ap_generator0_t generator;
  ap_linexpr0_t* linexpr;
  size_t i;
  int sgn;

  linexpr = ap_linexpr0_alloc(AP_LINEXPR_DENSE, size - pk->dec);
  ap_coeff_set_scalar_int(&linexpr->cst, 0);
  sgn = numint_sgn(q[polka_cst]);
  for (i=pk->dec; i<size; i++){
    ap_dim_t dim = i - pk->dec;
    ap_coeff_set_scalar_numint(&linexpr->p.coeff[dim],q[i]);
    if (sgn){
      mpz_set_numint(mpq_denref(linexpr->p.coeff[dim].val.scalar->val.mpq),
		     q[polka_cst]);
      mpq_canonicalize(linexpr->p.coeff[dim].val.scalar->val.mpq);
    }
  }
  if (!numint_sgn(q[0])){
    generator.gentyp = AP_GEN_LINE;
  }
  else if (sgn==0){
    generator.gentyp = AP_GEN_RAY;
  }
  else {
    generator.gentyp = AP_GEN_VERTEX;
  }
  generator.linexpr0 = linexpr;
  return generator;
}

/* ====================================================================== */
/* VI Conversion from user datatypes */
/* ====================================================================== */

matrix_t* matrix_of_lincons_array(pk_internal_t* pk,
				  ap_lincons0_array_t* array,
				  size_t intdim, size_t realdim,
				  bool integer)
{
  size_t i,j;
  matrix_t* mat = matrix_alloc(array->size,pk->dec+intdim+realdim,false);
  j = 0;
  for (i=0; i<array->size; i++){
    switch (array->p[i].constyp){
    case AP_CONS_EQ:
    case AP_CONS_SUPEQ:
    case AP_CONS_SUP:
      vector_set_lincons(pk,mat->p[j],&array->p[i],intdim,realdim,integer);
      j++;
      break;
    default:
      break;
    }
  }
  mat->nbrows = j;
  return mat;
}

matrix_t* matrix_of_generator_array(pk_internal_t* pk,
				    ap_generator0_array_t* array,
				    size_t intdim, size_t realdim)
{
  size_t i;
  matrix_t* mat = matrix_alloc(array->size,pk->dec+intdim+realdim,false);
  for (i=0; i<array->size; i++){
    vector_set_generator(pk,mat->p[i],&array->p[i],intdim,realdim);
  }
  return mat;
}
