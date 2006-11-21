(* $Id$ *)

(* This file is part of the APRON Library, released under LGPL license.
   Please read the COPYING file packaged in the distribution  *)

(** Lexical analysis of expressions, constraints, generators *)

{
open Apron_parser
open Lexing
exception Error of int * int
} 

rule lex = parse
  eof    { TK_EOF }
| [' ' '\t' '\n'] +  { lex lexbuf }

| "["    { TK_LBRACKET }
| "]"    { TK_RBRACKET }
| ";"    { TK_SEMICOLON }

| "V:"   { TK_VERTEX }
| "R:"   { TK_RAY }
| "L:"   { TK_LINE }

(* Arithmetic operations *)
| "+"   { TK_ADD }
| "-"   { TK_SUB }
| "*"   { TK_MUL }
| "/"   { TK_DIV }

(* Boolean operations *)
| ">"  { TK_SUP }
| "<"   { TK_INF }
| ">="  { TK_SUPEG }
| "<="   { TK_INFEG }
| "="   { TK_EG }
| "!="   { TK_DISEG }
| (['0'-'9'])+ ('/'['0'-'9']+)?  
  {
    let str = lexeme lexbuf in
    TK_MPQF(Mpqf.of_string str)
  }
| ['0'-'9']* ('.' ['0'-'9']+) (['e' 'E'] ['+' '-']? ['0'-'9']+)?
  { 
    let str = lexeme lexbuf in
    TK_FLOAT(float_of_string str)
  }
(* Identifiers *)
| ['A'-'Z' 'a'-'z' '_']
    ( ['_' 'A'-'Z' 'a'-'z' '0'-'9' '.'] ) *
    ( ['\''] ) *
    { TK_VAR (lexeme lexbuf) }
| _ { raise (Error(lexeme_start lexbuf, lexeme_end lexbuf)) }
