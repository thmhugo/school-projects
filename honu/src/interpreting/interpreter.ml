(**
   The [Intepreter] module contains the function that interpret an AST in order to draw it.
*)

open Parsing
open Ast
open Turtle
open Utils
open Stack

exception Interpreter_error of string

(** Corresponds to the path to save the compiled drawing as PNG *)
let save_path = ref None

(** Number of maximum step allowed to a while loop interpretation. *)
let max_loop_step = ref 100

(** Is set to [false] when the [--no-graphic] flag is set. *)
let is_graphical = ref true

(** Stores the interpreter state. *)
type interpreter_state =
  { pos : turtle_pos (** Is the pen position on the canvas. *)
  ; procs : (string * string list * instr) list (** Is the list of defined procedures. *)
  ; vars : (string * value option) list (** Is the list of declared variables. *)
  ; penup : bool
  }

(** Stack used for the push and pop instructions*)
let stack = create ()

(** Returns the float value of a type value.
    @raise [Interpreter_error] is the given value is a string.
 *)
let float_of_value v =
  match v with
  | Int i -> float_of_int i
  | Float f -> f
  | Bool b -> if b then 1. else 0.
  | _ ->
    raise (Interpreter_error ("Expected a number or a boolean, got: " ^ value_to_str v))
;;

(** Auxiliary function to evaluate number operations. *)
let eval_fi_binop_aux fop iop = function
  | Int i, Int j -> Int (iop i j)
  | Float i, Float j -> Float (fop i j)
  | Float i, Int j -> Float (fop i (float_of_int j))
  | Int i, Float j -> Float (fop (float_of_int i) j)
  | _ -> raise (Interpreter_error "Not a number operation.")
;;

(** Auxiliary function to evaluate boolean operations. *)
let eval_bool_binop_aux fop iop = function
  | Int i, Int j -> Bool (iop i j)
  | Float i, Float j -> Bool (fop i j)
  | Float i, Int j -> Bool (fop i (float_of_int j))
  | Int i, Float j -> Bool (fop (float_of_int i) j)
  | _ -> raise (Interpreter_error "Not a boolean operation.")
;;

(** Notifies an illegal [binop] operation exception by raising an [Interpreter_error]. *)
let raise_illegal_binop_operation x y op =
  raise
    (Interpreter_error
       ("Illegal operation: "
       ^ value_to_str x
       ^ " "
       ^ binop_to_str op
       ^ " "
       ^ value_to_str y))
;;

(** Notifies an illegal [unop] operation exception by raising an [Interpreter_error]. *)
let raise_illegal_unop_operation v op =
  raise
    (Interpreter_error ("Illegal operation: " ^ unop_to_str op ^ " " ^ value_to_str v))
;;

(** Notifies a division by zero exception by raisin an [Inerpreter_error]. *)
let raise_division_by_zero s =
  raise (Interpreter_error ("Illegal division by zero : " ^ s ^ " / 0"))
;;

(** Evaluates an [Ast.exp].
    @raise [Interpreter_error] if a variable in an [Ast.exp] is undefined. *)
let rec eval_exp (e : exp) (vars : (string * value option) list) : value =
  match e with
  | Const v -> v
  | Var_id id ->
    (try
       match List.assoc id vars with
       | None -> raise (Interpreter_error ("Unassigned variable '" ^ id ^ "'"))
       | Some v -> v
     with
    | Not_found ->
      raise
        (Interpreter_error
           (Printf.sprintf
              "Undeclared variable '%s' (hint: add 'Var %s' after procedures definitions)"
              id
              id)))
  | Binop (o, x, y) -> eval_binop o x y vars
  | Unop (o, x) -> eval_unop o x vars

and color_of_exp exp vars =
  let e = eval_exp exp vars in
  match e with
  | Color c -> c
  | _ -> raise (Interpreter_error ("Illegal color: " ^ exp_to_str exp))

and eval_unop o x vars =
  let v = eval_exp x vars in
  match o with
  | UMinus ->
    (match v with
    | Int i -> Int ~-i
    | Float f -> Float ~-.f
    | Bool b -> Bool (not b)
    | _ -> raise_illegal_unop_operation v UMinus)
  | Not ->
    (match v with
    | Bool b -> Bool (not b)
    | _ -> raise_illegal_unop_operation v Not)

and eval_binop o x y vars =
  let x = eval_exp x vars in
  let y = eval_exp y vars in
  match o with
  | Plus ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_fi_binop_aux ( +. ) ( + ) (x, y)
    | String s, String t -> String (s ^ t)
    | _ -> raise_illegal_binop_operation x y Plus)
  | Minus ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_fi_binop_aux ( -. ) ( - ) (x, y)
    | _ -> raise_illegal_binop_operation x y Minus)
  | Times ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_fi_binop_aux ( *. ) ( * ) (x, y)
    | _ -> raise_illegal_binop_operation x y Times)
  | Div ->
    (* Not that much factored to display a correct error message. *)
    (match x, y with
    | Int i, Int j ->
      if j <> 0 then Int (i / j) else raise_division_by_zero (string_of_int i)
    | Float i, Float j ->
      if j <> 0. then Float (i /. j) else raise_division_by_zero (string_of_float i)
    | Float i, Int j ->
      if j <> 0
      then Float (i /. float_of_int j)
      else raise_division_by_zero (string_of_float i)
    | Int i, Float j ->
      if j <> 0.
      then Float (float_of_int i /. j)
      else raise_division_by_zero (string_of_int i)
    | _ -> raise_illegal_binop_operation x y Div)
  | And ->
    (match x, y with
    | Bool i, Bool j -> Bool (i && j)
    | _ -> raise_illegal_binop_operation x y And)
  | Or ->
    (match x, y with
    | Bool i, Bool j -> Bool (i || j)
    | _ -> raise_illegal_binop_operation x y Or)
  | Equals ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_bool_binop_aux ( = ) ( = ) (x, y)
    | Color a, Color b -> Bool (a = b)
    | String a, String b -> Bool (a = b)
    | Bool a, Bool b -> Bool (a = b)
    | _ -> raise_illegal_binop_operation x y Equals)
  | Different ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_bool_binop_aux ( <> ) ( <> ) (x, y)
    | Color a, Color b -> Bool (a <> b)
    | String a, String b -> Bool (a <> b)
    | Bool a, Bool b -> Bool (a <> b)
    | _ -> raise_illegal_binop_operation x y Different)
  | Greater_than ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_bool_binop_aux ( > ) ( > ) (x, y)
    | _ -> raise_illegal_binop_operation x y Greater_than)
  | Smaller_than ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_bool_binop_aux ( < ) ( < ) (x, y)
    | _ -> raise_illegal_binop_operation x y Smaller_than)
  | Greater_than_or_eq ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_bool_binop_aux ( >= ) ( >= ) (x, y)
    | _ -> raise_illegal_binop_operation x y Greater_than_or_eq)
  | Smaller_than_or_eq ->
    (match x, y with
    | Int _, Int _ | Float _, Float _ | Float _, Int _ | Int _, Float _ ->
      eval_bool_binop_aux ( <= ) ( <= ) (x, y)
    | _ -> raise_illegal_binop_operation x y Smaller_than_or_eq)
;;

let eval_exp_to_float exp vars = float_of_value (eval_exp exp vars)

(** Consume the last [interpreter_state] and waits for a [Key_pressed]
 before closing the graphic window. *)
let consume_and_wait (_ : interpreter_state) : unit =
  match !save_path with
  | Some path -> Png.save path
  | None ->
    let event = Graphics.wait_next_event [ Key_pressed ] in
    (match event.key with
    | _ -> ())
;;

(** Returns the first variables defined twice in the program [Ast.statements]. *)
let rec get_duplicated_var = function
  | [] -> None
  | var :: tl -> if List.exists (( = ) var) tl then Some var else get_duplicated_var tl
;;

(** Check if the expresion contains a variable. *)
let rec contains_var = function
  | Const _ -> false
  | Var_id _ -> true
  | Binop (_, e1, e2) -> contains_var e1 || contains_var e2
  | Unop (_, e) -> contains_var e
;;

(** Evaluates an [Ast.instr] according the current [state] and program [Ast.statements].
    @raise [Interpreter_error]. *)
let eval_instr
    (instrs : instr)
    (state : interpreter_state)
    (printer_visitor : string -> unit)
    : interpreter_state
  =
  let open List in
  let rec eval_loop instrs state =
    match instrs with
    | Forward e ->
      { state with pos = forward (eval_exp_to_float e state.vars) state.pos state.penup }
    | Turn e -> { state with pos = turn (eval_exp_to_float e state.vars) state.pos }
    | Down -> { state with penup = (if !is_graphical then false else true) }
    | Up -> { state with penup = true }
    | Var_assign (id, e) ->
      if exists (fun (var, _) -> var = id) state.vars
      then
        { state with
          vars =
            map
              (fun (name, value) ->
                if name = id then name, Some (eval_exp e state.vars) else name, value)
              state.vars
        }
      else raise (Interpreter_error ("Unknown variable '" ^ id ^ "'"))
    | Scope linstr -> fold_left (fun state e -> eval_loop e state) state linstr
    | If_else (cond, i1, i2) ->
      let i = if 0. <> eval_exp_to_float cond state.vars then i1 else i2 in
      eval_loop i state
    | If (cond, i) ->
      if 0. <> eval_exp_to_float cond state.vars then eval_loop i state else state
    | While (cond, i) ->
      let rec eval_while cond i state nb_step =
        if nb_step > !max_loop_step
        then
          raise
            (Interpreter_error
               ("Max loop step reached (" ^ string_of_int !max_loop_step ^ " steps)"))
        else if 0. = eval_exp_to_float cond state.vars
        then state
        else (
          let state = eval_loop i state in
          eval_while cond i state (nb_step + 1))
      in
      if (not (contains_var cond)) && 0. <> eval_exp_to_float cond state.vars
      then
        raise
          (Interpreter_error
             ("Infinite loop detected because of the while condition: '"
             ^ infixexp_to_str cond
             ^ "' != 0"))
      else eval_while cond i state 0
    | Print e ->
      printer_visitor
        (Printf.sprintf
           "[ %s ] => %s\n"
           (infixexp_to_str e)
           (value_to_str (eval_exp e state.vars)));
      state
    | Set_color e ->
      Graphics.set_color (color_of_exp e state.vars);
      state
    | Set_pen_size s ->
      Graphics.set_line_width (int_of_float (eval_exp_to_float s state.vars));
      state
    | Push ->
      push state.pos stack;
      state
    | Pop ->
      (try { state with pos = pop stack } with
      | Empty -> raise (Interpreter_error "Empty stack"))
    | Proc_call (name, arg_exps) ->
      (match filter (fun (name', _, _) -> name = name') state.procs |> rev with
      (* Try to get the last procedure definition named [name]. *)
      | [] -> raise (Interpreter_error ("Undefined procedure '" ^ name ^ "'"))
      | (_, arg_names, instr) :: _ ->
        (* Verifies that the procedure is called with the same argument number as definied *)
        if 0 <> compare_lengths arg_names arg_exps
        then
          raise
            (Interpreter_error
               (Printf.sprintf
                  "The procedure '%s' expects %d arguments got %d"
                  name
                  (length arg_names)
                  (length arg_exps)))
        else (
          (* Inside of a procedure scope, global variables aren't accessible.
           Consequently, they needs to be stored in order to be able to restore
           them after procedure execution. *)
          let current_vars = state.vars in
          let state =
            { state with
              (* Creates a new [interpreter_state.vars] from arguments. *)
              vars =
                fold_left2
                  (fun args name exp -> (name, Some (eval_exp exp state.vars)) :: args)
                  []
                  arg_names
                  arg_exps
            }
          in
          (* Restore the [interpreter_state.vars] after avaluating the procedure. *)
          { (eval_loop instr state) with vars = current_vars }))
    | Write exp ->
      Graphics.draw_string (value_to_str ~add_quote:false (eval_exp exp state.vars));
      state
  in
  eval_loop instrs state
;;

(** Interprets a [Ast.prog] and draw it, starting from [starting_pos].
    @raise [Interpreter_error]. *)
let interpret_prog
    (prog : prog)
    (starting_pos : turtle_pos)
    (printer_visitor : string -> unit)
    : unit
  =
  let (Prog (procedures, statements, instructions)) = prog in
  match get_duplicated_var statements with
  | Some var ->
    raise (Interpreter_error ("Duplicate variable declaration of '" ^ var ^ "'"))
  | None ->
    eval_instr
      instructions
      { pos = starting_pos
      ; procs = procedures
      ; vars = List.fold_left (fun vars v -> (v, None) :: vars) [] statements
      ; penup = true
      }
      printer_visitor
    |> consume_and_wait
;;
