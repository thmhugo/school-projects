%{
  open Ast
%}
%token EOF
%token VAR
%token <string> ID
%token DOWN UP FORWARD TURN COLOR PEN_SIZE PUSH POP WRITE
%token SCOPE_START SCOPE_END
%token EQ MINUS PLUS TIMES DIV OR AND
%token NOT EQUALS DIFFERENT 
%token GT  /* greater than */ 
%token ST  /* smaller than */ 
%token GTE /* smaller than or equal */ 
%token STE /* smaller than or equal */
%token PLUSEQ /* += */ MINUSEQ /* -= */
%token LPAREN RPAREN COMMA SEMICOLON
%token IF THEN ELSE WHILE DO PROC
%token PRINT
%token <int> INT COLOR_ID
%token <string> STRING
%token <bool> BOOL
%token <float> FLOAT

%left MINUS PLUS OR EQUALS DIFFERENT GT ST GTE STE
%left TIMES DIV AND NOT

%nonassoc THEN
%nonassoc ELSE

%type <Ast.prog> prog repl_prog

%start prog repl_prog

%%

prog: p = procedures*; s = statements*; i = instruction; EOF { Prog (p, s, i) }

repl_prog: p = procedures*; s = statements*; i = maybe_instruction; EOF { Prog (p, s, i) }

procedures:
  | PROC; id = ID; LPAREN; args = arguments; RPAREN; EQ; i = instruction { (id, args, i) }

arguments:
  | id = ID; p = remaining_arguments* { id :: p }
  | { [] }

remaining_arguments: COMMA; id = ID { id }

statements: VAR; id = ID; SEMICOLON { id }

maybe_instruction:
  | i = instruction { i }
  | { Scope [] }

instruction:
  | FORWARD; e = expression { Forward e }
  | TURN; e = expression { Turn e }
  | PRINT; e = expression { Print e }
  | COLOR; e = expression { Set_color e }
  | PEN_SIZE; e = expression { Set_pen_size e }
  | DOWN { Down }
  | UP { Up }
  | PUSH { Push }
  | POP { Pop }
  | id = ID; PLUSEQ; e = expression { Var_assign (id , Binop (Plus, Var_id id, e)) }
  | id = ID; MINUSEQ; e = expression { Var_assign (id , Binop (Minus, Var_id id, e)) }
  | WRITE; e = expression { Write e }
  | id = ID; EQ; e = expression { Var_assign (id, e) }
  | id = ID; LPAREN; p = proc_call_arguments; RPAREN { Proc_call (id, p) }
  | SCOPE_START; instrs = scope*; SCOPE_END { Scope instrs }
  | IF e = expression THEN i = instruction { If (e, i) }
  | IF cond = expression THEN i1 = instruction; ELSE; i2 = instruction { If_else (cond, i1, i2) }
  | WHILE; cond = expression; DO; i = instruction { While (cond, i) }

scope: i = instruction; SEMICOLON { i }

proc_call_arguments:
  | e = expression; p = remaining_proc_call_arguments* { e :: p }
  | { [] }

remaining_proc_call_arguments: COMMA; e = expression { e }

expression:
  | n = INT { Const (Int n) }
  | s = STRING { Const (String s) }
  | b = BOOL { Const (Bool b) }
  | f = FLOAT { Const (Float f) }
  | c = COLOR_ID { Const (Color c) }
  | id = ID { Var_id id }
  | o = uop; e = expression { Unop (o, e) }
  | e1 = expression; o = bop; e2 = expression { Binop (o, e1, e2) }
  | LPAREN; e = expression; RPAREN; { e }

%inline bop:
  | PLUS { Plus }
  | MINUS { Minus }
  | TIMES { Times }
  | DIV { Div }
  | AND { And }
  | OR { Or }
  | EQUALS { Equals }
  | DIFFERENT { Different }
  | GT { Greater_than }
  | ST { Smaller_than }
  | GTE { Greater_than_or_eq }
  | STE { Smaller_than_or_eq }

%inline uop:
 | MINUS { UMinus }
 | NOT { Not }
