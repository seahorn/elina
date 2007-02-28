(* File generated from apron_ppl.idl *)
(*
 This file is part of the APRON Library, released under LGPL license.
 Please read the COPYING file packaged in the distribution.
*)
(** APRON PPL: Parma Polyhedra Library wrapper. 
*)
(* *)
external poly_manager_alloc : bool -> Apron.Manager.t
	= "camlidl_apron_ppl_ap_ppl_poly_manager_alloc"
(** Allocate a new manager to manipulate PPL Polyhedra. *)
external grid_manager_alloc : unit -> Apron.Manager.t
	= "camlidl_apron_ppl_ap_ppl_grid_manager_alloc"
(** Allocate a new manager to manipulate PPL Grids. *)
