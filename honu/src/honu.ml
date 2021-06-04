(**
   The [Honu] module is the entry point of the program.
*)

open Lexing
open Parsing
open Core
open Interpreting
open Utils
open Cli
open To_ocaml
module T = Turtle
module A = Ast
module L = Lexer
module P = Parser
module I = Interpreter

(** Creates [A.prog] from the given file. *)
let parse (lexbuf : lexbuf) : A.prog option =
  try Some (P.prog L.read lexbuf) with
  | L.Syntax_error msg ->
    print_err_with_style (get_position lexbuf) msg;
    None
  | P.Error ->
    print_err_with_style (get_position lexbuf) "Syntax error";
    None
;;

(** Parses and interprets a file. *)
let execute (filename : string) : unit =
  if !I.is_graphical then open_window ();
  let inx = In_channel.create filename in
  let lexbuf = Lexing.from_channel inx in
  lexbuf.lex_curr_p <- { lexbuf.lex_curr_p with pos_fname = filename };
  (match parse lexbuf with
  | Some prog ->
    if !is_verbose then prog |> A.prog_to_str |> print_info_with_style "Builded AST";
    I.interpret_prog prog !T.starting_pos print_string
  | None -> ());
  In_channel.close inx
;;

(** Manages exceptions from [execute]. *)
let execute_safe filein =
  try execute filein with
  | I.Interpreter_error msg -> print_err_with_style "Interpreter error" msg
  | T.Turtle_error msg -> print_err_with_style "Graphic error" msg
  | Graphics.Graphic_failure _ -> ()
  | Sys_error msg -> print_err_with_style "System error" msg
;;

(** Make sure that the file destination for the compile subcommand has the right format. *)
let correct_filename_extension (filename : string) : bool * string =
  let ext = List.hd (List.rev (String.split ~on:'.' filename)) in
  match ext with
  | None -> false, filename ^ ".png"
  | Some s ->
    if List.mem [ "ml" ] s ~equal:String.equal
    then true, filename
    else if List.mem [ "png"; "jpg" ] s ~equal:String.equal
    then false, filename
    else false, filename ^ ".png"
;;

(* Flag associated to the verbose option. *)
let is_verbose_flag =
  Command.Spec.flag "--verbose" Command.Spec.no_arg ~doc:"Enable verbose mode."
;;

(** Flag associed to the color option. *)
let no_color_flag =
  Command.Spec.flag "--no-color" Command.Spec.no_arg ~doc:"Disable colored output."
;;

(** Flag associed to the [Interpreter.is_graphical] reference. *)
let no_graphic_flag =
  Command.Spec.flag "--no-graphic" Command.Spec.no_arg ~doc:"Disable the graphic window."
;;

(** Flag associed to the starting position. *)
let centering_flag =
  Command.Spec.flag
    "--center"
    Command.Spec.no_arg
    ~doc:"Use a centered starting position instead of the default bottom-left one."
;;

(** Apply references assignment based of the given flags. *)
let apply_flags is_verbose_flag no_color_flag no_graphic_flag is_centered =
  is_verbose := is_verbose_flag;
  Cli.is_styled := not no_color_flag;
  I.is_graphical := not no_graphic_flag;
  if is_centered then T.starting_pos := T.centered_pos
;;

(** Compile sub-command. *)
let compile =
  Command.basic
    ~summary:"Compile the specified Honu file."
    ~readme:(fun () ->
      "Targeted file type is infered from the FILEOUT extension.\n"
      ^ "Available file extensions: .png .jpg .ml (by default .png is used).")
    Command.Let_syntax.(
      let%map_open filein = anon ("filein" %: string)
      and fileout = anon ("fileout" %: string)
      and is_verbose_flag = is_verbose_flag
      and no_color_flag = no_color_flag
      and no_graphic_flag = no_graphic_flag
      and is_centered = centering_flag in
      fun () ->
        let is_to_ocaml, fileout = correct_filename_extension fileout in
        I.save_path := Some fileout;
        apply_flags is_verbose_flag no_color_flag no_graphic_flag is_centered;
        if is_to_ocaml then to_ocaml_safe filein fileout else execute_safe filein)
;;

(** Show sub-command used to display the graphic window. *)
let show =
  Command.basic
    ~summary:"Render the result of the interpreted Honu file."
    Command.Let_syntax.(
      let%map_open filename = anon ("filename" %: string)
      and is_verbose_flag = is_verbose_flag
      and no_color_flag = no_color_flag
      and no_graphic_flag = no_graphic_flag
      and is_centered = centering_flag in
      fun () ->
        apply_flags is_verbose_flag no_color_flag no_graphic_flag is_centered;
        execute_safe filename)
;;

(** Launches the REPL. *)
let repl_cmd =
  Command.basic
    ~summary:"Launch the interactive mode."
    ~readme:(fun () ->
      "Keyboard arrows can be used to go back in the history or to navigate in the"
      ^ " current prompt.\nTo close the session press CTRL-D.")
    Command.Let_syntax.(
      let%map_open is_verbose_flag = is_verbose_flag
      and no_color_flag = no_color_flag
      and is_centered = centering_flag in
      fun () ->
        apply_flags is_verbose_flag no_color_flag false is_centered;
        Repl.run ())
;;

(** Compile the documentation into Markdown. *)
let hdoc =
  Command.basic
    ~summary:"Compile the documentation."
    Command.Let_syntax.(
      let%map_open filein = anon ("filein" %: string)
      and fileout = anon ("fileout" %: string)
      and is_verbose_flag = is_verbose_flag
      and no_color_flag = no_color_flag
      and no_graphic_flag = no_graphic_flag
      and is_centered = centering_flag in
      fun () ->
        apply_flags is_verbose_flag no_color_flag no_graphic_flag is_centered;
        Hdoc.To_markdown.compile_safe filein fileout)
;;

(* Program entry point *)
let () =
  Command.group
    ~summary:"Honu, a programming language and toolchain for turtle graphics."
    [ "show", show; "compile", compile; "repl", repl_cmd; "doc", hdoc ]
  |> Command.run ~version:"2.0"
;;
