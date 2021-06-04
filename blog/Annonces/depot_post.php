<?php
session_start();
require '../database.php';

$datetime = date("Y-m-d H:i:s");

 $req  = $bdd->prepare('INSERT INTO billets(auteur, titre, contenu, type, sujet, ville_annonce, date_creation) VALUES(?,?,?,?,?,?,?)');
 $req->execute(array($_SESSION['pseudo'],$_SESSION['titreAnnonce'],$_SESSION['messageAnnonce'],$_SESSION['typeAnnonce'],$_SESSION['sujetAnnonce'],$_SESSION['villeAnnonce'],$datetime ));
 header('Location:../Accueil/accueil.php');
 ?>
