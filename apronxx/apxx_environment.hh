/* -*- C++ -*-
 * apxx_environment.hh
 *
 * APRON Library / C++ class wrappers
 *
 * Copyright (C) Antoine Mine' 2007
 *
 */
/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#ifndef __APXX_ENVIRONMENT_HH
#define __APXX_ENVIRONMENT_HH

#include <string>
#include <vector>

#include "ap_environment.h"
#include "apxx_dimension.hh"
#include "apxx_var.hh"


namespace apron {


/* ================================= */
/* environment                       */
/* ================================= */

/*! \brief ap_environment_t* wrapper.
 *
 * An environment represents a set of integer and real variable (var) and associates a dimension to each.
 * It stores a number intdim of integer dimensions, a number realdim of real dimensions, and
 * an array of size intdim+realdim variable indexed by dimension.
 * Dimensions 0 to intdim-1 are associated to integer variables while dimensions intdim to intdim+realdim-1
 * are associated to real variables.
 * Variables in both slices [0;intdim-1] and [intdim;intdim+realdim-1] are always sorted in
 * strictly increasing order.
 * (Recall that variables have the following user-specified functions: copy, deallocation, comparison, conversion to string.)
 * This invariant is automatically enforced by the library by performing the appropriate permutation after
 * dimension addition, deletion, or renaming.
 *
 * Environments are reference-counted. 
 * Operations are pure (i.e., do not change *this).
 */
class environment : public use_malloc {

protected:

  ap_environment_t* e;

  //! Used internally only. Wraps the APRON object in an environment object (no copy, no change in reference counter).
  environment(ap_environment_t* x) : e(x) {}

public:


  /* constructor */
  /* =========== */


  /** @name Constructors */
  //@{

  //! Creates a new empty environment.
  environment();

  /*! \brief Creates a new environment with given integer and real variable (copied).
   *
   * \throw std::invalid_argument if variables are not distinct.
   */
  environment(const std::vector<var>& intdim, const std::vector<var>& realdim);

  //! Increments the reference counter.
  environment(const environment& x);

  /*! \brief Creates a copy of the environment with some integer and/or real variables added.
   *
   * \throw std::invalid_argument if the resulting environment has duplicate variables.
   */
  environment(const environment& x, const std::vector<var>& intdim, const std::vector<var>& realdim);

  /*! \brief Creates a copy of the environment with some variables removed.
   *
   * \throw std::invalid_argument if some variable does not exist.
   */
  environment(const environment& x, const std::vector<var>& dims);

  /*! \brief Creates a copy of the environment with some variables substituted.
   *
   * dims[i].first is replaced with dims[i].second.
   *
   * \throw std::invalid_argument if some variable does not exist.
   */
  environment(const environment& x, const std::vector<std::pair<var,var> >& dims);

  /*! \brief Creates the least common environment of two environments.
   *
   * \throw std::invalid_argument if some variable has different type in both environments
   */
  environment(const environment& x, const environment& y);

  /*! \brief Creates the least common environment of an array of environments.
   *
   * \throw std::invalid_argument if some variable has different type in different environments
   */
  environment(const std::vector<environment>& x);

  //@}


  /** @name Destructor */
  //@{

  //! Decrements the reference counter and, if null, actually frees the environment.
  ~environment();

  //@}


  /* assignment */
  /* ========== */

  /** @name Assignment */
  //@{

  //! Copy (through reference counter manipulation).
  const environment& operator=(const environment& x);

  //@}


  /* operations */
  /* ========== */

  /** @name Operations */
  //@{

  /*! \brief Returns a new environment with some integer and/or real variables added.
   *
   * \throw std::invalid_argument if the resulting environment has duplicate variables.
   */
  environment add(const std::vector<var>& intdim, const std::vector<var>& realdim) const;

  /*! \brief Returns a new environment with some integer and/or real variables added.
   *
   * Returns in perm the permutation that has been performed after adding dimensions at the end of
   * the integer and real dimensions to get them contiguous and sorted.
   *
   * \throw std::invalid_argument if the resulting environment has duplicate variables.
   */
  environment add(const std::vector<var>& intdim, const std::vector<var>& realdim, dimperm& perm) const;

  /*! \brief Returns a new environment with some variables removed.
   *
   * \throw std::invalid_argument if some variable does not exist.
   */
  environment remove(const std::vector<var>& dims) const;

  /*! \brief Returns a new environment with some variables substituted.
   *
   * dims[i].first is replaced with dims[i].second.
   *
   * \throw std::invalid_argument if some variable does not exist.
   */
  environment rename(const std::vector<std::pair<var,var> >& dims) const;

  /*! \brief Returns a new environment with some variables substituted.
   *
   * dims[i].first is replaced with dims[i].second.
   * Returns in perm the permutation that has been performed after renaming the variables to get them 
   * sorted.
   *
   * \throw std::invalid_argument if some variable does not exist.
   */
  environment rename(const std::vector<std::pair<var,var> >& dims, dimperm& perm) const;

  /*! \brief Returns the least common environment of two environments.
   *
   * \throw std::invalid_argument if some variable has different type in both environments
   */
  friend environment lce(const environment& x, const environment& y);

  /*! \brief Returns the least common environment of two environments.
   *
   * Returns in chgx (resp. chgy) the transformation that has been performed after adding the required variables
   * to x (resp. y) to keep them sorted.
   *
   * \throw std::invalid_argument if some variable has different type in both environments
   */
  friend environment lce(const environment& x, const environment& y, dimchange& chgx, dimchange& chgy);

  /*! \brief Returns the least common environment of an vector of environments.
   *
   * \throw std::invalid_argument if some variable has different type in different environments
   */
  friend environment lce(const std::vector<environment>& x);

  /*! \brief Returns the least common environment of an vector of environments.
   *
   * Returns in chg[i] the transformation that has been performed after adding the required variables
   * to x[i] to keep them sorted.
   *
   * \throw std::invalid_argument if some variable has different type in different environments
   */
  friend environment lce(const std::vector<environment>& x, std::vector<dimchange>& chg);

  /*! \brief Returns the dimension change to transform an environment x into a super-environment y.
   *
   * \throw std::invalid_argument if x is not a subset of y, or if x and chg have different size.
   */
  friend dimchange get_dimchange(const environment& x, const environment& y);

  //@}


  /* access */
  /* ====== */

  /** @name Accesses */
  //@{

  //! Returns the number of integer dimensions in the environment.
  size_t intdim() const;

  //! Returns the number of real dimensions in the environment.
  size_t realdim() const;

  //! Whether the environment contains a variable x.
  bool contains(const var& x) const;

  /*! \brief Returns the dimension associated with the variable.
   *
   * \throw std::invalid_argument if there is no such variable in the environment.
   */
  ap_dim_t operator[] (const var& x) const;

  //! \brief Returns the variable at a given dimension (no bound-check).
  const var& operator[] (ap_dim_t d) const;

  /*! \brief Returns the dimension associated with a variable.
   *
   * \throw std::invalid_argument if there is no such variable in the environment.
   */
  ap_dim_t get_dim(const var& x) const;

  /*! \brief Returns the variable at a given dimension (bound-checked).
   *
   * \throw std::out_of_range if the dimension is larger than the environment size.
   */
  const var& get_var(ap_dim_t d) const;

  //! Returns (a copy of) the full map form dimensions to variables.
  std::vector<var> get_vars() const;


  //@}


  /* Tests */
  /* ===== */

  /** @name Tests */
  //@{

  //! Equality testing.
  friend bool operator==(const environment& x, const environment& y);

  //! Disequality testing.
  friend bool operator!=(const environment& x, const environment& y);

  //! Inclusion testing.
  friend bool operator<=(const environment& x, const environment& y);

  //! Inclusion testing.
  friend bool operator>=(const environment& x, const environment& y);

  /*! \brief Environment comparison.
   *
   * \return 
   *  - -2 if some variable has different type in both environments
   *  - -1 if x is a sub-environment of y
   *  -  0 if x equals y
   *  - +1 if x is a super-environment of y
   *  - +2 if x and y a not comparable but there exists a lowest upper bound
   */
  friend int cmp(const environment& x, const environment& y);

  //@}


  /* print */
  /* ===== */

  /** @name Printing */
  //@{

   //! Printing.
  friend std::ostream& operator<< (std::ostream& os, const environment& s); 

  //! Prints to a C stream.  
  void print(FILE* stream=stdout) const; 

  //@}


  /* C API compatibility */
  /* =================== */

  /** @name C API compatibility */
  //@{

  //! Returns a pointer to the internal APRON object pointed by *this.
  const ap_environment_t* get_ap_environment_t() const;

  //! Returns a pointer to the internal APRON object pointed by *this.
  ap_environment_t* get_ap_environment_t();

  //@}

};

#include "apxx_environment_inline.hh"

}

#endif /* __APXX_ENVIRONMENT_HH */
