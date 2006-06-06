(* compile with either:
ocamlc -I ${INSTALL_DIR}/lib mltest.ml bigarray.cma gmp.cma octZi.cma
ocamlopt -I ${INSTALL_DIR}/lib mltest.ml bigarray.cmxa gmp.cmxa octZi.cmxa
*)

open Apron

let m = Oct.manager_alloc ()
let _ = 
  Printf.printf "name: %s\nversion: %s\n" 
    (Manager.get_library m) (Manager.get_version m)
