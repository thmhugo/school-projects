<?php
session_start();
$_SESSION['pseudo']=$_POST['pseudo'];

function checkInput($var)
{
     $var= trim($var);
     $var= stripslashes($var);
     $var= htmlspecialchars($var);

     return $var;
}

$pseudoIns =checkInput($_POST['pseudo']);
$emailIns = checkInput($_POST['email']);
$datetime = date("Y-m-d");
$passhache=password_hash(checkInput($_POST['password']),PASSWORD_DEFAULT);

require '../database.php';

if(checkInput($_POST['password']) == checkInput($_POST['Vpassword']))
  {
    if( preg_match ( " /^[^\W][a-zA-Z0-9_]+(\.[a-zA-Z0-9_]+)*\@[a-zA-Z0-9_]+(\.[a-zA-Z0-9_]+)*\.[a-zA-Z]{2,4}$/ " , checkInput($_POST['email']) ))
      {
        $sth = $bdd->prepare('SELECT EXISTS (SELECT * FROM membres WHERE email= :emailIns)');
        $sth->execute(array(':emailIns'=>$emailIns));
        $resultM = $sth->fetchColumn();

          if($resultM>=1)
            {
              header('Location:inscription.php?cd=4');//code = 4 : mail deja utilisé
            }else if(isset($_POST['pseudo']) && $resultM==0)
              {
                $sth = $bdd->prepare('SELECT EXISTS (SELECT * FROM membres WHERE pseudo= :pseudoIns)');
                $sth->execute(array(':pseudoIns'=>$pseudoIns));
                $resultP = $sth->fetchColumn();

                  if($resultP>=1)
                    {
                      header('Location:inscription.php?cd=2');//code = 2 : pseudo deja utilisé
                    }else{
                      $req = $bdd->prepare('INSERT INTO membres(pseudo, pass, email, date_inscription) VALUES(?, ?, ?, ?)');
                      $req->execute(array(checkInput($_POST['pseudo']),$passhache, checkInput($_POST['email']), $datetime));
                      header('Location:../Accueil/accueil.php'); // connection au blog
                    }
              }else {
              header('Location:inscription?cd=3');//pseudo non valide
            }
      }else{
        header('Location:inscription.php?cd=1');//code = 1 :adresse mail invalide
      }
  }else {
    header('Location:inscription.php?cd=0');//code = 0 :mot de passe différents
  }

 ?>
