(* File generated from apron_ppl.idl *)
(*
 This file is part of the APRON Library, released under LGPL license.
 Please read the COPYING file packaged in the distribution.
*)
external poly_manager_alloc : bool -> Apron.Manager.t
	= "camlidl_apron_ppl_ap_ppl_poly_manager_alloc"
external grid_manager_alloc : unit -> Apron.Manager.t
	= "camlidl_apron_ppl_ap_ppl_grid_manager_alloc"
