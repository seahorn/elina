/* -*- C++ -*-
 * apxx_manager_inline.hh
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


/* =============== */
/* tbool_t         */
/* =============== */

inline tbool::tbool(bool a) : x(a ? tbool_true : tbool_false) 
{
}

inline tbool::tbool(tbool_t a) : x(a)
{
}

inline tbool::operator tbool_t()
{
  return x;
}

inline tbool::operator bool()
{ 
  return x==tbool_true; 
}

inline tbool operator|| (tbool a, tbool b)
{ 
  return tbool(tbool_or(a.x,b.x)); 
}
  
inline tbool operator&& (tbool a, tbool b)
{
  return tbool(tbool_and(a.x,b.x)); 
}

inline tbool operator! (tbool a)
{
  return tbool(tbool_not(a.x)); 
}

inline std::ostream& operator<<(std::ostream& os, tbool x)
{
  switch (x.x) {
  case tbool_true:  return os << "true";
  case tbool_false: return os << "false";
  case tbool_top:   return os << "top";
  default: throw std::invalid_argument("operator<< tbool");
  }
}



/* =============== */
/* manager         */
/* =============== */


inline manager::manager(ap_manager_t* m) : m(m)
{
  // disable aborting as we manually check exception flags and throw C++ exceptions (see raise)
  for (size_t i=0; i<AP_EXC_SIZE; i++)
    m->option.abort_if_exception[i] = 0;
}

// called at the end of every abstract function
inline void manager::raise(ap_manager_t* m, const char* msg)
{
  switch (ap_manager_get_exception(m)) {
    
  case AP_EXC_NONE:
    break;
    
    case AP_EXC_TIMEOUT:
      ap_manager_clear_exclog(m);
      throw timeout(msg);
      break;
      
    case AP_EXC_OUT_OF_SPACE:
      ap_manager_clear_exclog(m);
      throw std::length_error(msg);
      break;

    case AP_EXC_OVERFLOW:
      ap_manager_clear_exclog(m);
      throw std::overflow_error(msg);
      break;

    case AP_EXC_INVALID_ARGUMENT:
      ap_manager_clear_exclog(m);
      throw std::invalid_argument(msg);
      break;

    case AP_EXC_NOT_IMPLEMENTED:
      ap_manager_clear_exclog(m);
      throw not_implemented(msg);
      break;

    default:
      throw std::range_error("manager::raise");
      break;
    }   
}

inline void manager::raise(const char* msg)
{ 
  manager::raise(m, msg); 
} 

inline manager::manager(const manager& x) 
  : m(ap_manager_copy(x.m)) 
{
}

inline manager::~manager() 
{ 
  ap_manager_free(m); 
}

inline manager& manager::operator= (const manager& x)
{
  ap_manager_t* mm = ap_manager_copy(x.m);
  ap_manager_free(m);
  m = mm;
  return *this;
}

inline const char* manager::get_library() const 
{
  return ap_manager_get_library(m); 
}

inline const char* manager::get_version() const 
{ 
  return ap_manager_get_version(m); 
}

inline ap_funopt_t& manager::get_funopt(ap_funid_t funid)
{ 
  if (funid<=AP_FUNID_UNKNOWN || funid>=AP_FUNID_SIZE) throw std::out_of_range("manager::get_funopt");
  return m->option.funopt[funid]; 
}

inline ap_scalar_discr_t& manager::get_scalar_discr()
{ 
  return m->option.scalar_discr; 
}

inline tbool manager::get_flag_exact()
{
  return tbool(ap_manager_get_flag_exact(m)); 
}

inline tbool manager::get_flag_best()
{
  return tbool(ap_manager_get_flag_best(m)); 
}


inline ap_manager_t* manager::get_ap_manager_t()
{ 
  return m; 
}

inline void manager::fpu_init()
{
  if (!ap_fpu_init())
    throw std::runtime_error("manager::fpu_init failed");
}


/* Box manager */
/* =========== */

inline box_manager::box_manager() 
  : manager(box_manager_alloc()) 
{
}


/* NewPolka manager */
/* ================ */

inline polka_manager::polka_manager(bool strict) 
  : manager(pk_manager_alloc(strict)) 
{
}


/* Oct manager */
/* =========== */

inline oct_manager::oct_manager() 
  : manager(oct_manager_alloc()) 
{
}

