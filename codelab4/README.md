# CODELAB 4 - L'aventure

## Présentation

`CodeLab4 - L'aventure` est un logiciel ayant comme objectif de s'initier à la programmation de
façon ludique et ergonomique, en effet, les problèmes à résoudre sont
camouflés par un scénario permettant l'immersion de l'utilisateur.

L'utilisateur va devoir aider un naufragé à récupérer des ressources pour pouvoir construire un
radeau et retrouver la civilisation.
Mais attention ! Il ne devra pas mourir de faim ni se faire surprendre par une tempête.

`CodeLab4 - L'aventure` est construit autour d'un langage simplifié permettant de se familiariser avec
les conditions ainsi que les boucles.
De façon textuel ou bien graphique l'utilisateur doit écrire un ensemble
d'instructions permettant au fur et à mesure de l'aventure de remplir des quêtes.
Une partie est divisée en plusieurs chapitres de plus en plus coriace à résoudre.

## Rapport

Le fichier PDF du rapport est disponible ici : [./documentation/project-report.pdf](https://gaufre.informatique.univ-paris-diderot.fr/sangnier/codelab4/blob/master/documentation/project-report.pdf)

## Compilation et exécution

### Installation de `JavaFX`

1. Téléchargez un [runtime JavaFX](https://gluonhq.com/products/javafx/) approprié à votre système d'exploitation et décompressez-le à l'endroit souhaité.

2. Ajoutez la variable d'environement **PATH_TO_FX** correspondant au chemin vers `JavaFX`.
Le plus simple est de rajouter à la fin du fichier de configuration de votre terminal :
```
PATH_TO_FX="path/to/javafx-sdk/lib"
```

3. Relancez votre terminal !

### Commandes

Pour compiler `CodeLab4 - L'aventure` il suffit d'exécuter la commande
suivante :

	make

Pour lancer le `Launcher` :

    make run

## Architecture

### *documentation*

Contient :  

* *BUILD.md* contient toutes les commandes du *Makefile* ainsi que les dépendences du projet.
* *specification.pdf* est le cahier des charges expliquant ce qu'est censé être `CodeLab4 - L'aventure`.
* *quests.pdf* est la description précises des différents chapitres ainsi que leurs quêtes.
* *UML-diagram.png* est la représentation en diagramme de l'architecture du projet.
* Toutes les resources nécessaires à la documentation du projet.

### *lib*

Ce dossier contient les archives `JARs` des différentes librairies utilisées
pour le projet :

* `JUnit 4`
* `CommonsIO 2`
* `JSON`
* `JavaFX 14` (for GitLab-CI)

### *src*

Contient :  

* Le *package* **codelab4** lui même divisé en *subpackages*.
* Le dossier *resources* contenant toutes les resources utilisées par les
  classes `Java` (fichiers `FXML` pour les vues, images `PNGs`, feuilles de
style `CSS`, etc...).

### *test*

Est divisé en *package* et *subpackages* comme *src* et contient toutes les
classes de tests `JUnit`.

### *build*

Contient les classes `Java` dans le sous dossier *classes* ainsi que l'archive `JAR` du projet.

## Informations à propos des membres de l'équipe 4

Antoine LIU \
Benjamin NICOLAS \
Emile ROLLEY \
Hugo THOMAS

