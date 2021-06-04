open Graphics
open Stack

type command =
| Line of int
| Move of int
| Turn of int
| Color of int
| Store
| Restore

type position = {
	x: float;      (** position x *)
	y: float;      (** position y *)
	a: float;      (** angle of the direction *)
}

(*	Calculs issue de 
	http://vps275670.ovh.net/static/info/2B-Caml/TD-OCaml.02.pdf, page 6. *)
let pi_over_180 = atan 1. /. 45.
let of_degrees d = d *. pi_over_180

let start_x = 400.
let start_y = 100.
let start_a = 90.
let memoire = create ()

let open_window () : unit = 
	open_graph " 800x800";
	set_window_title "L-system"

let get_starting_pos =  
	{
		x = start_x; 
		y = start_y; 
		a = of_degrees start_a
	}

let get_starting_angle = of_degrees start_a

let forward (d: float) (draw: bool) (p: position) : position =
	let new_x = p.x +. (d) *. cos (p.a) in
	let new_y = p.y +. (d) *. sin (p.a) in
	if draw then (
		moveto (truncate p.x) (truncate p.y);
		lineto (truncate new_x) (truncate new_y)
	)
	else
		moveto (truncate new_x) (truncate new_y);
	{
		x = new_x; 
		y = new_y; 
		a = p.a
	} 


let turn (p: position) (new_a: int) : position = 
	{
		x = p.x; 
		y = p.y; 
		a = (p.a +. (of_degrees (float_of_int new_a)))
	}

let restore (p: position) : position =
	if not (is_empty memoire) then 
		pop memoire
	else 
	(* Une pile vide dépilée est due à une mauvaise écriture du LSystème. *)
		failwith "L-System might be not correctly written."

let store (p: position) : position =
	push p memoire; 
	p
