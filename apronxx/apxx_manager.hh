/* -*- C++ -*-
 * apxx_manager.hh
 *
 * APRON Library / C++ class wrappers
 *
 * Copyright (C) Antoine Mine' 2007
 *
 */
/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#ifndef __APXX_MANAGER_HH
#define __APXX_MANAGER_HH

#include <string>

#include "ap_manager.h"
#include "apxx_lincons0.hh"
#include "apxx_tcons0.hh"
#include "apxx_generator0.hh"

#include "box.h"
#include "pk.h"
#include "oct.h"

namespace apron {


/* =============== */
/* tbool           */
/* =============== */

/*! \brief tbool_t wrapper.
 *
 * A tbool_t holds a value in 3-valued logic: tbool_true, tbool_false, or tbool_top (don't know).
 */
struct tbool {

public:
  
  tbool_t x; //!< Either tbool_false, tbool_true, or tbool_top.

  //! Converts true to tbool_true and false to tbool_false.
  tbool(bool a);

  //! Conversion from tbool_t.
  tbool(tbool_t a);

  //! Conversion to tbool_t.
  operator tbool_t();

  //! Returns true if and only if the value is tbool_true.
  operator bool();

  //! 3-valued logical or.
  friend tbool operator|| (tbool a, tbool b);

  //! 3-valued logical and.
  friend tbool operator&& (tbool a, tbool b);

  //! 3-valued logical not. 
  friend tbool operator! (tbool a);

  //! Printing.
  friend std::ostream& operator<<(std::ostream& os, tbool x);

  
};


/* =============== */
/* exceptions      */
/* =============== */


//! Exception raised when a timeout occurs.
class timeout : public std::logic_error {
public:
  /*! \arg \c x name of function in which the timeout occurred.
   */
  timeout(const std::string& x) : logic_error(x) {}
};


//! Exception raised when a function is not implemented.
class not_implemented : public std::logic_error {
public:
  /*! \arg \c x name of unimplemented function.
   */
  not_implemented(const std::string& x) : logic_error(x) {}
};


class abstract0;


/* =============== */
/* manager         */
/* =============== */

/*! \brief ap_manager_t* wrapper.
 *
 * A manager is an instance of a numerical abstract domain library.
 * It has three roles:
 * - dispatch functions to the correct library (allowing multiple domain libraries to be used in a program),
 * - store thread-local data (allowing multiple threads to call functions in the same domain library),
 * - store instance-specific configuration options (such as precision/cost ratios) and feed-backs
 * (such as exactness and best flag).
 *
 * The following guidelines should be followed:
 * - a manager instance is required to create and manipulate abstract0 elements,
 * - an abstract0 element can be manipulated with any instance of the same library it was created with
 * (not necessary the very same instance it was created with),
 * - two threads should always operate on distinct arguments, and using distinct managers.
 *
 * The manager class is an abstract base class for all managers. It has no effective constructor.
 * Instead, you should use the derived class corresponding to the numerical domain library you
 * wish to use.
 * 
 * Managers are reference-counted.
 */
class manager : public use_malloc {
  
protected:

  ap_manager_t* m;  //!< Pointer managed by APRON.

  //! Internal use only (by subclasses). Initialise from a ap_manager_t* and take ownership (no copy).
  manager(ap_manager_t* m);

  friend class abstract0;

public:

  /** @name Internal use */
  //@{

  //! Internal use only (by abstract0). Translates APRON exceptions to C++ ones.
  static void raise(ap_manager_t* m, const char* msg);
  
  //! Internal use only (by abstract0). Translates APRON exceptions to C++ ones.
  void raise(const char* msg);

  //@}


  /** @name Constructor */
  //@{

  //! Increments the reference counter.
  manager(const manager& x);

  //@}


  /** @name Destructor */
  //@{
 
  //! Decrements the reference counter and, when reaching 0, frees the manager.
  ~manager();

  //@}


  /** @name Assignment */
  //@{

  //! Assignment (actually performs some reference counter management).
  manager& operator= (const manager& x);

  //@}


  /** @name Accesses */
  //@{

  //! Returns the name of the library the manager comes from.
  const char* get_library() const;

  //! Returns the version name of the library the manager comes from.
  const char* get_version() const;

  /*! \brief Returns a (modifiable) reference to the options associated to some abstract function.
   *
   * \throw std::out_of_range if funid is no between AP_FUNID_COPY and AP_FUNID_CLOSURE (inclusive).
   */
  ap_funopt_t& get_funopt(ap_funid_t funid);

  /*! \brief Returns a (modifiable) reference to the user's preferred scalar type.
   *
   * \return the scalar type can be either AP_SCALAR_DOUBLE or AP_SCALAR_MPQ.
   */
  ap_scalar_discr_t& get_scalar_discr();

  //! Returns the 'is exact' flag associated to the last computed abstract function.
  tbool get_flag_exact();

  //! Returns the 'is best' flag associated to the last computed abstract function.
  tbool get_flag_best();

  //@}

 
  /** @name Initialisation */
  //@{

  /*! \brief Sets the FPU rounding-mode towards +oo.
   *
   * This is needed to ensure the soundness of all double-manipulating functions in APRON
   * (coefficients or intervals with double bounds, floating-point abstract domains, etc.)
   *
   * The FPU rounding-mode change is global.
   * It affects all managers (already created or yet to be created) as well as all other floating-point
   * computations in the program.
   *
   * \throw std::runtime_error if the APRON was not able to set the rounding-mode.
   */
  static void fpu_init();

  //@}


#if 0

  /* Abstract functions */
  /* ================== */

  abstract0* copy(const abstract0& x);
  void free(abstract0& x);
  size_t size(const abstract0& x);

  void minimize(abstract0& x);
  void canonicalize(abstract0& x);
  void approximate(abstract0& x);

  void fprint(FILE* stream, const abstract0& x, const char** name_of_dim);
  void fprintdiff(FILE* stream, const abstract0& x, const abstract0& y, const char** name_of_dim);
  void fdump(FILE* stream, const abstract0& x);
  
  std::string serialize_raw(const abstract0& x);
  abstract0 deserialize_raw(const std::string& x);

  abstract0 bottom();
  abstract0 top();
  abstract0 of_box(size_t intdim, size_t realdim, const interval_array& x);
  abstract0 of_lincons0_array(const lincons0_array& x);
  abstract0 of_tcons0_array(const tcons0_array& x);

  ap_dimension_t dimension(const abstract0& x);
  
  tbool is_bottom(const abstract0& x);
  tbool is_top(const abstract0& x);
  tbool is_leq(const abstract0& x, const abstract0& y);
  tbool sat(const abstract0& x, const lincons0& c);
  tbool sat(const abstract0& x, const tcons0& c);
  tbool sat(const abstract0& x, ap_dim_t dim, const interval& c);
  tbool is_unconstrained(const abstract0& x, ap_dim_t dim);

  interval bound(const abstract0& x, const lincons0& c);
  interval bound(const abstract0& x, const tcons0& c);
  interval bound(const abstract0& x, ap_dim_t dim);

  lincons0_array   to_lincons_array(const abstract0& x);
  tcons0_array     to_tcons_array(const abstract0& x);
  interval_array   to_bar(const abstract0& x);
  generator0_array to_generator_array(const abstract0& x);

  abstract0 meet(abstract0& x, const abstract0& y, bool destructive=false);
  abstract0 meet(const abstract0 x[], size_t size);
  abstract0 join(abstract0& x, const abstract0& y, bool destructive=false);
  abstract0 join(const abstract0 x[], size_t size);

  abstract0 meet(abstract0& x, const lincons0& l, bool destructive=false);
  abstract0 meet(abstract0& x, const lincons0_array& l, bool destructive=false);
  abstract0 meet(abstract0& x, const tcons0& l, bool destructive=false);
  abstract0 meet(abstract0& x, const tcons0_array& l, bool destructive=false);
  abstract0 add(abstract0& x, const generator0& l, bool destructive=false);
  abstract0 add(abstract0& x, const generator0_array& l, bool destructive=false);

  abstract0 assign(abstract0& x, ap_dim_t dim, const linexpr0& l, bool destructive=false, abstract0* dst=NULL);
  abstract0 assign(abstract0& x, const ap_dim_t dim[], const linexpr0 l[], size_t size, bool destructive=false, abstract0* dst=NULL);
  abstract0 assign(abstract0& x, ap_dim_t dim, const texpr0& l, bool destructive=false, abstract0* dst=NULL);
  abstract0 assign(abstract0& x, const ap_dim_t dim[], const texpr0 l[], size_t size, bool destructive=false, abstract0* dst=NULL);
 
  abstract0 substitute(abstract0& x, ap_dim_t dim, const linexpr0& l, bool destructive=false, abstract0* dst=NULL);
  abstract0 substitute(abstract0& x, const ap_dim_t dim[], const linexpr0 l[], size_t size, bool destructive=false, abstract0* dst=NULL);
  abstract0 substitute(abstract0& x, ap_dim_t dim, const texpr0& l, bool destructive=false, abstract0* dst=NULL);
  abstract0 substitute(abstract0& x, const ap_dim_t dim[], const texpr0 l[], size_t size, bool destructive=false, abstract0* dst=NULL);

  abstract0 forget(abstract0& x, const ap_dim_t dim[], size_t size, bool project=false, bool destructive=false);

  abstract0 add_dimensions(abstract0& x, const dimchange& dim, bool destructive=false);
  abstract0 remove_dimensions(abstract0& x, const dimchange& dim, bool destructive=false);
  abstract0 permute_dimensions(abstract0& x, const dimperm& dim, bool destructive=false);

  abstract0 expand(abstract0& x, ap_dim_t dim, size_t n, bool destructive=false);
  abstract0 fold(abstract0& x, const ap_dim_t dim[], size_t size, bool destructive=false);
  
  abstract0 widening(const abstract0& x, const abstract0& y);
  abstract0 widening(const abstract0& x, const abstract0& y, const lincons0_array& a);

  abstract0 closure(abstract0& x, bool destructive=false);
#endif



  /* C-level compatibility */
  /* ===================== */

  /** @name C API compatibility */
  //@{

  //! Returns a pointer to the internal APRON object stored in *this.
  ap_manager_t* get_ap_manager_t();

  //@}

};


/* =============== */
/* box_manager     */
/* =============== */


//! Manager factory for the Box interval-domain library.
class box_manager : public manager {

public:

  //! \brief Creates a new manager.
  box_manager();

};


/* =============== */
/* polka_manager   */
/* =============== */


//! Manager factory for the NewPolka polyhedra library.
class polka_manager : public manager {

public:

  /*! \brief Creates a new manager.
   *
   * \arg \c strict whether to allow strict inequalities as well as non-strict inequalities.
   */
  polka_manager(bool strict = false);

};


/* =============== */
/* oct_manager     */
/* =============== */


//! Manager factory for the Octagon domain library.
class oct_manager : public manager {

public:

  //! \brief Creates a new manager.
  oct_manager();

};


#include "apxx_manager_inline.hh"

}

#endif /* __APXX_MANAGER_HH */
