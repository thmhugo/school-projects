Modalités de rendu du projet et évaluations
===========================================

Bonjour à tous,

Ce message est en trois parties. Compte tenu de la situation sanitaire,
nous avons pris la décision d'évaluer vos projets sans organiser de
soutenances. Vos projets seront téléchargés, compilés, exécutés,
testés et, bien évidemment, relus.


### Modalités de rendu ###

La date limite de rendu est fixée au 11/01/2021.
Le lendemain, nous téléchargerons une copie de vos dépôts,
en l'état, cette version étant considérée comme définitive.

Une remarque importante : assurez-vous que votre rendu intègre bien
les toutes dernières modifications apportées au dépôt original, via
la commande "git pull prof master".


### Rapport.txt ###

Un fichier texte Rapport.txt (ou Rapport.md) à la racine de votre dépôt devra nous
fournir toutes les informations spécifiées ci-dessous, clairement
séparées en sections distinctes. Ce rapport n'est pas une simple
formalité : il sera pris en compte dans l'évaluation.

1. (Identifiants)
   Commencez par mentionner, pour chacun des membres du groupe :
   nom, prénom, identifiant sur GitLab, numéro d'étudiant.

2. (Fonctionnalités)
   Donnez une description précise des fonctionnalités implémentées
   par votre rendu - sujet minimal, extensions éventuelles,
   éventuellement parties non réalisées ou non encore fonctionnelles.

3. (Compilation et exécution)
   Documentez ensuite de façon précise la manière dont votre
   projet doit être compilé et exécuté. Précisez si vous vous êtes
   servi de bibliothèques externes, et donnez dans ce cas un pointeur
   vers leur documentation.

4. (Découpage modulaire)
   Donnez une description des traitements pris en charge par chaque
   module (.ml) de votre projet. Précisez le rôle et la nécessité
   de chaque module ajouté au dépôt initial.

5. (Organisation du travail)
   Cette partie est plus libre dans sa forme. Indiquez la manière
   dont les tâches ont été réparties entre les membres du groupe
   au cours du temps. Donnez une brève chronologie de votre travail
   sur ce projet au cours de ce semestre, avant et après le
   confinement.

6. (Misc)
   Cette partie est entièrement libre : remarques, suggestions,
   questions...


### Critères d'évaluation ###

Cette grille n'est donnée qu'a titre indicatif. Elle récapitule tout
ce que nous attendrions d'un projet "parfait" - faites simplement au
mieux pour vous rapprocher de ce projet idéal.

(1) Rapport.txt (ou Rapport.md) complet et bien rédigé.

(2) Usage approprié et régulier de GitLab. Commits (ou branches et
    merge-requests) correspondant à des tâches précises et effectués
    par la personne ayant effectué la tâche, équilibrage de l'usage
    entre les membres du groupe.

(3) Découpage du code en modules consistant. Un rôle précis attribué
    à chaque module, masquage approprié des éléments utilitaires
    dans les .mli.

(4) Projet minimal pleinement et correctement réalisé, et de manière
    flexible.

(5) Choix de nouveaux types ou de nouveaux modules justifiés et
    appropriés.

(6) Écriture fonctionnelle pure privilégiée. Éviter autant que possible
    les références, tableaux, boucles et enregistrements à champs mutables.
    Justification précise de chaque usage restant de ces éléments
    impératifs, via un commentaire détaillant pourquoi une solution
    fonctionnelle pure était impossible ou trop lourde à cet endroit.

(7) Qualité de l'écriture fonctionnelle. Usage approprié de fonctions
    d'ordre supérieur (les "fonctionnelles"), de la récurrence terminale,
    des fonctions de la bibliothèque standard.

(8) Qualité d'écriture du code. Pas de redondances, aucun copier-coller,
    code bien factorisé, choix de noms de fonctions parlants.

(9) Code pleinement nettoyé, commenté et mis en page lors du rendu final.
    Pas de code résiduel laissé en commentaire. Des lignes limitées à 80
    caractères. Des commentaires concis mais suffisants, souvent mais pas
    trop.

(10) Des extensions pertinentes et intéressantes - ce point est moins
    important que les précédents.

Sur le point (6) : l'invocation des fonctions de la librairie Graphics et
certaines opérations d'entrée-sortie peut nécessiter localement l'usage
du type unit, mais toutes les fonctions de votre programme peuvent être
écrites en renvoyant une valeur d'un type autre que unit - même celles du
module Turtle.

Sur les points (8) et (9), rappelons-le : nous relirons (très
attentivement) votre code. Facilitez au mieux cette lecture.

Cordialement,
L'équipe pédagogique de pf5.
