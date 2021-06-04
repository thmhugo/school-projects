(**
  The [Turtle] module contains all functions and types needed to implement a
  graphic turtle.
*)

open Graphics
open Utils

exception Turtle_error of string

(** Stores the turtle position on the canvas. *)
type turtle_pos =
  { x : float (** position x *)
  ; y : float (** position y *)
  ; a : float (** angle of the direction *)
  }

let pi_over_180 = atan 1. /. 45.
let of_degrees d = d *. pi_over_180
let start_a = 90.

(** Default starting position following the subject. *)
let default_pos = { x = 0.; y = 0.; a = of_degrees start_a }

(** Centered position to avoid centering instructions in .hn files. *)
let centered_pos = { x = 400.; y = 400.; a = of_degrees start_a }

(** Is set to [centered_pos] when the [--center] flag is mentioned. *)
let starting_pos = ref default_pos

(** Returns true if [x] and [y] are in the canvas. *)
let is_out_boundary x y =
  x > float_of_int Cli.dimx || y > float_of_int Cli.dimy || x < 0. || y < 0.
;;

(** Draw a line or move to [new_p] depending on [penup]. *)
let move_forward_and_draw (p : turtle_pos) (new_p : turtle_pos) (penup : bool) : unit =
  if not penup
  then (
    moveto (truncate p.x) (truncate p.y);
    lineto (truncate new_p.x) (truncate new_p.y))
  else moveto (truncate new_p.x) (truncate new_p.y)
;;

(** Moves the turtle of a distance [d], starting from [p], draw a line if [penup] is false.
    @raise [TurtleError] if the final position is not in the window boundaries
*)
let forward
    ?(visitor : turtle_pos -> turtle_pos -> bool -> unit = move_forward_and_draw)
    (d : float)
    (p : turtle_pos)
    (penup : bool)
    : turtle_pos
  =
  let new_x = p.x +. (d *. cos p.a) in
  let new_y = p.y +. (d *. sin p.a) in
  if is_out_boundary new_x new_y
  then
    raise
      (Turtle_error
         ("New position ("
         ^ string_of_int (truncate new_x)
         ^ ","
         ^ string_of_int (truncate new_y)
         ^ ") is out of boundaries"))
  else visitor p { x = new_x; y = new_y; a = 0. } penup;
  { p with x = new_x; y = new_y }
;;

(** Turns of [new_a] degrees the turtle. *)
let turn (new_a : float) (p : turtle_pos) : turtle_pos =
  { p with a = p.a +. of_degrees new_a }
;;
