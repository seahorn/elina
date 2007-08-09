/* -*- C++ -*-
 * apxx_var.hh
 *
 * APRON Library / C++ class wrappers
 *
 * Copyright (C) Antoine Mine' 2007
 *
 */
/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#ifndef __APXX_VAR_HH
#define __APXX_VAR_HH

#include "ap_var.h"


namespace apron {

/*! \brief ap_var_t wrapper.
 *
 * A var object designates a variable, using a void* pointer (i.e., ap_var_t).
 * Variables can be copied, freed, compared, and converted to string.
 * The actual implementation of these operators is defined using the global pointer ap_var_operations.
 */
class var {
  
protected:
  ap_var_t v; //! Variable data.
  
public:
  
  //! Wraps the ap_var_t into a var object. No copy is done.
  var(ap_var_t v);
  
  //! Copies the variable (calls ap_var_operations->copy).
  var(const var& v);
  
  //! Frees the variables (calls ap_var_operations->free).
  ~var();
  
  //! Assigns the variable (calls ap_var_operations->copy, ->free).
  var& operator=(const var& v);
  
  /*! \brief Converts to a C string (calls ap_var_operations->to_string).
   *
   * The C string should be freed with malloc by the caller.
   */
  operator const char*() const;

  //! \brief Converts to a C++ string (calls ap_var_operations->to_string).
  operator std::string() const;

  //! \brief Compares two variables (calls ap_var_operations->to_compare).
  friend int  compare(const var& x, const var& y);

  //! \brief Compares two variables (calls ap_var_operations->to_compare).
  friend bool operator==(const var& x, const var& y);

  //! \brief Compares two variables (calls ap_var_operations->to_compare).
  friend bool operator!=(const var& x, const var& y);
 
  //! \brief Compares two variables (calls ap_var_operations->to_compare).
  friend bool operator>=(const var& x, const var& y);

  //! \brief Compares two variables (calls ap_var_operations->to_compare).
  friend bool operator<=(const var& x, const var& y);

  //! \brief Compares two variables (calls ap_var_operations->to_compare).
  friend bool operator>(const var& x, const var& y);

  //! \brief Compares two variables (calls ap_var_operations->to_compare).
  friend bool operator<(const var& x, const var& y);

  //! \brief Returns a reference to the APRON object wrapped (no copy).
  const ap_var_t& get_ap_var_t() const;

  //! \brief Returns a (modifiable) reference to the APRON object wrapped (no copy).
  ap_var_t& get_ap_var_t();

};

#include "apxx_var_inline.hh"

}


#endif /* __APXX_VAR_HH */
