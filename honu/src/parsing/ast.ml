(**
   The [Ast] module contains all types needed in order to build the AST and
   print functions.
*)
open Utils

module C = Cli

type value =
  | Int of int
  | Float of float
  | String of string
  | Bool of bool
  | Color of Graphics.color

(* Unary operators. *)
and unop =
  | UMinus
  | Not

(* Binary operators. *)
and binop =
  | Plus
  | Minus
  | Times
  | Div
  | And
  | Or
  | Equals
  | Different
  | Greater_than
  | Smaller_than
  | Greater_than_or_eq
  | Smaller_than_or_eq

(** Expressions *)
and exp =
  | Const of value
  | Var_id of string
  | Binop of binop * exp * exp
  | Unop of unop * exp

(** Instructions. *)
and instr =
  | Forward of exp
  | Turn of exp
  | Print of exp
  | Down
  | Up
  | Write of exp
  | Var_assign of string * exp
  | Scope of instr list
  | If_else of exp * instr * instr
  | If of exp * instr
  | While of exp * instr
  | Set_color of exp
  | Set_pen_size of exp
  | Push
  | Pop
  | Proc_call of string * exp list

(** Procedures definition. *)
and proc_def = string * string list * instr

(** A complete Honu AST. *)
and prog = Prog of proc_def list * string list * instr

(** Transforms [statements] into their string representation. *)
let statements_to_str (statements : string list) : string =
  List.fold_left (fun acc s -> acc ^ "\t" ^ s ^ ",\n") "" statements
;;

(** Transforms a [binop] into its string representation. *)
let binop_to_str = function
  | Plus -> "+"
  | Minus -> "-"
  | Times -> "*"
  | Div -> "/"
  | And -> "and"
  | Or -> "or"
  | Equals -> "=="
  | Different -> "!="
  | Greater_than -> ">"
  | Smaller_than -> "<"
  | Greater_than_or_eq -> ">="
  | Smaller_than_or_eq -> "<="
;;

(** Transforms an [unop] into its string representation. *)
let unop_to_str = function
  | UMinus -> "-"
  | Not -> "not "
;;

(** Transforms a [color] into its string representation. *)
let color_to_str (color : Graphics.color) : string =
  if color = Graphics.black
  then "black"
  else if color = Graphics.white
  then "white"
  else if color = Graphics.red
  then "red"
  else if color = Graphics.green
  then "green"
  else if color = Graphics.blue
  then "blue"
  else if color = Graphics.yellow
  then "yellow"
  else if color = Graphics.cyan
  then "cyan"
  else "magenta"
;;

(** Transforms a [value] into its string representation. *)
let value_to_str ?(add_quote = true) = function
  | Int i -> string_of_int i
  | String s -> if add_quote then "\"" ^ s ^ "\"" else s
  | Color c -> color_to_str c
  | Bool b -> string_of_bool b
  | Float f -> string_of_float f
;;

(** Transforms an [exp] into its string representation. *)
let rec exp_to_str = function
  | Const v -> value_to_str v
  | Var_id id -> id
  | Binop (op, e1, e2) ->
    "Binop (" ^ binop_to_str op ^ ", " ^ exp_to_str e1 ^ ", " ^ exp_to_str e2 ^ ")"
  | Unop (op, e) -> "Unop (" ^ unop_to_str op ^ ", " ^ exp_to_str e ^ ")"
;;

(** Transforms an [exp] into its infix string representation. *)
let rec infixexp_to_str = function
  | Const v -> value_to_str v
  | Var_id id -> id
  | Binop (op, e1, e2) ->
    "(" ^ infixexp_to_str e1 ^ " " ^ binop_to_str op ^ " " ^ infixexp_to_str e2 ^ ")"
  | Unop (op, e) -> "(" ^ unop_to_str op ^ " " ^ infixexp_to_str e ^ ")"
;;

(** Transforms [instrs] into their string representation. *)
let rec instructions_to_str (instrs : instr) (depth : int) : string =
  C.str_add_tab
    depth
    (match instrs with
    | Forward e -> "Forward (" ^ exp_to_str e ^ ")"
    | Turn e -> "Turn (" ^ exp_to_str e ^ ")"
    | Down -> "Down"
    | Up -> "Up"
    | Var_assign (id, e) -> "Var_assign (" ^ id ^ ", " ^ exp_to_str e ^ ")"
    | Scope linstr ->
      "Scope (\n"
      ^ List.fold_left
          (fun acc i -> acc ^ instructions_to_str i (depth + 1) ^ ",\n")
          ""
          linstr
      ^ "\t)"
    | If_else (e, i1, i2) ->
      "If_else ("
      ^ exp_to_str e
      ^ ",\n"
      ^ instructions_to_str i1 (depth + 1)
      ^ ",\n"
      ^ instructions_to_str i2 (depth + 1)
      ^ ")"
    | If (e, i) -> "If (" ^ exp_to_str e ^ ",\n" ^ instructions_to_str i (depth + 1) ^ ")"
    | While (e, i) ->
      "While (" ^ exp_to_str e ^ ",\n" ^ instructions_to_str i (depth + 1) ^ ")"
    | Print e -> "Print (" ^ exp_to_str e ^ ")"
    | Set_color e -> "Set_color (" ^ exp_to_str e ^ ")"
    | Set_pen_size s -> "Set_pen_size (" ^ exp_to_str s ^ ")"
    | Push -> "Push"
    | Pop -> "Pop"
    | Proc_call (name, exps) ->
      "Proc_call ("
      ^ name
      ^ ","
      ^ List.fold_left (fun exps e -> exps ^ " (" ^ exp_to_str e ^ ") ") "" exps
      ^ ")"
    | Write e -> "Write (" ^ exp_to_str e ^ ")")
;;

(** Transforms [procedures] into their string representation. *)
let procs_def_to_str procs =
  List.fold_left
    (fun acc (name, args, instr) ->
      acc
      ^ "("
      ^ name
      ^ ",\n\t\t"
      ^ List.fold_left (fun exps e -> exps ^ "(" ^ e ^ ")") "" args
      ^ ",\n"
      ^ instructions_to_str instr 2)
    ""
    procs
;;

(** Transforms a [prog] into its string representation. *)
let prog_to_str (prog : prog) : string =
  let (Prog (procedures, statements, instructions)) = prog in
  "\tProg (\n\t"
  ^ procs_def_to_str procedures
  ^ ",\n"
  ^ statements_to_str statements
  ^ instructions_to_str instructions 1
  ^ ")"
;;
