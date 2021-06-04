(**
   The [Hast] module contains all types needed in order to build the AST and
   print functions for the Honu documentation compilation.
*)

(** Starting by @ in the comment. *)
type tag =
  | Note of string
  | Fixme of string
  | Todo of string
  | Param of string * string
  | Text of string

(** Procedure or variable declaration. *)
and decl =
  | Proc of string * string list
  | Var of string

(** Set of all comments. *)
and hdoc = Doc of (tag list * decl) list

(** Transforms a [tag] into its string representation. *)
let tag_to_string = function
  | Note s -> "Note (" ^ s ^ ")"
  | Fixme s -> "Fixme (" ^ s ^ ")"
  | Todo s -> "Todo (" ^ s ^ ")"
  | Text s -> "Text (" ^ s ^ ")"
  | Param (id, text) -> "Param (" ^ id ^ ", " ^ text ^ ")"
;;

(** Transforms a [decl] into its string representation. *)
let decl_to_string = function
  | Proc (name, args) -> "Proc (" ^ name ^ ", [ " ^ String.concat ", " args ^ " ])"
  | Var id -> "Var (" ^ id ^ ")"
;;

(** Transforms a [hdoc] into its string representation. *)
let hdoc_to_string = function
  | Doc items ->
    "\tDoc ("
    ^ List.fold_left
        (fun str item ->
          let tags, decl = item in
          str
          ^ "\n\t\t[  "
          ^ List.fold_left (fun acc t -> acc ^ tag_to_string t ^ ", ") "" tags
          ^ "\n\t\t"
          ^ decl_to_string decl
          ^ "  ]")
        ""
        items
    ^ ")"
;;
