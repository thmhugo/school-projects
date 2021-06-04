(* Ce mini-projet porte sur l'apprentissage d'automates séparateurs.
   La lecture de la Section 16.3 des notes de cours est fortement
   recommandée. Le code que vous devez écrire prend en entrée deux
   listes de mots I et E, lues à partir d'un fichier passé en argument
   et renvoie sur la sortie standard le code SMT-LIB à faire passer
   dans une solveur SMT, comme Z3. 
 *)

open Printf

(* ensembles de test : ensemble I *)
(* let li = ["";"ab"]["a";"aba"; "ba"] *)
             
(* ensembles de test : ensemble E *)
(* let le = ["aa";"b"] ["bc";"ca"]  *)

let print_char_list l =
  List.iter (fun x -> Printf.printf "%c" x) l

let print_str_list l =
  Printf.printf "\n[";
  List.iter (fun x -> Printf.printf "%s; " x) l;
  Printf.printf "]\n"

(* ======================================================================= *)
(* EXERCICE 1 : extraire  l'alphabet de l'automate.
   La fonction alphabet_from_list prend en entrée une liste de
   chaînes de caractères et retourne une liste triée de
   caractères sans duplication. 
 *)
(* explode : string -> char list 
   prend une chaîne de caractères et renvoie la liste de ses caractères 
 *)
let explode s =
  let rec exp i l =
    if i < 0 then l else exp (i - 1) (s.[i] :: l) in
  exp (String.length s - 1) []

(* alphabet_from_list : string list -> char list  
   - prend en entrée une liste l de chaînes de caractères 
   - renvoie la liste triée et sans duplication de caractères
     apparaissant dans l
 *)
let rec alphabet_from_list l : char list =
  let rec to_alphabet (l: string list) (alpha: char list) : char list =
    match l with 
    | [] -> (* Comme alpha contient l'alphabet de chaque chaîne de la liste l, 
                on retourne l'alphabet de alpha *)
      List.sort_uniq compare alpha 
    | x::ls -> (*Concatène l'alphabet de chaque chaîne à alpha*)
      to_alphabet ls ((List.sort_uniq compare (explode x))@alpha)
  in to_alphabet l []
 
  
(* test *)
(* let a = alphabet_from_list (li @ le) *)

(* ======================================================================= *)
(* EXERCICE 2 : définition de l'alphabet A et de l'arbre préfixe T en
   SMT-LIB Pour les listes données en exemple, la sortie de la
   fonction declare_types doit être la chaîne de caractères
   "(declare-datatypes () ((A a b) (T e ea eaa eab eb)))" *)

(* prefixes : string -> string list
   renvoie la liste des préfixes d'une chaîne de caractères 
   Nous allons ajouter à chaque préfixe le caractère 'e'.
   Par exemple, prefixes "aba" = ["e"; "ea"; "eab"; "eaba"] *)
let prefixes (s: string) : string list =
  let rec pref (s: string) (p: string list) : string list = 
    let s_len = String.length s in
    if s_len=0 then 
      "e"::p
    else
      pref (String.sub s 0 ((s_len)-1)) (("e"^s)::p)

  in pref s []
  
(* prefixes_of_list : string list -> string list
   renvoie la liste triée et sans doublons des préfixes des chaînes 
   de caractères d'une liste donnée *)
let prefixes_of_list (l: string list) : string list =
  let rec pref_list (l: string list) (p: string list) : string list =
    match l with 
    | [] -> p 
    | x :: ls -> (*On concatène les préfixes de x à p*) 
        pref_list ls ((prefixes x)@p)
  in (* Comme p contiendra tous les préfixes de chaque chaînes de l, on 
      utilise sort_uniq dessus *) 
  List.sort_uniq compare (pref_list l [])
 
  
(* declare_types_alphabet : char list -> string
   prend une liste de caractères [c_1; ...; c_n] et renvoie une chaîne 
   de la forme "(A c_1... c_n)" *)
let declare_types_alphabet (cl: char list) : string =
  "(A " ^ String.concat " " (List.map (String.make 1) cl) ^")"

(* declare_types_trie : string list -> string 
   prend une liste l de chaînes de caractères et 
   renvoie une chaîne de la forme "(T es_1 ... es_n)" où 
   s_1... s_n est une énumération de tous les 
   prefixes des chaînes apparaissant dans l *)
let declare_types_trie (l : string list) : string =
  "(T " ^ String.concat " " (prefixes_of_list l ) ^")"

(* declare_types : string list -> char list -> string *)
let declare_types (l : string list) (cl: char list) : string =
  "(declare-datatypes () ("
  ^ declare_types_alphabet cl
  ^ " "
  ^ declare_types_trie l
  ^ "))"

  
(* test *)
(* let _ = Printf.printf "%s\n" (declare_types (li @ le) a) *)
  

(* ======================================================================= *)
(* EXERCICE 3 : définir une chaîne de caractères pour les définitions
   en SMT-LIB de
   - l'ensemble d'états Q,
   - la fonction delta de transition de l'automate,
   - l'ensemble final des états acceptants et
   - la fonction f,
   ainsi que les assertions associées.
   Ces définitions ne dépendent pas des listes de mots acceptés ou rejetés. *)

let define_sorts_and_functions  =
  "(define-sort Q () Int)\n"
  ^ "(declare-const n Q)\n"
  ^ "(assert (> n 0))\n"                              (* assertion *)
  ^ "(declare-fun delta (Q A) Q)\n"                   (* fonction delta de transition *)
  ^ "(assert (forall ((q Q) (a A))\n"                 (* assertion *)
  ^ "(and (>= (delta q a) 0) (< (delta q a) n))))\n"  (* assertion *)
  ^ "(declare-fun final (Q) Bool)\n"                  (* ensemble final des états acceptants*)
  ^ "(declare-fun f (T) Q)\n"                         (*la fonction f*)
  ^ "(assert (forall ((x T))\n"                       (* assertion *)
  ^ "(and (>= (f x) 0) (< (f x) n))))\n"              (* assertion *)
  ^ "(assert (= 0 (f e)))\n"                          (* contrainte sur l'état initial *)
   
(* ======================================================================= *)
(* EXERCICE 4 : contraintes sur les transitions
   La fonction assert_transition_constraints prend en entrée une trie 
   et retourne une chaîne qui modélise les contraintes sur les transitions 
   de l'automate décrites par la formule (56). *)
  
(* eq_trans_constr : string -> char -> string
   renvoie une chaîne de caractères qui correspond à une formule atomique pour 
   la transition étiquetée avec 'a' à partir de l'état s
   Par exemple, pour s = "abc" et  c = 'd' on a 
   eq_trans_constr outputs "(= (f abcd)  (delta (f abc)  d))" *)
let eq_trans_constr (s: string) (a: char) : string =
  let a_str = String.make 1 a in 
  "\n       (= (f " ^ s ^ a_str ^ ")  (delta (f " ^ s ^ ")  " ^ a_str ^ "))"

(* let _ = Printf.printf "%s\n" (eq_trans_constr "abc" 'd') *)

(* list_transition_constraints : string list -> string 
   prend une liste de chaînes de caractères et génère une 
   chaîne de caractère des équations associées comme suit :
   - pour une chaîne non vide de la forme sa on obtient
     une chaine correspondant à l'équation f(sa) = delta (fs) a
   - pour la chaîne vide on obtient rien *)
let list_transition_constraints (l: string list) : string =
  let rec make_constraints_eq_str (l: string list) (eq_str: string) : string = 
    match l with
    | [] -> eq_str
    | x :: ls -> 
    let x_len = String.length x in
      if (x_len<2) then 
        make_constraints_eq_str ls eq_str
      else
        make_constraints_eq_str ls (
          eq_str ^ (eq_trans_constr (String.sub x 0 ((x_len)-1)) (String.get x (x_len-1)))
        )
    in make_constraints_eq_str l ""

(* assert_transition_constraints : string list -> string
   prend en entrée une liste de mots et renvoie une chaîne qui modélise 
   les contraintes sur les transitions de l'automate décrit par la 
   formule (56).
   Par exemple, pour la liste [""; "ab"; "aa"; "b"] on obtient la chaîne
   "(assert (and 
               (= (f ea)  (delta (f e)  a))
               (= (f eaa)  (delta (f ea)  a))
               (= (f eab)  (delta (f ea)  b))
               (= (f eb)  (delta (f e)  b))))"
 *)
let assert_transition_constraints (l: string list) : string =
  "(assert (and "
  ^ (list_transition_constraints (prefixes_of_list l))
  ^ "))\n"

(* test *)
(* let _ = Printf.printf "%s" (assert_transition_constraints (li @ le)) *)

(* ======================================================================= *)
(* EXERCICE 5 : contraintes sur les états acceptants La fonction
   assert_acceptance prend en entrée deux listes de mots et retourne
   une chaîne de caractères qui modélise les contraintes sur les états
   acceptants décrites par la formule (57). *)

(* eq_accept : string -> string 
   - prend une chaîne de caractères s et renvoie une chaîne de caractères 
   qui modélise l'appartenance de s à l'ensemble final des états acceptants *)
let eq_accept (s: string) : string =
  "(final (f " ^ s ^"))"

(* eq_non_accept : string -> string 
   - prend une chaîne de caractères s et renvoie une chaîne de caractères 
   qui modélise la non-appartenance de s à l'ensemble final des états acceptants 
 *)
let eq_non_accept (s: string) : string =
  if (String.length s) < 2 then 
    "" 
  else
    "(not"^ (eq_accept s) ^ ")"

(* assert_acceptance : string list -> string list > string
   prend deux listes de chaînes de caractères, li et le, et renvoie une
   chaine qui modélise les contraintes sur les états acceptants
   décrites par la formule (52). 
   Les mots dans li sont acceptés et les mots dans le ne le sont pas. *)
let assert_acceptance (li: string list) (le: string list) : string =
  let acc_e_li = List.map (fun x -> eq_accept ("e"^x)) (li) in
  let non_acc_e_le =  List.map (fun x -> eq_non_accept ("e"^x)) (le) in
  "(assert (and "
  ^ String.concat "\n           " acc_e_li
  ^ "\n           "
  ^ String.concat "\n           " non_acc_e_le
  ^ "))\n"
  
(* test *)
(* let _ = Printf.printf "%s" (assert_acceptance li le) *)
  
(* ======================================================================= *)
(* EXERCICE 6 :
   La fonction smt_code prend en entrée deux listes de mots
   et retourne une chaîne de caractères qui donne l'implémentation 
   en SMT-LIB. *)

(* smt_code : string list -> string list -> string 
   prend deux listes de chaînes de caractères, li et le, et renvoie une chaîne 
   de caractères qui donne l'implémentation en SMT-LIB.
   Les mots dans li sont acceptés et les mots dans le ne le sont pas. 
   Pour vérifier votre algorithme, vous pouvez essayer le code SMT-LIB 
   que vous obtenez dans le solveur Z3: https://rise4fun.com/z3 *)
let smt_code li le =
  declare_types (li @ le) (alphabet_from_list (li @ le))
  ^ "\n\n"
  ^ define_sorts_and_functions
  ^ "\n"
  ^ assert_transition_constraints (li @ le)
  ^ "\n"
  ^ assert_acceptance li le
  ^ "\n(check-sat-using (then qe smt))\n\n"
  ^ "(get-model)\n\n"  
  ^ "(exit)\n"
  
(* test *)
(* Printf.printf "%s" (smt_code li le) *)


(* ======================================================================= *)
(* lire deux listes de chaîne de caractères I et E d'un fichier *)
(* Ne pas modifier cette partie *)

let input_line_opt ic =
  try Some (input_line ic)
  with End_of_file -> None
                    
let read_lines ic =
  let rec aux acc =
    match input_line_opt ic with
    | Some line -> aux (line::acc)
    | None -> (List.rev acc)
  in
  aux []
  
let lines_of_file filename =
  let ic = open_in filename in
  let lines = read_lines ic in
  close_in ic;
  (lines)

let read_lists_from_file (filename: string): ((string list) * (string list))  =
  let lines = lines_of_file filename in
  (String.split_on_char ' ' (List.nth lines 0),
   String.split_on_char ' ' (List.nth lines 1))
  
let () =
  let (li,le) = (read_lists_from_file Sys.argv.(1)) in
  Printf.printf "%s\n" (smt_code li le);
  (* print_char_list a; *)
  (* print_str_list (prefixes_of_list ["abac";"ad";"abc"]) *)
  (* Printf.printf "%s" (declare_types_alphabet a) ; *)
  (* Printf.printf "%s\n" (declare_types_alphabet a);
  Printf.printf "%s\n" (declare_types_trie (li@le)) *)
  (* Printf.printf "%s\n" (assert_transition_constraints (li@le)); *)
  (* Printf.printf "%s\n" (assert_transition_constraints [""; "ab"; "aa"; "b"])  *)