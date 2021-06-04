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

  (** Returns the boolean value of a string. *)
  let bool_of_string s = match s with
  | "true"|"vrai" -> true
  | "false"|"faux" -> false
  | _ -> raise (Syntax_error (s ^ " is not a correct boolean value."))

  (** Returns the integer value of the string representation of a Graphics.color.
    @raise [Interpreter_error] if the color is not a valid Graphics.color. *)
  let color_of_string = function
    | "noir" | "black" -> Graphics.black
    | "blanc" | "white" -> Graphics.white
    | "rouge" | "red" -> Graphics.red
    | "vert" | "green" -> Graphics.green
    | "bleu" | "blue" -> Graphics.blue
    | "jaune" | "yellow" -> Graphics.yellow
    | "cyan" -> Graphics.cyan
    | "magenta" -> Graphics.magenta
    | _ as color ->
      raise (Syntax_error ("Color '" ^ color ^ "' is not a correct color."))
  ;;
}

let white = [' ' '\t']+
let newline = '\r' | '\n' | "\r\n"
let digit = ['0'-'9']
let low_letter = ['a'-'z']
let up_letter = ['A'-'Z']
let not_low_letter = [^'a'-'z']
let word = ([^ '"'])*

let num = ['1'-'9'] digit* | digit
let id = low_letter (low_letter | up_letter | digit) *
let float_num = digit+"."digit*
let color =  "noir"|"black"|"blanc"|"white"|"rouge"|"red"|"vert"|"green"|"bleu"|"blue"|"jaune"|"yellow"|"cyan"|"magenta"

rule read = parse
  | white  { read lexbuf }
  | newline { next_line lexbuf; read lexbuf }
  | "Var" | "var" { VAR }
  | "Debut" | "{" { SCOPE_START }
  | "Fin" | "}" { SCOPE_END }
  | "Si" | "if" { IF }
  | "Alors" | "then" { THEN }
  | "Sinon" | "else" { ELSE }
  | "Tant" white+ "que" | "while" { WHILE }
  | "Faire" | "do" { DO }
  | "Avance" | "forward"  { FORWARD }
  | "Tourne" | "turn" { TURN }
  | "BasPinceau" | "down" { DOWN }
  | "HautPinceau" | "up" { UP }
  | "Affiche" | "print" { PRINT }
  | "ChangeCouleur" | "color" { COLOR }
  | "ChangeEpaisseur" | "size" { PEN_SIZE }
  | "Empile" | "push" { PUSH }
  | "Depile" | "pop" { POP }
  | "Proc" | "proc" { PROC }
  | "Ecrit" | "write" { WRITE }
  | "\"" (word as s) "\"" { STRING (s) }
  | ("true"|"false"|"vrai"|"faux") as b { BOOL (bool_of_string b) }
  | color as c { COLOR_ID (color_of_string c) }
  | "Non" | "not" { NOT }
  | id as i { ID (i) }
  | float_num as f { FLOAT (float_of_string f) }
  | num as n { INT (int_of_string n) }
  | "+" { PLUS }
  | "-" { MINUS }
  | "*" { TIMES }
  | "/" { DIV }
  | "=" { EQ }
  | "(" { LPAREN }
  | ")" { RPAREN }
  | "#" | "~|" { comment lexbuf }
  | "," { COMMA }
  | "&" { AND }
  | "|" { OR }
  | ";" { SEMICOLON }
  | "==" { EQUALS }
  | "!=" { DIFFERENT }
  | ">" { GT }
  | "<" { ST }
  | ">=" { GTE }
  | "<=" { STE }
  | "+=" { PLUSEQ }
  | "-=" { MINUSEQ }
  | eof { EOF }
  | _ {
    let errbuf = Buffer.create 80 in
    startpos := lexbuf.lex_curr_pos;
    Buffer.add_string errbuf (Lexing.lexeme lexbuf);
    get_unmatched_word  errbuf lexbuf
  }

and comment = parse
 | newline | eof { next_line lexbuf; read lexbuf }
 | _ { comment lexbuf }

and get_unmatched_word errbuf = parse
  | white | newline | eof {
    let pos = lexbuf.lex_curr_p in
    lexbuf.lex_curr_p <-
      { pos with pos_cnum = !startpos
      };
    raise ( Syntax_error ("Unknown keyword: '" ^ Buffer.contents errbuf ^ "'"))
  }
  | _  {
    Buffer.add_string errbuf (Lexing.lexeme lexbuf);
    get_unmatched_word errbuf lexbuf
  }
