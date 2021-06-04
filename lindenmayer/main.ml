open Graphics
open Lsystems (* Librairie regroupant le reste du code. Cf. fichier dune *)
open Turtle
open Systems
open Examples


(** Gestion des arguments de la ligne de commande.
		Nous suggérons l'utilisation du module Arg
		http://caml.inria.fr/pub/docs/manual-ocaml/libref/Arg.html
*)

(* Références : utilisation via Arg *)
let use_gradient = ref false
let source_path = ref ""

let usage = (* Entête du message d'aide pour --help *)
	"Interpretation de L-systemes et dessins fractals"

let action_what () = Printf.printf "%s\n" usage; exit 0

let action_keys () = Printf.printf "%s\n%s\n%s\n%s\n%s\n%s\n%s\n" 
	" 'a' pour dessiner l'itération suivante"
	" 'z' pour dessiner l'itération précédente"
	" 'e' pour augmenter l'angle de 10"
	" 'r' pour réduire l'angle de 10"
	" 't' pour réinitialiser l'angle à la valeur par défaut"
	" 's' pour sauvegarder l'image actuelle"
	" 'q' pour fermer la fenêtre et quitter arrêter l'exécution";
	exit 0

let action_use_gradient () = use_gradient:=true

let action_set_gradient_from  (rgb : string) : unit = 
	rgb_from := rgb_string_to_list rgb; 
	check_rgb_list !rgb_from

let action_set_gradient_to (rgb : string) : unit = 
	rgb_to := rgb_string_to_list rgb;
	check_rgb_list !rgb_to

let action_set_source_file path = source_path := path

let cmdline_options = 
[
("--what" , Arg.Unit action_what, 
	"        | Description");
("--keys" , Arg.Unit action_keys, 
	"        | Liste de chaque touches utiles et leur action");
("--gradient", Arg.Unit action_use_gradient, 
	"    | Cette option fait faire un dégradé au dessin");
("-g", Arg.Unit action_use_gradient, 
	"            | Cette option fait faire un dégradé au dessin 
		 |  (raccourci pour --gradient)");

("--from", Arg.String action_set_gradient_from, 
	"[r,g,b] | Couleur initiale du dégradé (--gradient/-g nécessaire).
		 |  ex : --from 62,129,244 ");
("--to", Arg.String action_set_gradient_to, 
	"[r,g,b]   | Couleur finale du dégradé (--gradient/-g nécessaire). 
		 |  ex : --to 255,0,251 ");
("--load", Arg.String action_set_source_file, 
	"[path]  | Déclare le fichier source à utiliser\n")
]

let extra_arg_action = fun s -> failwith ("Argument inconnu :"^s)

let draw 
	(n: int) 
	(sys: 's system) 
	(grad: bool) : unit =
	reset_bounds ();
	set_color white;
	clear_graph ();
	let _ = 
		word_derivation estimate_drawing_size sys n get_starting_pos false in 
		();
	set_color black;
	(* Etant donné les bornes du dessin en taille réelle, mix et miy 
		représentant respectivement le milieu de l'axe x et le milieu de 
		l'axe y, cette nouvelle position donne le point de départ du dessin 
		pour que celui-ci soit centré dans la fenêtre. *)
	let pos =  
		{
			x = 400. +.(400. -. (mix ())) *. (get_factor ()); 
			y = 400. +.(100. -. (miy ())) *. (get_factor ()); 
			a = get_starting_angle
		} in
	let _ = word_derivation execute_symbole_commands sys n pos grad in ()

let draw_syst 
	(default_s : 's system)
	(grad : bool) : unit =
	let rec key_binded_drawing (s: 's system) (count: int ) : unit= 
		let event = wait_next_event [Key_pressed] in
		match event.key with
		| 'a' -> (
			let c = count+1 in
			draw c s grad;
			key_binded_drawing s c
		)
		| 'z' -> (
			if count = 0 then 
				key_binded_drawing s count
			else
				let c = count-1 in
				draw c s grad;
				key_binded_drawing s c
		)
		| 'e' -> (
			let new_syst = change_interp s true in
			draw count new_syst grad;
			key_binded_drawing new_syst count
		)
		| 'r' -> (
			let new_syst = change_interp s false in
			draw count new_syst grad;
			key_binded_drawing new_syst count
		)
		| 't' -> (
			draw count default_s grad;
			key_binded_drawing default_s count
		)
		| 'q' -> (
			close_graph ();
			exit 0
		)
		| 's' -> (
			FileManager.save ();  
			key_binded_drawing s count
		)
		| _ -> key_binded_drawing s count
	in
	key_binded_drawing default_s 0

let main () =
	open_window ();
	set_line_width 1;
	Arg.parse cmdline_options extra_arg_action usage;
	try(
		if !source_path <> "" then  
			draw_syst (FileManager.load_file !source_path) !use_gradient
		else draw_syst arrow !use_gradient
	) 
	with Graphics.Graphic_failure("fatal I/O error") -> (
		close_graph ();
		exit 0
	)
	
(** On ne lance ce main que dans le cas d'un programme autonome
		(c'est-à-dire que l'on est pas dans un "toplevel" ocaml interactif).
		Sinon c'est au programmeur de lancer ce qu'il veut *)

let () = if not !Sys.interactive then main ()