(* File generated from apron_ppl.idl *)


(*
 This file is part of the APRON Library, released under LGPL license.
 Please read the COPYING file packaged in the distribution.
*)

(** Convex Polyhedra and Linear Congruences abstract domains (PPL wrapper) *)

(** This module is a wrapper around the Parma Polyhedra Library *)


type loose
type strict
(** Two flavors for convex polyhedra: loose or strict.

Loose polyhedra cannot have strict inequality constraints like [x>0].
They are algorithmically more efficient
(less generators, simpler normalization).
*)

type grid
(** Linear congruences *)

type 'a t
(** Type of convex polyhedra/linear congruences, where ['a] is [loose], [strict] or [grid].

Abstract values which are convex polyhedra have the type
[loose t Apron.AbstractX.t] or [strict t Apron.AbstractX.t].

Abstract values which are conjunction of linear congruences equalities have the type
[grid t Apron.AbstractX.t].

Managers allocated by PPL have the type ['a t Apron.Manager.t].
*)



(** Allocate a PPL manager for loose convex polyhedra. *)
external manager_alloc_loose : unit -> loose t Apron.Manager.t
	= "camlidl_apron_ppl_ap_ppl_manager_alloc_loose"



(** Allocate a PPL manager for strict convex polyhedra. *)
external manager_alloc_strict : unit -> strict t Apron.Manager.t
	= "camlidl_apron_ppl_ap_ppl_manager_alloc_strict"



(** Allocate a new manager for linear congruences (grids) *)
external manager_alloc_equalities : unit -> grid t Apron.Manager.t
	= "camlidl_apron_ppl_ap_ppl_manager_alloc_grid"

