# Architecture

## Fichiers

Bien que cela rende notre programme moins modulaire et extensible, 
les commandes ne sont pas compilées sous formes d'exécutables indépendants, notamment pour faciliter
le passage de paramètres via l'utilisation de *struct*, et également dans l'objectif de factoriser 
les codes d'erreurs et le *parsing* des lignes de commandes.

### src/

Ce dossier regroupe les fichiers sources (.c) et le dossier *inc/*.

*   cattar.c : Contient l'implémentation de la commande `cat` pour les tarballs.
*   cd.c : Contient l'implémentation de la commande `cd`  pour les tarballs.
*   commons_tar.c : Regroupe des fonctions de gestion des tarballs (préfixées par *commons_tar_*).
*   commons_tsh.c : Regroupe des fonctions de gestion de *tsh* (préfixées par *commons_tsh_*)
*   commons.c : Regroupe les définitions de macros et de constantes.
*   cptar.c : Contient l'implémentaion de la commande `cp` pour les tarballs.
*   lstar.c : Contient l'implémentation de la commande `ls` pour les tarballs.
*   mkdirtar.c : Contient l'implémentation de la commande `mkdir` pour les tarballs.
*   mvtar.c : Contient l'implémentation de `mv` pour les tarballs.
*   parser.c : *Parse* les lignes de commandes.
*   redirection.c : Regroupe les fonctions de gestion de redirection des flux.
*   rmdirtar.c : Contient l'implémentation de la commande `rmdir` pour les tarballs.
*   rmtar.c : Contient l'implémentation de la commande `rm` pour les tarballs.
*   tar.c : Regroupe les fonctions de mise à jour et vérification du *checksum* d'un header.
*   tsh.c : Contient le *main*, gère les entrées/sorties utilisateur et l'execution des lignes de commandes.

### src/inc/

Ce dossier regroupe les fichiers d'entête (.h) des fichiers sources.

*  ulhash.h : Contient une implémentation d'une [hash table](http://troydhanson.github.com/uthash/).
*  utlist.h : Contient une implémentation d'une [linked list](https://troydhanson.github.io/uthash/utlist.html).

### test/

Regroupe les fichiers de tests unitaires, portant le même nom que leur fichier source 
associé et prefixé par *test_*.   
Les fichiers au nom suffixé par *_aux* 
contiennent les méthodes auxiliaires de test.

### test/inc

Ce dossier regroupe les fichiers d'entête des fichiers de tests.

### test/resources/

Ce dossier regroupe les resources nécessaires aux tests.


### build/

Ce dossier regroupe les fichiers exécutables `test_runner` et `tsh`

