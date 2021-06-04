## Exemples de cas d'usage

> pile et historique de la forme [(index) valeur, ....]


### Syntaxe RPN

#### Opérations dans les entiers 

    Please choose your calculator's mode between 'RPN' and 'ALG' (Stands for algebraic notation).
    > RPN
    > 1                         // empile 1
    1
    > 2                         // empile 2
    2
    > +                         // dépile deux fois -> empile 1+2=3
    3
    > 4 5 *                     // empile 4*5=20
    20
    > *                         // dépile deux fois -> empile 3*20=60
    60
    > showStack
    Current stack: [(0) 60]     // commande "show" montre la pile actuelle (ici contient 60 à l'indice 0)
    > -                         // dépile deux fois, mais il n'y a qu'un élément dans la pile : erreur.
    Not enough operands in stack
    Stack emptied               // La pile est vidée lors d'une erreur impliquant des dépilages 

#### Opérations dans les décimaux 

    Please choose your calculator's mode between 'RPN' and 'ALG' (Stands for algebraic notation).
    > RPN
    > 1.5 1.5 +                 // empile 3.0 , qui est de type décimal
    3.0 
    > 2                         // empile 2 , de type entier
    2   
    > /                         // division entier,décimal de type décimal : empile 3.0/2.0 = 1.5
    1.5
    > 5 *                       // multiplication entier,décimal de type décimal : empile 1.5*5=7.5
    7.5

#### Opérations dans les ensembles

    Please choose your calculator's mode between 'RPN' and 'ALG' (Stands for algebraic notation).
    > RPN
    > {1;2;3} {2;3;4;5} U       // union des ensembles {1;2;3} et {2;3;4;5}
    {1;2;3;4;5}
    > {1;4;7}                   // empile {1;4;7}
    {1;4;7}
    > I                         // intersection de {1;4;7} et {1;2;3;4;5} = {1;4}
    {1;4}
    > {3;2} U                   // union de {1;4} et {3;2} = {1;2;3;4}
    {1;2;3;4}

#### Opérations dans les booléens

    Please choose your calculator's mode between 'RPN' and 'ALG' (Stands for algebraic notation).
    > RPN
    > true
    true
    > false
    false
    > or                    // empile true or false = true
    true
    >false
    false
    > xor                   // empile true xor false = true
    true


### Syntaxe ALG (algébrique)

    Please choose your calculator's mode between 'RPN' and 'ALG' (Stands for algebraic notation).
    > ALG
    > ( 3 + 2 ) * 5   
    25
    > ( ( true or false ) and ( false or true ) )
    true


### Gestion de la pile et de l'historique en mode RPN

    > showStack                  // on part d'une pile contenant ces valeurs
    Current stack: [(0) 1, (1) 2, (2) 3, (3) 4, (4) 5, (5) 6] 
    > pile(2)               // pile(2) empile la valeur à l'emplacement 2 de la pile = 3                          
    3
    > showStack
    Current stack: [(0) 1, (1) 2, (2) 3, (3) 4, (4) 5, (5) 6, (6) 3]
    > +                     // depile deux fois, empile 6 + 3 = 9
    9
    > showStack
    Current stack: [(0) 1, (1) 2, (2) 3, (3) 4, (4) 5, (5) 9]
    > +                     // on réduit la pile...
    14
    > +
    18
    > +
    21
    > +
    23
    > showStack     
    Current stack: [(0) 1, (1) 23]
    > pile(1)               // on empile la valeur à l'emplacement 1 = 23
    23
    > *                     // empile 23*23
    529
    > showStack
    Current stack: [(0) 1, (1) 529]
    > +                     // empile 529+1 = 530
    530
    > showHist              // affiche l'historique de toute les valeurs qui ont été empilées
    Current history: [(0) 1, (1) 2, (2) 3, (3) 4, (4) 5, (5) 6, (6) 3, (7) 9, (8) 14, (9) 18, (10) 21, (11) 23, (12) 23, (13) 529, (14) 530]
    > clear                 // on vide la pile 
    > showStack
    Current stack: []
    > hist(13)              // empile la 13ème valeur de l'historique = 529
    529
    > hist(11)              // empile la 11ème valeur de l'historique = 23
    23
    > /                     // empile 529 / 23 = 23
    23
    > showStack
    Current stack: [(0) 23]
    > hist(12) pile(0) *   // empile (hist(12) = 23) * (pile(0) = 23) = 529
    529


### Gestion de la pile et de l'historique en mode ALG

    > showHist              // on part de cet historique de valeurs
    Current history: [(0) 1, (1) 2, (2) 3, (3) 4, (4) 5, (5) 6]
    > hist(0) + hist(1) + hist(2) + hist(3) + hist(4) + hist(5)  // calcule 1+2+3+4+5+6=21 (et ajout à l'historique)
    21
    > hist(3) !  // calcule 4! 
    24


### Variables 

    Please choose your calculator's mode between 'RPN' and 'ALG' (Stands for algebraic notation).
    > RPN
    > 10                    // empile 10
    10
    > 5                     // empile 5
    5
    > !x                    // attribue à "x" la valeur 5 qui est dépilée
    > !y                    // attribue à "y" la valeur 10 qui est dépilée
    > showVar               // affichage des variables connues
    [2]
    x=5 y=10 
    > showStack                  // la pile est bien vide
    Current stack: []
    > ?x ?y *               // empile (x=5) + (y=10) = 50
    50
    > ?y                    // empile la valeur de y
    10
    > ?a                    // variable "a" encore inconnue
    Error: the variable was not initialized
    > showStack                  // la pile contient le résultat du calcul précédent et le resultat de "?y"
    Current stack: [(0) 50, (1) 10]
    > +                     
    60
    > !a                    // attribue à "a" la valeur 60
    > ?a ?y * ?y *               
    6000
    > showStack
    Current stack: [(0) 6000]