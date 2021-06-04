type 's word =
  | Symb of 's
  | Seq of 's word list
  | Branch of 's word

type 's rewrite_rules = 's -> 's word

type 's system = {
    axiom : 's word;
    rules : 's rewrite_rules;
    interp : 's -> Turtle.command list }

val interpret_s_word_list : 
  (Turtle.position -> Turtle.command list -> bool -> Turtle.position)
  -> Turtle.position -> 's word list-> 's system -> bool -> Turtle.position
val execute_symbole_commands : 
  Turtle.position -> Turtle.command list -> bool -> Turtle.position
val word_derivation : 
  (Turtle.position -> Turtle.command list -> bool -> Turtle.position) 
  -> 's system -> int -> Turtle.position -> bool -> Turtle.position
val estimate_drawing_size : 
  Turtle.position -> Turtle.command list -> bool -> Turtle.position
val get_factor : unit -> float
val mix : unit -> float
val miy : unit -> float 
val reset_bounds : unit -> unit
val rgb_string_to_list :  string -> int list
val check_rgb_list : int list -> unit 
val rgb_from : int list ref
val rgb_to : int list ref
val change_interp : 's system -> bool -> 's system
