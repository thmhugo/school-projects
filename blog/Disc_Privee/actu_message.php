<!DOCTYPE html>
<html lang="fr" dir="ltr">
  <head>
    <meta charset="utf-8">
    <title></title>
    <link rel="stylesheet" href="style.css" />
  </head>
  <body>
    <?php
      require '../database.php';
      require '../verif.php' ;
      require '../fonctions.php' ;
      $id_sent = $_SESSION['id'] ;
      $id_dest = id_of_pseudo($_SESSION['destinataire']) ;
      $id_conv = conv_of_pseudos($id_sent,$id_dest) ;
      $req = $bdd->prepare('SELECT * FROM message WHERE id_conversation=? ORDER BY date_envoi');
      $req->execute(array($id_conv)) ;
      while($donnees = $req->fetch())
      {
        if($donnees['id_auteur']==$_SESSION['id'])
        {
          ?>
          <div id="message_send" class="message"> <?php echo $donnees['contenu']  ?>  <br> </div>
          <?php
        }
        else
        {
          ?>
          <div id="message_received" class="message">  <?php echo $donnees['contenu'] ?> <br> </div>
          <?php
        }
      }
    ?>
  </body>
</html>
