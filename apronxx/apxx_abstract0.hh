/* -*- C++ -*-
 * apxx_abstract0.hh
 *
 * APRON Library / C++ class wrappers
 *
 * Copyright (C) Antoine Mine' 2007
 *
 */
/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#ifndef __APXX_ABSTRACT0_HH
#define __APXX_ABSTRACT0_HH

#include <string>
#include "ap_abstract0.h"
#include "apxx_linexpr0.hh"
#include "apxx_lincons0.hh"
#include "apxx_generator0.hh"
#include "apxx_manager.hh"


namespace apron {


/* ================================= */
/* abstract0                         */
/* ================================= */

/*! \brief ap_abstract0_t* wrapper.
 *
 *
 */
class abstract0 : public use_malloc {

protected:
  
  ap_abstract0_t* a; //!< Pointer managed by APRON.

  //! Internal use only. Wraps a abstract0 around the pointer x, taking ownership of the object.
  abstract0(ap_abstract0_t* x) : a(x) {}

public:

  /* constructors */
  /* ------------ */

  abstract0(manager &m, size_t intdim, size_t realdim, top x) 
    : a(ap_abstract0_top(m.get_ap_manager_t(), intdim, realdim)) 
  { m.raise("constructor (top)"); }

  abstract0(manager &m, size_t intdim, size_t realdim, bottom x) 
    : a(ap_abstract0_bottom(m.get_ap_manager_t(), intdim, realdim)) 
  { m.raise("constructor (bottom)"); }

  abstract0(manager &m, const abstract0& t)
    : a(ap_abstract0_copy(m.get_ap_manager_t(), t.a))
  { m.raise("constructor (copy)"); }

  abstract0(manager &m, size_t intdim, size_t realdim, const interval_array& x) 
  {
    if (x.get_size()<intdim+realdim) throw std::out_of_range("abstract0::abstract0 (interval_array)");
    a = ap_abstract0_of_box(m.get_ap_manager_t(), intdim, realdim, 
			    const_cast<ap_interval_t**>(x.get_ap_interval_t_array()));
    m.raise("constructor (interval_array)"); 
  }
  
  abstract0(manager &m, size_t intdim, size_t realdim, const lincons0_array& x)  
  {
    if (x.get_size()<intdim+realdim) throw std::out_of_range("abstract0::abstract0 (lincons0_array)");
    assert(x.get_size()>=intdim+realdim);
    a = ap_abstract0_of_lincons_array(m.get_ap_manager_t(), intdim, realdim, 
				      const_cast<ap_lincons0_array_t*>(x.get_ap_lincons0_array_t()));
    m.raise("constructor (lincons_array)"); 
  }


  /* uses the manager of the argument */
  abstract0(const abstract0& t)
    : a(ap_abstract0_copy(t.a->man, t.a))
  { manager::raise(a->man, "constructor (copy)"); }


  
  /* destructor */
  /* ---------- */

  ~abstract0()
  { if (a->value) ap_abstract0_free(a->man, a); manager::raise(a->man, "destructor"); }


  /* assignments */
  /* ----------- */

  /* for =, we use the manager of the assigned object */

  abstract0& operator= (const abstract0& t)
  {
    if (&t!=this) {
      ap_abstract0_free(a->man, a);
      a = ap_abstract0_copy(a->man, t.a);
    }
    manager::raise(a->man, "operator= (t)");
    return *this;
  }

  abstract0& operator= (top t)
  {
    ap_dimension_t d = ap_abstract0_dimension(a->man, a);
    ap_abstract0_free(a->man, a);
    a = ap_abstract0_top(a->man, d.intdim, d.realdim);
    manager::raise(a->man, "operator= (top)");
    return *this;
  }

  abstract0& operator= (bottom t)
  {
    ap_dimension_t d = ap_abstract0_dimension(a->man, a);
    ap_abstract0_free(a->man, a);
    a = ap_abstract0_bottom(a->man, d.intdim, d.realdim);
    manager::raise(a->man, "operator= (bottom)");
    return *this;
  }

  abstract0& operator= (const interval_array& x) 
  {
    ap_dimension_t d = ap_abstract0_dimension(a->man, a);
    if (x.get_size()<d.intdim+d.realdim) throw std::out_of_range("abstract0::operator= (interval_array)");
    ap_abstract0_free(a->man, a);
    a = ap_abstract0_of_box(a->man, d.intdim, d.realdim, 
			    const_cast<ap_interval_t**>(x.get_ap_interval_t_array()));
    manager::raise(a->man, "operator= (box)");
    return *this;
  }
  
  abstract0& operator= (const lincons0_array& x) 
  {
    ap_dimension_t d = ap_abstract0_dimension(a->man, a);
    if (x.get_size()<d.intdim+d.realdim) throw std::out_of_range("abstract0::operator= (lincons0_array)");
    ap_abstract0_free(a->man, a);
    a = ap_abstract0_of_lincons_array(a->man, d.intdim, d.realdim, 
				      const_cast<ap_lincons0_array_t*>(x.get_ap_lincons0_array_t()));
    manager::raise(a->man, "operator= (lincons0_array)");
    return *this;
  }


  /* control of internal representation */
  /* ---------------------------------- */

  void minimize(manager& m)
  { ap_abstract0_minimize(m.get_ap_manager_t(), a); m.raise("minimize"); }

  void canonicalize(manager& m)
  { ap_abstract0_canonicalize(m.get_ap_manager_t(), a); m.raise("canonicalize"); }

  void approximate(manager& m, int algorithm)
  { ap_abstract0_approximate(m.get_ap_manager_t(), a, algorithm); m.raise("approximate"); }


  /* printing */
  /* -------- */

  void print(manager& m, char** name_of_dim=NULL, FILE* stream=stdout)
  { ap_abstract0_fprint(stream, m.get_ap_manager_t(), a, name_of_dim); m.raise("print"); }

  void printdiff(manager& m, const abstract0& x, char** name_of_dim=NULL, FILE* stream=stdout)
  { ap_abstract0_fprintdiff(stream, m.get_ap_manager_t(), a, x.a, name_of_dim); m.raise("printdiff"); }

  void dump(manager& m, FILE* stream=stdout)
  { ap_abstract0_fdump(stream, m.get_ap_manager_t(), a); m.raise("dump"); }


  /* serialisation */
  /* ------------- */

  std::string* serialize_raw(manager& m) const 
  {
    ap_membuf_t x = ap_abstract0_serialize_raw(m.get_ap_manager_t(), a);
    std::string* s = new std::string((char*)x.ptr, x.size);
    free(x.ptr);
    m.raise("serialize_raw");
    return s;
  }

  static abstract0 deserialize_raw(manager& m, const std::string& s)
  {
    size_t x = s.size();
    ap_abstract0_t* r = ap_abstract0_deserialize_raw(m.get_ap_manager_t(), const_cast<char*>(s.data()), &x);
    /* TODO: x out value */
    m.raise("deserialize_raw");
    return abstract0(r);
  }


  /* accessors */
  /* --------- */

  manager get_manager() const
  { return ap_manager_copy(ap_abstract0_manager(a)); }

  ap_dimension_t get_dimension(manager& m) const
  { 
    ap_dimension_t d = ap_abstract0_dimension(m.get_ap_manager_t(), a); 
    m.raise("get_dimension");
    return d;
  }

  
  /* tests */
  /* ----- */

  tbool is_bottom(manager& m) const
  {
    tbool r = ap_abstract0_is_bottom(m.get_ap_manager_t(), a);
    m.raise("is_bottom");
    return r;
  }
  
  tbool is_top(manager& m) const
  {
    tbool r = ap_abstract0_is_top(m.get_ap_manager_t(), a);
    m.raise("is_top");
    return r;
  }
  
  friend tbool is_eq(manager& m, const abstract0& x, const abstract0& y)
  {
    tbool r = ap_abstract0_is_eq(m.get_ap_manager_t(), x.a, y.a);
    m.raise("is_eq");   
    return r;
  }
  
  friend tbool is_leq(manager& m, const abstract0& x, const abstract0& y)
  {
    tbool r = ap_abstract0_is_leq(m.get_ap_manager_t(), x.a, y.a);
    m.raise("is_leq");   
    return r;
  }

  tbool sat(manager& m, const lincons0& l) const
  {
    tbool r = ap_abstract0_sat_lincons(m.get_ap_manager_t(), a, const_cast<ap_lincons0_t*>(l.get_ap_lincons0_t()));
    m.raise("sat (lincons0)");   
    return r;
  }

  tbool sat(manager& m, ap_dim_t dim, const interval& i) const
  {
    tbool r = ap_abstract0_sat_interval(m.get_ap_manager_t(), a, dim, const_cast<ap_interval_t*>(i.get_ap_interval_t()));
    m.raise("sat (interval)");   
    return r;
  }

  tbool is_dimension_unconstrained(manager& m, ap_dim_t dim) const
  {
    tbool r = ap_abstract0_is_dimension_unconstrained(m.get_ap_manager_t(), a, dim);
    m.raise("is_dimension_unconstrained");   
    return r;
  }


  /* use the manager of the left argument */

  friend tbool operator== (const abstract0& x, const abstract0& y)
  {
    tbool r = ap_abstract0_is_eq(x.a->man, x.a, y.a);
    manager::raise(x.a->man, "operator==");
    return r;
  }
  
  friend tbool operator!= (const abstract0& x, const abstract0& y)
  { return !(x==y); }
  
  friend tbool operator<= (const abstract0& x, const abstract0& y)
  {
    tbool r = ap_abstract0_is_leq(x.a->man, x.a, y.a);
    manager::raise(x.a->man, "operator<=");
    return r;
  }
  
  friend tbool operator>= (const abstract0& x, const abstract0& y)
  { return y<=x; }
  
  friend tbool operator> (const abstract0& x, const abstract0& y)
  { return !(x<=y); }
  
  friend tbool operator< (const abstract0& x, const abstract0& y)
  { return !(y<=x); }
  

  /* extraction of properties */
  /* ------------------------ */

  interval bound(manager& m, const linexpr0& l) const
  { 
    ap_interval_t* r = ap_abstract0_bound_linexpr(m.get_ap_manager_t(), a, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()));
    m.raise("bound (linexpr0)");   
    return r;
  }

  interval bound(manager& m, ap_dim_t d) const
  { 
    ap_interval_t* r = ap_abstract0_bound_dimension(m.get_ap_manager_t(), a, d);
    m.raise("bound (ap_dim_t)");   
    return r;
  }

  interval_array box(manager& m) const
  {
    ap_dimension_t d = ap_abstract0_dimension(a->man, a);
    ap_interval_t** r = ap_abstract0_to_box(m.get_ap_manager_t(), a);
    m.raise("box");   
    return interval_array(d.intdim+d.realdim, r);
  }
  
  generator0_array to_generator_array(manager& m) const
  {
    ap_generator0_array_t r = ap_abstract0_to_generator_array(m.get_ap_manager_t(), a);
    m.raise("to_generator_array");   
    return r;
  }


  /* meet and join */
  /* ------------- */

  void meet_with(manager& m, const abstract0& x)
  { 
    a = ap_abstract0_meet(m.get_ap_manager_t(), true, a, x.a); 
    m.raise("meet_with (abstract0)"); 
  }

  void meet_with(manager& m, const lincons0_array& x)
  { 
    a = ap_abstract0_meet_lincons_array(m.get_ap_manager_t(), true, a, 
					const_cast<ap_lincons0_array_t*>(x.get_ap_lincons0_array_t())); 
    m.raise("meet_with (lincons0_array)"); 
  }

  void join_with(manager& m, const abstract0& x)
  { 
    a = ap_abstract0_join(m.get_ap_manager_t(), true, a, x.a); 
    m.raise("join_with (abstract0)"); 
  }

  void join_with(manager& m, const generator0_array& x)
  { 
    a = ap_abstract0_add_ray_array(m.get_ap_manager_t(), true, a, 
				   const_cast<ap_generator0_array_t*>(x.get_ap_generator0_array_t())); 
    m.raise("join_with (generator0_array)"); 
  }

  abstract0 meet(manager& m, const abstract0& x) const
  { 
    ap_abstract0_t* r = ap_abstract0_meet(m.get_ap_manager_t(), false, a, x.a); 
    m.raise("meet (abstract0)"); 
    return r; 
  }

  abstract0 meet(manager& m, const lincons0_array& x) const
  { 
    ap_abstract0_t* r = 
      ap_abstract0_meet_lincons_array(m.get_ap_manager_t(), false, a, 
				      const_cast<ap_lincons0_array_t*>(x.get_ap_lincons0_array_t())); 
    m.raise("meet (lincons0_array)");
    return r; 
  }

  abstract0 join(manager& m, const abstract0& x) const
  { 
    ap_abstract0_t* r = ap_abstract0_join(m.get_ap_manager_t(), false, a, x.a); 
    m.raise("join (abstract0)"); 
    return r; 
  }

  abstract0 join(manager& m, const generator0_array& x) const
  { 
    ap_abstract0_t* r = 
      ap_abstract0_add_ray_array(m.get_ap_manager_t(), false, a, 
				 const_cast<ap_generator0_array_t*>(x.get_ap_generator0_array_t())); 
    m.raise("join (generator0_array)"); 
    return r; 
  }



  abstract0& operator*= (const abstract0& x)
  { 
    a = ap_abstract0_meet(a->man, true, a, x.a); 
    manager::raise(a->man, "operator*= (abstract0)"); 
    return *this; 
  }

  abstract0& operator*= (const lincons0_array& x)
  { 
    a = ap_abstract0_meet_lincons_array(a->man, true, a, 
					const_cast<ap_lincons0_array_t*>(x.get_ap_lincons0_array_t())); 
    manager::raise(a->man, "operator*= (lincons0_array)"); 
    return *this; 
  }

  abstract0& operator+= (const abstract0& x)
  { 
    a = ap_abstract0_join(a->man, true, a, x.a); 
    manager::raise(a->man, "operator+= (abstract0)"); 
    return *this; 
  }

  abstract0& operator+= (const generator0_array& x)
  { 
    a = ap_abstract0_add_ray_array(a->man, true, a, 
				   const_cast<ap_generator0_array_t*>(x.get_ap_generator0_array_t())); 
    manager::raise(a->man, "operator+= (generator0_array)"); 
    return *this; 
  }

  friend abstract0 operator* (const abstract0& x, const abstract0& y)
  { 
    ap_abstract0_t* r = ap_abstract0_meet(x.a->man, false, x.a, y.a); 
    manager::raise(x.a->man, "operator* (abstract0)"); 
    return r; 
  }

  friend abstract0 operator* (const abstract0& x, const lincons0_array& y)
  { 
    ap_abstract0_t* r = 
      ap_abstract0_meet_lincons_array(x.a->man, false, x.a, const_cast<ap_lincons0_array_t*>(y.get_ap_lincons0_array_t())); 
    manager::raise(x.a->man, "operator* (lincons0_array");
    return r; 
  }

  friend abstract0 operator+ (const abstract0& x, const abstract0& y)
  { 
    ap_abstract0_t* r = ap_abstract0_join(x.a->man, false, x.a, y.a); 
    manager::raise(x.a->man, "operator+ (abstract0)"); 
    return r; 
  }

  friend abstract0 operator+ (const abstract0& x, const generator0_array& y)
  { 
    ap_abstract0_t* r = 
      ap_abstract0_add_ray_array(x.a->man, false, x.a, const_cast<ap_generator0_array_t*>(y.get_ap_generator0_array_t())); 
    manager::raise(x.a->man, "operator+ (generator0_array)"); 
    return r; 
  }

  /* TODO: meet & join array */


  /* assignment transfer functions */
  /* ----------------------------- */
 
  void assign_with(manager& m, ap_dim_t dim, const linexpr0& l)
  {
    a = ap_abstract0_assign_linexpr(m.get_ap_manager_t(), true, a, dim, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()), NULL);
    m.raise("assign_with (linexpr0)");
  }

  void assign_with(manager& m, ap_dim_t dim, const linexpr0& l, const abstract0& x)
  {
    a = ap_abstract0_assign_linexpr(m.get_ap_manager_t(), true, a, dim, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()), x.a);
    m.raise("assign_with (linexpr0)");
  }

  void substitute_with(manager& m, ap_dim_t dim, const linexpr0& l)
  {
    a = ap_abstract0_substitute_linexpr(m.get_ap_manager_t(), true, a, dim, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()), NULL);
    m.raise("substitute_with (linexpr0)");
  }

  void substitute_with(manager& m, ap_dim_t dim, const linexpr0& l, const abstract0& x)
  {
    a = ap_abstract0_substitute_linexpr(m.get_ap_manager_t(), true, a, dim, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()), x.a);
    m.raise("substitute_with (linexpr0)");
  }

  
  abstract0 assign(manager& m, ap_dim_t dim, const linexpr0& l) const
  {
    ap_abstract0_t* r =
      ap_abstract0_assign_linexpr(m.get_ap_manager_t(), false, a, dim, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()), NULL);
    m.raise("assign (linexpr0)");
    return r;
  }

  abstract0 assign(manager& m, ap_dim_t dim, const linexpr0& l, const abstract0& x) const
  {
    ap_abstract0_t* r =
      ap_abstract0_assign_linexpr(m.get_ap_manager_t(), false, a, dim, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()), x.a);
    m.raise("assign (linexpr0)");
    return r;
  }

  abstract0 substitute(manager& m, ap_dim_t dim, const linexpr0& l) const
  {
   ap_abstract0_t* r =
     ap_abstract0_substitute_linexpr(m.get_ap_manager_t(), false, a, dim, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()), NULL);
    m.raise("substitute (linexpr0)");
    return r;
  }

  abstract0 substitute(manager& m, ap_dim_t dim, const linexpr0& l, const abstract0& x) const
  {
    ap_abstract0_t* r =
      ap_abstract0_substitute_linexpr(m.get_ap_manager_t(), false, a, dim, const_cast<ap_linexpr0_t*>(l.get_ap_linexpr0_t()), x.a);
    m.raise("substitute (linexpr0)");
    return r;
  }


  void assign_with(manager& m, size_t size, const ap_dim_t* dim, const linexpr0** l)
  {
    a = ap_abstract0_assign_linexpr_array(m.get_ap_manager_t(), true, a, 
					  const_cast<ap_dim_t*>(dim), 
					  const_cast<ap_linexpr0_t**>(reinterpret_cast<const ap_linexpr0_t**>(l)), 
					  size, NULL);
    m.raise("assign_with (linexpr0 array)");
  }


  /* projection */
  /* ---------- */

  
  /* dimension manipulation */
  /* ---------------------- */


  /* widening */
  /* -------- */


  /* closure */
  /* ------- */



  /* C-level compatibility */

  ap_abstract0_t* get_ap_abstract0_t()
  { return a; }

  const ap_abstract0_t* get_ap_abstract0_t() const
  { return a; }

};

#include "apxx_abstract0_inline.hh"
  
}

#endif /* __APXX_ABSTRACT0_HH */
