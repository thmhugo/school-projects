<!DOCTYPE html>
<html lang="fr" dir="ltr">
  <head>
    <meta charset="utf-8">
    <script src="https://ajax.googleapis.com/ajax/libs/jquery/3.3.1/jquery.min.js"></script>
    <link rel="stylesheet" href="https://pro.fontawesome.com/releases/v5.7.2/css/all.css" />
    <link rel="stylesheet" href="style.css" />
    <title> message </title>
  </head>
  <body>
    <script>
      setInterval('load_messages()', 1000) ;
      function load_messages()
      {
        $('#conversation').load("actu.php") ;
      }
    </script>

    <?php
      require '../verif.php' ;
      require '../fonctions.php' ;
      include("discussion.php") ;
      $contacts = recupconvs() ;
    ?>

    <form class="destinataire" action="destinataire_post.php" method="post">
      <div id="contacts">
      <?php
        foreach($contacts as $pseudoliste)
        {
          ?>
          <button type="submit" id="user_button" name= <?php echo $pseudoliste ?> value=<?php echo $pseudoliste ?>>
            <div id="user">  <?php echo $pseudoliste ; ?> </div>
          </button>
          <?php
        }
          ?>
      </div>
    </form>
    <?php
      if(isset($_SESSION['destinataire']))
      {
        $pseudo_dest = $_SESSION['destinataire'] ;
        $id_sent = $_SESSION['id'] ;
        $id_dest = id_of_pseudo($pseudo_dest) ;
        $id_conv = conv_of_pseudos($id_sent,$id_dest) ;
        $contacts = recupconvs() ;
      ?>
      <div id="conversation">
        <?php
          affiche_messages($id_sent,$id_dest) ;
        ?>
        <script>
          element = document.getElementById('conversation');
          element.scrollTop = element.scrollHeight;
        </script>
      </div>
      <form action="message_post.php" class="dd" method="POST">
        <div>
          <input type="text" name="message" required>
        <button type="submit" id="send"> <i class="far fa-paper-plane"></i>
        </div>
      </form>
      <?php
      }
        else
        {
          echo "non vous nnnnnnn ............. " ;
        }


       ?>
  </body>
</html>
