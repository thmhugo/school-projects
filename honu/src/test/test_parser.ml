open Lexing
open OUnit2
open Core
open Parsing
open Ast
module P = Parser
module L = Lexer

let examples_path = "../../../../src/test/resources/"

(** Creates [A.prog] from the given file. *)
let parse (lexbuf : lexbuf) : prog = P.prog L.read lexbuf

let get_prog (filename : string) =
  let inx = In_channel.create filename in
  let lexbuf = Lexing.from_channel inx in
  lexbuf.lex_curr_p <- { lexbuf.lex_curr_p with pos_fname = filename };
  let prog_op = parse lexbuf in
  In_channel.close inx;
  prog_op
;;

let () =
  "Parser suite"
  >::: [ ("Parsing of test/resources/good/TestSimpleProg.hn."
         >:: fun _ ->
         let expected_prog = Prog ([], [], Scope [ Forward (Const (Int 90)) ]) in
         try
           assert_equal
             expected_prog
             (get_prog (examples_path ^ "good/TestSimpleProg.hn"))
         with
         | _ -> assert_failure "Should produce a correct AST.")
       ; ("Parsing of test/resources/good/TestSiAlorsSinon."
         >:: fun _ ->
         let f_10 = Forward (Const (Int 10)) in
         let t_40 = Turn (Const (Int 40)) in
         let expected_prog =
           Prog
             ( []
             , []
             , Scope
                 [ Down
                 ; If_else (Binop (Plus, Const (Int 10), Const (Int 50)), f_10, t_40)
                 ; If_else
                     ( Binop (Minus, Const (Int 10), Const (Int 10))
                     , Scope [ f_10; t_40; f_10 ]
                     , t_40 )
                 ] )
         in
         try
           assert_equal expected_prog (get_prog (examples_path ^ "good/TestSiAlorsSinon"))
         with
         | _ -> assert_failure "Should produce a correct AST.")
       ; ("Parsing of test/resources/good/TestSimpleProgAbrvSyntax.hn."
         >:: fun _ ->
         let cond = Var_id "cond" in
         let condmin1 = Var_assign ("cond", Binop (Minus, cond, Const (Int 1))) in
         let print_cond = Print cond in
         let expected_prog =
           Prog
             ( []
             , [ "cond" ]
             , Scope
                 [ Var_assign ("cond", Const (Int 10))
                 ; While
                     ( Var_id "cond"
                     , Scope
                         [ Forward (Const (Int 90))
                         ; If_else (cond, print_cond, condmin1)
                         ; If_else
                             ( cond
                             , Scope [ condmin1; print_cond ]
                             , Scope
                                 [ Var_assign ("cond", Binop (Plus, cond, Const (Int 1)))
                                 ; print_cond
                                 ] )
                         ] )
                 ] )
         in
         try
           assert_equal
             expected_prog
             (get_prog (examples_path ^ "good/TestSimpleProgAbrvSyntax.hn"))
         with
         | _ -> assert_failure "Should produce a correct AST.")
       ; ("Parsing of test/resources/bad/TestSiAlorsSinon."
         >:: fun _ ->
         assert_raises ~msg:"Condition should be an expression" P.Error (fun () ->
             get_prog (examples_path ^ "bad/TestSiAlorsSinon")))
       ; ("Parsing of test/resources/good/TestTantQue."
         >:: fun _ ->
         let init_cond = Var_assign ("cond", Const (Int 150)) in
         let decrement_cond =
           Var_assign ("cond", Binop (Minus, Var_id "cond", Const (Int 3)))
         in
         let expected_prog =
           Prog
             ( []
             , [ "cond" ]
             , Scope
                 [ Turn (Binop (Minus, Const (Int 0), Const (Int 45)))
                 ; Forward (Const (Int 250))
                 ; Turn (Const (Int 45))
                 ; Down
                 ; init_cond
                 ; While (Var_id "cond", decrement_cond)
                 ; init_cond
                 ; While
                     ( Var_id "cond"
                     , Scope
                         [ Forward (Const (Int 60))
                         ; Turn (Binop (Minus, Const (Int 0), Var_id "cond"))
                         ; decrement_cond
                         ] )
                 ] )
         in
         try
           assert_equal expected_prog (get_prog (examples_path ^ "good/TestTantQue"))
         with
         | _ -> assert_failure "Should produce a correct AST.")
       ; ("Parsing of test/resources/good/TestProc.hn"
         >:: fun _ ->
         let expected_prog =
           Prog
             ( [ ( "multBy2"
                 , [ "a" ]
                 , Scope [ Print (Binop (Times, Var_id "a", Const (Int 2))) ] )
               ]
             , []
             , Scope [ Proc_call ("multBy2", [ Const (Int 10) ]) ] )
         in
         try
           assert_equal expected_prog (get_prog (examples_path ^ "good/TestProc.hn"))
         with
         | _ -> assert_failure "Should produce a correct AST.")
       ; ("Parsing of test/resources/good/TestSimpleIf.hn"
         >:: fun _ ->
         let red = Graphics.red in
         let expected_prog =
           Prog
             ( []
             , []
             , Scope
                 [ Down
                 ; If (Const (Int 0), Set_color (Const (Color red)))
                 ; Forward (Const (Int 100))
                 ; If_else
                     (Const (Int 2), Set_color (Const (Color red)), Turn (Const (Int 90)))
                 ; Forward (Const (Int 100))
                 ] )
         in
         try
           assert_equal expected_prog (get_prog (examples_path ^ "good/TestSimpleIf.hn"))
         with
         | _ -> assert_failure "Should produce a correct AST.")
       ; ("Parsing of test/resources/good/TestOperateurs.hn"
         >:: fun _ ->
         let expected_prog =
           Prog
             ( []
             , [ "x"; "y"; "z" ]
             , Scope
                 [ Var_assign ("x", Const (Int 4))
                 ; Var_assign ("y", Const (Int 10))
                 ; Var_assign
                     ( "x"
                     , Binop
                         (Plus, Var_id "x", Binop (Times, Const (Int 2), Const (Int 3)))
                     )
                 ; Var_assign ("x", Binop (Minus, Var_id "x", Const (Int 5)))
                 ; Var_assign ("x", Binop (Plus, Var_id "x", Var_id "y"))
                 ; Var_assign ("z", Binop (Greater_than, Var_id "x", Var_id "y"))
                 ; Var_assign ("y", Unop (Not, Var_id "z"))
                 ] )
         in
         try
           assert_equal
             expected_prog
             (get_prog (examples_path ^ "good/TestOperateurs.hn"))
         with
         | _ -> assert_failure "Should produce a correct AST.")
       ; ("Parsing of examples/bad/TestEOF"
         >:: fun _ ->
         assert_raises
           ~msg:"Wrong interger format."
           (Parsing.Lexer.Syntax_error "Unknown keyword: 'Pippo'")
           (fun () -> get_prog (examples_path ^ "bad/TestEOF")))
       ; ("Interpreting of examples/bad/TestSyntErr"
         >:: fun _ ->
         assert_raises ~msg:"Wrong interger format." P.Error (fun () ->
             get_prog (examples_path ^ "bad/TestSyntErr")))
       ]
  |> run_test_tt_main
;;

;;
100
