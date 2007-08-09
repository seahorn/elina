/*
 * apronxx.cc
 *
 * APRON Library / C++ class wrappers
 *
 * Copyright (C) Antoine Mine' 2007
 *
 */

/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#include "apronxx.hh"

namespace apron {

static const char* apxx_texpr_op_name[] =
  { "+", "-", "*", "/", "%", /* binary */
    "-", "cast", "sqrt",     /* unary */
  };

static const int apxx_texpr_op_precedence[] =
  { 1, 1, 2, 2, 2,  /* binary */
    3, 4, 4         /* unary */
  };    

static const char* apxx_texpr_rtype_name[] =
  { "", "i", "f", "d", "l", "q", };

static const char* apxx_texpr_rdir_name[] =
  { "n", "0", "+oo", "-oo", "?", "", };

/* node induces some rounding (to float or integer) */
static inline bool apxx_texpr0_node_exact(ap_texpr0_node_t* a)
{
  if (a->op==AP_TEXPR_NEG || a->op==AP_TEXPR_MOD ||
      a->type==AP_RTYPE_REAL) return true;
  return false;
}

static inline int apxx_texpr0_precedence(ap_texpr0_t* a)
{
  if (!a || a->discr!=AP_TEXPR_NODE) return apxx_texpr_op_precedence[AP_TEXPR_NEG];
  return apxx_texpr_op_precedence[a->val.node->op];
}

static void apxx_texpr0_node_ostream(std::ostream& os, ap_texpr0_node_t* a, 
				   char** name_of_dim)
{
  int prec = apxx_texpr_op_precedence[a->op];

  /* left argument (if binary) */
  if (a->exprB) {
    int prec2 = apxx_texpr0_precedence(a->exprA);
    if (prec2<prec) os << "(";
    apxx_texpr0_ostream(os, a->exprA, name_of_dim);
    if (prec2<prec) os << ")";
  }

  /* operator & rounding mode */
  if (a->exprB) os << " ";
  os << apxx_texpr_op_name[a->op];
  if (!apxx_texpr0_node_exact(a))
    os << "_" << apxx_texpr_rtype_name[a->type] 
       << "," << apxx_texpr_rdir_name[a->dir];
  
  /* right argument */
  ap_texpr0_t* arg = a->exprB ? a->exprB : a->exprA;
  int prec2 = apxx_texpr0_precedence(arg);
  if (a->exprB) os << " ";
  if (prec2<=prec) os << "(";
  apxx_texpr0_ostream(os,arg,name_of_dim);
  if (prec2<=prec) os << ")";
}

void apxx_texpr0_ostream(std::ostream& os, ap_texpr0_t* a, char** name_of_dim)
{
  if (!a) return;
  switch (a->discr) {
  case AP_TEXPR_CST:
    os <<  reinterpret_cast<coeff&>(a->val.cst);
    break;
  case AP_TEXPR_DIM:
    if (name_of_dim) os << name_of_dim[a->val.dim];
    else             os << "x" << a->val.dim;
    break;
  case AP_TEXPR_NODE:
    apxx_texpr0_node_ostream(os, a->val.node, name_of_dim);
    break;
  default:
    assert(false);
  }
}

}
