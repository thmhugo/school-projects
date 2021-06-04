<?php
    // Connexion à la base de donnée

    try
      {
        $bdd = new PDO('mysql:host=localhost;dbname=PP;charset=utf8','root','',array(PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION));
      }
    catch (Exception $e)
      {
        die('erreur : '. $e->getMessage());
      }


    // Fin de la connexion à la base de donnée
?>
