(* File generated from oct.idl *)
type internal
(*
 This file is part of the APRON Library, released under LGPL license.
 Please read the COPYING file packaged in the distribution.
*)
external manager_alloc : unit -> Apron.Manager.t
	= "camlidl_oct_oct_manager_alloc"
external manager_get_internal : Apron.Manager.t -> internal
	= "camlidl_oct_manager_get_internal"
external of_generator_array : Apron.Manager.t -> int -> int -> Apron.Generator0.t array -> Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_of_generator_array"
external widening_thresholds : Apron.Manager.t -> Apron.Abstract0.t -> Apron.Abstract0.t -> Apron.Scalar.t array -> Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_widening_thresholds"
external narrowing : Apron.Manager.t -> Apron.Abstract0.t -> Apron.Abstract0.t -> Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_narrowing"
external add_epsilon : Apron.Manager.t -> Apron.Abstract0.t -> Apron.Scalar.t -> Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_add_epsilon"
external add_epsilon_bin : Apron.Manager.t -> Apron.Abstract0.t -> Apron.Abstract0.t -> Apron.Scalar.t -> Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_add_epsilon_bin"
let pre_widening = 99
