open Turtle
open Graphics
open List

type 's word =
  | Symb of 's
  | Seq of 's word list
  | Branch of 's word

type 's rewrite_rules = 's -> 's word

type 's system = {
	axiom : 's word;
	rules : 's rewrite_rules;
	interp : 's -> command list 
}

(* 	Variables pour l'estimation de la taille du dessin pour l'adapter.

	Références : nous avons utilisé des références pour les bornes du graphique 
	d'une part pour que "estimate_drawing_size" ait la même signature que 
	"execute_symbole_commands" et d'autre part car se sont des variables 
	qui sont modifiées au court de l'exécution sans pour autant être passé en 
	paramètre d'une fonction.
*)
let xmax = ref neg_infinity
let xmin = ref infinity 
let ymax = ref neg_infinity 
let ymin = ref infinity 

(* 	Vaut vrai si l'itération courante sert à l'estimation de la taille du dessin.

	Référence : nous avons utilisé une référence pour "estimation" car sa valeur 
	est modifiée à chaque appel de "word_derivation" et certaines fonctions sont 
	appelées indépendement de "word_derivation" comme "get_factor". 
	"estimation" sert également de contexte global pour toutes les fonctions : 
	dessin en couleur ou non, changement de la longueur du trait ou non.
*)
let estimation = ref true

(* Listes rgb pour le dégradé (avec des valeurs par défaut) *)
(* référence : utilisation via Arg *)
let rgb_from = ref [62;129;244] (* bleu *)
let rgb_to = ref [255; 0; 251] (* violet *)

let rgb_string_to_list (rgb: string) : int list =
	map (int_of_string) (String.split_on_char ',' rgb)

let check_rgb_list (lst: int list) : unit = 
	if length lst != 3 then
		failwith "RGB format is incorrect !"
	else (
		let rec check_rgb_list_el 
			(l : int list) : unit =
			match l with
			| [] -> ()
			| x :: ls -> 
				if x > 255 || x < 0 then
					failwith "RGB format is incorrect !"
				else 
					check_rgb_list_el ls
		in 
		check_rgb_list_el lst
	)

let reset_bounds () : unit = 
	xmax := neg_infinity;
	xmin := infinity ;
	ymax := neg_infinity ;
	ymin := infinity

let get_factor () : float =
	if !estimation = false then (
		let dimx = !xmax -. !xmin in
		let dimy = !ymax -. !ymin in
		(* Calcul empirique du facteur de réduction. *)
		750. /. (max dimx dimy)
	)
	else 1.

(* mix et miy servent à récupérer le centre du dessin. *)
let mix () = (!xmax +. !xmin) /. 2.
let miy () = (!ymax +. !ymin) /. 2.

let get_y_from_current_point () : float = 
	match current_point () with
	| (x,y)-> float_of_int y

let get_color_from i =
	(nth !rgb_from i)
	+ 	(int_of_float ((float_of_int ((nth !rgb_to i)-(nth !rgb_from i))) 
			*. (get_y_from_current_point ()) /. 800.)
		)

let set_gradient_color () : unit =
	set_color (
		rgb 
			(get_color_from 0)
			(get_color_from 1)
			(get_color_from 2)
		)
	
let exec_command (c: command) (p: position) (grad: bool) : position =
	if !estimation = false && grad = true then 
		set_gradient_color ();

	match c with
	| Line (x) -> forward ((float_of_int x) *. (get_factor ())) true p
	| Move (x) -> forward ((float_of_int x) *. (get_factor ())) false p 
	| Turn (x) -> turn p x
	| Color(x) when !estimation = false -> set_color x; p
	| Color(x) -> p
	| Store    -> store p
	| Restore  -> restore p

let rec execute_symbole_commands 
	(pos: position) 
	(c: command list) 
	(grad: bool) : position =
	match c with 
  	| [] -> pos
  	| [cmd] -> exec_command cmd pos grad
	| cmd::cmds -> 
		execute_symbole_commands (exec_command cmd pos grad) cmds grad

(* Fonctions qui interprètent le mot / liste de mots. *)
let rec interpret_s_word_list 
	(f: (position -> command list -> bool -> position ))
	(pos: position) 
	(lst: 's word list) 
	(sys: 's system) 
	(grad: bool) : position =

  	match lst with
	| [] -> pos
	| w::l -> 
		interpret_s_word_list f (interpret_s_word f pos w sys grad) l sys grad

and interpret_s_word 
	(f: (position -> command list -> bool -> position )) 
	(pos: position) 
	(word: 's word) 
	(sys: 's system) 
	(grad: bool) : position =

	match word with 
	| Symb s -> f pos (sys.interp s) grad
	| Branch b -> 	
				exec_command Restore (
					interpret_s_word f (exec_command Store pos grad) b sys grad
				) grad
	| Seq wl -> interpret_s_word_list f pos wl sys grad

let word_derivation 
	(f: (position -> command list -> bool -> position )) 
	(system: 's system)
	(n: int) 
	(pos: position) 
	(grad: bool) : position =

	let rec word_iter
		(word : 's word) 
		(n: int) 
		(pos: position) : position =
		(* Fonction qui s'occupe de traiter les mots. *)
		if n <= 0 then
			interpret_s_word f pos word system grad
		else
			match word with
			| Symb sb -> let derivation = system.rules sb in
						word_iter derivation (n-1) pos 

			| Seq sq -> seq_iter sq n pos

			| Branch br -> word_iter br n pos

	and seq_iter 
		(list: 's word list) 
		(n: int) 
		(pos: position) : position =
		(* Fonction qui parcourt une séquence de mots. *)
		if n <= 0 then
			interpret_s_word_list f pos list system grad
		else (
			match list with
			| [] -> pos
			| l :: ls -> (
				match l with
				| Symb sb ->
					let derivation = system.rules sb in

					if Symb sb = derivation then(
						(* Cas où le symbole est une feuille. *)
						seq_iter ls n 
							(
								interpret_s_word 
								execute_symbole_commands
								pos derivation system grad
							)
					)
					else
						seq_iter ls n (word_iter derivation (n-1) pos)
						
				| Seq sq -> 
				(* On dérive son fils jusqu'à 0. 
					puis on continue à parcourir la séquence. *)
					seq_iter ls n (seq_iter sq (n-1) pos)

				| Branch br -> 
				(* On Store la position de base puis on dérive et on Restore *)
					seq_iter ls n 
						(
							exec_command Restore 
							(word_iter br n (exec_command Store pos grad)) 
							grad
						)
			)
		)
	in
	let p = word_iter system.axiom n pos in 
	(* Comme une fois sur deux on fait l'estimation, et l'autre fois 
		le dessin, on peut changer la valeur de estimation à chaque appel 
		de word_derivation *)
	estimation:= not !estimation ; 
	p

let estimate_drawing_size 
	(pos: position) 
	(c: command list) 
	(grad: bool) : position = 
	
	let p = execute_symbole_commands pos c false in
	
	if (p.x > !xmax ) then xmax := p.x;
	if (p.x < !xmin ) then xmin := p.x;
	if (p.y < !ymin ) then ymin := p.y;
	if (p.y > !ymax ) then ymax := p.y;
	p

let change_interp (sys: 's system) (left: bool) : 's system =
	(* L'argument left est un booleen qui indique si le dessin doit être tourné
		vers la gauche ou non. *)
	let change_cmd (cmd_list: command list) : command list = 
		let cmd = hd cmd_list in

		match cmd with
		| Turn v when left = true -> (Turn (v + 10))::[]
		| Turn v when left = false -> (Turn (v - 10))::[]
		| _ -> cmd_list
	in
	let new_interp = fun x -> change_cmd (sys.interp x) in
	{
		axiom = sys.axiom; 
		rules =  sys.rules; 
		interp = new_interp
	}