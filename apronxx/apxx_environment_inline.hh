/* -*- C++ -*-
 * apxx_environment_inline.hh
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


// shallow copy (does not call ap_var_operations->copy)
static inline ap_var_t* apxx_environment_get_names(const std::vector<var>& n)
{
  ap_var_t* r = (ap_var_t*)malloc(sizeof(ap_var_t)*n.size());
  assert(r);
  for (size_t i; i<n.size(); i++)  r[i] = n[i].get_ap_var_t();
  return r;
}

// shallow copy (does not call ap_var_operations->copy)
static inline void apxx_environment_get_names_pair(const std::vector<std::pair<var,var> >& n, ap_var_t** x, ap_var_t** y)
{
  *x = (ap_var_t*)malloc(sizeof(ap_var_t)*n.size());
  *y = (ap_var_t*)malloc(sizeof(ap_var_t)*n.size());
  assert(x && y);
  for (size_t i; i<n.size(); i++)  {
    (*x)[i] = n[i].first.get_ap_var_t();
    (*y)[i] = n[i].second.get_ap_var_t();
  }
}


/* constructor */
/* =========== */

inline environment::environment() 
  : e(ap_environment_alloc_empty())
{
}

inline environment::environment(const std::vector<var>& intdim, const std::vector<var>& realdim)
{
  ap_var_t *i = apxx_environment_get_names(intdim);
  ap_var_t *r = apxx_environment_get_names(realdim);
  e = ap_environment_alloc(i, intdim.size(), r, realdim.size());
  free(i);  free(r);
  if (!e) throw std::invalid_argument("environment::environment");
}

inline environment::environment(const environment& x) 
  : e(ap_environment_copy(x.e))
{
}

inline environment::environment(const environment& x, const std::vector<var>& intdim, const std::vector<var>& realdim)
{
  ap_var_t *i = apxx_environment_get_names(intdim);
  ap_var_t *r = apxx_environment_get_names(realdim);
  e = ap_environment_add(x.e, i, intdim.size(), r, realdim.size());
  free(i); free(r);
  if (!e) throw std::invalid_argument("environment::environment");
}

inline environment::environment(const environment& x, const std::vector<var>& dims)
{
  ap_var_t *d = apxx_environment_get_names(dims);
  e = ap_environment_remove(x.e, d, dims.size());
  free(d);
  if (!e) throw std::invalid_argument("environment::environment");
}

inline environment::environment(const environment& x, const std::vector<std::pair<var,var> >& dims)
{
  ap_var_t *a, *b;
  ap_dimperm_t p;
  apxx_environment_get_names_pair(dims, &a, &b);
  e = ap_environment_rename(x.e, a, b, dims.size(), &p);
  free(a); free(b); ap_dimperm_clear(&p);
  if (!e) throw std::invalid_argument("environment::environment");
}

inline environment::environment(const environment& x, const environment& y)
{
  ap_dimchange_t *a = NULL, *b = NULL;
  e = ap_environment_lce(x.e, y.e, &a, &b);
  if (a) ap_dimchange_free(a); 
  if (b) ap_dimchange_free(b);
  if (!e) throw std::invalid_argument("environment::environment");
}

inline environment::environment(const std::vector<environment>& x)
{
  ap_environment_t** ar = (ap_environment_t**)malloc(sizeof(ap_environment_t*)*x.size());
  ap_dimchange_t** r;
  assert(ar);
  for (size_t i=0; i<x.size(); i++) ar[i] = x[i].e;
  e = ap_environment_lce_array(ar, x.size(), &r);
  free(ar);
  if (r) {
    for (size_t i=0; i<x.size(); i++)
      if (r[i]) ap_dimchange_free(r[i]);
    free(r);
  }
  if (!e) throw std::invalid_argument("environment::environment");
}

inline environment::~environment()
{
  ap_environment_free(e);
}


/* assignment */
/* ========== */

inline const environment& environment::operator=(const environment& x)
{
  ap_environment_t* ee = ap_environment_copy(x.e);
  ap_environment_free(e);
  e = ee;
  return *this;
}


/* operations */
/* ========== */


inline environment environment::add(const std::vector<var>& intdim, const std::vector<var>& realdim) const
{
  ap_var_t *i = apxx_environment_get_names(intdim);
  ap_var_t *r = apxx_environment_get_names(realdim);
  ap_environment_t* e = ap_environment_add(e, i, intdim.size(), r, realdim.size());
  free(i); free(r);
  if (!e) throw std::invalid_argument("environment::add");
  return e;
}

inline environment environment::add(const std::vector<var>& intdim, const std::vector<var>& realdim, dimperm& perm) const
{
  ap_var_t *i = apxx_environment_get_names(intdim);
  ap_var_t *r = apxx_environment_get_names(realdim);
  ap_dimperm_t p;
  ap_environment_t* e = ap_environment_add_perm(e, i, intdim.size(), r, realdim.size(), &p);
  free(i); free(r);
  if (!e) throw std::invalid_argument("environment::add");
  else {
    ap_dimperm_clear(perm.get_ap_dimperm_t());
    *perm.get_ap_dimperm_t() = p;
  }
  return e;
}

inline environment environment::remove(const std::vector<var>& dims) const
{
  ap_var_t *d = apxx_environment_get_names(dims);
  ap_environment_t* e = ap_environment_remove(e, d, dims.size());
  free(d);
  if (!e) throw std::invalid_argument("environment::remove");
  return e;
}

inline environment environment::rename(const std::vector<std::pair<var,var> >& dims) const
{
  ap_var_t *a, *b;
  ap_dimperm_t p;
  apxx_environment_get_names_pair(dims, &a, &b);
  ap_environment_t* e = ap_environment_rename(e, a, b, dims.size(), &p);
  free(a); free(b); ap_dimperm_clear(&p);
  if (!e) throw std::invalid_argument("environment::rename");
  return e;
}

inline environment environment::rename(const std::vector<std::pair<var,var> >& dims, dimperm& perm) const
{
  ap_var_t *a, *b;
  ap_dimperm_t p;
  apxx_environment_get_names_pair(dims, &a, &b);
  ap_dimperm_clear(perm.get_ap_dimperm_t());
  ap_environment_t* e = ap_environment_rename(e, a, b, dims.size(), &p);
  free(a); free(b);
  if (!e) throw std::invalid_argument("environment::rename");
  else {
    ap_dimperm_clear(perm.get_ap_dimperm_t());
    *perm.get_ap_dimperm_t() = p;
  }
  return e;
}

inline environment lce(const environment& x, const environment& y)
{
  ap_dimchange_t *a = NULL, *b = NULL;
  ap_environment_t* e = ap_environment_lce(x.e, y.e, &a, &b);
  if (a) ap_dimchange_free(a); 
  if (b) ap_dimchange_free(b);
  if (!e) throw std::invalid_argument("environment::lce");
  return e;
}

inline environment lce(const environment& x, const environment& y, dimchange& chgx, dimchange& chgy)
{
  ap_dimchange_t *a = NULL, *b = NULL;
  ap_environment_t* e = ap_environment_lce(x.e, y.e, &a, &b);
  if (a) {
    ap_dimchange_clear(chgx.get_ap_dimchange_t());
    *chgx.get_ap_dimchange_t() = *a;
    free(a);
  }
  if (b) {
    ap_dimchange_clear(chgy.get_ap_dimchange_t());
    *chgy.get_ap_dimchange_t() = *b;
    free(b);
  }
  if (!e) throw std::invalid_argument("environment::lce");
  return e;
}

inline environment lce(const std::vector<environment>& x)
{
  ap_environment_t** ar = (ap_environment_t**)malloc(sizeof(ap_environment_t*)*x.size());
  ap_dimchange_t** r;
  assert(ar);
  for (size_t i=0; i<x.size(); i++) ar[i] = x[i].e;
  ap_environment_t* e = ap_environment_lce_array(ar, x.size(), &r);
  free(ar);
  if (r) {
    for (size_t i=0; i<x.size(); i++)
      if (r[i]) ap_dimchange_free(r[i]);
    free(r);
  }
  if (!e) throw std::invalid_argument("environment::lce");
  return e;
}

inline environment lce(const std::vector<environment>& x, std::vector<dimchange>& chg)
{
  if (x.size()!=chg.size()) throw std::invalid_argument("environment::lce");
  ap_environment_t** ar = (ap_environment_t**)malloc(sizeof(ap_environment_t*)*x.size());
  ap_dimchange_t** r;
  assert(ar);
  for (size_t i=0; i<x.size(); i++) ar[i] = x[i].e;
  ap_environment_t* e = ap_environment_lce_array(ar, x.size(), &r);
  free(ar);
  if (r) {
    for (size_t i=0; i<x.size(); i++) {
      ap_dimchange_clear(chg[i].get_ap_dimchange_t());
      if (r[i]) { 
	*chg[i].get_ap_dimchange_t() = *(r[i]);
	free(r[i]);
      }
      else {
	ap_dimchange_init(chg[i].get_ap_dimchange_t(), 0, 0);
      }
    }
    free(r);
  }
  if (!e) throw std::invalid_argument("environment::lce");
  return e;
}

inline dimchange get_dimchange(const environment& x, const environment& y)
{
  ap_dimchange_t* d = ap_environment_dimchange(x.e,y.e);
  if (!d) throw std::invalid_argument("environment::dimchange");
  dimchange c;
  *c.get_ap_dimchange_t() = *d;
  free(d);
  return c;
}


/* access */
/* ====== */

inline size_t environment::intdim() const
{
  return e->intdim;
}

inline size_t environment::realdim() const
{
  return e->realdim;
}

inline bool environment::contains(const var& x) const
{
  return ap_environment_mem_var(e, x.get_ap_var_t());
}

inline ap_dim_t environment::operator[] (const var& x) const
{
  ap_dim_t d = ap_environment_dim_of_var(e, x.get_ap_var_t());
  if (d==AP_DIM_MAX) throw std::invalid_argument("environment::operator[]");
}

inline const var& environment::operator[] (ap_dim_t d) const
{
  return *reinterpret_cast<var*>(&e->var_of_dim[d]);
}

inline ap_dim_t environment::get_dim(const var& x) const
{
  ap_dim_t d = ap_environment_dim_of_var(e, x.get_ap_var_t());
  if (d==AP_DIM_MAX) throw std::invalid_argument("environment::get_dim");
}

inline const var& environment::get_var(ap_dim_t d) const
{
  if (d >= e->intdim+e->realdim) throw std::invalid_argument("environment::get_name");
  return *reinterpret_cast<var*>(&e->var_of_dim[d]);
}

inline std::vector<var> environment::get_vars() const
{
  std::vector<var> v = std::vector<var>(e->intdim+e->realdim, NULL);
  for (size_t i=0; i<e->intdim+e->realdim; i++)
    v[i].get_ap_var_t() = ap_var_operations->copy(e->var_of_dim[i]);
  return v;
}


/* Tests */
/* ===== */

inline bool operator==(const environment& x, const environment& y)
{
  return ap_environment_is_eq(x.e, y.e);
}

inline bool operator!=(const environment& x, const environment& y)
{
 return !ap_environment_is_eq(x.e, y.e);
}

inline bool operator<=(const environment& x, const environment& y)
{
 return ap_environment_is_leq(x.e, y.e);
}

inline bool operator>=(const environment& x, const environment& y)
{
 return ap_environment_is_leq(y.e, x.e);
}

inline int cmp(const environment& x, const environment& y)
{
  return ap_environment_compare(x.e, y.e);
}


/* print */

inline std::ostream& operator<< (std::ostream& os, const environment& s)
{
  for (size_t i=0; i<s.e->intdim+s.e->realdim; i++) {
    char* c = ap_var_operations->to_string(s.e->var_of_dim[i]);
    os << i << ": " << c << (i<s.e->intdim ? "(int)" : "(real)") << std::endl;
    free(c);
  }
  return os;
}

inline void environment::print(FILE* stream) const
{
  ap_environment_fdump(stream, e);
}


/* C API compatibility */
/* =================== */

inline const ap_environment_t* environment::get_ap_environment_t() const
{
  return e;
}

inline ap_environment_t*environment:: get_ap_environment_t()
{
  return e;
}

