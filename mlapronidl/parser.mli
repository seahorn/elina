(* $Id$ *)

(* This file is part of the APRON Library, released under LGPL license.
   Please read the COPYING file packaged in the distribution  *)

(** APRON Parsing of expressions *)

(** {2 Introduction}

  This small module implements the parsing of expressions, constraints and
  generators. The allowed syntax is simple (no parenthesis) but supports
  interval expressions.

  [cons ::= expr ('>' | '>=' | '=' | '!=' | '=' | '<=' | '<') expr | expr = expr 'mod' scalar]

  [gen ::= ('V:' | 'R:' | 'L:' | 'RM:' | 'LM:') expr]

  [expr ::= expr '+' term | expr '-' term | term] 
  
  [term ::= coeff ['*'] identifier | coeff | ['-'] identifier]

  [coeff ::= scalar | ['-'] '['scalar ';' scalar ']']

  [scalar ::= ['-'] (integer | rational | floating_point_number)]

  There is the possibility to parse directly from a lexing buffer, 
  or from a string (from which one can generate a buffer with the
  function [Lexing.from_string].

  This module uses the underlying modules [Apron_lexer] and [Apron_parser].
*)

(** {2 Interface} *)

exception Error of string
  (** Raised by conversion functions *)

val linexpr1_of_lexbuf : Environment.t -> Lexing.lexbuf -> Linexpr1.t
val lincons1_of_lexbuf : Environment.t -> Lexing.lexbuf -> Lincons1.t
val generator1_of_lexbuf : Environment.t -> Lexing.lexbuf -> Generator1.t
  (** Conversion from lexing buffers to resp. linear expressions, linear
    constraints and generators, defined on the given environment. *)

val linexpr1_of_string : Environment.t -> string -> Linexpr1.t
val lincons1_of_string : Environment.t -> string -> Lincons1.t
val generator1_of_string : Environment.t -> string -> Generator1.t
  (** Conversion from strings to resp. linear expressions, linear
    constraints and generators, defined on the given environment. *)

val lincons1_of_lstring : Environment.t -> string list -> Lincons1.earray
val generator1_of_lstring : Environment.t -> string list -> Generator1.earray
  (** Conversion from lists of strings to array of resp. linear
    constraints and generators, defined on the given environment. *)

val of_lstring : 'a Manager.t -> Environment.t -> string list -> 'a Abstract1.t
  (** Abstraction of lists of strings representing constraints to abstract
    values, on the abstract domain defined by the given manager. *)
