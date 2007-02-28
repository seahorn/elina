(* mltest.ml
 * 
 * A very simple example
 *
 * APRON Library / Octagonal Domain
 *
 * Copyright (C) Antoine Mine' 2007
 *)


(* compile with:

ocamlc -I ${INSTALL_DIR}/lib mltest.ml \
  oct.cma -cclib -loctQg -cclib -loct_caml \
  apron.cma -cclib -lapron -cclib -lapron_caml \
  gmp.cma -cclib -lgmp_caml \
  -cclib -lcamlidl

you can change:
- INSTALL_DIR to where APRON is installed,
- -loctQg to the desired underlying numeric type,
- ocamlc to ocamlopt, and replace each .cma to the corresponding .cmxa

*)

open Apron

let m = Oct.manager_alloc ()
let _ = 
  Printf.printf "name: %s\nversion: %s\n" 
    (Manager.get_library m) (Manager.get_version m)

(* TO BE CONTINUED *)
