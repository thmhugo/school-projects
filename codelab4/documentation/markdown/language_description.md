# Description du Langage

> Pas à jour !

## Actions

* tourner_a_gauche(): oriente le personnage vers sa gauche.
* tourner_a_droite(): oriente le personnage vers sa droite.
* avancer(): bouge le personnage sur la case en face de lui.


## Actions d'interaction:

* manger(): consomme un fruit de son inventaire.
* ramasser(): ramasse l'objet (fruit, branche) situé dans la case en face de lui.
* couper(): récupère une buche d'un arbre situé dans la case en face de lui.
* jeter_fruit(): supprime le dernier fruit *stacké* dans l'inventaire.


## Boucles:

* while condition do [...]: répète les actions tant que la condition est vraie.
* if condition then [...] else [...] : exécute les actions si la condition est vraie.

> A noter : une ligne "if" se termine par "then" et une ligne "while" se termine par "do"

> Les fins de boucles sont marquées par l'indentation (similaire à Python) . Voir exemples ci-dessous

## Conditions:

* est_libre(): est vraie si la case en face ne contient pas d'objet.
* est_pourri(): est vraie si le fruit est pourri.

# Exemple de programme

## if [...] then [...]

    if est_libre() then
        avancer()

*Le personnage avance si le chemin devant lui est libre*


## if [...] then [...] else [...]

    if est_libre() then
        avancer()
    else
        ramasser()

*Le personnage avance si le chemin devant lui est libre, sinon il ramasse l'objet situé devant lui*


## while [...] do [...]

    while est_libre() do
        avancer()

*Le personnage avance tant que le chemin devant lui est libre*


## Exemple d'imbrication 1

    while est_libre() do
        avancer()
    ramasser()

*Le personnage avance tant que le chemin devant lui est libre, dès que la case devant est occupée, il ramasse l'objet situé dans cette case*

