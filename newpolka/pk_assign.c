/* ********************************************************************** */
/* pk_assign.c: Assignements and Substitutions */
/* ********************************************************************** */

#include "pk_config.h"
#include "pk_vector.h"
#include "pk_satmat.h"
#include "pk_matrix.h"
#include "pk.h"
#include "pk_user.h"
#include "pk_representation.h"
#include "pk_constructor.h"
#include "pk_resize.h"
#include "pk_meetjoin.h"
#include "pk_assign.h"

/* ********************************************************************** */
/* I. Matrix operations */
/* ********************************************************************** */

/* ====================================================================== */
/* Matrix transformations: a variable and an expression */
/* ====================================================================== */

/* ---------------------------------------------------------------------- */
/* Assignement of an expression to a variable */
/* ---------------------------------------------------------------------- */

/* Hypothesis:

  - either nmat is a matrix allocated with _matrix_alloc_int,
    and his coefficients are not initialized,

  - or nmat==mat
*/

matrix_t* matrix_assign_variable(pk_internal_t* pk,
				 bool destructive,
				 matrix_t* mat,
				 ap_dim_t dim, numint_t* tab)
{
  size_t i,j,var;
  bool den;
  matrix_t* nmat;

  var = pk->dec + dim;
  den = numint_cmp_int(tab[0],1)>0;

  nmat = 
    destructive ? 
    mat :
    _matrix_alloc_int(mat->nbrows,mat->nbcolumns,false);
  
  for (i=0; i<mat->nbrows; i++){
    /* product for var column */
    vector_product(pk,pk->matrix_prod,
		   (const numint_t*)mat->p[i],
		   (const numint_t*)tab,mat->nbcolumns);
    /* columns != var */
    if (!destructive){	
      /* Functional */
      numint_init_set(nmat->p[i][0],mat->p[i][0]);
      for (j=1; j<mat->nbcolumns; j++){
	if (j!=var){
	  numint_init_set(nmat->p[i][j],mat->p[i][j]);
	  if (den){
	    numint_mul(nmat->p[i][j],mat->p[i][j],tab[0]);
	  }
	}
      }
    } 
    else {
      /* Side-effect */
      for (j=0; j<mat->nbcolumns; j++){
	if (j!=var){
	  if (den)
	    numint_mul(nmat->p[i][j],mat->p[i][j],tab[0]);
	  else
	    numint_set(nmat->p[i][j],mat->p[i][j]);
	}
      }
    }  
    /* var column */
    if (!destructive)
      numint_init_set(nmat->p[i][var],pk->matrix_prod);
    else
      numint_set(nmat->p[i][var],pk->matrix_prod);

    matrix_normalize_row(pk,nmat,i);
  }
  return nmat;
}

/* ---------------------------------------------------------------------- */
/* Substitution of a variable by an expression */
/* ---------------------------------------------------------------------- */

/* Hypothesis:

  - either nmat is a matrix allocated with _matrix_alloc_int,
    and his coefficients are not initialized,

  - or nmat==mat
*/

matrix_t* matrix_substitute_variable(pk_internal_t* pk,
				     bool destructive,
				     matrix_t* mat,
				     ap_dim_t dim, numint_t* tab)
{
  size_t i,j,var;
  bool den;
  matrix_t* nmat;

  var = pk->dec + dim;
  den = numint_cmp_int(tab[0],1)>0;
  nmat = 
    destructive ? 
    mat :
    _matrix_alloc_int(mat->nbrows,mat->nbcolumns,false);

  for (i=0; i<mat->nbrows; i++) {
    if (numint_sgn(mat->p[i][var])) {
      /* The substitution must be done */
      if (!destructive){
	/* Functional */
	numint_init_set(nmat->p[i][0],mat->p[i][0]);
	/* columns != var */
	for (j=1; j<mat->nbcolumns; j++) {
	  if (j!=var){
	    if (den){
	      numint_init(nmat->p[i][j]);
	      numint_mul(nmat->p[i][j],mat->p[i][j],tab[0]);
	    } 
	    else {
	      numint_init_set(nmat->p[i][j],mat->p[i][j]);
	    }
	    numint_mul(pk->matrix_prod,mat->p[i][var],tab[j]);
	    numint_add(nmat->p[i][j],nmat->p[i][j],pk->matrix_prod);
	  }
	}
	/* var column */
	numint_init(nmat->p[i][var]);
	numint_mul(nmat->p[i][var],mat->p[i][var],tab[var]);
      }
      else {
	/* Side-effect */
	/* columns != var */
	for (j=1; j<mat->nbcolumns; j++) {
	  if (j!=var){
	    if (den){
	      numint_mul(nmat->p[i][j],nmat->p[i][j],tab[0]);
	    } 
	    numint_mul(pk->matrix_prod,mat->p[i][var],tab[j]);
	    numint_add(nmat->p[i][j],nmat->p[i][j],pk->matrix_prod);
	  }
	}
	/* var column */
	numint_mul(nmat->p[i][var],nmat->p[i][var],tab[var]);
      }
      matrix_normalize_row(pk,nmat,i);
    }
    else {
      /* No substitution */
      if (!destructive){
	for (j=0; j<mat->nbcolumns; j++) {
	  numint_init_set(nmat->p[i][j],mat->p[i][j]);
	}
      }
    }
  }
  return nmat;
}

/* ====================================================================== */
/* Matrix transformations: several variables and expressions */
/* ====================================================================== */

/* The list of pair (variable,expr) is given by an array of type
   equation_t. IMPORTANT: the array is supposed to be sorted in increasing
   order w.r.t. the field dim. The vectors ar eof size the number of columns of
   the matrix.
*/

/* ---------------------------------------------------------------------- */
/* Assignement by an array of equations */
/* ---------------------------------------------------------------------- */

matrix_t* matrix_assign_variables(pk_internal_t* pk,
				  matrix_t* mat,
				  const ap_dim_t* tdim,
				  numint_t** tvec,
				  size_t size)
{
  size_t i,j,eindex;
  matrix_t* nmat = _matrix_alloc_int(mat->nbrows, mat->nbcolumns,false);
  numint_t den;

  /* Computing common denominator */
  numint_init_set(den,tvec[0][0]);
  for (i=1; i<size; i++){
    numint_mul(den,den,tvec[i][0]);
  }

  if (numint_cmp_int(den,1)!=0){
    /* General case */
    numint_t* vden = vector_alloc(size);
    for (i=0; i<size; i++){
      numint_divexact(vden[i],den,tvec[i][0]);
    }
    /* Column 0: copy */
    for (i=0; i<mat->nbrows; i++){
      numint_init_set(nmat->p[i][0],mat->p[i][0]);
    }
    /* Other columns */
    eindex = 0;
    for (j=1; j<mat->nbcolumns; j++){
      if (eindex < size && pk->dec + tdim[eindex] == j){
	/* We are on a assigned column */
	for (i=0; i<mat->nbrows; i++){ /* For each row */
	  vector_product(pk,pk->matrix_prod,
			 (const numint_t*)mat->p[i],
			 (const numint_t*)tvec[eindex],mat->nbcolumns);
	  numint_mul(pk->matrix_prod,pk->matrix_prod,vden[eindex]);
	  /* Put the result */
	  numint_init_set(nmat->p[i][j],pk->matrix_prod);
	}
	eindex++;
      }
      else {
	/* We are on a normal column */
	for (i=0; i<mat->nbrows; i++){ /* For each row */
	  numint_init(nmat->p[i][j]);
	  numint_mul(nmat->p[i][j],mat->p[i][j],den);
	}
      }
    }
    vector_free(vden,size);
  }
  else {
    /* Special case: all denominators are 1 */
    /* Column 0: copy */
    for (i=0; i<mat->nbrows; i++){
      numint_init_set(nmat->p[i][0],mat->p[i][0]);
    }
    /* Other columns */
    eindex = 0;
    for (j=1; j<mat->nbcolumns; j++){
      if (eindex < size && pk->dec + tdim[eindex] == j){
	/* We are on a assigned column */
	for (i=0; i<mat->nbrows; i++){ /* For each row */
	  vector_product(pk,pk->matrix_prod,
			 (const numint_t*)mat->p[i],
			 (const numint_t*)tvec[eindex],mat->nbcolumns);
	  numint_init_set(nmat->p[i][j],pk->matrix_prod);
	}
	eindex++;
      }
      else {
	/* We are on a normal column */
	for (i=0; i<mat->nbrows; i++){ /* For each row */
	  numint_init_set(nmat->p[i][j],mat->p[i][j]);
	}
      }
    }
  }
  numint_clear(den);
  for (i=0; i<mat->nbrows; i++){
    matrix_normalize_row(pk,nmat,i);
  }

  return nmat;
}

/* ---------------------------------------------------------------------- */
/* Substitution by an array of equations */
/* ---------------------------------------------------------------------- */

matrix_t* matrix_substitute_variables(pk_internal_t* pk,
				      matrix_t* mat,
				      const ap_dim_t* tdim,
				      numint_t** tvec,
				      size_t size)
{
  size_t i,j,eindex;
  matrix_t* nmat = matrix_alloc(mat->nbrows, mat->nbcolumns,false);
  numint_t den;

  /* Computing common denominator */
  numint_init_set(den,tvec[0][0]);
  for (i=1; i<size; i++){
    numint_mul(den,den,tvec[i][0]);
  }

  if (numint_cmp_int(den,1)!=0){
    /* General case */
    numint_t* vden = vector_alloc(size);
    for (i=0; i<size; i++){
      numint_divexact(vden[i],den,tvec[i][0]);
    }
    /* For each row */
    for (i=0; i<mat->nbrows; i++) {
      /* Column 0 */
      numint_set(nmat->p[i][0],mat->p[i][0]);
      /* Other columns */
      /* First, copy the row and sets to zero substituted variables */
      eindex = 0;
      for (j=1; j<mat->nbcolumns; j++){
	if (eindex < size && pk->dec + tdim[eindex] == j)
	  eindex++;
	else
	  numint_mul(nmat->p[i][j],mat->p[i][j],den);
      }
      /* Second, add things coming from substitution */
      for (j=1; j<mat->nbcolumns; j++){
	for (eindex=0; eindex<size; eindex++){
	  if (numint_sgn(mat->p[i][pk->dec + tdim[eindex]])) {
	    numint_mul(pk->matrix_prod,
		      mat->p[i][pk->dec + tdim[eindex]],
		      tvec[eindex][j]);
	    numint_mul(pk->matrix_prod,pk->matrix_prod,vden[eindex]);
	    numint_add(nmat->p[i][j],nmat->p[i][j],pk->matrix_prod);
	  }
	}
      }
    }
    vector_free(vden,size);
  }
  else {
    /* Special case: all denominators are 1 */
    /* For each row */
    for (i=0; i<mat->nbrows; i++) {
      /* Column 0 */
      numint_set(nmat->p[i][0],mat->p[i][0]);
      /* Other columns */
      /* First, copy the row and sets to zero substituted variables */
      eindex = 0;
      for (j=1; j<mat->nbcolumns; j++){
	if (eindex < size && pk->dec + tdim[eindex] == j)
	  eindex++;
	else
	  numint_set(nmat->p[i][j],mat->p[i][j]);
      }
      /* Second, add things coming from substitution */
      for (j=1; j<mat->nbcolumns; j++){
	for (eindex=0; eindex<size; eindex++){
	  if (numint_sgn(mat->p[i][pk->dec + tdim[eindex]])) {
	    numint_mul(pk->matrix_prod,
		      mat->p[i][pk->dec + tdim[eindex]],
		      tvec[eindex][j]);
	    numint_add(nmat->p[i][j],nmat->p[i][j],pk->matrix_prod);
	  }
	}
      }
    }
  }
  numint_clear(den);
  for (i=0; i<mat->nbrows; i++){
    matrix_normalize_row(pk,nmat,i);
  }

  return nmat;
}

/* ********************************************************************** */
/* II. Auxiliary functions */
/* ********************************************************************** */

/* ====================================================================== */
/* Inversion of a (deterministic) linear expression */
/* ====================================================================== */

void _poly_invert_vector(pk_internal_t* pk,
			 numint_t* ntab,
			 ap_dim_t dim,
			 const numint_t* tab,
			 size_t size)
{
  int i;
  size_t var = pk->dec+dim;
  int sgn = numint_sgn(tab[var]);

  assert(sgn!=0);
  if (sgn>0){
    numint_set(ntab[0], tab[var]);
    numint_set(ntab[var], tab[0]);
    for (i=1; i<size; i++){
      if (i!=var)
	numint_neg(ntab[i],tab[i]);
    }
  } else {
    numint_neg(ntab[0], tab[var]);
    numint_neg(ntab[var], tab[0]);
    for (i=1; i<size; i++){
      if (i!=var)
	numint_set(ntab[i],tab[i]);
    }
  }
  vector_normalize(pk,ntab,size);
  return;
}

/* ====================================================================== */
/* Building a relation from a parallel assignement */
/* ====================================================================== */

matrix_t*
_matrix_relation_of_tdimexpr(pk_internal_t* pk,
			     size_t intdim, size_t realdim,
			     const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
			     const ap_dimchange_t* dimchange)
{
  size_t size;
  matrix_t* matrel;
  size_t nintdim,nrealdim;
  size_t nnbdims, nnbcols;
  size_t nbcols,row,col;
  size_t i,k;

  size = dimchange->intdim+dimchange->realdim;
  nbcols = pk->dec + intdim + realdim;
  nintdim = intdim + dimchange->intdim;
  nrealdim = realdim + dimchange->realdim;
  nnbdims = nintdim + nrealdim;
  nnbcols = pk->dec + nnbdims;

  /* Convert linear assignements in (in)equalities put in matrel */
  matrel = matrix_alloc(2*size, nnbcols, false);

  row = 0;
  for (i=0; i<size; i++){
    col = pk->dec + (i<dimchange->intdim ? intdim+i : intdim+realdim+i);
    switch(texpr[i]->cst.discr){
    case AP_COEFF_SCALAR:
      /* equality */
      vector_set_linexpr(pk,matrel->p[row],
			 texpr[i],nnbdims,0);
      vector_add_dimensions(pk,matrel->p[row],
			    (const numint_t*)matrel->p[row],nbcols,
			    dimchange);
      numint_neg(matrel->p[row][col], matrel->p[row][0]);
      numint_set(matrel->p[row][0],0);
      row ++;
      break;
    case AP_COEFF_INTERVAL:
      /* lower bound */
      vector_set_linexpr(pk,matrel->p[row],
			 texpr[i],nnbdims,-1);
      for (k=1; k<nbcols; k++)
	numint_neg(matrel->p[row][k],matrel->p[row][k]);
      vector_add_dimensions(pk,matrel->p[row],
			    (const numint_t*)matrel->p[row],nbcols,
			    dimchange);
      numint_set(matrel->p[row][col], matrel->p[row][0]);
      numint_set_int(matrel->p[row][0],1);
      row++;

      /* upper bound */
      vector_set_linexpr(pk,matrel->p[row],
			 texpr[i],nnbdims,+1);
      vector_add_dimensions(pk,matrel->p[row],
			    (const numint_t*)matrel->p[row],nbcols,
			    dimchange);
      numint_neg(matrel->p[row][col], matrel->p[row][0]);
      numint_set_int(matrel->p[row][0],1);
      row++;
    }
  }
  matrel->nbrows = row;
  for (i=0; i<row; i++){
    vector_normalize_constraint_int(pk,matrel->p[i],
				    nintdim,nrealdim);
  }
  matrix_sort_rows(pk,matrel);
  return matrel;
}

/* ********************************************************************** */
/* III. Assignement/Substitution of a single dimension */
/* ********************************************************************** */

/* ====================================================================== */
/* Assignement/Substitution by a *deterministic* linear expression */
/* ====================================================================== */


poly_t* poly_asssub_linear_linexpr(bool assign,
				   ap_manager_t* man,
				   bool destructive,
				   poly_t* pa,
				   ap_dim_t dim, const ap_linexpr0_t* linexpr)
{
  int sgn;
  poly_t* po;
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  
  po = destructive ? pa : poly_alloc(pa->intdim,pa->realdim);

  if (!assign) poly_dual((poly_t*)pa);

 /* Convert linear expression */
  vector_set_linexpr(pk,pk->poly_numintp,
		     linexpr,
		     pa->intdim+pa->realdim,
		     0);
  sgn = numint_sgn(pk->poly_numintp[pk->dec + dim]);

  if (!sgn){ /* Expression is not invertible */
    /* Get the needed matrix */
    poly_obtain_F_dual(man,pa,"of the argument",assign);
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      poly_set_top(pk,po);
      man->result.flag_best = man->result.flag_exact = tbool_false;
      goto  _poly_asssub_linear_linexpr_exit;
    }
    if (destructive){
      /* If side-effect, free everything but generators */
      if (po->satC){ satmat_free(po->satC); po->satC = NULL; }
      if (po->satF){ satmat_free(po->satF); po->satF = NULL; }
      if (po->C){ matrix_free(po->C); po->C = NULL; }
    }
  }
  if (pa->F){
    /* Perform assignements on generators */
      po->F = 
	assign ?
	matrix_assign_variable(pk, destructive, pa->F, dim, pk->poly_numintp) :
	matrix_substitute_variable(pk, destructive, pa->F, dim, pk->poly_numintp);
  }
  if (sgn && pa->C){ /* Expression is invertible and we have constraints */
    /* Invert the expression in pk->poly_numintp2 */
    _poly_invert_vector(pk,
			pk->poly_numintp2,
			dim, (const numint_t*)pk->poly_numintp,
			pa->C->nbcolumns);
    /* Perform susbtitution on constraints */
    po->C =
      assign ?
      matrix_substitute_variable(pk,destructive,pa->C, dim, pk->poly_numintp2) :
      matrix_assign_variable(pk,destructive,pa->C, dim, pk->poly_numintp2);   
  }
  if (po->C && po->F){
    po->nbeq = pa->nbeq;
    po->nbline = pa->nbline;
  } else {
    po->nbeq = 0;
    po->nbline = 0;
  }
  po->status = 0;
_poly_asssub_linear_linexpr_exit:
  if (!assign){
    poly_dual((poly_t*)pa);
    if (!destructive) poly_dual(po);
  }
  assert(poly_check(pk,po));
  return po;
}

/* ====================================================================== */
/* Assignement/Substitution by a linear expression */
/* ====================================================================== */

poly_t* poly_asssub_linexpr(bool assign,
			    bool lazy,
			    ap_manager_t* man,
			    bool destructive,
			    poly_t* pa,
			    ap_dim_t dim, const ap_linexpr0_t* linexpr)
{
  poly_t* po;
  pk_internal_t* pk = (pk_internal_t*)man->internal;
  pk_internal_realloc_lazy(pk,pa->intdim+pa->realdim+1);
  
  /* Minimize the argument if option say so */
  if (!lazy){
    poly_chernikova(man,pa,"of the argument");
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      man->result.flag_best = man->result.flag_exact = tbool_false;
      if (destructive){
	poly_set_top(pk,pa);
	return pa;
      } else {
	return poly_top(man,pa->intdim,pa->realdim);
      }
    }
  }
  /* Return empty if empty */
  if (!pa->C && !pa->F){
    man->result.flag_best = man->result.flag_exact = tbool_true;
    return destructive ? pa : poly_bottom(man,pa->intdim,pa->realdim);
  }
  /* Choose the right technique */
  switch (linexpr->cst.discr){
  case AP_COEFF_SCALAR:
    po = poly_asssub_linear_linexpr(assign,man,destructive,pa,dim,linexpr);
    break;
  case AP_COEFF_INTERVAL:
    {
      ap_dim_t tdim[1];
      const ap_linexpr0_t* texpr[1];
      size_t intdimsup, realdimsup;
      
      tdim[0] = dim;
      texpr[0] = linexpr;
      intdimsup = dim < pa->intdim ? 1 : 0;
      realdimsup = dim < pa->intdim ? 0 : 1;
      po = poly_asssub_quasilinear_linexpr_array(assign,man,
						 destructive,pa,
						 tdim,texpr,
						 intdimsup,realdimsup);
    }
    break;
  default:
    abort();
  }
  /* Minimize the result if option say so */
  if (!lazy){
    poly_chernikova(man,po,"of the result");
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      man->result.flag_best = man->result.flag_exact = tbool_false;
      poly_set_top(pk,po);
      return po;
    }
  }
  /* Is the result exact or best ? */
  if (pk->funopt->flag_best_wanted || pk->funopt->flag_exact_wanted){
    man->result.flag_best = man->result.flag_exact = 
      (dim < pa->intdim || !ap_linexpr0_is_real(linexpr, pa->intdim)) ?
      tbool_top :
      tbool_true;
  }
  else {
    man->result.flag_best = man->result.flag_exact = 
      pa->intdim>0 ? tbool_top : tbool_true;
  }
  return po;
}

/* ********************************************************************** */
/* IV. Assignement/Substitution of several dimensions */
/* ********************************************************************** */

/* ====================================================================== */
/* Assignement/Substitution by several *deterministic* linear expressions */
/* ====================================================================== */

poly_t* poly_asssub_linear_linexpr_array(bool assign,
					 ap_manager_t* man,
					 bool destructive,
					 poly_t* pa,
					 const ap_dim_t* tdim, const ap_linexpr0_t** texpr, 
					 size_t size)
{
  int i;
  numint_t** tvec;
  size_t nbcols;
  matrix_t* mat;
  poly_t* po;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  po = destructive ? pa : poly_alloc(pa->intdim,pa->realdim);

  if (!assign) poly_dual((poly_t*)pa);

  /* Obtain the needed matrix */
  poly_obtain_F_dual(man,pa,"of the argument",assign);
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_best = man->result.flag_exact = tbool_false;
    poly_set_top(pk,po);
    goto _poly_asssub_linear_linexpr_array_exit;
  }
  /* Return empty if empty */
  if (!pa->C && !pa->F){
    man->result.flag_best = man->result.flag_exact = tbool_true;
    poly_set_bottom(pk,po);
    return po;
  }
  /* Convert linear expressions */
  nbcols = pk->dec + pa->intdim + pa->realdim;
  tvec = (numint_t**)malloc(size*sizeof(numint_t*));
  for (i=0; i<size; i++){
    tvec[i] = vector_alloc(nbcols);
    vector_set_linexpr(pk,tvec[i],texpr[i],
		       pa->intdim + pa->realdim,
		       0);
  }
  /* Perform the operation */
  mat = 
    assign ?
    matrix_assign_variables(pk, pa->F, tdim, tvec, size) :
    matrix_substitute_variables(pk, pa->F, tdim, tvec, size);
  /* Free allocated stuff */
  for (i=0; i<size; i++){
    vector_free(tvec[i],nbcols);
  }
  free(tvec);

  /* Update polyhedra */
  if (destructive){
    poly_clear(po);
  }
  po->F = mat;
  po->status = 0;
 _poly_asssub_linear_linexpr_array_exit:
  if (!assign){
    poly_dual((poly_t*)pa);
    if (!destructive) poly_dual(po);
  }
  assert(poly_check(pk,po));
  return po;
}

/* ====================================================================== */
/* Assignement/Substitution by several *non deterministic* linear expressions */
/* ====================================================================== */

/* DISTINGUER l'addition de dimensions entières ou réelles ! */

poly_t*
poly_asssub_quasilinear_linexpr_array(bool assign,
				      ap_manager_t* man,
				      bool destructive,
				      poly_t* pa,
				      const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
				      size_t intdimsup,
				      size_t realdimsup)
{
  bool res;
  size_t size;
  int i;
  matrix_t* matrel;
  ap_dimperm_t permutation;
  poly_t* po;
  ap_dimchange_t dimchange;

  pk_internal_t* pk = (pk_internal_t*)man->internal;
  size = intdimsup+realdimsup;
  pk_internal_realloc_lazy(pk,pa->intdim+pa->realdim+size);

  /* Minimize the argument */
  poly_chernikova(man,pa,"of the argument");
  if (pk->exn){
    pk->exn = AP_EXC_NONE;
    man->result.flag_best = man->result.flag_exact = tbool_false;
    return destructive ? pa : poly_top(man,pa->intdim,pa->realdim);
  }

  /* Return empty if empty */
  if (!pa->C && !pa->F){
    man->result.flag_best = man->result.flag_exact = tbool_true;
    return destructive ? pa : poly_bottom(man,pa->intdim,pa->realdim);
  }
  
  /* Build dimchange */
  ap_dimchange_init(&dimchange,intdimsup,realdimsup);
  for (i=0;i<intdimsup;i++) 
    dimchange.dim[i]=pa->intdim;
  for (i=intdimsup;i<intdimsup+realdimsup;i++) 
    dimchange.dim[i]=pa->intdim+pa->realdim;
  
  /* Build permutation exchanging primed and unprimed dimensions */
  ap_dimperm_init(&permutation,pa->intdim+pa->realdim+intdimsup+realdimsup);
  ap_dimperm_set_id(&permutation);
  for (i=0; i<size; i++){
    ap_dim_t dim = tdim[i];
    ap_dim_t dimp = dim<pa->intdim ? pa->intdim+i : pa->intdim+pa->realdim+i;
    permutation.dim[dim] = dimp;
    permutation.dim[dimp] = dim;
  }

  /* Add dimensions to polyhedra */
  po = poly_add_dimensions(man,destructive,pa,&dimchange);
  /* From now, work by side-effect on po */
  /* Permute unprimed and primed dimensions if !assign */
  if (!assign){
    po = poly_permute_dimensions(man,true,po,&permutation);
  }
  /* Perform intersection of po with matrel */
  matrel = _matrix_relation_of_tdimexpr(pk,
					po->intdim-intdimsup,
					po->realdim-realdimsup,
					tdim,texpr,
					&dimchange);
  poly_obtain_satC(po);
  res = _poly_meet_matrix(true,false,man,po,po,matrel);
  if (res){
    po->intdim -= intdimsup;
    po->realdim -= realdimsup;
    man->result.flag_best = man->result.flag_exact = tbool_false;
    poly_set_top(pk,po);
    goto _poly_asssub_quasilinear_linexpr_array_exit;
  }
  if (!po->C && !po->F){ /* possible if !assign */
    assert(!assign);
    po->intdim -= intdimsup;
    po->realdim -= realdimsup;
    man->result.flag_best = man->result.flag_exact = tbool_true;
    poly_set_bottom(pk,po);
    goto _poly_asssub_quasilinear_linexpr_array_exit;
  }
  /* Permute unprimed and primed dimensions if assign */
  if (assign){
    po = poly_permute_dimensions(man,true,po,&permutation);
  }
  /* Remove extra dimensions */
  ap_dimchange_add_invert(&dimchange);
  po = poly_remove_dimensions(man,true,po,&dimchange);
 _poly_asssub_quasilinear_linexpr_array_exit:
  ap_dimperm_clear(&permutation);
  ap_dimchange_clear(&dimchange);
  matrix_free(matrel);
  return po;
}

/* ====================================================================== */
/* Assignement/Substitution by an array of linear expressions */
/* ====================================================================== */

poly_t* poly_asssub_linexpr_array(bool assign, 
				  bool lazy,
				  ap_manager_t* man,
				  bool destructive,
				  poly_t* pa,
				  const ap_dim_t* tdim,
				  const ap_linexpr0_t** texpr,
				  size_t size)
{
  int i;
  size_t intdimsup,realdimsup;
  poly_t* po;
  pk_internal_t* pk = (pk_internal_t*)man->internal;

  /* Minimize the argument if option say so */
  if (!lazy){
    poly_chernikova(man,pa,"of the argument");
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      man->result.flag_best = man->result.flag_exact = tbool_false;
      if (destructive){
	poly_set_top(pk,pa);
	return pa;
      } else {
	return poly_top(man,pa->intdim,pa->realdim);
      }
    }
  }
  /* Return empty if empty */
  if (!pa->C && !pa->F){
    man->result.flag_best = man->result.flag_exact = tbool_true;
    return destructive ? pa : poly_bottom(man,pa->intdim,pa->realdim);
  }
  /* Choose the right technique */
  intdimsup = realdimsup = 0;
  for (i=0; i<size; i++){
    if (texpr[i]->cst.discr==AP_COEFF_INTERVAL){
      if (tdim[i]<pa->intdim) intdimsup++;
      else realdimsup++;
    }
  }
  if (intdimsup+realdimsup==0){
    po = poly_asssub_linear_linexpr_array(assign,man,destructive,pa,tdim,texpr,size);
  } else {
    po = poly_asssub_quasilinear_linexpr_array(assign,man,destructive,pa,
					       tdim,texpr,
					       intdimsup,realdimsup);
  }
  /* Minimize the result if option say so */
  if (!lazy){
    poly_chernikova(man,po,"of the result");
    if (pk->exn){
      pk->exn = AP_EXC_NONE;
      man->result.flag_best = man->result.flag_exact = tbool_false;
      poly_set_top(pk,po);
      return po;
    }
  }

  /* Is the result exact or best ? */
  if (pk->funopt->flag_best_wanted || pk->funopt->flag_exact_wanted){
    man->result.flag_best = tbool_true;
    for (i=0;i<size;i++){
      if (tdim[i] < pa->intdim || !ap_linexpr0_is_real(texpr[i], pa->intdim)){
	man->result.flag_best = tbool_top;
	break;
      }
    }
    man->result.flag_exact = man->result.flag_best;
  }
  else {
    man->result.flag_best = man->result.flag_exact = 
      pa->intdim>0 ? tbool_top : tbool_true;
  }
  return po;
}


/* ********************************************************************** */
/* V. Assignement/Substitution: interface */
/* ********************************************************************** */

poly_t* poly_assign_linexpr(ap_manager_t* man,
			    bool destructive, poly_t* pa, 
			    ap_dim_t dim, const ap_linexpr0_t* linexpr,
			    const poly_t* pb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_ASSIGN_LINEXPR);
  poly_t* po;
  po = poly_asssub_linexpr(true,
			   pk->funopt->algorithm<=0,
			   man,destructive,pa,dim,linexpr);
  if (pb!=NULL){
    po = poly_meet(man,true,po,pb);
  }
  return po;
}


poly_t* poly_assign_linexpr_array(ap_manager_t* man,
				  bool destructive, poly_t* pa,
				  const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
				  size_t size,
				  const poly_t* pb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_ASSIGN_LINEXPR_ARRAY);
  poly_t* po;
  po = poly_asssub_linexpr_array(true,
				 pk->funopt->algorithm<=0,
				 man,destructive,pa,tdim,texpr,size);
  if (pb!=NULL){
    po = poly_meet(man,true,po,pb);
  }
  return po;
}

poly_t* poly_substitute_linexpr(ap_manager_t* man,
				bool destructive, poly_t* pa, 
				ap_dim_t dim, const ap_linexpr0_t* linexpr,
				const poly_t* pb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_SUBSTITUTE_LINEXPR);
  poly_t* po;
  po = poly_asssub_linexpr(false,
			   pk->funopt->algorithm<=0,
			   man,destructive,pa,dim,linexpr);
  if (pb!=NULL){
    po = poly_meet(man,true,po,pb);
  }
  return po;
}


poly_t* poly_substitute_linexpr_array(ap_manager_t* man,
				      bool destructive, poly_t* pa,
				      const ap_dim_t* tdim, const ap_linexpr0_t** texpr,
				      size_t size,
				      const poly_t* pb)
{
  pk_internal_t* pk = pk_init_from_manager(man,AP_FUNID_SUBSTITUTE_LINEXPR_ARRAY);
  poly_t* po;
  po = poly_asssub_linexpr_array(false,
				 pk->funopt->algorithm<=0,
				 man,destructive,pa,tdim,texpr,size);
  if (pb!=NULL){
    po = poly_meet(man,true,po,pb);
  }
  return po;
}
