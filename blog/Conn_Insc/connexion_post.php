<?php
session_start();

function checkInput($var)
{
     $var= trim($var);
     $var= stripslashes($var);
     $var= htmlspecialchars($var);

     return $var;
}

$_SESSION['pseudo']=checkInput($_POST['pseudo']);

require '../database.php';

$pseudo=$_POST['pseudo'];

$req = $bdd->prepare('SELECT id, pass FROM membres WHERE pseudo = :pseudo');
$req->execute(array('pseudo' => $pseudo));
$resultat = $req->fetch();


$isPasswordCorrect = password_verify($_POST['password'], $resultat['pass']);

if (!$resultat)
{
  header('Location:connexion.php?cd=6');
}
else
{
    if ($isPasswordCorrect) {
        $_SESSION['id'] = $resultat['id'];
        $_SESSION['pseudo'] = $pseudo;
        header('Location:../Accueil/accueil.php');
    }
    else {
        header('Location:connexion.php?cd=5');
    }
}
