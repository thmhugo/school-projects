[![pipeline status](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/badges/master/pipeline.svg)](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/commits/master)

# Projet système

## `tsh`, un shell pour les tarballs

Le but du projet est de faire tourner un shell qui permet à
l'utilisateur de traiter les tarballs comme s'il s'agissait de
répertoires, **sans que les tarballs ne soient désarchivés**.

## Documentation

Les fichiers liés à la documentation se trouvent dans le repértoire *doc/* contenant:

* [*algorithms.md*](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/blob/master/doc/algorithms.md) contenant les **premières esquisses** des algorithmes utilisés pour implémenter les commandes.
* [*ARCHITECTURE.md*](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/blob/master/doc/ARCHITECTURE.md) décrivant l'architecture logicielle du projet.
* [*AUTHORS.md*](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/blob/master/doc/AUTHORS.md) fournissant les coordonnées des différents membres de l'équipes.
* [*command-list.md*](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/blob/master/doc/commands-list.md) rassemblant l'ensemble des commandes testées et fonctionnelles pour `tsh`.
* [*guideline.md*](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/blob/master/doc/guideline.md) contenant nos traces de recherches et nos premières idées concernant le développement de `tsh`.
* [*user-manual.md*](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/blob/master/doc/user-manual.md) fournissant un mode d'emploi du projet.

L'énoncé du projet se trouve [ici](https://gaufre.informatique.univ-paris-diderot.fr/klimann/systL3_2020-2021/blob/master/Projet/README.md).

### `Doxygen`

Possibilité de générer de la documentation avec
[`doxygen`](https://github.com/doxygen/doxygen).

Pour cela, exécuter simplement :

```make
make doc
```

La documentation au format HTML se trouvera alors dans le repértoire *./doc/doxygen*.

## Organisation

Le contenu de nos différents sprints se trouvent dans l'onglet [*milestones*](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/tsh/milestones?sort=due_date_desc&state=closed).

## Fonctionnalités implémentées

* les commandes `cd`, `exit` et `pwd`.  les commandes sans option dans un
* *tarball* `mkdir`, `rmdir` et `mv`.  les commandes `cp` et `rm` avec
* l'option `-r` lorsque les chemins impliquent des *tarballs* la commande
* `ls` avec l'option `-l` dans les *tarballs*.  la commande `cat` sans option
* dans les *tarballs*.  les combinaisons de commandes avec `|`.  toutes les
* redirection de flux standards dont les chemins n'impliquent pas de
* *tarball*.  les redirections liées aux symboles `>` , `<` et `2>` quand les
* chemins impliquent des *tarballs*.

## ... et les manquantes

* les redirections `>>` et `2>>` lorsque le chemin de destination implique
* des *tarballs*.  la commande `ls` dont les arguments impliquent à la fois
* des chemins externes à des *tarballs*
    et des chemins internes (telle que `ls outside_dest inside_dest`) ont des
    problèmes d'affichage.  (ici la commande n'affichera pas les noms des deux
    dossiers comme l'aurait fait `ls` avec deux chemins exclusivement internes
    ou externes).
* les commandes externes lancées à l'intérieur d'un *tarball* peuvent
* directement (*i.e.* sans `../` dans le chemin des arguments) accéder aux
* fichiers à l'intérieur du dossier actuel au sens de `bash`
    (choix d'implémentation).

