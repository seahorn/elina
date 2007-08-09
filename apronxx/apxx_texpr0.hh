/* -*- C++ -*-
 * apxx_texpr0.hh
 *
 * APRON Library / C++ class wrappers
 *
 * Copyright (C) Antoine Mine' 2007
 *
 */
/* This file is part of the APRON Library, released under LGPL license.  
   Please read the COPYING file packaged in the distribution.
*/

#ifndef __APXX_TEXPR0_HH
#define __APXX_TEXPR0_HH

#include <vector>

#include "ap_texpr0.h"
#include "ap_linearize.h"

namespace apron {

class abstract0;


//! A dim represents a dimension (i.e., variable by index) in an expression tree.
struct dim {
  ap_dim_t d; //!< Dimension index.

  //! Standard constructor.
  dim(ap_dim_t d) : d(d) {}
};

class texpr0;



/* ================================= */
/* texpr0_node                       */
/* ================================= */


/*! \brief Temporary expression nodes used when constructing a texpr0.
 *
 * texpr0_node allows constructing expression trees one node at a time using overloaded
 * operators (+, -, *, /, %) and friend functions (add, sub, mul, div, mod, neg, sqrt, cast, 
 * unary, binary).
 *
 * A texpr0_node manages a single node.
 * As their usage is temporary (they are normally destructed at the end of full expressions),
 * assignment, copy, and destruction can be shallow (i.e., non-recursive), which results in
 * improved efficiency.
 *
 * You should not create variables of type texpr0_node, nor define functions that return or take
 * texpr0_node objects as arguments.
 */
class texpr0_node : public use_malloc {

protected:

  ap_texpr0_t* l; //!< Pointer to structure managed by APRON.


  //! Not to be used. (Temporaries are not to be re-assigned).
  texpr0_node& operator= (const texpr0_node& x) { assert(0); }

public:


  /* constructors */
  /* ============ */

  /** @name Constructors */
  //@{

  //! Makes a shallow copy, copying only the root node. Sub-expressions are aliased, not copied.
  texpr0_node(const texpr0_node& x);

  //! Makes a shallow copy, copying only the root node. Sub-expressions are aliased, not copied.
  texpr0_node(const texpr0& x);

  //! Makes a constant leaf from a scalar or interval coefficient (copied).
  texpr0_node(const coeff& x);

  //! Makes a constant scalar leaf from a scalar (copied).
  texpr0_node(const scalar& x);

  //! Makes a constant scalar leaf from a MPQ (copied).
  texpr0_node(const mpq_class& x);

  //! Makes a constant scalar MPQ leaf from an integer.
  texpr0_node(int x);

  //! Makes a constant scalar MPQ leaf from an integer.
  texpr0_node(long x);

  //! Makes a constant scalar double leaf from a double.
  texpr0_node(double x);

  //! Makes a constant scalar MPQ leaf from a fraction with integer coefficients.
  texpr0_node(const frac& x);

  //! Makes a constant interval leaf from an interval (copied).
  texpr0_node(const interval& x);

  //! Makes a constant interval leaf from two scalar bounds (copied).
  texpr0_node(const scalar& inf, const scalar& sup);

  //! Makes a constant interval leaf from two MPQ bounds (copied).
  texpr0_node(const mpq_class& inf, const mpq_class& sup);

  //! Makes a constant interval leaf from two integer bounds (converted to MPQ).
  texpr0_node(int inf, int sup);

  //! Makes a constant interval leaf from two integer bounds (converted to MPQ).
  texpr0_node(long inf, long sup);

  //! Makes a constant interval leaf from two double bounds.
  texpr0_node(double inf, double sup);

  //! Makes a constant interval leaf from two fraction bounds (converted to MPQ).
  texpr0_node(const frac& inf, const frac& sup);
  
  //! Makes a constant interval leaf equal to ]-oo;+oo[.
  texpr0_node(top t);

  //! Makes a dimension leaf (i.e., variable of specified index).
  texpr0_node(dim d);

  //! Makes a unary expression node.
  texpr0_node(ap_texpr_op_t op, const texpr0_node& argA, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir);
  
  //! Makes a binary expression node.
  texpr0_node(ap_texpr_op_t op, const texpr0_node& argA, const texpr0_node& argB, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir);

  //@}


  /* destructor */
  /* ========== */

  /** @name Destructor */
  //@{

  //! Frees the memory occupied by the node, not its sub-expressions.
  ~texpr0_node();

  //@}


  /* tests */
  /* ===== */


  /** @name Tests */
  //@{

  //! Whether the expression is a single coefficient node with 0 value.
  bool is_zero() const;

  //@}

  /* C-level compatibility */
  /* ===================== */

  /** @name C API compatibility */
  //@{

  //! Returns a pointer to the internal APRON object stored in *this.
  ap_texpr0_t* get_ap_texpr0_t();
  
  //! Returns a pointer to the internal APRON object stored in *this.
  const ap_texpr0_t* get_ap_texpr0_t() const;

  //@}


  /* 'Intelligent' constructors */
  /* ========================== */

  /** @name 'Intelligent' constructors */
  //@{

  /*! \brief Makes a unary expression node.
   *
   * Creates only one node. The argument expression is aliased, not copied.
   *
   * \arg \c op should be a unary operator: negation (AP_TEXPR_NEG), typecast (AP_TEXPR_CAST), 
   * or square root (AP_TEXPR_SQRT).
   * \arg \c argA should point to the argument expression.
   * \arg \c rtype is the destination type (for rounding): reals (AP_RTYPE_REAL, means no rounding),
   * perfect integers (AP_RTYPE_INT), 32-bit single precision (AP_RTYPE_SINGLE), 64-bit double precision
   * (AP_RTYPE_DOUBLE), 80-bit extended precision (AP_RTYPE_EXTENDED), or
   * 128-bit quadruple precision (AP_RTYPE_QUAD).
   * \arg \c rdir is the rounding direction: to nearest (AP_RDIR_NEAREST), truncation (AP_RDIR_ZERO),
   * towards +oo (AP_RDIR_UP), towards -oo (AP_RDIR_DOWN), or non-deterministic (AP_RDIR_RND).
   * \throw std::invalid_argument if \c op is not a unary operator.
   */
  friend texpr0_node unary(ap_texpr_op_t op, const texpr0_node& a,
			   ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);
  
  /*! \brief Makes a binary expression node.
   *
   * Creates only one node. The argument expressions is aliased, not copied.
   *
   * \arg \c op should be a binary operator: addition (AP_TEXPR_ADD), subtraction (AP_TEXPR_SUB), 
   * multiplication (AP_TEXPR_MUL), division (AP_TEXPR_DIV), or modulo (AP_TEXPR_MOD).
   * \arg \c argA should point to the left argument expression.
   * \arg \c argB should point to the right argument expression.
   * \arg \c rtype is the destination type (for rounding): reals (AP_RTYPE_REAL, means no rounding),
   * perfect integers (AP_RTYPE_INT), 32-bit single precision (AP_RTYPE_SINGLE), 64-bit double precision
   * (AP_RTYPE_DOUBLE), 80-bit extended precision (AP_RTYPE_EXTENDED).
   * \arg \c rdir is the rounding direction: to nearest (AP_RDIR_NEAREST), truncation (AP_RDIR_ZERO),
   * towards +oo (AP_RDIR_UP), towards -oo (AP_RDIR_DOWN), or non-deterministic (AP_RDIR_RND).
   * \throw std::invalid_argument if \c op is not a binary operator.
   */
  friend texpr0_node binary(ap_texpr_op_t op, const texpr0_node& a, const texpr0_node& b, 
			    ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);
  //! Makes an AP_TEXPR_ADD expression node.
  friend texpr0_node add(const texpr0_node& a, const texpr0_node& b, 
			 ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);

  //! Makes an AP_TEXPR_SUB expression node.
  friend texpr0_node sub(const texpr0_node& a, const texpr0_node& b, 
			 ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);

  //! Makes an AP_TEXPR_MUL expression node.
  friend texpr0_node mul(const texpr0_node& a, const texpr0_node& b, 
			 ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);

  //! Makes an AP_TEXPR_DIV expression node.
  friend texpr0_node div(const texpr0_node& a, const texpr0_node& b, 
			 ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);

  //! Makes an AP_TEXPR_MOD expression node.
  friend texpr0_node mod(const texpr0_node& a, const texpr0_node& b, 
			 ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);

  //! Makes an AP_TEXPR_NEG expression node.
  friend texpr0_node neg(const texpr0_node& a, 
			 ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);

  //! Makes an AP_TEXPR_CAST expression node.
  friend texpr0_node cast(const texpr0_node& a, ap_texpr_rtype_t rtype, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);

  //! Makes an AP_TEXPR_CAST expression node to AP_RTYPE_INT type rounded towards AP_RDIR_DOWN.
  friend texpr0_node floor(const texpr0_node& a);

  //! Makes an AP_TEXPR_CAST expression node to AP_RTYPE_INT type rounded towards AP_RDIR_UP.
  friend texpr0_node ceil(const texpr0_node& a);

  //! Makes an AP_TEXPR_CAST expression node to AP_RTYPE_INT type rounded towards AP_RDIR_ZERO.
  friend texpr0_node trunc(const texpr0_node& a);

  //! Makes an AP_TEXPR_SQRT expression node.
  friend texpr0_node sqrt(const texpr0_node& a, 
			  ap_texpr_rtype_t rtype = AP_RTYPE_REAL, ap_texpr_rdir_t rdir = AP_RDIR_NEAREST);
  
  //! Makes a copy of the node.
  friend texpr0_node operator+(const texpr0_node& a);

  //! Makes a AP_TEXPR_NEG expression node using AP_RTYPE_REAL type (no rounding).
  friend texpr0_node operator-(const texpr0_node& a);

  //! Makes a AP_TEXPR_ADD expression node using AP_RTYPE_REAL type (no rounding).
  friend texpr0_node operator+(const texpr0_node& a, const texpr0_node& b);

  //! Makes a AP_TEXPR_SUB expression node using AP_RTYPE_REAL type (no rounding).
  friend texpr0_node operator-(const texpr0_node& a, const texpr0_node& b);

  //! Makes a AP_TEXPR_MUL expression node using AP_RTYPE_REAL type (no rounding).
  friend texpr0_node operator*(const texpr0_node& a, const texpr0_node& b);

  //! Makes a AP_TEXPR_DIV expression node using AP_RTYPE_REAL type (no rounding).
  friend texpr0_node operator/(const texpr0_node& a, const texpr0_node& b);

  //! Makes a AP_TEXPR_MOD expression node using AP_RTYPE_REAL type (no rounding).
  friend texpr0_node operator%(const texpr0_node& a, const texpr0_node& b);

  //@}
};



/* ================================= */
/* texpr0                            */
/* ================================= */


/*! \brief ap_texpr0_t wrapper.
 *
 * A texpr0 represents an expression tree.
 * Leafs include dimensions (i.e., variables by index) as well as constants (coefficients).
 * Binary operators include addition, subtraction, multiplication, division, and modulo.
 * Unary operators include opposite, square root, and typecast.
 * Additionally, all operators are tagged with a destination type: perfect integers, reals, 
 * or floating-point numbers (of various bit-size). 
 * For integer and floating-point operators, a rounding direction can be specified
 * (as well as 'any direction').
 * The concrete semantics of all operators is to first perform the operation on perfect reals
 * (typecast being the identity operator here), and then round the result to the destination type
 * in the specified direction.
 *
 * A texpr0 manages a whole expression tree: assignment, copy, and destruction are deep (recursive).
 *
 * Complex expression trees can be constructed using the standard +, -, *, /, % operators, as well as
 * functions such as add, mul, div, etc. (allowing to set the destination type and rounding direction).
 * \code 
 * texpr0 x = dim(1) + dim(2) * 5;
 * texpr0 y = add(sqrt(x), x, AP_RTYPE_FLOAT);
 * \endcode
 * Temporaries created by the operators have the texpr0_node type.
 * See the texpr0_node class documentation for the details on operator arguments.
 */
class texpr0 : public use_malloc {

protected:

  ap_texpr0_t l; //!< Structure managed by APRON.

  //! Internal use only. Makes a shallow copy and takes ownership of data-structure.
  void init_from(ap_texpr0_t* x);

  //! Internal use only. Makes a shallow copy and takes ownership of data-structure.
  texpr0(ap_texpr0_t* x);

  friend class texpr0_node;

public:


  /* constructors */
  /* ============ */

  /** @name Constructors */
  //@{

  /*! \brief Makes an expression tree from a temporary (performing a deep copy).
   *
   * A deep copy is performed so that, after the assignment, temporaries can be safely destroyed
   * and we have a private copy of all aliased sub-trees.
   */
  texpr0(const texpr0_node& x);


  //! Makes a (deep) copy of the expression tree.
  texpr0(const texpr0& x);


  //! Makes a constant expression from a scalar or interval coefficient (copied).
  texpr0(const coeff& x);

  //! Makes a constant scalar expression from a scalar (copied).
  texpr0(const scalar& x);

  //! Makes a constant scalar expression from a MPQ (copied).
  texpr0(const mpq_class& x);

  //! Makes a constant scalar MPQ expression from an integer.
  texpr0(int x);

  //! Makes a constant scalar MPQ expression from an integer.
  texpr0(long x);

  //! Makes a constant scalar double expression from a double.
  texpr0(double x);

  //! Makes a constant scalar MPQ expression from a fraction with integer coefficients.
  texpr0(const frac& x);

  //! Makes a constant interval expression from an interval (copied).
  texpr0(const interval& x);

  //! Makes a constant interval expression from two scalar bounds (copied).
  texpr0(const scalar& inf, const scalar& sup);

  //! Makes a constant interval expression from two MPQ bounds (copied).
  texpr0(const mpq_class& inf, const mpq_class& sup);

  //! Makes a constant interval expression from two integer bounds (converted to MPQ).
  texpr0(int inf, int sup);

  //! Makes a constant interval expression from two integer bounds (converted to MPQ).
  texpr0(long inf, long sup);

  //! Makes a constant interval expression from two double bounds.
  texpr0(double inf, double sup);

  //! Makes a constant interval expression from two fraction bounds (converted to MPQ).
  texpr0(const frac& inf, const frac& sup);
  
  //! Makes a constant interval expression equal to ]-oo;+oo[.
  texpr0(top t);

  //! Makes an expression equal to a variable with specified index.
  texpr0(dim d);

  
  /*! \brief Makes an expression tree from a linear expression (copying coefficients).
   *
   * Real-valued addition and multiplication operators are used (i.e., no rounding).
   */
  texpr0(const linexpr0& l);


  /*! \brief Makes a (deep) copy of the expression, and then add or remove some dimensions.
   *
   * \arg \c add whether to add or remove dimensions.
   */
  texpr0(const texpr0& x, const dimchange& d, bool add=true);
  
  //! Makes a (deep) copy of the expression, and then permute dimensions.
  texpr0(const texpr0& x, const dimperm& d);

  //! Makes a (deep) copy of x and substitutes each occurrence of dim with (a fresh copy of) dst.
  texpr0(const texpr0& x, ap_dim_t dim, const texpr0& dst);

  //@}

  /* destructor */
  /* ========== */

  /** @name Destructor */
  //@{

  //! Frees the memory occupied by the expression (and, recursively, all sub-expressions).
  ~texpr0();
  
  //@}

  /* assignment */
  /* ========== */

  /** @name Assignments */
  //@{

  //! Assigns from a temporary expression, performing a deep copy and deleting the previous value of *this.
  texpr0& operator= (const texpr0_node& x);


  //! Assigns to *this a (deep) copy of x, deleting the previous value of *this.
  texpr0& operator= (const texpr0& x);


  //! Makes *this a constant expression equal to a coefficient (copied), deleting the previous value of *this.
  texpr0& operator= (const coeff& x);

  //! Makes *this a constant expression equal to a scalar (copied), deleting the previous value of *this.
  texpr0& operator= (const scalar& x);

  //! Makes *this a constant expression equal to a MPQ scalar (copied), deleting the previous value of *this.
  texpr0& operator= (const mpq_class& x);

  //! Makes *this a constant expression equal to an integer (converted to a MPQ), deleting the previous value of *this.
  texpr0& operator= (int x);

  //! Makes *this a constant expression equal to an integer (converted to a MPQ), deleting the previous value of *this.
  texpr0& operator= (long x);

  //! Makes *this a constant expression equal to a double, deleting the previous value of *this.
  texpr0& operator= (double x);

  //! Makes *this a constant expression equal to an integer fraction (converted to a MPQ), deleting the previous value of *this.
  texpr0& operator= (const frac& x);

  //! Makes *this a constant expression equal to an interval (copied), deleting the previous value of *this.
  texpr0& operator= (const interval& x);

  //! Makes *this a constant expression equal to the interval ]-oo;+oo[, deleting the previous value of *this.
  texpr0& operator= (top t);

  //! Makes *this an expression equal to the dimension d, deleting the previous value of *this.
  texpr0& operator= (dim d);


  /*! \brief Makes *this equal to the linear expression x (coefficients are copied), deleting the previous value of *this.
   *
   * Real-valued addition and multiplication operators are used (i.e., no rounding).
   */
  texpr0& operator= (const linexpr0& x);

  //@}

  /* access */
  /* ====== */


  /** @name Accesses */
  //@{

  /*! \brief Returns the kind of the root of the expression.
   *
   * \return AP_TEXPR_CST for a constant expression, AP_TEXPR_DIM for a dimension,
   * and AP_TEXPR_NODE for an operator node.
   */
  const ap_texpr_discr_t& get_discr() const;

  /*! \brief Returns a (modifiable) reference to the coefficient of a constant expression.
   *
   * \throw bad_discriminant if the expression root is not a constant leaf.
   */
  coeff& get_coeff();

  /*! \brief Returns a reference to the coefficient of a constant expression.
   *
   * \throw bad_discriminant if the expression root is not a constant leaf.
   */
  const coeff& get_coeff() const;

  /*! \brief Returns a (modifiable) reference to the dimension of a dimension expression.
   *
   * \throw bad_discriminant if the expression root is not a dimension leaf.
   */
  ap_dim_t& get_dim();
  
  /*! \brief Returns a reference to the dimension of a dimension expression.
   *
   * \throw bad_discriminant if the expression root is not a dimension leaf.
   */
  const ap_dim_t& get_dim() const;
  
  /*! \brief Returns a (modifiable) reference to the operator kind at the root of the expression.
   *
   * \return either AP_TEXPR_ADD, AP_TEXPR_SUB, AP_TEXPR_MUL, AP_TEXPR_DIV, AP_TEXPR_MOD,
   * AP_TEXPR_NEG, AP_TEXPR_CAST, or AP_TEXPR_SQRT.
   * \throw bad_discriminant if the expression root is not an operator node.
   */
  ap_texpr_op_t& get_op();

  /*! \brief Returns a reference to the operator kind at the root of the expression.
   *
   * \return either AP_TEXPR_ADD, AP_TEXPR_SUB, AP_TEXPR_MUL, AP_TEXPR_DIV, AP_TEXPR_MOD,
   * AP_TEXPR_NEG, AP_TEXPR_CAST, or AP_TEXPR_SQRT.
   * \throw bad_discriminant if the expression root is not an operator node.
   */
  const ap_texpr_op_t& get_op() const;

  /*! \brief Returns a (modifiable) reference to the destination type at the root of the expression.
   *
   * \return either AP_RTYPE_REAL, AP_RTYPE_INT, AP_RTYPE_SINGLE (32-bit), AP_RTYPE_DOUBLE (64-bit),
   *  AP_RTYPE_EXTENDED (80-bit), or AP_RTYPE_QUAD (128-bit).
   * \throw bad_discriminant if the expression root is not an operator node.
   */
  ap_texpr_rtype_t& get_rtype();

  /*! \brief Returns a reference to the destination type at the root of the expression.
   *
   * \return either AP_RTYPE_REAL, AP_RTYPE_INT, AP_RTYPE_SINGLE (32-bit), AP_RTYPE_DOUBLE (64-bit),
   *  AP_RTYPE_EXTENDED (80-bit), or AP_RTYPE_QUAD (128-bit).
   * \throw bad_discriminant if the expression root is not an operator node.
   */
  const ap_texpr_rtype_t& get_rtype() const;

  /*! \brief Returns a (modifiable) reference to the rounding direction at the root of the expression.
   *
   * \return either AP_RDIR_NEAREST, AP_RDIR_ZERO, AP_RDIR_UP, AP_RDIR_DOWN, or AP_RDIR_RND.
   * \throw bad_discriminant if the expression root is not an operator node.
   */
  ap_texpr_rdir_t& get_rdir();

  /*! \brief Returns a reference to the rounding direction at the root of the expression.
   *
   * \return either AP_RDIR_NEAREST, AP_RDIR_ZERO, AP_RDIR_UP, AP_RDIR_DOWN, or AP_RDIR_RND.
   * \throw bad_discriminant if the expression root is not an operator node.
   */
  const ap_texpr_rdir_t& get_rdir() const;

  /*! \brief Returns a (modifiable) reference to the left (or only) expression argument of the operator 
   * node at the root of the expression.
   *
   * \throw bad_discriminant if the expression root is not an operator node.
   */
  texpr0& get_argA();

  /*! \brief Returns a reference to the left (or only) expression argument of the operator 
   * node at the root of the expression.
   *
   * \throw bad_discriminant if the expression root is not an operator node.
   */
  const texpr0& get_argA() const;

  /*! \brief Returns a (modifiable) reference to the right expression argument of the binary operator 
   * node at the root of the expression.
   *
   * \throw bad_discriminant if the expression root is not an operator node.
   * \throw std::out_of_range if the operator at the root is not binary.
   */
  texpr0& get_argB();

  /*! \brief Returns a reference to the right expression argument of the binary operator 
   * node at the root of the expression.
   *
   * \throw bad_discriminant if the expression root is not an operator node.
   * \throw std::out_of_range if the operator at the root is not binary.
   */
  const texpr0& get_argB() const;

  //! Whether the operator is unary.
  static bool is_unop(ap_texpr_op_t op);

  //! Whether the operator is binary.
  static bool is_binop(ap_texpr_op_t op);

  //@}


  /* print */
  /* ===== */

  /** @name Printing */
  //@{

  //! Printing.
  friend std::ostream& operator<< (std::ostream& os, const texpr0& s);

  //! Prints to a C stream.
  void print(char** name_of_dim = NULL, FILE* stream=stdout) const;

  //@}

  /* tests, size, dimensions */
  /* ======================= */

  /** @name Tests, size, dimensions */
  //@{

  //! Whether the expression is a single coefficient node with 0 value.
  bool is_zero() const;

  //! Whether two expressions are syntactically, structurally equal.
  friend bool equal (const texpr0& x, const texpr0& y);

#if 0 // overloaded to make constraints
  //! Whether two expressions are syntactically, structurally equal.
  friend bool operator== (const texpr0& x, const texpr0& y);

  //! Whether two expressions are syntactically, structurally different.
  friend bool operator!= (const texpr0& x, const texpr0& y);
#endif

  //! Returns the depth of the expression tree (counting only operator nodes).
  size_t depth() const;

  //! Returns the number of operator nodes in the expression tree.
  size_t size() const;

  //! Returns the maximal dimension occurring in the expression (or 0 if there is no variable).
  ap_dim_t max_dim() const;

  //! Whether the given dimension occurs in the expression.
  bool has_dim(ap_dim_t d) const;

  /*! \brief Returns a list of all dimensions occurring in the expression 
   * (in strict increasing order)
   */
  std::vector<ap_dim_t> dimlist() const;

  //! Whether the expression is constant (i.e., has no dimension leaves).
  bool is_interval_cst() const;

  //! Whether the expression is linear and there is no rounding.
  bool is_interval_linear() const;

  //! Whether the expression is polynomial and there is no rounding.
  bool is_interval_polynomial() const;

  //! Whether the expression is a polynomial fraction and there is no rounding.
  bool is_interval_polyfrac() const;

  //! Whether all occurring constants are scalar.
  bool is_scalar() const;

  //@}

  /* operations */
  /* ========== */

  /** @name Operations */
  //@{

  //! Substitutes each occurrence of dim with (a fresh copy of) dst.
  void substitute(ap_dim_t dim, const texpr0& dst);

#if 0
  /*! \brief Evaluates the expression given an abstract environment.
   *
   * \arg \c discr whether to evaluate using double (AP_SCALAR_DOUBLE) or MPQ (AP_SCALAR_MPQ).
   * \arg \c pexact if not NULL, sets to true whenever the evaluation was exact 
   * (i.e., not over-approximated).
   */
  interval eval(manager& m, const abstract0& a, ap_scalar_discr_t discr=AP_SCALAR_DOUBLE, 
		bool* pexact=NULL) const;

  linexpr0 intlinearize(manager& m, const abstract0& a, ap_scalar_discr_t discr=AP_SCALAR_DOUBLE, 
			bool quasilinearize=false, bool* pexact=NULL) const;
#endif

  //! Returns a hash code.
  long hash() const;

  //@}

  /* change of dimension */
  /* =================== */

  /** @name Changes of dimension */
  //@{

  //! Adds some dimensions, shifting dimension if needed.
  void add_dimensions(const dimchange& d);

  /*! \brief Removes some dimensions, shifting dimension if needed.
   *
   * Nodes corresponding to deleted dimensions are replaced with ]-oo;+oo[.
   */
  void remove_dimensions(const dimchange& d);
  
  //! Permute dimensions.
  void permute_dimensions(const dimperm& d);

  //@}


  /* C-level compatibility */
  /* ===================== */

  /** @name C API compatibility */
  //@{

  //! Returns a pointer to the internal APRON object stored in *this.
  ap_texpr0_t* get_ap_texpr0_t();
  
  //! Returns a pointer to the internal APRON object stored in *this.
  const ap_texpr0_t* get_ap_texpr0_t() const;

  //@}

};

#include "apxx_texpr0_inline.hh"

}

#endif /* __APXX_TEXPR0_HH */
