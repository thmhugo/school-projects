
(** Commandes de la tortue*)
type command =
| Line of int      (** avance la tortue en dessinant *)
| Move of int      (** avance la tortue sans dessiner *)
| Turn of int      (** tourne la tortie selon un degré n *)
| Color of int     (** change la couleur de la ligne *)
| Store            (** sauvegarde la position actuelle de la tortue *)
| Restore          (** recharge la dernière position non rechargé *)

(** Position et angle de la tortue *)
type position = {
	x: float;        (** position x *)
	y: float;        (** position y *)
	a: float;        (** angle de la direction *)
}

val open_window : unit -> unit
val get_starting_pos : position
val get_starting_angle : float
val forward : float -> bool -> position -> position
val turn : position -> int -> position
val restore : position -> position
val store : position -> position