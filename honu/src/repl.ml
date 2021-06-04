(**
   The [Repl] module redefined some [Honu] functions in order to manage string
   as input and keep a persitence in the [I.interpreter_state].

   @note The main REPL structure comes from:
            https://github.com/ocaml-community/lambda-term/blob/master/examples/repl.ml
*)

open Parsing
open Core
open Interpreting
open Utils
open Cli
open React
open Lwt
open LTerm_text
open LTerm_style
module T = Turtle
module A = Ast
module L = Lexer
module P = Parser
module I = Interpreter

(** [Zed_string list] used for the completion. *)
let available_instructions =
  ref
    (List.map
       ~f:(fun s -> Zed_string.unsafe_of_utf8 s)
       [ "if"
       ; "then"
       ; "else"
       ; "while"
       ; "do"
       ; "down"
       ; "up"
       ; "size"
       ; "color"
       ; "var"
       ; "turn"
       ; "forward"
       ; "proc"
       ; "pop"
       ; "push"
       ; "Si"
       ; "Alors"
       ; "Sinon"
       ; "Tant que"
       ; "Faire"
       ; "Var"
       ; "Proc"
       ; "BasPinceau"
       ; "HautPinceau"
       ; "ChangeEpaisseur"
       ; "ChangeCouleur"
       ; "Tourne"
       ; "Avance"
       ; "Depile"
       ; "Empile"
       ])
;;

(** References used in order to store the current [I.interpreter_state] between
    two interpretations. *)
let curr_state =
  ref { I.pos = { x = 0.; y = 0.; a = 0. }; procs = []; vars = []; penup = true }
;;

(** Returns the first variable defined twice in the program [Ast.statements]
    and if is already defined in [curr_state.vars]. *)
let rec get_duplicated_var = function
  | [] -> None
  | var :: tl ->
    if List.exists ~f:(String.equal var) tl
       || List.exists ~f:(fun (name, _) -> String.equal var name) !curr_state.vars
    then Some var
    else get_duplicated_var tl
;;

(** Interprets a [Ast.prog] and draw it, starting from [starting_pos].
    @raise [Interpreter_error]. *)
let interpret_prog (prog : A.prog) : I.interpreter_state =
  let (Prog (procedures, statements, instructions)) = prog in
  match get_duplicated_var statements with
  | Some var ->
    raise (I.Interpreter_error ("Duplicate variable declaration of '" ^ var ^ "'"))
  | None ->
    let new_vars =
      List.fold_left ~init:[] ~f:(fun vars v -> (v, None) :: vars) statements
    in
    available_instructions
      := !available_instructions
         @ List.map ~f:(fun (name, _, _) -> Zed_string.unsafe_of_utf8 name) procedures
         @ List.map ~f:(fun (name, _) -> Zed_string.unsafe_of_utf8 name) new_vars;
    curr_state
      := { !curr_state with
           procs = !curr_state.procs @ procedures
         ; I.vars = !curr_state.vars @ new_vars
         };
    I.eval_instr instructions !curr_state (fun s ->
        let _ignore = LTerm.print s in
        ())
;;

(** Parses and interprets a file. *)
let execute (input : string) : I.interpreter_state =
  Lexing.from_string input |> P.repl_prog L.read |> interpret_prog
;;

(** Creates a styled prompt. *)
let make_prompt () =
  LTerm_text.eval
    (if !Cli.is_styled
    then [ B_bold true; B_fg lgreen; S "honu"; B_bold false; S "> " ]
    else [ S "honu> " ])
;;

(** Creates an error styled error message. *)
let make_err msg =
  LTerm_text.eval (if !Cli.is_styled then [ B_fg lred; S msg ] else [ S msg ])
;;

(** Customization of the read-line engine. *)
class read_line ~term ~history =
  object (self)
    inherit LTerm_read_line.read_line ~history ()

    inherit [Zed_string.t] LTerm_read_line.term term

    method! completion =
      let prefix = Zed_rope.to_string self#input_prev in
      let instructions =
        List.filter
          ~f:(fun instr -> Zed_string.starts_with ~prefix instr)
          !available_instructions
      in
      self#set_completion
        0
        (List.map ~f:(fun instr -> instr, Zed_string.unsafe_of_utf8 " ") instructions)

    initializer self#set_prompt (S.const (make_prompt ()))
  end

(** REPL core. *)
let rec loop term history =
  Lwt.catch
    (fun () ->
      (new read_line ~term ~history:(LTerm_history.contents history))#run
      >|= fun command -> Some command)
    (function
      | Sys.Break -> return None
      | exn -> Lwt.fail exn)
  >>= function
  | Some command ->
    let command_utf8 = Zed_string.to_utf8 command in
    (try
       curr_state := execute command_utf8;
       LTerm.fprint term ""
     with
    | L.Syntax_error msg -> LTerm.fprintls term (make_err msg)
    | P.Error -> LTerm.fprintls term (make_err "Syntax error")
    | I.Interpreter_error msg -> LTerm.fprintls term (make_err msg)
    | T.Turtle_error msg -> LTerm.fprintls term (make_err msg)
    | Sys_error msg -> LTerm.fprintls term (make_err msg))
    >>= fun () ->
    LTerm_history.add history command;
    loop term history
  | None -> loop term history
;;

(** Starts the REPL loop. *)
let repl () =
  if !I.is_graphical then open_window ();
  (* Waits the opening of the graphics's window in order to be used with tiling WMs. *)
  let _a = Unix.nanosleep 0.2 in
  LTerm_inputrc.load ()
  >>= fun () ->
  Lwt.catch
    (fun () ->
      Lazy.force LTerm.stdout >>= fun term -> loop term (LTerm_history.create []))
    (function
      | LTerm_read_line.Interrupt -> Lwt.return ()
      | exn -> Lwt.fail exn)
;;

let run () =
  try Lwt_main.run (repl ()) with
  | Graphics.Graphic_failure _ ->
    print_endline "";
    ()
;;
