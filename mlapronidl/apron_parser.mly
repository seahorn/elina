/* $Id$ */

/* Syntaxical analysis to convert strings to objects. */

%{
(* This file is part of the APRON Library, released under LGPL license.
   Please read the COPYING file packaged in the distribution  *)

let rec neg acc = function
  | [] -> acc
  | (var,coeff)::l ->
      neg ((var,(Coeff.neg coeff))::acc) l

%}

/* \section{Lexems} %======================================================== */

%token TK_EOF

%token TK_VERTEX TK_RAY TK_LINE

%token TK_MUL TK_ADD TK_SUB TK_DIV

%token TK_SUPEG TK_INFEG TK_SUP TK_INF TK_EG TK_DISEG

%token TK_LBRACKET TK_RBRACKET TK_SEMICOLON

%token <Mpqf.t> TK_MPQF
%token <float> TK_FLOAT
%token <string> TK_VAR


%start lincons generator linexpr

%type <Lincons0.typ * (string*Coeff.t) list> lincons
%type <Generator0.typ * (string*Coeff.t) list> generator
%type <(string*Coeff.t) list> linexpr

%%

/* \section{Rules} %========================================================= */

lincons:
  linexpr0 TK_EG linexpr0 TK_EOF { (Lincons0.EQ, neg $1 $3) }
| linexpr0 TK_DISEG linexpr0 TK_EOF { failwith "!= not yet supported" }
| linexpr0 TK_SUP linexpr0 TK_EOF { (Lincons0.SUP, neg $1 $3) }
| linexpr0 TK_SUPEG linexpr0 TK_EOF { (Lincons0.SUPEQ, neg $1 $3) }
| linexpr0 TK_INFEG linexpr0 TK_EOF { (Lincons0.SUPEQ, neg $3 $1) }
| linexpr0 TK_INF linexpr0 TK_EOF { (Lincons0.SUP, neg $3 $1) }

generator:
  TK_VERTEX linexpr0 TK_EOF { (Generator0.VERTEX,$2) }
| TK_RAY linexpr0 TK_EOF { (Generator0.RAY,$2) }
| TK_LINE linexpr0 TK_EOF { (Generator0.LINE,$2) }

linexpr:
  linexpr0 TK_EOF { $1 }

linexpr0:
  linexpr0 TK_ADD term 
    { $3::$1 }
| linexpr0 TK_SUB term 
{ let (var,coeff) = $3 in (var,Coeff.neg coeff)::$1 }
| term { [$1] }
term:
  coeff id { ($2,$1) }
| coeff TK_MUL id { ($3,$1) }
| coeff { ("",$1) }
| id { ($1, Coeff.s_of_int 1) }
| TK_SUB id { ($2, Coeff.s_of_int (-1)) }

id:
  TK_VAR { $1 }

scalar0:
  TK_MPQF { Scalar.Mpqf($1) }
| TK_FLOAT { Scalar.Float($1) }
scalar:
  scalar0 { $1 }
| TK_SUB scalar0 { Scalar.neg $2 }
coeff0:
  scalar0 
    { Coeff.Scalar $1 }
| TK_LBRACKET scalar TK_SEMICOLON scalar TK_RBRACKET 
    { Coeff.Interval(Interval.of_infsup $2 $4) }
coeff:
  coeff0 { $1 }
| TK_SUB coeff0 { Coeff.neg $2 }

