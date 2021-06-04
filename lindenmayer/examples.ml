open Systems
open Turtle

(* Examples tirés du livre "The Algorithmic Beauty of Plants".
   Un exemple consiste en un axiome, un système de réécriture,
   et une interprétation. Pour plus d'exemples, voir les fichiers
   dans le répertoire examples/
*)

type symbol = A|P|M|B|G|X

(* snow flake  - Figure 3 du sujet *)
let snow : symbol system =
  let a = Symb A in
  let p = Symb P in
  let m = Symb M in
  let x = Symb X in
  let b = Symb B in
  let g = Symb G in 
  {
    axiom = Seq [g;a;p;p;b;a;p;p;a];
    rules =
      (function
       | A -> Seq [g;a;b;m;a;p;p;a;m;g;a]
       | s -> Symb s);
    interp =
      (function
       | A -> [Line 5]
       | P -> [Turn 60]
       | M -> [Turn (-60)]
       | G -> [Color (Graphics.rgb 36 113 163)]
       | B -> [Color (Graphics.rgb 23 165 137)]
       | X -> [Color (Graphics.rgb 244 208 63)])
  }

(* cantor  - Figure 2 du sujet *)
let cantor : symbol system =
  let a = Symb A in
  let p = Symb P in
  {
    axiom = Seq [a];
    rules =
      (function
       | A -> Seq [a;p;a]
       | P -> Seq [p;p;p]
       | s -> Symb s
       );
    interp =
      (function
       | A -> [Line 10]
       | P -> [Move 10]
       | s -> [Line 0]
       )
  }

(* plant *)
let plant : symbol system =
  let a = Symb A in
  let p = Symb P in
  let m = Symb M in
  {
    axiom = Seq [a];
    rules =
      (function
       | A -> Seq [a;Branch(Seq [p;a]);a;Branch(Seq [m;a]);a]
       | s -> Symb s
       );
    interp =
      (function
       | A -> [Line 10]
       | P -> [Turn 25]
       | s -> [Turn (-25)]
       )
  }

let plant_colored : symbol system =
  let a = Symb A in
  let p = Symb P in
  let m = Symb M in
  let b = Symb B in
  let g = Symb G in 
  {
    axiom = Seq [a];
    rules =
      (function
        | A -> Seq [b;a;Branch(Seq [p;g;a;b]);b;a;g;Branch(Seq [m;g;a;b]);b;a]
        | s -> Symb s
        );
    interp =
      (function
        | A -> [Line 10]
        | P -> [Turn 25]
        | G -> [Color (Graphics.rgb 51 204 51)]
        | B -> [Color (Graphics.rgb 153 102 51)]
        | s -> [Turn (-25)]
        )
    }

let arrow : symbol system =
  let a = Symb A in
  let p = Symb P in
  let m = Symb M in
  let x = Symb X in 
  {
    axiom = Seq [x];
    rules =
      (function
       | A -> Seq [a;a]
       | X -> Seq [a;Branch(Seq [p;x]);Branch(Seq [m;x]);a;x]
       | s -> Symb s
       );
    interp =
      (function
       | A -> [Line 10]
       | P -> [Turn 25]
       | M -> [Turn (-25)]
       | s -> [Line 0]
       )
  }

let twiggy : symbol system =
  let a = Symb A in
  let p = Symb P in
  let m = Symb M in
  let x = Symb X in
  {
    axiom = Seq [x];
    rules =
      (function
        | A -> Seq [a;a]
        | X -> Seq [a;Branch(Seq [m;x]);a;Branch(Seq [m;x]);p;x]
        | s -> Symb s
        );
    interp =
      (function
        | A -> [Line 10]
        | P -> [Turn 25]
        | M -> [Turn (-25)]
        | s -> [Line 0]
        )
  }

let twiggy_colored : symbol system =
  let a = Symb A in
  let p = Symb P in
  let m = Symb M in
  let x = Symb X in
  let b = Symb B in
  let g = Symb G in 
  {
    axiom = Seq [x];
    rules =
      (function
        | A -> Seq [b;a;a]
        | X -> Seq [g;a;g;Branch(Seq [g;m;x]);a;g;Branch(Seq [g;m;x]);g;p;x;b]
        | s -> Symb s
        );
    interp =
      (function
        | A -> [Line 10]
        | P -> [Turn 25]
        | M -> [Turn (-25)]
        | G -> [Color (Graphics.rgb 51 204 51)]
        | B -> [Color (Graphics.rgb 153 102 51)]
        | s -> [Line 0]
        )
  }