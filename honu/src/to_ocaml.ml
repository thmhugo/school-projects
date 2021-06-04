(**
   The [To_ocaml] module contains all functions needed to compile Honu program
   to OCaml file.
*)

open Lexing
open Parsing
open Interpreting
open Utils
open Cli
open Ast
open Turtle
open Stack
open List
open Core
module L = Lexer
module P = Parser
module I = Interpreter

(** Are first lines, used to setup the [Graphics] environment. *)
let ml_file_header =
  "open Graphics\n\n"
  ^ "(* Program entry point. *)\n"
  ^ "let () =\n"
  ^ "  (* Start of generated header. *)\n"
  ^ "  open_graph \" "
  ^ string_of_int Cli.dimx
  ^ "x"
  ^ string_of_int Cli.dimy
  ^ "\";\n"
  ^ "  set_window_title \"honu\";\n"
  ^ "  set_line_width 1;\n"
  ^ "  set_color black;\n"
  ^ "  (* End of generated header. *)\n"
;;

(** Are last lines, used for keeping the [Graphics.window] open. *)
let ml_file_footer =
  "  (* Start of generated footer. *)\n"
  ^ "  try \n"
  ^ "    match (wait_next_event [ Key_pressed ]).key with\n"
  ^ "    | _ -> ()\n"
  ^ "  with \n"
  ^ "  | Graphics.Graphic_failure _ -> ()\n"
  ^ "  (* End of generated footer. *)\n;;"
;;

(** Evaluates the first [Ast.instr] of a Honu program according the current
    [state] and program [Ast.statements], and writes needed OCaml expression in
    to [out].  *)
let to_ocaml_eval_instr
    (instrs : instr)
    (state : I.interpreter_state)
    (out : Out_channel.t)
    : I.interpreter_state
  =
  (* Vistor function used in [Turtle.forward] in order to writes [Graphics] instructions. *)
  let move_forward_and_write (p : turtle_pos) (new_p : turtle_pos) (penup : bool) =
    let print_out fmt x y =
      Printf.fprintf
        out
        fmt
        (Float.iround_towards_zero_exn x)
        (Float.iround_towards_zero_exn y)
    in
    let print_lineto = print_out "  lineto %d %d;\n" in
    let print_moveto = print_out "  moveto %d %d;\n" in
    if not penup
    then (
      print_moveto p.x p.y;
      print_lineto new_p.x new_p.y)
    else print_moveto p.x p.y
  in
  (* Stack used to store [Turtle.turtle_pos]. *)
  let to_ocaml_stack = create () in
  (* Main loop to evaluate instructions. *)
  let rec to_ocaml_eval_loop instr (state : I.interpreter_state) : I.interpreter_state =
    match instr with
    | Forward e ->
      { state with
        pos =
          Turtle.forward
            ~visitor:move_forward_and_write
            (I.eval_exp_to_float e state.vars)
            state.pos
            state.penup
      }
    | Turn e -> { state with pos = turn (I.eval_exp_to_float e state.vars) state.pos }
    | Down -> { state with penup = false }
    | Up -> { state with penup = true }
    | Var_assign (id, e) ->
      if exists (fun (var, _) -> String.equal var id) state.vars
      then
        { state with
          vars =
            map
              (fun (name, value) ->
                if String.equal name id
                then name, Some (I.eval_exp e state.vars)
                else name, value)
              state.vars
        }
      else raise (I.Interpreter_error ("Unknown variable '" ^ id ^ "'"))
    | If_else (cond, i1, i2) ->
      let i = if Float.( <> ) 0. (I.eval_exp_to_float cond state.vars) then i1 else i2 in
      to_ocaml_eval_loop i state
    | If (cond, i) ->
      if Float.( <> ) 0. (I.eval_exp_to_float cond state.vars)
      then to_ocaml_eval_loop i state
      else state
    | While (cond, i) ->
      let rec eval_while cond i (state : I.interpreter_state) nb_step =
        if nb_step > !I.max_loop_step
        then
          raise
            (I.Interpreter_error
               ("Max loop step reached (" ^ string_of_int !I.max_loop_step ^ " steps)"))
        else if Float.( = ) 0. (I.eval_exp_to_float cond state.vars)
        then state
        else (
          let state = to_ocaml_eval_loop i state in
          eval_while cond i state (nb_step + 1))
      in
      if (not (I.contains_var cond))
         && Float.( <> ) 0. (I.eval_exp_to_float cond state.vars)
      then
        raise
          (I.Interpreter_error
             ("Infinite loop detected because of the while condition: '"
             ^ infixexp_to_str cond
             ^ "' != 0"))
      else eval_while cond i state 0
    | Print e ->
      Printf.fprintf
        out
        "  print_endline (\"%s\");\n"
        (Printf.sprintf
           "[ %s ] => %s"
           (infixexp_to_str e)
           (value_to_str (I.eval_exp e state.vars)));
      state
    | Set_color e ->
      Printf.fprintf out "  set_color %s;\n"
      @@ color_to_str
      @@ I.color_of_exp e state.vars;
      state
    | Set_pen_size s ->
      Printf.fprintf
        out
        "  set_line_width %d;\n"
        (int_of_float (I.eval_exp_to_float s state.vars));
      state
    | Write exp ->
      Printf.fprintf
        out
        "  draw_string \"%s\";\n"
        (value_to_str ~add_quote:false (I.eval_exp exp state.vars));
      state
    | Push ->
      push state.pos to_ocaml_stack;
      state
    | Pop ->
      (try { state with pos = pop to_ocaml_stack } with
      | Empty -> raise (I.Interpreter_error "Empty to_ocaml_stack"))
    | Scope linstr -> fold_left (fun state e -> to_ocaml_eval_loop e state) state linstr
    | Proc_call (name, arg_exps) ->
      (match filter (fun (name', _, _) -> String.equal name name') state.procs |> rev with
      (* Try to get the last procedure definition named [name]. *)
      | [] -> raise (I.Interpreter_error ("Undefined procedure '" ^ name ^ "'"))
      | (_, arg_names, instr) :: _ ->
        (* Verifies that the procedure is called with the same argument number as definied *)
        if Int.( <> ) 0 (compare_lengths arg_names arg_exps)
        then
          raise
            (I.Interpreter_error
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
                  (fun args name exp -> (name, Some (I.eval_exp exp state.vars)) :: args)
                  []
                  arg_names
                  arg_exps
            }
          in
          (* Restore the [interpreter_state.vars] after avaluating the procedure. *)
          { (to_ocaml_eval_loop instr state) with vars = current_vars }))
  in
  to_ocaml_eval_loop instrs state
;;

(** Interprets a [Ast.prog] and draw it, starting from [starting_pos].
    @todo to factorize.
    @raise [Interpreter_error]. *)
let interpret_prog (prog : prog) (starting_pos : turtle_pos) (out : Out_channel.t) : unit =
  let (Prog (procedures, statements, instructions)) = prog in
  match I.get_duplicated_var statements with
  | Some var ->
    raise (I.Interpreter_error ("Duplicate variable declaration of '" ^ var ^ "'"))
  | None ->
    to_ocaml_eval_instr
      instructions
      { pos = starting_pos
      ; procs = procedures
      ; vars = fold_left (fun vars v -> (v, None) :: vars) [] statements
      ; penup = true
      }
      out
    |> fun _ -> ()
;;

(** Writes the OCaml code in [out] in order to reproduce the evaluted [prog]. *)
let write_prog (prog : Ast.prog) (pos : turtle_pos) (out : Out_channel.t) : unit =
  Printf.fprintf out "%s" ml_file_header;
  Printf.fprintf out "\n  (* Start of translated Honu program. *)\n";
  interpret_prog prog pos out;
  Printf.fprintf out "  (* End of translated Honu program. *)\n\n";
  Printf.fprintf out "%s" ml_file_footer
;;

(** Creates [A.prog] from the given file.
    @note Should be factorizable with the [Honu.parse] function. *)
let parse (lexbuf : lexbuf) : Ast.prog option =
  try Some (P.prog L.read lexbuf) with
  | L.Syntax_error msg ->
    print_err_with_style (get_position lexbuf) msg;
    None
  | P.Error ->
    print_err_with_style (get_position lexbuf) "Syntax error";
    None
;;

(** Parses and compile an Honu file into a standalone OCaml file. *)
let to_ocaml (filein : string) (fileout : string) : unit =
  let out = Out_channel.create fileout in
  let inx = In_channel.create filein in
  let lexbuf = Lexing.from_channel inx in
  lexbuf.lex_curr_p <- { lexbuf.lex_curr_p with pos_fname = filein };
  (match parse lexbuf with
  | Some prog ->
    if !is_verbose then prog |> Ast.prog_to_str |> print_info_with_style "Builded AST";
    write_prog prog !Turtle.starting_pos out
  | None -> ());
  Out_channel.close out;
  In_channel.close inx
;;

(** Manages exceptions from [to_ocaml]. *)
let to_ocaml_safe (filein : string) (fileout : string) : unit =
  try to_ocaml filein fileout with
  | I.Interpreter_error msg -> print_err_with_style "Interpreter error" msg
  | Turtle_error msg -> print_err_with_style "Graphic error" msg
  | Graphics.Graphic_failure _ -> ()
  | Sys_error msg -> print_err_with_style "System error" msg
;;
