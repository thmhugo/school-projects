# Product backlog

> Pas à jour !

# Implémentation des différents modules

*Note de difficulté de 1 à 10*

## Front - **5**

### Affichage de la carte/inventaire - 3 -

*   Afficher les images crées par le générateur de carte   -  1  -
*   Afficher l'état de l'inventaire, de la faim du joueur  -  2  -
*   Afficher la barre de faim du joueur					   -  2  -

### Editeur de programme : drag and drop/texte - 5 -

*   Editeur de texte avec mise en avant de la syntaxe   -  4  -
*   Ouverture de fichiers existant ( -> Gestionnaire de fichier)  -  2  -
*   Editeur de programme avec le drag and drop   -  5  -
*   Différents types d'enregistrement de fichier (sauvegarde auto(il faudra définir le type de sauvegarde auto : multi thread ou change listener), sauvegarder sous un fichier texte pour l'utilisateur, *utilisateur qui sauvegarde sans savoir où est enregisté le fichier, juste pour le récupérer a la prochaine connexion*)   -  3  -

### Launcher - 4 -

*   Créer une nouvelle partie   		-  2  -
*	Lancer une partie déja existante	-  2  -

### Fenêtre principale - 6 -

*   Aggrégation des différents modules de l'interface graphique - fenêtre principale   -  5   -
*   Fonctionnalitée pour changer la scène lors de l'animation (plein écran, ... )   -  5  -

## Back - **8**

### Gestionnaire de fichier - 5 -

*   Sauvegarde du programme						-  3  -
*   Ouverture d'un programme sauvegardé   		-  3  -
*   Sauvegarde de l'état de la partie   		-  4  -
*   Chargement de l'état de la partie   		-  4  -
*   Chargement d'un fichier *auto-enregistré*   -  3  -

### Parser - 9 -

* A partir de la chaîne de caractères provenant de l'éditeur, founrnir une suite d'instructions pour l'interpreteur   -  9  -

### Interpreteur - 10 -

* Interpréter le programme parsé pour créer les différents fichiers de génération de carte   -  10  -

### Générateur de cartes - 5 -

*   A partir des fichiers de génération de carte, générer les images pour l'animation   -  7   -
* A partir des différents images générées, créer une animation   -  7  -
