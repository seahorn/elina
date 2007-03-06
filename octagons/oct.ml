(* File generated from oct.idl *)
type internal
(*
 This file is part of the APRON Library, released under LGPL license.
 Please read the COPYING file packaged in the distribution.
*)
 
type t
(** Type of octagons.
Abstract values which are octagons have the type
[t Apron.Abstract0.t] or [t Apron.Abstract1.t].
Managers allocated for octagons have the type [t Apron.manager.t].
*)
 
external manager_alloc : unit -> t Apron.Manager.t
	= "camlidl_oct_oct_manager_alloc"
external manager_get_internal : t Apron.Manager.t -> internal
	= "camlidl_oct_manager_get_internal"
external of_generator_array : t Apron.Manager.t -> int -> int -> Apron.Generator0.t array -> t Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_of_generator_array"
external widening_thresholds : t Apron.Manager.t -> t Apron.Abstract0.t -> t Apron.Abstract0.t -> Apron.Scalar.t array -> t Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_widening_thresholds"
external narrowing : t Apron.Manager.t -> t Apron.Abstract0.t -> t Apron.Abstract0.t -> t Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_narrowing"
external add_epsilon : t Apron.Manager.t -> t Apron.Abstract0.t -> Apron.Scalar.t -> t Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_add_epsilon"
external add_epsilon_bin : t Apron.Manager.t -> t Apron.Abstract0.t -> t Apron.Abstract0.t -> Apron.Scalar.t -> t Apron.Abstract0.t
	= "camlidl_oct_ap_abstract0_oct_add_epsilon_bin"
let pre_widening = 99
