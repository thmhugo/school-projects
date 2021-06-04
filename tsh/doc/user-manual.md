# Mode d'emploi

## Installation

Il existe deux moyens d'installer le projet via Docker:

* Avec le *Dockerfile* présent dans [doc/dockerfile](dockerfile/).
* Avec [hub.docker.com](https://hub.docker.com/r/antune/hea_tsh) en faisant un pull.

## Compilation

### Utilisation via le Dockerfile

    docker build -t tsh .
    docker run -ti tsh
    cd home/tsh
    make

### Utilisation via docker hub

    docker pull antune/hea_tsh
    docker run -ti antune/hea_tsh
    cd home/tsh
    git pull
    make

## Utilisation

### Exécuter `tsh`

    ./build/tsh
