(**
   The [Cli] module contains all common functions for print in the console.
*)

open Core
open Lexing

(** [Grahpics.window] width. *)
let dimx = 1000

(** [Grahpics.window] height. *)
let dimy = 1000

(** Is set to [false] when the [--no-color] flag is mentioned. *)
let is_styled = ref true

(** Is set to [true] when the [--verbose] flag is mentioned. *)
let is_verbose = ref false

(** Print the position to [outx] with the following format: 'filename:line:offset' *)
let print_position (outx : Out_channel.t) (lexbuf : lexbuf) : unit =
  let pos = lexbuf.lex_curr_p in
  fprintf outx "%s:%d:%d" pos.pos_fname pos.pos_lnum (pos.pos_cnum - pos.pos_bol + 1)
;;

(** Open the Graphics window depending on [dimx] and [dimy]. *)
let open_window () : unit =
  Graphics.open_graph (" " ^ string_of_int dimx ^ "x" ^ string_of_int dimy);
  Graphics.set_window_title "honu";
  Graphics.set_line_width 1;
  Graphics.set_color Graphics.black
;;

(** Applies the an [ANSITerminal.style] to a string. *)
let apply_style (styles : ANSITerminal.style list) (str : ('a, unit, string) format) =
  if !is_styled then ANSITerminal.sprintf styles str else Printf.sprintf str
;;

let error_marker () = apply_style [ ANSITerminal.Bold; ANSITerminal.red ] "|  "

(** Returns a formatted string containing position of the lexbuf. *)
let get_position (lexbuf : lexbuf) : string =
  let pos = lexbuf.lex_curr_p in
  sprintf
    "File \"%s\", line %d, column %d"
    pos.pos_fname
    pos.pos_lnum
    (pos.pos_cnum - pos.pos_bol)
;;

(** Print a [msg] to stderr. *)
let print_err (msg : string) : unit = eprintf "%s\n" msg

(** Prints formatted informations in [stderr]. *)
let print_err_with_style (header : string) (msg : string) : unit =
  let apply_red_style = apply_style [ ANSITerminal.red ] in
  let marker = error_marker () in
  marker
  ^ "\n"
  ^ marker
  ^ apply_red_style "%s:\n" header
  ^ marker
  ^ apply_red_style "%s\n" "|"
  ^ marker
  ^ apply_red_style "+--[ %s ]" msg
  ^ "\n"
  ^ marker
  |> print_err
;;

let info_marker () = apply_style [ ANSITerminal.Bold; ANSITerminal.blue ] "|  "

(** Adds [nb_tab] at the start of [str]. *)
let str_add_tab (nb_tab : int) (str : string) : string = String.make nb_tab '\t' ^ str

(** Prints formatted informations in [stdout]. *)
let print_info_with_style (info : string) (msg : string) : unit =
  let apply_blue_style = apply_style [ ANSITerminal.blue ] in
  let marker = info_marker () in
  marker
  ^ "\n"
  ^ marker
  ^ apply_blue_style "%s:\n" info
  ^ marker
  ^ apply_blue_style "%s\n" "|"
  ^ marker
  ^ apply_blue_style "+--[\n%s\n     ]\n" msg
  ^ marker
  |> print_endline
;;
