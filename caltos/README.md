# Calculatrice

## Manuel d'utilisation 

> Gestion du projet avec *maven*

## Compilation

    make

## Execution 

    make run

## Générer la documentation au format html

    make doc

> accessible dans le dossier `calculator/apidocs`.


La structure du projet peut être consultée [ici](./documents/Structure.png).

## Extensions implémentées

1. **Syntaxe algébrique**

En premier temps, [`MxParser`](https://github.com/mariuszgromada/MathParser.org-mXparser) a été utilisé pour cette extension mais celle-ci ne prenait pas en compte
les vecteurs et donc les Ensembles n'étaient pas reconnus.  
De plus, `MxParser` évalue et calcule l'expression directement, cela avait pour conséquence de pouvoir se passer de tout ce qui a été fait auparavant.  
Nous avons donc enlevé cette librarie et décider d'utiliser l'algorithme shunting-yard afin de convertir une formule infixe en RPN.  
Par manque de temps, nous ne disposons pas d'un vrai lexer/parser pour analyser les entrées des utilisateurs.

**Il est donc important de mettre des espaces entre chaque parenthèses, opérandes et opérateurs.**  
Sinon l'expression ne sera pas reconnue.

2. **Rappel de valeurs**

*   `showVar` : affiche les variables enregistrées.

*   `!x` : attribue à *x* la valeur du sommet de la pile (tête de l'historique en ALG).

*   `?x` : vaut la valeur contenue dans *x*.

*   `showHist` : affiche l'historique.

*   `showStack` : affiche la pile (seulement en RPN).

*   `hist(i)` : empile la valeur à l'index *i* de l'historique en mode RPN; ajoute en tête de l'historique sinon.

*   `pile(i)` : empile la valeur à l'index *i* de la pile (seulement en RPN).



3. **Calculatrice multi-types**

Entier - Décimaux - Booleens - Ensembles

## Les différents types et leurs syntaxes ainsi que leurs opérations associées

### Type ensemble :

#### Syntaxe 

    {i1;i2;...;in}


#### Opérations

*   `U` : *{i1;i2;...;in} U {j1;j2;...;jm}* retourne l'union des deux ensembles sans répétition (d'arité 2)

*   `I` : *{i1;i2;...;in} I {j1;j2;...;jm}* retourne l'intersection des deux ensembles (d'arité 2)

*   `S` : *{i1;i2;...;in} I {j1;j2;...;jm}* retourne l'ensemble 1 dont tous les éléments de l'ensemble 2 ont été supprimés (d'arité 2)

### Type booléen :

#### Syntaxe 

    true
    false


#### Opérations

*   `or` , `and` , `not` , `xor` : opérations habituelles avec leur arité habituelle 


### Type Entier :

#### Syntaxe 

    Nombres dans Z

#### Opérations

*   `+`, `*`, `-`, `/`, `^`, `!` : opérations habituelles avec leur arité habituelle 


### Type Décimal :

#### Syntaxe 

    Nombres dans R

#### Opérations

*   `+`, `*`, `-`, `/`, `^` : opérations habituelles avec leur arité habituelle 


Des exemples sont disponibles [ici](./documents/examples.md)