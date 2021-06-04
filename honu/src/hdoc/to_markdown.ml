(**
   The [To_markdown] module contains all functions needed for the writting of
   a Markdown file from an [Hast].
 *)

open Core
open Utils
open Cli
open Hast
open Printf

(** Returns the filename from a string of the form: /path/filename.ext. *)
let get_filename (file : string) : string =
  match List.hd @@ String.split ~on:'.' file with
  | Some path ->
    (match List.hd @@ List.rev @@ String.split ~on:'/' path with
    | Some name -> name
    | None -> path)
  | None -> file
;;

(** Returns the procedure or variable id. *)
let get_decl_name = function
  | Proc (name, _) -> name
  | Var id -> id
;;

(** Returns the procedure or variable déclaration. *)
let get_formatted_decl = function
  | Proc (name, args) -> "proc " ^ name ^ "(" ^ String.concat ~sep:", " args ^ ")"
  | Var id -> "var " ^ id
;;

(** Writes to [out] only [Text] and [Note] tags. *)
let write_notes_and_text (out : Out_channel.t) = function
  | Text s -> if String.equal s "" then () else fprintf out "%s\n" s
  | Note s -> fprintf out "\n> %s\n\n" s
  | _ -> ()
;;

(** Writes to [out] only [Fixme] tag. *)
let write_fixme (out : Out_channel.t) = function
  | Fixme s -> fprintf out "* [ ] **%s**\n" s
  | _ -> ()
;;

(** Writes to [out] only [Todo] tag. *)
let write_todo (out : Out_channel.t) = function
  | Todo s -> fprintf out "* [ ] %s\n" s
  | _ -> ()
;;

(** Writes to [out] only [Param] tag. *)
let write_param (out : Out_channel.t) = function
  | Param (id, t) -> fprintf out "* `%s`: %s\n" id t
  | _ -> ()
;;

(** Returns true if both tags have the same constructor. *)
let are_tag_constructors_equal (t1 : tag) (t2 : tag) =
  match t1, t2 with
  | Text _, Text _ | Note _, Note _ | Param _, Param _ | Todo _, Todo _ | Fixme _, Fixme _
    -> true
  | _ -> false
;;

(** Returns true if a list of tag contains a tag with the same constructor than [t]. *)
let contains_tag_constructor (t : tag) = List.exists ~f:(are_tag_constructors_equal t)

(** Writes to [out] the documentation of an [item]. *)
let write_item (out : Out_channel.t) (item : tag list * decl) : unit =
  let tags, decl = item in
  fprintf out "\n## `%s`\n\n" @@ get_decl_name decl;
  if contains_tag_constructor (Text "") tags || contains_tag_constructor (Note "") tags
  then List.iter ~f:(write_notes_and_text out) tags;
  if contains_tag_constructor (Param ("", "")) tags
  then (
    fprintf out "\n### Parameters\n\n";
    List.iter ~f:(write_param out) tags);
  fprintf out "\n```\n%s\n```\n" @@ get_formatted_decl decl;
  if contains_tag_constructor (Fixme "") tags
  then (
    fprintf out "\n### Issues to fix\n\n";
    List.iter ~f:(write_fixme out) tags);
  if contains_tag_constructor (Todo "") tags then fprintf out "\n### Tasks to do\n\n";
  List.iter ~f:(write_todo out) tags
;;

(** Write to [out] the documentation. *)
let write_doc (filename : string) (doc : hdoc) (out : Out_channel.t) : unit =
  fprintf out "# `%s` documentation\n" filename;
  let (Doc items) = doc in
  List.iter ~f:(write_item out) items
;;

(** Parse and compile a commented Honu file into a Markdown file. *)
let compile (filein : string) (fileout : string) : unit =
  let out = Out_channel.create fileout in
  let inx = In_channel.create filein in
  let lexbuf = Lexing.from_channel inx in
  lexbuf.lex_curr_p <- { lexbuf.lex_curr_p with pos_fname = filein };
  let doc =
    try Some (Parser.hdoc Lexer.read lexbuf) with
    | Lexer.Syntax_error msg ->
      print_err_with_style (get_position lexbuf) msg;
      None
    | Parser.Error ->
      print_err_with_style (get_position lexbuf) "Syntax error";
      None
  in
  (match doc with
  | Some d ->
    if !is_verbose then hdoc_to_string d |> print_info_with_style "Builded AST";
    write_doc (get_filename filein) d out
  | None -> ());
  In_channel.close inx;
  Out_channel.close out
;;

(** Catch exceptions produced by [compile]. *)
let compile_safe (filein : string) (fileout : string) : unit =
  try compile filein fileout with
  | Sys_error msg -> print_err_with_style "System error" msg
;;
