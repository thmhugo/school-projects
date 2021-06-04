---
header-includes:
- |
  ```{=latex}
  \usepackage{amsmath}
  \usepackage[dvipsnames]{xcolor}
  \def\term #1{{\color{BlueViolet}\texttt{#1}}}
  \def\spe #1{{\color{Gray}\texttt{#1}}}
  ```
title: Rapport de projet $:$ $\textsc{Honu}$
author:
- Emile $\textsc{Rolley}$
- Hugo $\textsc{Thomas}$
date: 2020/2021
abstract: "Ce document regroupe toutes les informations nécessaires à la compréhension de notre projet réalisé dans le cadre du cours *Grammaires et Analyse Syntaxique*. A savoir, la description des extensions réalisées, les instructions permettant la compilation, l'exécution et l'utilisation de `Honu` et un résumé de notre organisation."
documentclass: report
dpi: 160
wrap: auto
toc: true
toc-depth: 5
shift-heading-level-by: -1
linkcolor: blue
colorlinks: true
urlcolor: blue
geometry:
- top=27mm
- bottom=27mm
- left=27mm
- right=27mm
---

\newpage

## Présentation

Le projet consiste en l'implémentation d'un interpréteur pour un langage de
création d'images basé sur le principe des *turtle graphics*, dans lequel,
une suite d'instructions, permettant de déplacer un curseur (ou tortue) sur un
canevas, est interprétée pour produire une image.

Nous avons choisi d'aller plus loin et de construire un ensemble d'outils en
plus de l'interpréteur demandé.
En effet, `Honu` est un langage de programmation strict et dynamiquement typé.
Mais également un programme permettant :

* l'interprétation de fichier `.hn` (`Honu`).
* l'utilisation d'un [mode interactif](#mode-interactif) pour l'interpréteur.
* la [compilation](#compilation) de fichier `.hn` vers différentes cibles.
* la [génération de documentation](#génération-de-documentation) pour les fichier `.hn`.

Le langage comprend également la [coloration syntaxique](#coloration-syntaxique)
pour l'IDE `Visual Studio Code` et l'éditeur de texte `Vim`.

> **Remarque** : Des exemples d'images produites avec
> `Honu` sont disponibles dans le fichier
> _[./doc/previews.pdf](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/blob/master/doc/previews.pdf)_.

## Organisation

Durant le développement du projet, nous avons suivi la méthode agile
[*scrum*](https://en.wikipedia.org/wiki/Scrum_%28software_development%29) simplifiée.
En effet, travaillant en binôme, nous n'avions pas de *product owner* ni de *scrum master*.

Cependant, nous avons gardé le système de *sprint* et de *product backlog*.
Chaque *sprint*, d'une durée de une semaine, se soldait d'une *sprint review* sur `Discord`
lors de laquelle nous faisions le point sur les *issues* terminées et celles à faire
pour le *sprint* suivant.
De plus, nous avons ainsi pu travailler chacun sur toutes les différentes parties du projets.

Hugo c'est occupé de :

- l'implémentation de la première version de l'interpréteur (MR [!2](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/2)).
- l'ajout des expressions arithmétiques dans le langage (MR [!3](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/3)).
- l'ajout de nouvelles instructions dans le langage (MR [!6](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/6)).
- l'ajout de la compilation vers `PNG` et `JPEG` (MR [!6](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/6)).
- l'ajout de l'instruction `if ... then ...` (MR [!9](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/9)).
- l'ajout de nouveaux types (MR [!10](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/10)).
- la résolution de différents *bugs*
 (MR [!14](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/14)
 [!15](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/15)
 [!17](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/17)).

Emile quant à lui s'est occupé de :

- mettre en place l'architecture du projet et `GitLab-CI`
 (MR [!1](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/1)
 [!4](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/4)).
- l'ajout des structures de contrôle (MR [!5](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/5)).
- la mise en place de la syntaxe abrégée (MR [!7](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/7)).
- l'ajout des procédures dans le langage (MR [!8](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/8)).
- l'ajout du mode intéractif (MR [!11](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/11)).
- l'ajout de la compilation vers `OCaml` (MR [!18](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/18)).
- l'ajout de la compilation de la documentation (MR [!22](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/22)).
- la résolution de différents *bugs*
 (MR [!12](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/12)
 [!13](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/13)
 [!16](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/merge_requests/16)).

## La grammaire

Étant donné les [extensions](#les-fonctionnalités) qui ont été réalisées, la
grammaire initiale a été enrichie (voir Figure \ref{fig:grammar1} et
\ref{fig:grammar2}).
Malgré cela, le langage initial reste reconnaissable.

\begin{figure}
\centering
\begin{alignat*}{2}
S ::=& \; P^{*} \; D^{*} \; I & &\text{programme (\textit{axiome})}\\
S' ::=& \; P^{*} \; D^{*} \; I^{*} & &\text{programme pour le mode intéractif (\textit{axiome})}\\
P ::=& \; (\term{Proc} \; |\; \term{proc}) \; \; id \; \term{(} \; A \; \term{)} \; \term{=} \; I & \;\;\; &\text{déclaration d'une procédure}\\
A ::=& \; [ \; (id \term{,})^{*} \; id \; ]  & &\text{arguments d'une procédure}\\
D ::=& \; (\term{Var} \; |\; \term{var}) \; id \; \term{;} & &\text{déclaration d'une variable}\\
I ::=& \; (\term{Avance} \; | \; \term{forward}) \; E & &\text{fait avancer la tortue}\\
&| \; (\term{Tourne} \; | \; \term{turn}) \; E & &\text{fait tourner la tortue}\\
&| \; (\term{ChangeCouleur} \; | \; \term{color}) \; E & &\text{change la couleur du crayon}\\
&| \; (\term{ChangeEpaisseur} \; | \; \term{size}) \; E & &\text{change la taille du crayon}\\
&| \; (\term{Affiche} \; | \; \term{print}) \; E & &\text{écrit sur la sortie standard}\\
&| \; (\term{Ecrit} \; | \; \term{write}) \; E & &\text{écrit sur la fenêtre \texttt{Graphics}}\\
&| \; (\term{BasPinceau} \; | \; \term{down}) \; & &\text{abaisse le crayon}\\
&| \; (\term{HautPinceau} \; | \; \term{down}) \; & &\text{élève le crayon}\\
&| \; (\term{Depile} \; | \; \term{pop}) \; & &\text{dépile l'état de la tortue}\\
&| \; (\term{Empile} \; | \; \term{push}) \; & &\text{empile l'état courant de la tortue}\\
&| \; id \; \term{=} \; E & &\text{affectation d'une variable}\\
&| \; id \; \term{+=} \; E & &\text{affectation après addition}\\
&| \; id \; \term{-=} \; E & &\text{affectation après soustraction}\\
&| \; id \; \term{(} \; A' \; \term{)} & &\text{appel d'une procédure}\\
&| \; (\term{Debut} \; | \; \term{\{}) \; Y \; (\term{Fin} \; | \; \term{\}}) & &\text{bloc d'instructions}\\
&| \; (\term{Si} \; | \; \term{if}) \; E \; (\term{Alors} \; | \; \term{then}) \; I \;  [\;(\term{Sinon} \; | \; \term{else}) \; I \;] & &\text{branchement conditionnel}\\
&| \; (\term{Tant que} \; | \; \term{while}) \; E \; (\term{Faire} \; | \; \term{do}) \; I & &\text{boucle \textit{while}}\\
A' ::=& \; [ \; (E \term{,})^{*} \; E \; ]  & &\text{arguments de l'appel d'une procédure}\\
Y ::=& \; [ \; I \term{;} \; Y \; ] \; & &\text{ensemble d'instructions}\\
\end{alignat*}
\caption{Première partie de la grammaire.}
\label{fig:grammar1}
\end{figure}
\begin{figure}
\centering
\begin{alignat*}{2}
E ::=& \; n & &\text{entier}\\
&| \; f & &\text{réel}\\
&| \; s & &\text{chaîne de caractères}\\
&| \; c & &\text{couleur (\texttt{Graphics.color})}\\
&| \; id & &\text{nom de variable}\\
&| \;(\term{vrai} \; | \; \term{faux} \; | \; \term{true} \; | \; \term{false}) \;\;\; & &\text{booléens}\\
&| \; \term{(} \; E \; \term{)} & &\text{expression parenthésée}\\
&| \; U \; E & &\text{opérateur unaire}\\
&| \; E \; B \; E & &\text{opérateur binaire}\\
U ::=& \; \term{-} & &\text{opposé}\\
|& \; (\term{Non} \; | \; \term{not}) & &\text{négation logique}\\
B ::=& \; \term{*} & &\text{multiplication}\\
|& \; \term{/} & &\text{division}\\
|& \; \term{+} & &\text{addition}\\
|& \; \term{-} & &\text{soustraction}\\
|& \; \term{\&} & &\text{conjonction}\\
|& \; \term{|} & &\text{disjonction}\\
|& \; \term{==} & &\text{égalité structurelle}\\
|& \; \term{!=} & &\text{inégalité structurelle}\\
|& \; \term{<} & &\text{strictement inférieur}\\
|& \; \term{>} & &\text{strictement supérieur}\\
|& \; \term{<=} & &\text{inférieur}\\
|& \; \term{>=} & &\text{supérieur}\\
\end{alignat*}
\caption{Deuxième partie de la grammaire.}
\label{fig:grammar2}
\end{figure}

Avec les définitions suivantes des jetons :
\begin{align*}
id =& \; \text{[a-z][a-zA-Z0-9]$^{*}$}\\
 n =& \; \text{[1-9][0-9]$^{*}$ | 0}\\
 f =& \; \text{[0-9]$^{+}$[.][0-9]$^{*}$}\\
 s =& \; \text{"[\^ \,"]$^{*}$"}\\
 c =& \; \text{(noir | black | blanc | white | rouge | red | vert | green | bleu | blue | jaune | yellow | magenta | cyan)}
\end{align*}

> **Remarque** : Toutes les lignes commençant par le caractères `#` sont ignorées.

## Les fonctionnalités

En plus des fonctionnalités de bases (sections 1 à 3 du
[sujet](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/blob/master/doc/subject.pdf)),
nous avons ajouté de nombreuses extensions.

> **Remarque** : pour l'implémentation de la boucle `while`, nous avons choisi de
> limiter le nombre d'itérations à 100 avant de considérer que la boucle est
> infinie et d'afficher un message erreur. De plus, si l'expression de la condition
> contient uniquement des consantes et qu'elle ne s'évalue pas à 0, la boucle
> est également considérée comme infinie.

### Extension du langage de base

Comme suggéré dans la quatrième section du
[sujet](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/blob/master/doc/subject.pdf),
nous avons étendu le langage de base avec de nouvelles instructions comme
`ChangeCouleur` ou `ChangeEpaisseur` ainsi que le branchement conditionnel `Si
... Alors ...`.  Mais nous avons voulu aller plus loin en rajoutant :

* une [syntaxe abrégée](#syntaxe-abrégée)
* des [instructions supplémentaires](#instructions-supplémentaires)
* des [opérateurs supplémentaires](#opérateurs-supplémentaires)
* des [types](#typage-dynamique)
* des [procédures](#procédures)

#### Syntaxe abrégée

Pour faciliter la rédaction et la lisibilité des fichiers de tests, nous avons
choisi d'ajouter une syntaxe alternative abrégée (voir [La grammaire](#la-grammaire)).

#### Instructions supplémentaires

En plus des instructions du langage de base, nous avons ajouté :

* `Ecrit | write` qui permet d'écrire sur le canevas d'interprétation.
* `Empile | push` qui permet d'empiler l'état courant de la tortue.
* `Depile | pop` qui permet de dépiler l'état courant de la tortue.
* `Affiche | print` qui permet d'écrire dans la console, et permettre ainsi de faciliter
le _debuggage_.
* `+=` qui est un sucre syntaxique pour l'addition suivie de l'assignation d'une variable.
* `-=` qui est un sucre syntaxique pour la soustraction suivie de l'assignation d'une variable.

#### Opérateurs supplémentaires

Nous avons également ajouté des opérateurs supplémentaires :

* `Non | not` la négation booléenne.
* `&` la conjonction booléenne.
* `|` la disjonction booléenne.
* `==` l'égalité structurelle.
* `!=` l'inégalité structurelle.
* `<`, `>`, `<=`, `>=` les comparateurs usuels sur les entiers et réels.

#### Procédures

Afin de permettre la factorisation du code nous avons ajouté la possibilité de
déclarer des procédures avant les déclarations de variables.

Exemple d'une déclaration de procédure : `proc procName(arg1, arg2, arg3) = { }`

Une procédure est définie par un identifiant, une liste de paramètres et une
instruction qui représente le corps de la procédure.

Le langage ne supporte pas l'application partielle, par conséquent une procédure doit
être appelée avec le nombre exact de paramètres.
Sinon le message d'erreur suivant est affiché :
`The procedure '<id>' expects <nb> arguments got <nb>`.

Lors de l'évaluation d'un appel de procédure, la liste d'association d'un paramètre
au résultat de l'évaluation d'une expression remplace les variables d'environnement
le temps d'évaluer le corps de la procédure.

Ainsi dans le corps d'une procédure, seuls les paramètres sont accessibles
et se comportent comme des variables.

#### Typage dynamique

Les variables peuvent contenir différents types de valeurs :

* des entiers
* des réels (la séparation entre la partie entière et décimale se fait à l'aide d'un point)
* des couleurs (correspondant à celles de [`Graphics.color`](https://ocaml.github.io/graphics/graphics/Graphics/index.html#some-predefined-colors))
* des booléens (`true`, `false`, `vrai`, `faux`)
* des chaînes de caractères délimitées par `"`.

La vérification de la correspondance entre les types des opérandes aux opérateurs
se fait lors l'évaluation d'une expression.
Il n'y a donc pas de passe de _type checking_.

Lors de l'évaluation, une expression peut contenir uniquement des opérandes du
même type à l'exception des entiers et réels qui peuvent être présent
conjointement.
Sinon, une exception est levée et le message d'erreur suivant est affiché :
`Illegal operation: <exp>`.

### Mode interactif

Grâce à la sous-commande [`repl`](#exécution), il est possible d'utiliser l'interpréteur
dans un mode intéractif.

L'utilisateur peut alors rentrer un programme `Honu` dont l'axiome de départ a
été légèrement modifié (voir Figure \ref{fig:grammar1}) afin de pouvoir déclarer des
variables ou des procédures sans avoir à terminer par une instruction comme la
grammaire de base l'imposait.

Pour la partie CLI, nous avons utilisé la librairie [`lambda-term`](https://github.com/ocaml-community/lambda-term)
mais nous avons gardé le même interpréteur que pour la sous-commande [`show`](#exécution)
en rajoutant seulement un nouveau point d'entrée pour le _parser_ (module `Parsing.Parser`).

#### Utilisation

Une fois le mode intéractif lancé, il est possible de :

* se déplacer dans le _prompt_ avec $\Leftarrow$ et $\Rightarrow$
* se déplacer dans l'historique avec $\Uparrow$ et $\Downarrow$
* appliquer l'_autocompletion_ avec `Tab`
* faire une recherche dans l'historique avec `Ctrl-R`
* fermer le mode intéractif avec `Ctrl-D`.

### Compilation

`Honu` vient avec un compilateur (sous-commande [`compile`](#exécution)), permettant
de générer des images au format `PNG` ou `JPEG`, ainsi que du code `OCaml` compilable
en code binaire grâce au script [`compgraph`](#compiler-un-programme-honu-vers-un-fichier-exécutable).

#### Vers `.jpg` et `.png`

Les images produites correspondent au contenu de la fenêtre graphique après l'interprétation
du programme `.hn` fourni.

#### Vers `.ml`

Quant aux fichiers `OCaml` produits, ce sont des programmes autonomes
comprenant une dépendance avec la librairie
[`Graphics`](https://ocaml.github.io/graphics/graphics/Graphics/index.html)
nécessaire pour l'affichage du résultat.

Au niveau de l'implémentation, le _parser_ (module `Parsing`)
reste inchangé mais la traduction est faite grâce au module `To_ocaml`.

##### Exemple

Par exemple pour le programme `Honu` :

```
    var i;
    {
        i = 0;
        down;
        while i < 3 do {
            if i == 1 then forward i
            else print i;
            i += 1;
        };
    }
  ```

le programme `OCaml` suivant est produit :

```ocaml
    open Graphics
    (* Program entry point. *)
    let () =
      (* Start of generated header. *)
      open_graph " 1000x1000";
      set_window_title "honu";
      set_line_width 1;
      set_color black;
      (* End of generated header. *)

      (* Start of translated Honu program. *)
      print_endline ("[ i ] => 0");
      moveto 400 200;
      lineto 400 201;
      print_endline ("[ i ] => 2");
      (* End of translated Honu program. *)

      (* Start of generated footer. *)
      try
        match (wait_next_event [ Key_pressed ]).key with
        | _ -> ()
      with
        | Graphics.Graphic_failure _ -> ()
      (* End of generated footer. *)
    ;;
```

### Génération de documentation

Il est possible de générer un fichier de documentation au format `Markdown` à partir des
commentaires d'un fichier `.hn`, grâce à la commande `doc`.

#### La grammaire

Pour ce faire nous avons implémenté un nouveau _parser_ (`Hdoc.Parser`)
correspondant à la grammaire suivante :
\begin{figure}
\centering
\begin{alignat*}{2}
D ::=& \; (T^{*} \; P)^{*} & &\text{documentation (\textit{axiome})}\\
T ::=& \; \term{\textasciitilde|} \; \term{@note} \; text^{*} \; \term{EOL} & &\text{note}\\
&| \; \term{\textasciitilde|} \; \term{@fixme} \; text^{*} \; \term{EOL} & &\text{ajoute une erreur à corriger}\\
&| \; \term{\textasciitilde|} \; \term{@todo} \; text^{*} \; \term{EOL} & &\text{ajoute une tâche à faire}\\
&| \; \term{\textasciitilde|} \; \term{@param} \; ( \; id \; text^{*} \; )^{*} \; \term{EOL} \;\;\; & &\text{fournit la description d'un argument ou d'une variable}\\
&| \; \term{\textasciitilde|} \; text^{*} \; \term{EOL} & &\text{simple ligne commentée}\\
P ::=& \; (\term{Proc} \; |\; \term{proc}) \; \; id \; \term{(} \; A \; \term{)} \; \term{=} \;\;\; & &\text{déclaration d'une procédure}\\
&| \; (\term{Var} \; |\; \term{var}) \; id \; \term{;} & &\text{déclaration d'une variable}\\
A ::=& \; [ \; (id \term{,})^{*} \; id \; ]  & &\text{arguments d'une procédure}\\
\end{alignat*}
\caption{Grammaire de la documentation compilable.}
\end{figure}

Avec les définitions suivantes :
\begin{align*}
id =& \; \text{[a-z][a-zA-Z0-9]$^{*}$}\\
text =& \; \text{[\^\,\textbackslash n \textbackslash r]$^{*}$}
\end{align*}
\newline

> **Remarque** : Tous les commentaires situés après les déclarations de variables
> doivent commencer par `#` au lieu de `~|`.

#### Exemple

Par exemple, à partir du code suivant :

```
      ~| Set the given color and size.
      ~|
      ~| @note See documentation for `color` and `size` for more information.
      ~|
      ~| @todo add `size s`.
      ~| @todo remove `"`.
      ~|
      ~| @param c Is the color to set.
      ~| @param s Is the size to set.
      proc setStyle(c, s) = {
        color "c";
      }
```

Le fichier `Markdown` suivant est généré :

~~~
      # `TestDoc` documentation

      ## `setStyle`

      Set the given color and size.

      > See documentation for `color` and `size` for more information.

      ### Parameters

      * `c`: Is the color to set.
      * `s`: Is the size to set.

      ```
      proc setStyle(c, s)
      ```

      ### Tasks to do

      * [ ] add `size s`.
      * [ ] remove `"`.
~~~

### Coloration syntaxique

La coloration syntaxique des fichiers `.hn` est disponible pour l'IDE `Visual
Studio Code` et pour l'éditeur de text `Vim`.

Pour `Visual Studio Code`, il suffit d'exécuter la commande `make vscode`.

Pour `Vim`, il faut copier le fichier _./syntax-highlight/vim/honu.vim_ dans le
dossier _$(VIMCONFIG)/syntax_.

## Utilisation

### Compilation

Le projet est compilable avec la commande `make`.
De plus, il est possible d'exécuter les tests unitaires avec `make tests`.

### Exécution

A l'issue de la compilation, un lien symbolique (`./honu`) vers l'exécutable est disponible,
et s'utilise de la façon suivante :

```bash
	./honu subcommand [arguments] [flags]
```

Les sous-commandes disponibles sont :

* `help`, qui affiche un message d'aide.
* `repl`, qui lance un mode interactif.
* `show FILENAME`, qui affiche dans une fenêtre le résultat de l'interprétation du fichier `FILENAME`.
* `compile FILEIN FILEOUT`, qui compile un programme `Honu` (`FILEIN`) dans un fichier `FILEOUT` dont le type est inféré à partir l'extension du fichier `FILEOUT`.
Les extensions supportées sont : `.ml`, `.jpg` et `.png` (extension par défaut si `FILEOUT` ne contient pas d'extension).
* `doc FILEIN FILEOUT`, qui compile la documentation d'un programme `Honu` (`FILEIN`) dans un fichier `Markdown` (`FILEOUT`).

Les *flags* disponibles sont :

* `--verbose`, qui permet l'affichage d'informations supplémentaires (par exemple l'AST construit).
* `--no-color`, qui permet de désactiver l'affichage en couleur dans le terminal.
* `--no-graphics`, qui permet de désactiver l'ouverture de la fenêtre graphique (uniquement disponible pour la sous-commande `show`).
* `--center`, qui permet d'utiliser une position de départ centrée plutôt que (0,0).

### Compiler un programme `Honu` vers un fichier exécutable

Afin de faciliter la compilation des fichiers `OCaml` produits par la sous-commande `compile`,
nous avons ajouté le script suivant :

```bash
./compgraph FILENAME
```

Où, `FILENAME` correspond au nom du fichier `OCaml` sans l'extension `.ml`.

Un fichier executable `FILNAME.exe` est alors généré.
