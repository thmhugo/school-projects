# Solutions Codelab 4

## Chapitre 1

### Q1

    avancer()

### Q2

    tourner_a_gauche()
    avancer()
    avancer()
    avancer()

### Q3

    tourner_a_droite()
    ramasser()

### Q4

    avancer()
    avancer()
    tourner_a_droite()
    avancer()
    avancer()
    tourner_a_gauche()
    ramasser()
    tourner_a_droite()
    avancer()
    ramasser()

&nbsp;


&nbsp;


&nbsp;


&nbsp;


&nbsp;


### Q5

    tourner_a_gauche()
    tourner_a_gauche()
    avancer()
    tourner_a_gauche()
    ramasser()

&nbsp;

## Chapitre 2

### Q1

    tourner_a_gauche()
    avancer()
    avancer()
    avancer()
    tourner_a_droite()
    ramasser()
    tourner_a_droite()
    tourner_a_droite()
    avancer()
    ramasser()
    avancer()
    avancer()
    tourner_a_gauche()
    ramasser()

### Q2 

    if fruit_est_pourri() then
        jeter_fruit()
    else
        manger_fruit()

> Cette solution est celle qui était pensée au début, mais `manger_fruit()` serait suffisant. Si le fruit est jeté, il faut relancer la quête.

&nbsp;


&nbsp;


&nbsp;


&nbsp;




## Chapitre 3

### Q1

    avancer()
    couper()
    tourner_a_gauche()
    tourner_a_gauche()
    while chemin_est_libre() do
        avancer()
        ramasser()
    couper()
    tourner_a_gauche()
    couper()

### Q2

    tourner_a_droite()
    tourner_a_droite()
    avancer()
    ramasser()
    tourner_a_gauche()
    ramasser()
    tourner_a_droite()
    avancer()
    tourner_a_droite()
    while chemin_est_libre() do
        avancer()
    ramasser()

### Q3

    if fruit_est_pourri() then
        jeter_fruit()
    else
        manger_fruit()
    avancer()
    avancer()
    avancer()
    tourner_a_gauche()
    avancer()
    avancer()
    avancer()


&nbsp;


> Cette quête ne fonctionne que si `manger_fruit()` fonctionne. Si le fruit est pourri, le joueur ne mange pas de fruit et n'a pas assez de nourriture pour terminer la partie. Il faut donc recommencer jusqu'à ce que `manger_fruit()`fasse manger un fruit.