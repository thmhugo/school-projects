<?php
  require '../verif.php';
  require '../fonctions.php' ;
  if(isset($_POST['message']))
  {
    $id_dest = id_of_pseudo($_SESSION['destinataire']) ;
    send_message($_SESSION['id'],$id_dest,$_POST['message']) ;
  }
  header('Location: messages.php')
 ?>
