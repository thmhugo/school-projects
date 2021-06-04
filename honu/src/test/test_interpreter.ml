open Lexing
open OUnit2
open Core
open Parsing
open Ast
open Interpreting
module I = Interpreter
module P = Parser
module L = Lexer
module T = Turtle

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
  "Intepreter suite"
  >::: [ ("Evaluating 1+3*-5"
         >:: fun _ ->
         try
           let expected_result = -14 in
           assert_equal
             (Int expected_result)
             (I.eval_exp
                (Binop
                   ( Plus
                   , Const (Int 1)
                   , Binop (Times, Const (Int 3), Unop (UMinus, Const (Int 5))) ))
                [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating -300--100+50"
         >:: fun _ ->
         try
           let expected_result = -150 in
           assert_equal
             (Int expected_result)
             (I.eval_exp
                (Binop
                   ( Plus
                   , Binop
                       ( Minus
                       , Unop (UMinus, Const (Int 300))
                       , Unop (UMinus, Const (Int 100)) )
                   , Const (Int 50) ))
                [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 5+(-4+3)"
         >:: fun _ ->
         try
           let expected_result = 4 in
           assert_equal
             (Int expected_result)
             (I.eval_exp
                (Binop
                   ( Plus
                   , Const (Int 5)
                   , Binop (Plus, Unop (UMinus, Const (Int 4)), Const (Int 3)) ))
                [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 5+(-(4+3))"
         >:: fun _ ->
         try
           let expected_result = -2 in
           assert_equal
             (Int expected_result)
             (I.eval_exp
                (Binop
                   ( Plus
                   , Const (Int 5)
                   , Unop (UMinus, Binop (Plus, Const (Int 4), Const (Int 3))) ))
                [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1/0"
         >:: fun _ ->
         assert_raises
           ~msg:"Should not divide by zero"
           (I.Interpreter_error "Illegal division by zero : 1 / 0")
           (fun () -> I.eval_exp (Binop (Div, Const (Int 1), Const (Int 0))) []))
       ; ("Evaluating 2/(10*10-100)"
         >:: fun _ ->
         assert_raises
           ~msg:"Should not divide by zero"
           (I.Interpreter_error "Illegal division by zero : 2 / 0")
           (fun () ->
             I.eval_exp
               (Binop
                  ( Div
                  , Const (Int 2)
                  , Binop
                      ( Minus
                      , Binop (Times, Const (Int 10), Const (Int 10))
                      , Const (Int 100) ) ))
               []))
       ; ("Evaluating 1+x where x=1"
         >:: fun _ ->
         try
           let expected_result = 2. in
           assert_equal
             (Float expected_result)
             (I.eval_exp
                (Binop (Plus, Const (Float 1.), Var_id "x"))
                [ "x", Some (Float 1.) ])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1/x where x=0"
         >:: fun _ ->
         assert_raises
           ~msg:"Should not divide by zero"
           (I.Interpreter_error "Illegal division by zero : 1 / 0")
           (fun () ->
             I.eval_exp (Binop (Div, Const (Int 1), Var_id "x")) [ "x", Some (Int 0) ]))
       ; ("Evaluating 1+x where x is undefined"
         >:: fun _ ->
         assert_raises
           ~msg:"Should define used variables."
           (I.Interpreter_error
              "Undeclared variable 'x' (hint: add 'Var x' after procedures definitions)")
           (fun () -> I.eval_exp (Binop (Plus, Const (Int 1), Var_id "x")) []))
       ; ("Checking duplicates in [\"a\";\"b\";\"c\"]"
         >:: fun _ -> assert_equal (I.get_duplicated_var [ "a"; "b"; "c" ]) None)
       ; ("Checking duplicates in [\"a\";\"b\";\"a\"]"
         >:: fun _ -> assert_equal (I.get_duplicated_var [ "a"; "b"; "a" ]) (Some "a"))
       ; ("Color of string for 'noir'"
         >:: fun _ -> assert_equal (L.color_of_string "noir") Graphics.black)
       ; ("Color of string for 'xxx'"
         >:: fun _ ->
         assert_raises
           ~msg:"Should use a Graphics color."
           (L.Syntax_error "Color 'xxx' is not a correct color.")
           (fun () -> L.color_of_string "xxx"))
       ; ("Interpreting of examples/bad/TestProcArgs.hn"
         >:: fun _ ->
         assert_raises
           ~msg:"A procedure should be called whith the right number of arguments"
           (I.Interpreter_error "The procedure 'f' expects 3 arguments got 2")
           (fun () ->
             I.interpret_prog
               (get_prog (examples_path ^ "bad/TestProcArgs.hn"))
               !T.starting_pos
               print_endline))
       ; ("Testing if without else syntax with value assignation."
         >:: fun _ ->
         let instrs =
           Scope
             [ Down
             ; Var_assign ("var", Const (Int 1))
             ; If (Const (Int 1), Var_assign ("var", Const (Int 100)))
             ; Var_assign ("var", Binop (Plus, Var_id "var", Const (Int 1)))
             ; If_else
                 ( Const (Int 0)
                 , Var_assign ("var", Const (Int 12))
                 , Var_assign ("var", Binop (Plus, Var_id "var", Const (Int 1))) )
             ]
         in
         try
           match
             I.eval_instr
               instrs
               { pos = { x = 0.; y = 0.; a = 0. }
               ; procs = []
               ; vars = [ "var", Some (Int 0) ]
               ; penup = true
               }
               (fun _ -> ())
           with
           | { pos = _; procs = _; vars = v; penup = _ } ->
             assert_equal v [ "var", Some (Int 102) ]
         with
         | _ -> assert_failure "The variable 'var' should be equal to 102.")
       ; ("Evaluating \"ho\"+ \"nu\": string concatenation)"
         >:: fun _ ->
         try
           let expected_result = "honu" in
           assert_equal
             (String expected_result)
             (I.eval_exp (Binop (Plus, Const (String "ho"), Const (String "nu"))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 3*true"
         >:: fun _ ->
         assert_raises
           ~msg:"Can't add integer and boolean"
           (I.Interpreter_error "Illegal operation: 3 * true")
           (fun () -> I.eval_exp (Binop (Times, Const (Int 3), Const (Bool true))) []))
       ; ("Evaluating -true)"
         >:: fun _ ->
         try
           let expected_result = false in
           assert_equal
             (Bool expected_result)
             (I.eval_exp (Unop (UMinus, Const (Bool true))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1.5+2)"
         >:: fun _ ->
         try
           let expected_result = 3.5 in
           assert_equal
             (Float expected_result)
             (I.eval_exp (Binop (Plus, Const (Float 1.5), Const (Int 2))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Testing wrong type for color. (1)"
         >:: fun _ ->
         let instrs = Scope [ Set_color (Const (Int 3)) ] in
         assert_raises
           ~msg:"Colors are a well defined types, not intergers."
           (I.Interpreter_error "Illegal color: 3")
           (fun () ->
             I.eval_instr
               instrs
               { pos = { x = 0.; y = 0.; a = 0. }; procs = []; vars = []; penup = true }
               (fun _ -> ())))
       ; ("Testing wrong type for color. (2)"
         >:: fun _ ->
         let instrs = Scope [ Set_color (Const (String "blue")) ] in
         assert_raises
           ~msg:"Colors are a well defined types, not strings."
           (I.Interpreter_error "Illegal color: \"blue\"")
           (fun () ->
             I.eval_instr
               instrs
               { pos = { x = 0.; y = 0.; a = 0. }; procs = []; vars = []; penup = true }
               (fun _ -> ())))
       ; ("Testing booleans in if."
         >:: fun _ ->
         let instrs =
           Scope
             [ If_else
                 ( Const (Bool true)
                 , Var_assign ("var", Const (String "true"))
                 , Var_assign ("var", Const (String "false")) )
             ]
         in
         try
           match
             I.eval_instr
               instrs
               { pos = { x = 0.; y = 0.; a = 0. }
               ; procs = []
               ; vars = [ "var", Some (String "") ]
               ; penup = true
               }
               (fun _ -> ())
           with
           | { pos = _; procs = _; vars = v; penup = _ } ->
             assert_equal v [ "var", Some (String "true") ]
         with
         | _ -> assert_failure "The variable 'var' should be equal to 'true'.")
       ; ("Evaluating 1.5==2"
         >:: fun _ ->
         try
           let expected_result = false in
           assert_equal
             (Bool expected_result)
             (I.eval_exp (Binop (Equals, Const (Float 1.5), Const (Int 2))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1==1"
         >:: fun _ ->
         try
           let expected_result = true in
           assert_equal
             (Bool expected_result)
             (I.eval_exp (Binop (Equals, Const (Int 1), Const (Int 1))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1<>1)"
         >:: fun _ ->
         try
           let expected_result = false in
           assert_equal
             (Bool expected_result)
             (I.eval_exp (Binop (Different, Const (Int 1), Const (Int 1))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1<>1.0"
         >:: fun _ ->
         try
           let expected_result = false in
           assert_equal
             (Bool expected_result)
             (I.eval_exp (Binop (Different, Const (Int 1), Const (Float 1.0))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1>=1"
         >:: fun _ ->
         try
           let expected_result = true in
           assert_equal
             (Bool expected_result)
             (I.eval_exp
                (Binop (Greater_than_or_eq, Const (Int 1), Const (Float 1.0)))
                [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1>1.0"
         >:: fun _ ->
         try
           let expected_result = false in
           assert_equal
             (Bool expected_result)
             (I.eval_exp (Binop (Greater_than, Const (Int 1), Const (Float 1.0))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1<1.0"
         >:: fun _ ->
         try
           let expected_result = false in
           assert_equal
             (Bool expected_result)
             (I.eval_exp (Binop (Smaller_than, Const (Int 1), Const (Float 1.0))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1<2.0"
         >:: fun _ ->
         try
           let expected_result = true in
           assert_equal
             (Bool expected_result)
             (I.eval_exp (Binop (Smaller_than, Const (Int 1), Const (Float 2.0))) [])
         with
         | I.Interpreter_error msg -> assert_failure msg)
       ; ("Evaluating 1==true"
         >:: fun _ ->
         assert_raises
           ~msg:"Can't compare integer and boolean"
           (I.Interpreter_error "Illegal operation: 1 == true")
           (fun () -> I.eval_exp (Binop (Equals, Const (Int 1), Const (Bool true))) []))
       ; ("Interpreting examples/bad/TestAngle"
         >:: fun _ ->
         assert_raises
           ~msg:"All used variables must be declared."
           (I.Interpreter_error "Unknown variable 'angle'")
           (fun () ->
             I.interpret_prog
               (get_prog (examples_path ^ "bad/TestAngle"))
               !T.starting_pos
               print_endline))
       ; ("Interpreting of examples/bad/TestDiv0"
         >:: fun _ ->
         assert_raises
           ~msg:"Cannot divide by zero."
           (I.Interpreter_error "Illegal division by zero : 90 / 0")
           (fun () ->
             I.interpret_prog
               (get_prog (examples_path ^ "bad/TestDiv0"))
               !T.starting_pos
               print_endline))
       ; ("Interpreting of examples/bad/TestTantQue"
         >:: fun _ ->
         assert_raises
           ~msg:"Cannot run obvious infinite loops."
           (I.Interpreter_error
              "Infinite loop detected because of the while condition: '(10 - 5)' != 0")
           (fun () ->
             I.interpret_prog
               (get_prog (examples_path ^ "bad/TestTantQue"))
               !T.starting_pos
               print_endline))
       ]
  |> run_test_tt_main
;;
