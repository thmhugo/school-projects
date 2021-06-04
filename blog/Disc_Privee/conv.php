<!DOCTYPE html>
<html>
  <head>
    <meta charset="utf-8">
    <link rel="stylesheet" href="style.css" />
  </head>
  <body>
    <script>
      function load_destinataire()
      {
        $('#desti').load("conv.php") ;
      }
    </script>
    <form action="messages.php" method="POST">
      <label for="desti"> Sélectionner un destinataire : </label>
      <select name="destinataire" id="desti" onchange="load_destinataire() ; this.form.submit()" required>
        <?php
          foreach($contacts as $pseudoliste)
          {
            if($pseudoliste == $_SESSION['destinataire'])
            {
              echo '<option selected>'. $pseudoliste . '</option>' ;
            }
            else
            {
              echo '<option>'. $pseudoliste . '</option>' ;
            }
          }
        ?>
      </select>
    </form>
  </body>
</html>
