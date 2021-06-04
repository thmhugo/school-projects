open Turtle
open Stack
open Systems
open List

(** Fonctions qui s'occupent de charger un fichier système.*)

let convert_string_to_word_list (string_arg: string) : 'a word list =
	let rec explode (i: int) (list: 'a word list) =
		if i < 0 then 
			(list, i)
		else ( 
			let tmp = String.make 1 string_arg.[i] in
			match tmp with
			| "[" -> (list, i)
			| "]" -> 
					let (word_list, index) = explode (i - 1) [] in
					let branch = Seq word_list in
					explode (index - 1) ((Branch branch) :: list) 
			| _ ->  explode (i - 1) ((Symb tmp) :: list)
		)
	in
	let (word_list, index) = explode (String.length string_arg - 1) [] in
	word_list

(* Crée une liste d'uplet où le premier champ est un symbole et 
le deuxième est la règle associée. *)
let rec str_list_to_uplet_rules_list 
	(list: string list) 
	(uplet_list: (string * string word) list) : (string * 'a word) list =
	match list with
	| [] -> uplet_list
	| x1 :: x2 :: xs -> 
		let sequence = Seq (convert_string_to_word_list x2) in
		str_list_to_uplet_rules_list xs ((x1, sequence) :: uplet_list)
	| _ -> failwith "Loading file: Rules list is incomplete !"

(* Axiome *)
let convert_str_to_axiom (axiom: string) : 'a word =
	Seq (convert_string_to_word_list axiom)

(* Règles *)
let find_word_in_uplet_list
	(uplet_list: (string * 'a word) list)
	(word: string) : 'a word =
	let find = find_opt (fun (a,b) -> a = word) uplet_list in
	match find with
	| None -> Symb word
	| Some (a,b) -> b

(* Convertit une liste de lignes de règles en une liste de règles, par exemple:
	[A A[+A]; ..] -> [A; A[+A]; ...] *)
let str_lines_list_to_str_list (str_rules_list: string list) : string list =
	let rec split_into_list 
		(list: string list) 
		(splited_list: string list) : string list =
		match list with 
		| [] -> splited_list
		| x :: xs -> 
			(* Split la liste et supprime les lignes vides. *)
			let split = 
				filter (fun s -> s <> "") (String.split_on_char ' ' x) in 
			split_into_list xs (split @ splited_list)
	in
	split_into_list str_rules_list []

(* Interprétations *)
let str_to_turtle_cmd (str_cmd: string) : command =
	(* La première lettre du mot est l'abbréviation de la commande et 
	la suite correspond à la valeur associée à celle-ci. *)
	let str_value = String.sub str_cmd 1 ((String.length str_cmd) - 1) in

	if str_cmd.[0] = 'C' then (
		let list_colors = rgb_string_to_list str_value in
		check_rgb_list list_colors;
		let value = 
			Graphics.rgb 
				(nth list_colors 0) 
				(nth list_colors 1) 
				(nth list_colors 2) 
		in
		Color value
	)
	else (
		let value =  int_of_string str_value in
		match str_cmd.[0] with
		| 'L' -> Line value
		| 'M' -> Move value
		| 'T' -> Turn value
		| _ -> failwith "Loading file: Incorrect turtle command !"
	)

(* Crée une liste d'uplets où le premier champ est le symbole et 
le second est la règle associée. *)
let rec str_list_to_uplet_interp_list 
	(list: string list) 
	(uplet_list: (string * command list) list) : (string * command list) list =
	match list with
	| [] -> uplet_list
	| x1 :: x2 :: xs -> 
		let turtle_cmd = (str_to_turtle_cmd x2)::[] in
		str_list_to_uplet_interp_list xs ((x1, turtle_cmd)::uplet_list)
	| _ -> failwith "Loading file: Interpretation list is incomplete !"

let find_cmd_in_uplet_list 
	(uplet_list: (string * command list) list) 
	(word: string) : command list =
	let find = find_opt (fun (a,b) -> a = word) uplet_list in
	match find with
	| None -> (Line 0)::[]
	| Some (a,b) -> b

(* Fonctions qui chargent les fichiers. *)
let get_system_from_list_lines (list_lines: (string list)) : 's system =
	if length list_lines < 3 then
		failwith "Loading file: The file might be incomplete !";
	
	(* Associe chaque lignes de la liste à un des trois champs du système:
	axiome, règles ou interprétation; en utilisant le triplet. *)
	let rec interpret_lines
		(list_lines: string list)
		((ax,rl,it): string * (string list) * (string list)) 
		(index: int) : string * (string list) * (string list) =
		match list_lines with 
		| [] -> (ax,rl,it)
		| x :: xs -> (
			if x = "" then interpret_lines xs (ax,rl,it) (index+1)
			else (
				match index with
				| 0 -> interpret_lines xs (ax,rl,(x::it)) index
				| 1 -> interpret_lines xs (ax,(x::rl),it) index
				| 2 -> interpret_lines xs (x,rl,it) index
				| _ -> failwith 
					"Loading file: The number of lines is incorrect !"
			)
	)
	in
	let (ax,rl,it) = interpret_lines list_lines ("",[],[]) 0 in

	let rules_list = str_lines_list_to_str_list rl in 
	let uplet_list_rules = str_list_to_uplet_rules_list rules_list [] in

	let interp_list = str_lines_list_to_str_list it in 
	let uplet_list_interp = str_list_to_uplet_interp_list interp_list [] in

	{
		axiom = convert_str_to_axiom ax;
		rules = (function x -> find_word_in_uplet_list uplet_list_rules x);
		interp = (function x -> find_cmd_in_uplet_list uplet_list_interp x)
	}

let split_lines_and_escape_comments (filename: string) : string list =
	let ci = open_in filename in 

	let rec read_lines 
		(lines_acc: string list) 
		(file: in_channel) : string list =
		try ( 
			let current_line = input_line file in
			if String.index_opt current_line '#' <> Some 0 then 
				read_lines (current_line :: lines_acc) file
			else
				read_lines lines_acc file
		)
		with End_of_file -> (
			close_in file;
			lines_acc 
		)
	in
	read_lines [] ci

let has_right_extension (filename: string) : bool = 
	let len = String.length filename in
	let sub = String.sub filename (len - 4) 4 in

	String.equal sub ".sys" 

let load_file (filename: string) : 's system = 
	let len = String.length filename in
	if has_right_extension filename then (
		print_endline (filename ^ " is being loaded...\n");
		let split = split_lines_and_escape_comments filename in
		get_system_from_list_lines split
	)
	else 
		failwith "The loaded file hasn't the right extension."
	


(** Fonctions qui s'occupe de sauvegarder le système courant en une image 
	(format PNG).*)

(* Convertit un "Graphics.color" en un triplet de la forme (r,b,g) pour être 
utilisé par Cairo. Pour plus d'informations, cf :
https://caml.inria.fr/pub/docs/oreilly-book/html/book-ora048.html *)
let from_rgb (c : Graphics.color) : (float * float * float) = 
  let r = c / 65536  and  g = c / 256 mod 256  and  b = c mod 256 
  in 
  	(
	  (float_of_int r) /. 255., 
	  (float_of_int g) /. 255., 
	  (float_of_int b) /. 255.
	)

let save () =
  (* Transforme la fenêtre Graphics en matrice de couleurs. *)	
	let img = Graphics.dump_image (Graphics.get_image 0 0 800 800) in 
  	let surface = Cairo.Image.create Cairo.Image.ARGB32 ~w:800 ~h:800 in
  	let cr = Cairo.create surface in

  	Cairo.set_line_width cr 1.5;
  	Cairo.move_to cr 0. 0.;

  (* Dessine chaque lignes de la matrice. *)
  	let rec draw_colors_line 
		(l: Graphics.color list) (i: int) (j: int) : unit = 
		match l with
		| [] -> ()
		| x :: ls -> 
		let (r,g,b) = from_rgb x in
		(* Pour alléger le dessin, les éléments en blanc ne sont pas dessinés.*)
			if (r,b,g) <> (1.,1.,1.) then (
			Cairo.set_source_rgb cr r g b;
		(* Dessine un "point", pour plus d'informations cf:
		https://lists.cairographics.org/archives/cairo/2009-June/017459.html *)
			Cairo.move_to cr (float_of_int i) (float_of_int j);
			Cairo.line_to cr (float_of_int (i+1)) (float_of_int (j+1));
			Cairo.stroke cr; 
			);
			draw_colors_line ls (i+1) j

	(* Parcours des colonnes de la matrice. *)    
	and run_cols (c: Graphics.color list list) (j: int) : unit= 
		match c with 
		| [] -> ()
		| x :: cs -> draw_colors_line x 0 j; run_cols cs (j+1);
		in run_cols (map Array.to_list (Array.to_list img)) 0;

  Cairo.PNG.write surface "turtle.png"