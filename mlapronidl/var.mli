(* File generated from var.idl *)

(* This file is part of the APRON Library, released under LGPL license.
   Please read the COPYING file packaged in the distribution  *)

type t

external of_string : string -> t
	= "camlidl_var_ap_var_of_string"

external compare : t -> t -> int
	= "camlidl_var_ap_var_compare"

external to_string : t -> string
	= "camlidl_var_ap_var_to_string"

external hash : t -> int
	= "camlidl_var_ap_var_hash"


val print : Format.formatter -> t -> unit

