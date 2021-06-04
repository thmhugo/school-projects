<!DOCTYPE html>
<html lang="fr" dir="ltr">
  <head>
    <meta charset="utf-8">
    <title></title>
  </head>
  <body>
    <?php
      session_start() ;
      require '../fonctions.php' ;
      $contacts = recupconvs() ;
      foreach($contacts as $pseudoliste)
      {
        if(!empty($_POST[$pseudoliste]))
        {
          $_SESSION['destinataire'] = $_POST[$pseudoliste] ;
        }
      }
      header('Location:messages.php') ;
     ?>
  </body>
</html>
