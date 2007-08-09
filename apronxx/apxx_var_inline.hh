/* -*- C++ -*-
 * apxx_var_inline.hh
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

inline var::var(ap_var_t v) : v(v)
{
}
 
inline var::var(const var& v) : v(ap_var_operations->copy(v.v))
{
}

inline var::~var()
{
  ap_var_operations->free(v);
}

inline var& var::operator=(const var& x)
{
  if (x.v!=v) {
    ap_var_t vv = ap_var_operations->copy(x.v);
    ap_var_operations->free(v);
    v = vv;
  }
  return *this;
}

inline var::operator const char*() const
{
  return ap_var_operations->to_string(v);
}

inline var::operator std::string() const
{
  char* c = ap_var_operations->to_string(v);
  std::string s = c;
  free(c);
  return s;
}

inline int  compare(const var& x, const var& y)
{
  return ap_var_operations->compare(x.v,y.v);
}

inline bool operator==(const var& x, const var& y)
{
  return ap_var_operations->compare(x.v,y.v)==0;
}

inline bool operator!=(const var& x, const var& y)
{
  return ap_var_operations->compare(x.v,y.v)!=0;
}

inline bool operator>=(const var& x, const var& y)
{
  return ap_var_operations->compare(x.v,y.v)>=0;
}

inline bool operator<=(const var& x, const var& y)
{
  return ap_var_operations->compare(x.v,y.v)<=0;
}

inline bool operator>(const var& x, const var& y)
{
  return ap_var_operations->compare(x.v,y.v)>0;
}

inline bool operator<(const var& x, const var& y)
{
  return ap_var_operations->compare(x.v,y.v)<0;
}

inline const ap_var_t& var::get_ap_var_t() const
{
  return v;
}

inline ap_var_t& var::get_ap_var_t()
{
  return v;
}


