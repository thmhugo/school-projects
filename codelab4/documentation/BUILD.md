# How to build `CodeLab4 - L'aventure` ?

## Requirements

- Java JDK (>= 11.0)
- JavaFX (>= 11.0)

### Installation de JavaFx

1. Téléchargez un [runtime JavaFX](https://gluonhq.com/products/javafx/) approprié à votre système d'exploitation et décompressez-le à l'endroit souhaité.

2. Ajoutez la variable d'environement **PATH_TO_FX** correspondant au chemin vers `JavaFX`.
Le plus simple est de rajouter à la fin du fichier de configuration de votre terminal :
```
PATH_TO_FX="path/to/javafx-sdk/lib"
```

3. Relancez votre terminal !

## Commands

Makefile commands :
* `compile` will compile `Java` files into *build/classes/*.
* `package` will create a `JAR` file in *build/*.
* `verify` will compile and run `JUnit` tests.
* `run` after `package` will run the project.
* `clean` will removes *build* folder.
