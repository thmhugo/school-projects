%{
  open Hast
%}
%token EOF
%token <string> ID TEXT TODO NOTE FIXME
%token <string * string> PARAM
%token EQ LPAREN RPAREN COMMA SEMICOLON
%token PROC VAR
%token HASH

%type <Hast.hdoc> hdoc

%start hdoc

%%

hdoc: p = item* EOF { Doc (p) }

item: t = tag*; d = declaration { (t, d) }

tag:
  | HASH; t = TODO { Todo (t) }
  | HASH; t = NOTE { Note (t) }
  | HASH; t = FIXME { Fixme (t) }
  | HASH; t = TEXT { Text (t) }
  | HASH; p = PARAM {
    let (i, t) = p in Param (i, t)
  }

declaration:
  | PROC; id = ID; LPAREN; args = arguments; RPAREN; EQ { Proc (id, args) }
  | VAR; id = ID; SEMICOLON { Var (id) }


arguments:
  | id = ID; p = remaining_arguments* { id :: p }
  | { [] }

remaining_arguments: COMMA; id = ID { id }
