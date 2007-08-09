/* -*- C++ -*-
 * apxx_linexpr0_inline.hh
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
/* linexpr0                          */
/* ================================= */

static inline void apxx_linexpr0_init(ap_linexpr0_t* d, ap_linexpr_discr_t discr, size_t size)
{
  d->discr = discr;
  d->size = 0;
  ap_coeff_init(&d->cst,AP_COEFF_SCALAR);
  if (discr==AP_LINEXPR_DENSE) d->p.coeff = NULL;
  else d->p.linterm = NULL;
  if (size) ap_linexpr0_realloc(d,size);
}

static inline void apxx_linexpr0_clear(ap_linexpr0_t* d)
{ 
  ap_linexpr0_realloc(d, 0);
  ap_coeff_clear(&d->cst);
}

static inline void apxx_linexpr0_copy(ap_linexpr0_t* d, const ap_linexpr0_t* s)
{
  if (d==s) return;
  assert(d->discr==s->discr);
  ap_linexpr0_realloc(d,s->size);
  ap_coeff_set(&d->cst, const_cast<ap_coeff_t*>(&s->cst));
  if (d->discr==AP_LINEXPR_DENSE) {
    for (size_t i=0; i<d->size; i++)
      ap_coeff_set(&d->p.coeff[i], &s->p.coeff[i]);
  }
  else {
    for (size_t i=0; i<d->size; i++) {
      ap_coeff_set(&d->p.linterm[i].coeff, &s->p.linterm[i].coeff);
      d->p.linterm[i].dim = s->p.linterm[i].dim;
    }
  }
}

/* constructors */
/* ============ */

inline linexpr0::linexpr0(ap_linexpr0_t* p) : l(*p) 
{
  free(p); 
}

inline linexpr0::linexpr0(ap_linexpr_discr_t discr, size_t size)
{ 
  apxx_linexpr0_init(&l, discr, size);
}

inline linexpr0::linexpr0(const linexpr0& x)
{ 
  apxx_linexpr0_init(&l, x.l.discr, x.l.size); 
  apxx_linexpr0_copy(&l, &x.l);
}

inline linexpr0::linexpr0(const linexpr0& x, const dimchange& d)
{
  ap_linexpr0_t* p;
  p = ap_linexpr0_add_dimensions(const_cast<ap_linexpr0_t*>(&x.l), 
				 const_cast<ap_dimchange_t*>(d.get_ap_dimchange_t()));
  l = *p;
  free(p);
}

inline linexpr0::linexpr0(const linexpr0& x, const dimperm& d)
{
  ap_linexpr0_t* p;
  p = ap_linexpr0_permute_dimensions(const_cast<ap_linexpr0_t*>(&x.l), 
				     const_cast<ap_dimperm_t*>(d.get_ap_dimperm_t()));
  l = *p;
  free(p);  
}

inline linexpr0::linexpr0(size_t size, const coeff coeffs[], const coeff& cst, ap_linexpr_discr_t discr)
{
  apxx_linexpr0_init(&l, discr, size);
  for (size_t i=0;i<size;i++) (*this)[i] = coeffs[i];
  this->get_cst() = cst;
}
  
inline linexpr0::linexpr0(const std::vector<coeff>& coeffs, const coeff& cst, ap_linexpr_discr_t discr)
{
  size_t size = coeffs.size();
  apxx_linexpr0_init(&l, discr, size);
  for (size_t i=0;i<size;i++) (*this)[i] = coeffs[i];
  this->get_cst() = cst;
}

inline linexpr0::linexpr0(size_t size, const coeff coeffs[], const ap_dim_t dims[], const coeff& cst)
{
  apxx_linexpr0_init(&l, AP_LINEXPR_SPARSE, size);
  for (size_t i=0;i<size;i++) (*this)[dims[i]] = coeffs[i];
  this->get_cst() = cst;
}


/* destructor */
/* ========== */

inline linexpr0::~linexpr0()
{
  apxx_linexpr0_clear(&l);
}


/* assignment */
/* ========== */

inline linexpr0& linexpr0::operator= (const linexpr0& x)
{ 
  if (&x!=this) { 
    apxx_linexpr0_clear(&l); 
    apxx_linexpr0_init(&l, x.l.discr, x.l.size); 
    apxx_linexpr0_copy(&l, &x.l); 
  }
  return *this;
}


/* dimension operations */
/* ==================== */

inline void linexpr0::resize(size_t size)
{ 
  ap_linexpr0_realloc(&l, size); 
}

inline void linexpr0::add_dimensions(const dimchange& d)
{ 
  ap_linexpr0_add_dimensions_with(&l, const_cast<ap_dimchange_t*>(d.get_ap_dimchange_t()));
}

inline void linexpr0::permute_dimensions(const dimperm& d)
{ 
  ap_linexpr0_permute_dimensions_with(&l, const_cast<ap_dimperm_t*>(d.get_ap_dimperm_t()));
}
  

/* access */
/* ====== */

/* size */

inline size_t linexpr0::get_size() const
{ 
  return ap_linexpr0_size(const_cast<ap_linexpr0_t*>(&l)); 
}


/* get */
 
inline ap_linexpr_discr_t linexpr0::get_discr() const
{ 
  return l.discr; 
}


inline coeff& linexpr0::get_cst()
{ 
  return reinterpret_cast<coeff&>(*ap_linexpr0_cstref(const_cast<ap_linexpr0_t*>(&l))); 
}
 
inline const coeff& linexpr0::get_cst() const
{ 
  return reinterpret_cast<coeff&>(*ap_linexpr0_cstref(const_cast<ap_linexpr0_t*>(&l))); 
}


inline coeff& linexpr0::get_coeff(ap_dim_t dim)
{ 
  ap_coeff_t* x = ap_linexpr0_coeffref(&l, dim);
  if (!x) throw std::out_of_range("linexpr0::get_coeff");
  return reinterpret_cast<coeff&>(*x); 
}
  
inline coeff& linexpr0::operator[](int dim)
{ 
  ap_coeff_t* x = dim < 0 ? ap_linexpr0_cstref(&l) : ap_linexpr0_coeffref(&l, dim);
  if (!x) throw std::out_of_range("linexpr0::[]");
  return reinterpret_cast<coeff&>(*x); 
}

inline const coeff& linexpr0::operator[](int dim) const
{ 
  const ap_coeff_t* x = dim < 0 ? 
    ap_linexpr0_cstref(const_cast<ap_linexpr0_t*>(&l)) : 
    ap_linexpr0_coeffref(const_cast<ap_linexpr0_t*>(&l), dim);
  if (!x) throw std::out_of_range("linexpr0::[]");
  return reinterpret_cast<const coeff&>(*x);
}
  

/* print */
/* ===== */

static inline bool print_coeff_sign(std::ostream& os, const coeff& c, bool& first)
{
  if (c.is_zero()) return false;
  if (first) os << c;
  else {
    if (c.get_discr()==AP_COEFF_SCALAR && c.get_scalar().sgn()<0) os << " - " << -c;
    else os << " + " << c;
  }
  first = false;
  return true;
}

inline std::ostream& operator<<(std::ostream& os, const linexpr0& s)
{
  bool first = true;
  for (linexpr0::iterator i=const_cast<linexpr0&>(s).begin();!i.end();i++) {
    if (print_coeff_sign(os, i.get_coeff(), first))
      os << "x" << i.get_dim();
  }
  print_coeff_sign(os, s.get_cst(), first);
  if (first) os << "0";
  return os;
}

inline void linexpr0::print(char** name_of_dim, FILE* stream) const
{ 
  ap_linexpr0_fprint(stream, const_cast<ap_linexpr0_t*>(&l), name_of_dim);
}


/* tests */
/* ===== */

inline bool linexpr0::is_integer(size_t intdim) const
{ 
  return ap_linexpr0_is_integer(const_cast<ap_linexpr0_t*>(&l), intdim); 
}

inline bool linexpr0::is_real(size_t intdim) const
{ 
  return ap_linexpr0_is_real(const_cast<ap_linexpr0_t*>(&l), intdim); 
}

inline bool linexpr0::is_linear() const
{ 
  return ap_linexpr0_is_linear(const_cast<ap_linexpr0_t*>(&l)); 
}

inline bool linexpr0::is_quasilinear() const
{ 
  return ap_linexpr0_is_quasilinear(const_cast<ap_linexpr0_t*>(&l));
}

inline int compare(const linexpr0& x, const linexpr0& y)
{ 
  return ap_linexpr0_compare(const_cast<ap_linexpr0_t*>(&x.l), 
			     const_cast<ap_linexpr0_t*>(&y.l)); 
}

inline bool equal (const linexpr0& x, const linexpr0& y)
{ 
  return ap_linexpr0_equal(const_cast<ap_linexpr0_t*>(&x.l), 
			   const_cast<ap_linexpr0_t*>(&y.l));
}

#if 0 // overloaded to make constraints

inline bool operator>= (const linexpr0& x, const linexpr0& y)
{ 
  return ap_linexpr0_compare(const_cast<ap_linexpr0_t*>(&x.l), 
			     const_cast<ap_linexpr0_t*>(&y.l)) >= 0; }

inline bool operator<= (const linexpr0& x, const linexpr0& y)
{ 
  return ap_linexpr0_compare(const_cast<ap_linexpr0_t*>(&x.l), 
			     const_cast<ap_linexpr0_t*>(&y.l)) <= 0;
}

inline bool operator> (const linexpr0& x, const linexpr0& y)
{
  return ap_linexpr0_compare(const_cast<ap_linexpr0_t*>(&x.l),
			     const_cast<ap_linexpr0_t*>(&y.l)) > 0; 
}

inline bool operator< (const linexpr0& x, const linexpr0& y)
{ 
  return ap_linexpr0_compare(const_cast<ap_linexpr0_t*>(&x.l), 
			     const_cast<ap_linexpr0_t*>(&y.l)) < 0;
}

inline bool operator== (const linexpr0& x, const linexpr0& y)
{ 
  return ap_linexpr0_equal(const_cast<ap_linexpr0_t*>(&x.l), 
			   const_cast<ap_linexpr0_t*>(&y.l));
}

inline bool operator!= (const linexpr0& x, const linexpr0& y)
{
  return !ap_linexpr0_equal(const_cast<ap_linexpr0_t*>(&x.l), 
			    const_cast<ap_linexpr0_t*>(&y.l)); 
}

#endif

/* iterators */
/* ========= */

inline void linexpr0::iterator::skip_AP_DIM_MAX()
{ 
  if (l->discr == AP_LINEXPR_DENSE) return;
  while (pos < l->size && l->p.linterm[pos].dim == AP_DIM_MAX) pos++;
}

inline linexpr0::iterator::iterator(ap_linexpr0_t* l) : l(l), pos(0) 
{ 
  skip_AP_DIM_MAX(); 
}
    
inline ap_dim_t linexpr0::iterator::get_dim() const
{
  if (pos >= l->size) throw std::out_of_range("linexpr0::iterator::get_dim");
  if (l->discr == AP_LINEXPR_DENSE) return pos;
  else return l->p.linterm[pos].dim;
}

inline coeff& linexpr0::iterator::get_coeff() const
{
  if (pos >= l->size) throw std::out_of_range("linexpr0::iterator::get_gcoeff");
  if (l->discr == AP_LINEXPR_DENSE) return reinterpret_cast<coeff&>(l->p.coeff[pos]);
  else return reinterpret_cast<coeff&>(l->p.linterm[pos].coeff);
}

inline void linexpr0::iterator::operator++ (int)
{
  pos++; 
  skip_AP_DIM_MAX(); 
}

inline bool linexpr0::iterator::end() const
{ 
  return pos >= l->size;
}

inline linexpr0::iterator linexpr0::begin()
{
  return iterator(&l); 
}
  


/* other operators */
/* =============== */

inline void linexpr0::minimize()
{
  ap_linexpr0_minimize(&l);
}

inline long linexpr0::hash() const
{
  return ap_linexpr0_hash(const_cast<ap_linexpr0_t*>(&l)); 
}



/* C-level compatibility */
/* ===================== */

inline const ap_linexpr0_t* linexpr0::get_ap_linexpr0_t() const
{
  return &l; 
}

inline ap_linexpr0_t* linexpr0::get_ap_linexpr0_t()
{
  return &l; 
}
