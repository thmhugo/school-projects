<?php

  function checkInput($var)
  {
    $var= trim($var);
    $var= stripslashes($var);
    $var= htmlspecialchars($var);
    return $var;
  }

  function id_of_pseudo($pseudoE)
  {
    require '../database.php';
    $reqid = $bdd->prepare('SELECT id FROM membres WHERE pseudo=?') ;
    $reqid->execute(array($pseudoE)) ;
    $idS = $reqid->fetch() ;
    $id = $idS['id'] ;
    $reqid->closeCursor();
    return $id ;
  }

  function pseudo_of_id($idE)
  {
    require '../database.php';
    $reqPs = $bdd->prepare('SELECT pseudo FROM membres WHERE id=?') ;
    $reqPs->execute(array($idE)) ;
    $pseudoS = $reqPs->fetch() ;
    $pseudoS =  $pseudoS['pseudo'] ;
    $reqPs->closeCursor();
    return $pseudoS ;
  }

  function conv_of_pseudos($id_sent,$id_dest)
  {
    require '../database.php';
    $req = $bdd->prepare('SELECT id FROM conversation WHERE (id_membre=? AND id_createur=?) OR (id_createur=? AND id_membre=?  )');
    $req->execute(array($id_dest,$id_sent,$id_dest,$id_sent)) ;
    $donnees = $req->fetch() ;
    $idconv = $donnees['id'] ;
    return $idconv ;
  }

  function affiche_messages($id_sent,$id_dest)
  {
    ?>
    <link rel="stylesheet" href="style.css" />
    <?php
    require '../database.php';
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
  }

  function send_message($id_sent,$id_dest,$message)
  {
    require '../database.php';
    $id_conv = conv_of_pseudos($id_sent,$id_dest) ;
    $message = htmlspecialchars($message) ;
    $req = $bdd->prepare('INSERT INTO message(contenu, id_auteur, id_conversation, id_receveur) VALUES (?,?,?,?)') ;
    $req->execute(array($message,$id_sent,$id_conv,$id_dest)) ;
  }

  function recupconvs()
  {
    require '../database.php';
    $listePseudo = array() ;
    $req_pseudo = $bdd->prepare('SELECT pseudo FROM membres INNER JOIN conversation WHERE (conversation.id_membre = membres.id AND conversation.id_membre=?)
    OR (conversation.id_createur = membres.id AND conversation.id_createur=?)');
    $req = $bdd->prepare('SELECT * FROM conversation WHERE id_membre=? OR id_createur=? ORDER BY date_creation DESC');
    $req->execute(array($_SESSION['id'],$_SESSION['id'])) ;
    while($donnee = $req->fetch())
    {
      if($donnee['id_createur']==$_SESSION['id'])
      {
        $req_pseudo -> execute(array($donnee['id_membre'],$donnee['id_membre'])) ;
        $pseudo = $req_pseudo->fetch() ;
        array_push($listePseudo,$pseudo['pseudo']) ;
      }
      else
      {
        $req_pseudo -> execute(array($donnee['id_createur'],$donnee['id_createur'])) ;
        $pseudo = $req_pseudo->fetch() ;
        array_push($listePseudo,$pseudo['pseudo']) ;
      }
    }
    $req_pseudo->closeCursor();
    $req->closeCursor();
    return $listePseudo ;
  }









 ?>
