/* -*- C++ -*-
 * apxx_texpr0_inline.hh
 *
 * APRON Library / C++ inline functions
 *
 * DO NOT INCLUDE THIS FILE DIRECTLY
 *
 * Copyright (C) Antoine Mine' 2007
 *
 */
/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/


/* ================================= */
/* texpr0_node                       */
/* ================================= */

inline texpr0_node::texpr0_node(const texpr0_node& x)
{
  switch (x.l->discr) {
  case AP_TEXPR_DIM:  l = ap_texpr0_dim(x.l->val.dim); break;
  case AP_TEXPR_CST:  l = ap_texpr0_cst(const_cast<ap_coeff_t*>(&x.l->val.cst)); break;
  case AP_TEXPR_NODE: 
    l = ap_texpr0_node(x.l->val.node->op,
		       x.l->val.node->type, x.l->val.node->dir,
		       x.l->val.node->exprA, x.l->val.node->exprB);
    break;
  default: throw std::invalid_argument("texpr0_node::texpr0_node(const texpr0_node&)");
  }
}

inline texpr0_node::texpr0_node(const texpr0& x)
{
  switch (x.l.discr) {
  case AP_TEXPR_DIM:  l = ap_texpr0_dim(x.l.val.dim); break;
  case AP_TEXPR_CST:  l = ap_texpr0_cst(const_cast<ap_coeff_t*>(&x.l.val.cst)); break;
  case AP_TEXPR_NODE: 
    l = ap_texpr0_node(x.l.val.node->op,
		       x.l.val.node->type, x.l.val.node->dir,
		       x.l.val.node->exprA, x.l.val.node->exprB);
    break;
  default: throw std::invalid_argument("texpr0_node::texpr0_node(const texpr0&)");
  }
}

inline texpr0_node::texpr0_node(const coeff& x)
{ 
  l = ap_texpr0_cst(const_cast<ap_coeff_t*>(x.get_ap_coeff_t())); 
}

inline texpr0_node::texpr0_node(const scalar& x)
{ 
  l = ap_texpr0_cst_scalar(const_cast<ap_scalar_t*>(x.get_ap_scalar_t()));
}

inline texpr0_node::texpr0_node(const mpq_class& x)
{ 
  l = ap_texpr0_cst_scalar_mpq(const_cast<mpq_class&>(x).get_mpq_t());
}

inline texpr0_node::texpr0_node(int x)
{ 
  l = ap_texpr0_cst_scalar_int(x); 
}

inline texpr0_node::texpr0_node(long x)
{ 
  l = ap_texpr0_cst_scalar_int(x); 
}

inline texpr0_node::texpr0_node(double x)
{ 
  l = ap_texpr0_cst_scalar_double(x); 
}

inline texpr0_node::texpr0_node(const frac& x)
{ 
  l = ap_texpr0_cst_scalar_frac(x.num, x.den);
}

inline texpr0_node::texpr0_node(const interval& x)
{ 
  l = ap_texpr0_cst_interval(const_cast<ap_interval_t*>(x.get_ap_interval_t()));
}

inline texpr0_node::texpr0_node(const scalar& inf, const scalar& sup)
{ 
    
  l = ap_texpr0_cst_interval_scalar(const_cast<ap_scalar_t*>(inf.get_ap_scalar_t()),
					  const_cast<ap_scalar_t*>(sup.get_ap_scalar_t())); 
}

inline texpr0_node::texpr0_node(const mpq_class& inf, const mpq_class& sup)
{ 
  l = ap_texpr0_cst_interval_mpq(const_cast<mpq_class&>(inf).get_mpq_t(),
				 const_cast<mpq_class&>(sup).get_mpq_t()); 
}

inline texpr0_node::texpr0_node(int inf, int sup)
{
  l = ap_texpr0_cst_interval_int(inf, sup); 
}

inline texpr0_node::texpr0_node(long inf, long sup)
{
  l = ap_texpr0_cst_interval_int(inf, sup); 
}

inline texpr0_node::texpr0_node(double inf, double sup)
{
  l = ap_texpr0_cst_interval_double(inf, sup); 
}

inline texpr0_node::texpr0_node(const frac& inf, const frac& sup)
{
  l = ap_texpr0_cst_interval_frac(inf.num, inf.den, sup.num, sup.den); 
}

inline texpr0_node::texpr0_node(top t)
{
  l = ap_texpr0_cst_interval_top(); 
}

inline texpr0_node::texpr0_node(dim d)
{
  l = ap_texpr0_dim(d.d); 
}

inline texpr0_node::texpr0_node(ap_texpr_op_t op, const texpr0_node& argA, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{ 
  if (!ap_texpr_is_unop(op))
    throw std::invalid_argument("unary op in texpr0_node::texpr0_node");
  l = ap_texpr0_unop(op,
		     const_cast<ap_texpr0_t*>(argA.l),
		     rtype, rdir); 
}
  
inline texpr0_node::texpr0_node(ap_texpr_op_t op, const texpr0_node& argA, const texpr0_node& argB, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{ 
  if (!ap_texpr_is_binop(op))
    throw std::invalid_argument("binary op in texpr0_node::texpr0_node");
  l = ap_texpr0_binop(op,
		      const_cast<ap_texpr0_t*>(argA.l),
		      const_cast<ap_texpr0_t*>(argB.l),
		      rtype, rdir);
}


/* 'intelligent' constructors */

inline texpr0_node unary(ap_texpr_op_t op, const texpr0_node& a, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(op, a, rtype, rdir);
}

inline texpr0_node binary(ap_texpr_op_t op, const texpr0_node& a, const texpr0_node& b, 
		  ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(op, a, b, rtype, rdir);
}


inline texpr0_node add(const texpr0_node& a, const texpr0_node& b, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(AP_TEXPR_ADD, a, b, rtype, rdir);
}

inline texpr0_node sub(const texpr0_node& a, const texpr0_node& b, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(AP_TEXPR_SUB, a, b, rtype, rdir);
}

inline texpr0_node mul(const texpr0_node& a, const texpr0_node& b, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(AP_TEXPR_MUL, a, b, rtype, rdir);
}

inline texpr0_node div(const texpr0_node& a, const texpr0_node& b, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(AP_TEXPR_DIV, a, b, rtype, rdir);
}

inline texpr0_node mod(const texpr0_node& a, const texpr0_node& b, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(AP_TEXPR_MOD, a, b, rtype, rdir);
}

inline texpr0_node neg(const texpr0_node& a, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(AP_TEXPR_NEG, a, rtype, rdir);
}

inline texpr0_node cast(const texpr0_node& a, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(AP_TEXPR_CAST, a, rtype, rdir);
}

inline texpr0_node floor(const texpr0_node& a)
{
  return texpr0_node(AP_TEXPR_CAST, a, AP_RTYPE_INT, AP_RDIR_DOWN);
}

inline texpr0_node ceil(const texpr0_node& a)
{
  return texpr0_node(AP_TEXPR_CAST, a, AP_RTYPE_INT, AP_RDIR_UP);
}

inline texpr0_node trunc(const texpr0_node& a)
{
  return texpr0_node(AP_TEXPR_CAST, a, AP_RTYPE_INT, AP_RDIR_ZERO);
}

inline texpr0_node sqrt(const texpr0_node& a, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir)
{
  return texpr0_node(AP_TEXPR_SQRT, a, rtype, rdir);
}

inline texpr0_node operator+(const texpr0_node& a)
{
  return a;
}

inline texpr0_node operator-(const texpr0_node& a)
{
  return neg(a);
}

inline texpr0_node operator+(const texpr0_node& a, const texpr0_node& b)
{
  return add(a,b);
}

inline texpr0_node operator-(const texpr0_node& a, const texpr0_node& b)
{
  return sub(a,b);
}

inline texpr0_node operator*(const texpr0_node& a, const texpr0_node& b)
{
  return mul(a,b);
}

inline texpr0_node operator/(const texpr0_node& a, const texpr0_node& b)
{
  return div(a,b);
}

inline texpr0_node operator%(const texpr0_node& a, const texpr0_node& b)
{
  return mod(a,b);
}


/* tests */
/* ===== */

inline bool texpr0_node::is_zero() const
{
  return l->discr==AP_TEXPR_CST && ap_coeff_zero(const_cast<ap_coeff_t*>(&l->val.cst));
}


/* destructor */
/* ========== */

inline texpr0_node::~texpr0_node()
{
  switch(l->discr){
  case AP_TEXPR_CST:  ap_coeff_clear(&l->val.cst); break;
  case AP_TEXPR_DIM:  break;
  case AP_TEXPR_NODE: free(l->val.node); break;
  default: assert(false);
  }
  free(l);
}


/* C-level compatibility */
/* ===================== */

inline ap_texpr0_t* texpr0_node::get_ap_texpr0_t()
{
  return l;
}

inline const ap_texpr0_t* texpr0_node::get_ap_texpr0_t() const
{
  return l;
}



/* ================================= */
/* texpr0                            */
/* ================================= */

inline void texpr0::init_from(ap_texpr0_t* x)
{ 
  l = *x;
  free(x);
}

inline texpr0::texpr0(ap_texpr0_t* x) 
{ 
  init_from(x);
}

inline texpr0::texpr0(const texpr0_node& x)
{
  init_from(ap_texpr0_copy(const_cast<ap_texpr0_t*>(x.get_ap_texpr0_t())));
}


inline texpr0::texpr0(const texpr0& x)
{ 
  init_from(ap_texpr0_copy(const_cast<ap_texpr0_t*>(&x.l))); 
}


/* leaf */

inline texpr0::texpr0(const coeff& x)
{ 
  init_from(ap_texpr0_cst(const_cast<ap_coeff_t*>(x.get_ap_coeff_t()))); 
}

inline texpr0::texpr0(const scalar& x)
{ 
  init_from(ap_texpr0_cst_scalar(const_cast<ap_scalar_t*>(x.get_ap_scalar_t())));
}

inline texpr0::texpr0(const mpq_class& x)
{ 
  init_from(ap_texpr0_cst_scalar_mpq(const_cast<mpq_class&>(x).get_mpq_t()));
}

inline texpr0::texpr0(int x)
{ 
  init_from(ap_texpr0_cst_scalar_int(x)); 
}

inline texpr0::texpr0(long x)
{ 
  init_from(ap_texpr0_cst_scalar_int(x)); 
}

inline texpr0::texpr0(double x)
{ 
  init_from(ap_texpr0_cst_scalar_double(x)); 
}

inline texpr0::texpr0(const frac& x)
{ 
  init_from(ap_texpr0_cst_scalar_frac(x.num, x.den));
}

inline texpr0::texpr0(const interval& x)
{ 
  init_from(ap_texpr0_cst_interval(const_cast<ap_interval_t*>(x.get_ap_interval_t())));
}

inline texpr0::texpr0(const scalar& inf, const scalar& sup)
{ 
    
  init_from(ap_texpr0_cst_interval_scalar(const_cast<ap_scalar_t*>(inf.get_ap_scalar_t()),
					  const_cast<ap_scalar_t*>(sup.get_ap_scalar_t()))); 
}

inline texpr0::texpr0(const mpq_class& inf, const mpq_class& sup)
{ 
  init_from(ap_texpr0_cst_interval_mpq(const_cast<mpq_class&>(inf).get_mpq_t(),
				       const_cast<mpq_class&>(sup).get_mpq_t())); 
}

inline texpr0::texpr0(int inf, int sup)
{
  init_from(ap_texpr0_cst_interval_int(inf, sup)); 
}

inline texpr0::texpr0(long inf, long sup)
{
  init_from(ap_texpr0_cst_interval_int(inf, sup)); 
}

inline texpr0::texpr0(double inf, double sup)
{
  init_from(ap_texpr0_cst_interval_double(inf, sup)); 
}

inline texpr0::texpr0(const frac& inf, const frac& sup)
{
  init_from(ap_texpr0_cst_interval_frac(inf.num, inf.den, sup.num, sup.den)); 
}

inline texpr0::texpr0(top t)
{
  init_from(ap_texpr0_cst_interval_top()); 
}

inline texpr0::texpr0(dim d)
{
  init_from(ap_texpr0_dim(d.d)); 
}


/* linear expression */
  
inline texpr0::texpr0(const linexpr0& l)
{ 
  init_from(ap_texpr0_from_linexpr0(const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()))); 
}


/* change of dimension */
  
inline texpr0::texpr0(const texpr0& x, const dimchange& d, bool add)
{
  if (add)
    init_from
      (ap_texpr0_add_dimensions(const_cast<ap_texpr0_t*>(&x.l), 
				const_cast<ap_dimchange_t*>(d.get_ap_dimchange_t())));
  else
    init_from
      (ap_texpr0_remove_dimensions(const_cast<ap_texpr0_t*>(&x.l), 
				   const_cast<ap_dimchange_t*>(d.get_ap_dimchange_t())));
}

inline texpr0::texpr0(const texpr0& x, const dimperm& d)
{ 
  init_from
    (ap_texpr0_permute_dimensions(const_cast<ap_texpr0_t*>(&x.l), 
				  const_cast<ap_dimperm_t*>(d.get_ap_dimperm_t())));
}

inline texpr0::texpr0(const texpr0& x, ap_dim_t dim, const texpr0& dst)
{ 
  init_from(ap_texpr0_substitute(const_cast<ap_texpr0_t*>(&x.l),
				 dim, const_cast<ap_texpr0_t*>(&dst.l)));
}


/* destructor */
/* ========== */

/* (deep) destruction */

inline texpr0::~texpr0()
{
  ap_texpr0_clear(&l); 
}


/* assignment */
/* ========== */

/* (deep) copy */

inline texpr0& texpr0::operator= (const texpr0& x)
{ 
  if (&x!=this) { 
    ap_texpr0_clear(&l); 
    init_from(ap_texpr0_copy(const_cast<ap_texpr0_t*>(&x.l))); 
  } 
  return *this; 
}

inline texpr0& texpr0::operator= (const texpr0_node& x)
{ 
  // copy first, as x.l may alias *this!
  ap_texpr0_t* c = ap_texpr0_copy(const_cast<ap_texpr0_t*>(x.get_ap_texpr0_t()));
  ap_texpr0_clear(&l); 
  init_from(c);
  return *this; 
}

/* leaf */

inline texpr0& texpr0::operator= (const coeff& x)
{
  ap_texpr0_clear(&l); 
  init_from(ap_texpr0_cst(const_cast<ap_coeff_t*>(x.get_ap_coeff_t()))); 
  return *this; 
}

inline texpr0& texpr0::operator= (const scalar& x)
{
  ap_texpr0_clear(&l); 
  init_from(ap_texpr0_cst_scalar(const_cast<ap_scalar_t*>(x.get_ap_scalar_t()))); 
  return *this;
}

inline texpr0& texpr0::operator= (const mpq_class& x)
{ 
  ap_texpr0_clear(&l); 
  init_from(ap_texpr0_cst_scalar_mpq(const_cast<mpq_class&>(x).get_mpq_t()));
  return *this; 
}

inline texpr0& texpr0::operator= (int x)
{
  ap_texpr0_clear(&l); init_from(ap_texpr0_cst_scalar_int(x)); 
  return *this;
}

inline texpr0& texpr0::operator= (long x)
{ 
  ap_texpr0_clear(&l); 
  init_from(ap_texpr0_cst_scalar_int(x)); 
  return *this;
}

inline texpr0& texpr0::operator= (double x)
{ 
  ap_texpr0_clear(&l); 
  init_from(ap_texpr0_cst_scalar_double(x));
  return *this;
}

inline texpr0& texpr0::operator= (const frac& x)
{ 
  ap_texpr0_clear(&l);
  init_from(ap_texpr0_cst_scalar_frac(x.num, x.den)); 
  return *this; 
}

inline texpr0& texpr0::operator= (const interval& x)
{ 
  ap_texpr0_clear(&l); 
  init_from(ap_texpr0_cst_interval(const_cast<ap_interval_t*>(x.get_ap_interval_t())));
  return *this; 
}

inline texpr0& texpr0::operator= (top t)
{ 
  ap_texpr0_clear(&l);
  init_from(ap_texpr0_cst_interval_top()); 
  return *this;
}

inline texpr0& texpr0::operator= (dim d)
{ 
  ap_texpr0_clear(&l); 
  init_from(ap_texpr0_dim(d.d));
  return *this;
}


/* linear expression */

inline texpr0& texpr0::operator= (const linexpr0& x)
{ 
  ap_texpr0_clear(&l); 
  init_from(ap_texpr0_from_linexpr0(const_cast<ap_linexpr0_t*>(x.get_ap_linexpr0_t())));
  return *this;
}


/* access */
/* ====== */

inline const ap_texpr_discr_t& texpr0::get_discr() const
{
  return l.discr; 
}

inline coeff& texpr0::get_coeff()
{
  if (l.discr!=AP_TEXPR_CST) throw(bad_discriminant("texpr0::get_coeff"));
  return reinterpret_cast<coeff&>(l.val.cst);
}

inline const coeff& texpr0::get_coeff() const
{  
  if (l.discr!=AP_TEXPR_CST) throw(bad_discriminant("texpr0::get_coeff"));
  return reinterpret_cast<const coeff&>(l.val.cst);
}

inline ap_dim_t& texpr0::get_dim()
{  
  if (l.discr!=AP_TEXPR_DIM) throw(bad_discriminant("texpr0::get_dim"));
  return l.val.dim;
}
  
inline const ap_dim_t& texpr0::get_dim() const
{  
  if (l.discr!=AP_TEXPR_DIM) throw(bad_discriminant("texpr0::get_dim"));
  return l.val.dim;
}
  
inline ap_texpr_op_t& texpr0::get_op()
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_op"));
  return l.val.node->op;
}

inline const ap_texpr_op_t& texpr0::get_op() const
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_op"));
  return l.val.node->op;
}

inline ap_texpr_rtype_t& texpr0::get_rtype()
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_type"));
  return l.val.node->type;
}

inline const ap_texpr_rtype_t& texpr0::get_rtype() const
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_type"));
  return l.val.node->type;
}

inline ap_texpr_rdir_t& texpr0::get_rdir()
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_dir"));
  return l.val.node->dir;
}

inline const ap_texpr_rdir_t& texpr0::get_rdir() const
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_dir"));
  return l.val.node->dir;
}

inline texpr0& texpr0::get_argA()
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_argA"));
  if (!l.val.node->exprA) throw(std::out_of_range("texpr0::get_argA"));
  return reinterpret_cast<texpr0&>(*l.val.node->exprA);
}

inline const texpr0& texpr0::get_argA() const
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_argA"));
  if (!l.val.node->exprA) throw(std::out_of_range("texpr0::get_argA"));
  return reinterpret_cast<const texpr0&>(*l.val.node->exprA);
}

inline texpr0& texpr0::get_argB()
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_argB"));
  if (!l.val.node->exprB) throw(std::out_of_range("texpr0::get_argB"));
  return reinterpret_cast<texpr0&>(*l.val.node->exprB);
}

inline const texpr0& texpr0::get_argB() const
{  
  if (l.discr!=AP_TEXPR_NODE) throw(bad_discriminant("texpr0::get_argB"));
  if (!l.val.node->exprB) throw(std::out_of_range("texpr0::get_argB"));
  return reinterpret_cast<const texpr0&>(*l.val.node->exprB);
}

static inline bool is_unop(ap_texpr_op_t op)
{
  return ap_texpr_is_unop(op);
}

static inline bool is_binop(ap_texpr_op_t op)
{
  return ap_texpr_is_binop(op);
}


/* print */
/* ===== */

extern void apxx_texpr0_ostream(std::ostream& os, ap_texpr0_t* a, char** name_of_dim);

inline std::ostream& operator<< (std::ostream& os, const texpr0& s)
{
  apxx_texpr0_ostream(os, const_cast<ap_texpr0_t*>(&s.l), NULL);
  return os;
}

inline void texpr0::print(char** name_of_dim, FILE* stream) const
{
  ap_texpr0_fprint(stream, const_cast<ap_texpr0_t*>(&l), name_of_dim);
}


/* tests, size */
/* =========== */

inline bool texpr0::is_zero() const
{
  return l.discr==AP_TEXPR_CST && ap_coeff_zero(const_cast<ap_coeff_t*>(&l.val.cst));
}

inline bool equal (const texpr0& x, const texpr0& y)
{
  return ap_texpr0_equal(const_cast<ap_texpr0_t*>(&x.l), const_cast<ap_texpr0_t*>(&y.l)); 
}

#if 0 // overloaded to make constraints
inline bool operator== (const texpr0& x, const texpr0& y)
{
  return ap_texpr0_equal(const_cast<ap_texpr0_t*>(&x.l), const_cast<ap_texpr0_t*>(&y.l)); 
}

inline bool operator!= (const texpr0& x, const texpr0& y)
{ 
  return !ap_texpr0_equal(const_cast<ap_texpr0_t*>(&x.l), const_cast<ap_texpr0_t*>(&y.l));
}
#endif

inline size_t texpr0::depth() const
{ 
  return ap_texpr0_depth(const_cast<ap_texpr0_t*>(&l));
}

inline size_t texpr0::size() const
{ 
  return ap_texpr0_size(const_cast<ap_texpr0_t*>(&l)); 
}

inline ap_dim_t texpr0::max_dim() const
{
  return ap_texpr0_max_dim(const_cast<ap_texpr0_t*>(&l));
}

inline bool texpr0::has_dim(ap_dim_t d) const
{
  return ap_texpr0_has_dim(const_cast<ap_texpr0_t*>(&l), d);
}

inline std::vector<ap_dim_t> texpr0::dimlist() const
{
  ap_dim_t* d = ap_texpr0_dimlist(const_cast<ap_texpr0_t*>(&l));
  ap_dim_t i;
  for (i=0; d[i]!=AP_DIM_MAX; i++);
  std::vector<ap_dim_t> r = std::vector<ap_dim_t>(i,0);
  for (i=0; d[i]!=AP_DIM_MAX; i++) r[i] = d[i];
  free(d);
  return r;
}

inline bool texpr0::is_interval_cst() const
{ 
  return ap_texpr0_is_interval_cst(const_cast<ap_texpr0_t*>(&l));
}

inline bool texpr0::is_interval_linear() const
{
  return ap_texpr0_is_interval_linear(const_cast<ap_texpr0_t*>(&l));
}

inline bool texpr0::is_interval_polynomial() const
{
  return ap_texpr0_is_interval_polynomial(const_cast<ap_texpr0_t*>(&l));
}

inline bool texpr0::is_interval_polyfrac() const
{ 
  return ap_texpr0_is_interval_polyfrac(const_cast<ap_texpr0_t*>(&l)); 
}

inline bool texpr0::is_scalar() const
{ 
  return ap_texpr0_is_scalar(const_cast<ap_texpr0_t*>(&l));
}


/* operations */
/* ========== */

inline void texpr0::substitute(ap_dim_t dim, const texpr0& dst)
{ 
  ap_texpr0_substitute_with(&l,dim,const_cast<ap_texpr0_t*>(&dst.l)); 
}

#if 0
inline interval texpr0::eval(manager& m, const abstract0& a, ap_scalar_discr_t discr,
			     bool* pexact) const
{
  bool b;
  if (!pexact) pexact = &b;
  return
    ap_eval_texpr0(m.get_ap_manager_t(),
		   const_cast<ap_abstract0_t*>(a.get_ap_abstract0_t()),
		   const_cast<ap_texpr0_t*>(&l),
		   discr, pexact);
}

inline linexpr0 texpr0::intlinearize(manager& m, const abstract0& a, ap_scalar_discr_t discr, 
				     bool quasilinearize, bool* pexact) const
{
  bool b;
  if (!pexact) pexact = &b;
  return
    ap_intlinearize_texpr0(m.get_ap_manager_t(),
			   const_cast<ap_abstract0_t*>(a.get_ap_abstract0_t()),
			   const_cast<ap_texpr0_t*>(&l),
			   pexact, discr, quasilinearize);
}
#endif

inline long texpr0::hash() const
{
  return ap_texpr0_hash(const_cast<ap_texpr0_t*>(&l));
}



/* change of dimension */
/* =================== */


inline void texpr0::add_dimensions(const dimchange& d)
{ 
  ap_texpr0_add_dimensions_with(&l, const_cast<ap_dimchange_t*>(d.get_ap_dimchange_t()));
}


inline void texpr0::remove_dimensions(const dimchange& d)
{ 
  ap_texpr0_remove_dimensions_with(&l, const_cast<ap_dimchange_t*>(d.get_ap_dimchange_t())); 
}
  
inline void texpr0::permute_dimensions(const dimperm& d)
{ 
  ap_texpr0_permute_dimensions_with(&l, const_cast<ap_dimperm_t*>(d.get_ap_dimperm_t())); 
}
  


/* C-level compatibility */
/* ===================== */

inline ap_texpr0_t* texpr0::get_ap_texpr0_t()
{ 
  return &l; 
}

inline const ap_texpr0_t* texpr0::get_ap_texpr0_t() const
{ 
  return &l; 
}
