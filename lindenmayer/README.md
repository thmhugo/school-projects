Projet PF5 2020 : L-systèmes
============================

# Rapport THOMAS - LIU

1. [Fonctionnalités](#fonctionnalités)
    1. [Sujet minimal](#sujet-minimal)
    2. [Extensions](#extensions)
2. [Compilation et exécution](#compilation-et-exécution)
3. [Découpage modulaire](#découpage-modulaire)
4. [Organisation du travail](#organisation-du-travail)
5. [Misc](#misc)
    1. [Installation de Cairo2](#installation-de-Cairo2)
    2. [Quelques exemples de commandes](#quelques-exemples-de-commandes)


## Fonctionnalités

### Sujet minimal 

Voir [CONSIGNES.md](CONSIGNES.md)

1. Un L-Système et son interprétation peuvent être chargés à partir d'un fichier dont
le format est celui des fichiers du sous-répertoire *examples/*.  
2. L'interprétation se fait à la volée et sans mise en mémoire de
celle-ci.  
3. Pour ce qui est d'afficher les itérations, l'utilisateur peut à sa guise, soit ajouter
soit enlever une itération (0 étant la limite minimal) via les touches du clavier `a` et `z` respectivement.  
4. L'échelle du dessin est calculée automatiquement, évitant ainsi à l'utilisateur
de devoir redimensionner la fenêtre.  

### Extensions
1. **Sauvegarder le résultat visuel d'une itération**  
L'utilisateur a la possibilité de sauvegarder le résultat visuel d'une itération
en un fichier d'image dans le format *png*. Pour pouvoir l'enregistrer, l'utilisateur doit simplement appuyer sur la touche `s` et le fichier se trouvera dans le dossier *projet/* sous le nom de "turtle.png".
2. **Figures en couleurs**  
Le langage de la Tortue a été étendu afin de pouvoir crée des figures en couleurs. Il suffit d'associer à un symbole la lettre C suivie du code rbg de la couleur (exemple : B C153,102,51; au symbole *B* est associé le changement de couleur pour 153,102,51 : du marron). Les traits seront alors de cette couleur (cf [br1colored.sys](examples/br1colored.sys)).  
Il est également possible de générer des figures avec un dégradé de 
couleurs : il suffit de mettre l'option ``--gradient`` (``-g``) lors du lancement. Il est possible de modifier les couleurs de départ et de fin du dégradé via ``--from`` et ``--to`` avec un paramètre de la forme *r,b,g* (facultatif : il y a des couleurs par défaut). Note : l'utilisation de l'option ``--gradient`` écrase les couleurs données dans le fichier *.sys* d'un L-Système (exemple : ``./run --load examples/br1colored.sys -g`` est équivalent à ``./run --load examples/br1.sys -g``)
3. **Variations de l'angle**  
Il est possible de faire varier l'angle de la figure en temps réel en utilisant les touches `e` et `r` qui font tourner le dessin à gauche ou à droite respectivement. La touche `t` permet de réinitialiser l'angle et de retrouver la figure de base de l'itération en cours.
4. **Graphique en trois dimensions**  
Extension non traitée.


## Compilation et exécution
Par défaut, `̀make` est seulement utilisé pour abréger les commandes dune (voir Makefile pour plus de détails):  
``make`` sans argument lancera la compilation ``dune`` de main.exe,
c'est-à-dire votre programme en code natif.  
``make byte`` compilera si besoin en bytecode, utile pour faire
tourner votre code dans un toplevel OCaml, voir lsystems.top.  
``make clean`` pour effacer le répertoire provisoire _build
produit par dune lors de ses compilations.  


Il sera nécessaire d'installer *Cairo2* afin de pouvoir sauvegarder les images au format PNG.   
Vous pouvez consulter la librarie [ici](https://github.com/Chris00/ocaml-cairo) et savoir comment installer *Cairo2* [ici](#installation-de-Cairo2).

Notre projet utilise la version **2.7** de dune.  
Il doit être compilé à l'aide de la commande ``make``, exécuté via
``./run``.  
Par défaut, lorsque le programme est exécuté sans option, le L-Système "arrow" ([br3.sys](examples/br3.sys) implémenté dans [examples.ml](./examples.ml)) sera choisi.  
Sinon l'utilisateur peut choisir un autre L-Système (des exemples dans le sous-répertoire *examples/*) en chargeant un fichier *sys* via l'option ``--load chemin_du_fichier.sys``.  
Rappel: l'option  ``--gradient`` (``-g``) permet de générer un dégradé de couleur sur la figure et ``--from`` et ``--to`` modifient les couleurs de départ et de fin du dégradé respectivement (cf [exemples](#quelques-exemples-de-commandes)).  
L'utilisateur peut retrouver la liste des options grâce à ``./run --what`` et la liste des touches utilisables via ``./run --keys``.

## Découpage modulaire
* ``examples.ml`` contient des exemples de L-Systèmes.
* ``fileManager.ml`` s'occupe de charger des fichiers *sys* et de les convertir en donnée utilisable par le reste des autres modules. Il sauvegarde aussi l'itération actuelle en une image *PNG*. Ce module est solicité à chaque fois qu'un fichier externe doit être utilisé ou créé.
* ``main.ml`` parse ce que l'utilisateur écrit et lance le programme avec l'/les option(s) associée(s).
* ``systems.ml`` contient les fonctions d'itération et d'interprétation d'un L-Système et les fonctions servant au *bornage* du dessin 
* ``turtles.ml`` contient toutes les commandes associées à la Tortue.

## Organisation du travail
* Mi Novembre - Début Décembre: 
    * Prise en main du sujet 
    * Implémentation des actions de la tortue.
    * Implémentation du parcours d'une itération avec mise en mémoire. 
* Début Décembre: 
    * Mise en place de l'interprétation à la volée et échelle du dessin adaptée automatiquement.
    * Gestion des couleurs par la Tortue et pouvoir charger un L-Système et l'utiliser.
    * Sauvegarder une itération en une image *PNG* et pouvoir ajuster l'angle du dessin.
    * Harmonisation du code.
* Mi Décembre: Relecture du code et rédaction du rapport.

Hugo s'est occupé de la Tortue (déplacements, stockage, couleur),de l'adaptation de la taille du dessin et de la sauvegarde d'une itération en image png.  
Antoine s'est occupé de l'interprétation à la volée des itérations, du chargement des fichiers *sys* et de l'ajustement de l'angle du dessin.


## Misc

### Installation de Cairo2

    > opam install cairo2

il faudra peut-être ajouter des packets supplémentaires :

    > apt install libcairo2-dev

Vérifier le chemin d'installation via 

    > ocamlfind query cairo2

Si à l'exécution de `./run`, cairo2 n'est pas trouvé, il faudra remplacer le chemin de la première ligne 
du fichier **dune** (copy_files# PATH/cairo.ml) par le chemin donné par `ocamlfind query cairo2`.

### Quelques exemples de commandes 

Voir les touches utiles et leur action associée :

    > ./run --keys

Chargement du fichier [`examples/snow.sys`](examples/snow.sys) avec un dégradé utilisant les couleurs par défaut (bleu vers violet) : 

    > ./run --load examples/snow.sys --gradient

Execution sur le L-Système par défaut (arrow), avec un dégradé dont la couleur de départ (du bas) est r=1,g=206,b=66 (du vert) : 

    > ./run -g --from 1,206,66

Chargement du fichier [`examples/htree.sys`](examples/htree.sys), avec un dégradé du jaune vers le rouge :

    > ./run --load examples/htree.sys --gradient --from 255,254,0 --to 255,0,0  





  
