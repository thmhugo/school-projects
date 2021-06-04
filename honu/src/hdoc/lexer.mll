{
  open Lexing
  open Parser

  exception Syntax_error of string

  let next_line lexbuf =
    let pos = lexbuf.lex_curr_p in
    lexbuf.lex_curr_p <-
      { pos with pos_bol = lexbuf.lex_curr_pos
      ; pos_lnum = pos.pos_lnum + 1
      }

  let startpos = ref 0
}

let white = [' ' '\t']+
let newline = '\r' | '\n' | "\r\n"
let digit = ['0'-'9']
let low_letter = ['a'-'z']
let up_letter = ['A'-'Z']

let num = ['1'-'9'] digit* | digit
let id = low_letter (low_letter | up_letter | digit) *
let text = [^'\n' '\r']*

rule read = parse
  | white  { read lexbuf }
  | newline { next_line lexbuf; read lexbuf }
  | '~' { HASH }
  | "|" white* "@todo" white* (text as t) newline { next_line lexbuf; TODO (t) }
  | "|" white* "@note" white* (text as t) newline { next_line lexbuf; NOTE (t) }
  | "|" white* "@fixme" white* (text as t) newline { next_line lexbuf; FIXME (t) }
  | "|" white* "@param" white* (id as i) white* (text as t) newline { next_line lexbuf; PARAM (i, t) }
  | "|" white* (text as t) { TEXT (t) }
  | "proc" | "Proc" { PROC }
  | "Var" | "var" { VAR }
  | id as i { ID (i) }
  | "(" { LPAREN }
  | ")" { RPAREN }
  | ";" { SEMICOLON }
  | "," { COMMA }
  | "=" { EQ }
  | "#" { ignore_line lexbuf }
  | eof { EOF }
  | _ { read_code lexbuf }

and ignore_line = parse
 | newline | eof { next_line lexbuf; read lexbuf }
 | _ { ignore_line lexbuf }

and read_code = parse
  | "proc" | "Proc" { PROC }
  | "Var" | "var" { VAR }
  | '~' { HASH }
  | '"' { read_string lexbuf }
  | "#" { comment lexbuf }
  | _ { read_code lexbuf }
  | eof { EOF }

and comment = parse
 | newline | eof { next_line lexbuf; read_code lexbuf }
 | _ { comment lexbuf }

and read_string = parse
  | '"' { read_code lexbuf }
  | [^'"']* { read_string lexbuf }
