<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <title> Discussion privée </title>
  </head>
  <body>
    <?php
      require '../database.php';
      $pseudo_auteur = $_SESSION['pseudo'] ;
      $req = $bdd->prepare('SELECT id FROM membres WHERE pseudo = ?');
      $req -> execute(array($pseudo_auteur)) ;
      $donnees = $req->fetch() ;
      $id_auteur = $donnees['id'] ;
      $_SESSION['id'] = $id_auteur ;
      $verif = $bdd->prepare('SELECT pseudo FROM membres WHERE pseudo = ?');
      $message = "" ;
      if(isset($_GET['cd']))
      {
        switch ($_GET['cd'])
        {
          case 9:
            echo '<script> alert("Vous ne pouvez pas vous envoyez des messages") ; </script> ' ;
            break;
          case 10:
              echo '<script> alert("Impossible de demarrer une nouvelle conversation, vous avez deja une conversation avec ce membre") ; </script> ' ;
            break;
          case 11:
              echo '<script> alert(" Utilisateur introuvable " ) ; </script> ' ;
            break;
          default:
            break;
        }
      }
      $req->closeCursor();
    ?>

    <?php
      if(!isset($_POST['pseudo_receveur'])) {
        ?>
        <form class="search" action="" method="post">
          <label> Vous souhaitez envoyer un message à : </label>
          <input type="text" title="trois caractères ou plus" name="pseudo_receveur" value="" pattern="[a-zA-z0-9]{3,}" required>
          <input type="submit" name="" value="Démarrer conversation">
        </form>
    <?php
      echo $message ;
      }
    else {
      $pseudo_receveur = checkInput($_POST['pseudo_receveur']) ;
      $_SESSION['pseudo_receveur'] = $pseudo_receveur ;
      $verif -> execute(array($pseudo_receveur)) ;
      $verif_pseudo = $verif->rowCount() ;
      if($verif_pseudo==1)
      {
        $req = $bdd->prepare('SELECT id FROM membres WHERE pseudo = ?');
        $req -> execute(array($pseudo_receveur)) ;
        $donnees = $req->fetch() ;
        $id_receveur = $donnees['id'] ;
        $_SESSION['id_receveur'] = $id_receveur ;

        $req1 = $bdd->prepare('SELECT COUNT(*) AS nb1 FROM conversation WHERE id_createur=? AND id_membre=? ');
        $req1 -> execute(array($id_auteur,$id_receveur)) ;
        $verif_conv1 = $req1->fetch() ;   $req1->closeCursor();

        $req2 = $bdd->prepare('SELECT COUNT(*) AS nb2 FROM conversation WHERE id_createur=? AND id_membre=? ');
        $req2 -> execute(array($id_receveur,$id_auteur)) ;
        $verif_conv2 = $req2->fetch() ;   $req2->closeCursor();

        if ($id_receveur == $id_auteur) {
          header('Location:messages.php?cd=9') ;
        }
        else if($verif_conv1['nb1']!=0 || $verif_conv2['nb2']!=0) {   // La conversation existe deja
          header('Location:messages.php?cd=10') ;
          }
        else {
          $ajout_conv=$bdd->query('INSERT INTO conversation(id_createur,id_membre) VALUES ('.$id_auteur.','.$id_receveur.')');
            $ajout_conv->closeCursor();
        }
      }
      else{
        header('Location:messages.php?cd=11') ;
      }
    }
    ?>
  </body>
</html>
