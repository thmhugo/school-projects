 <?php
 session_start();
function checkInput($var)
{

     $var= trim($var);
     $var= stripslashes($var);
     $var= htmlspecialchars($var);

     return $var;
}

require '../database.php';

if ($_POST['message'] == '') {
	header('Location:commentaires.php?billet='.$_GET['billet']);
}else {

$datetime = date("Y-m-d H:i:s");

echo $_SESSION['pseudo'];

$req = $bdd->prepare('INSERT INTO commentaires (id_billet, auteur, commentaire, date_commentaire) VALUES(?, ?, ?, ?)');
$req->execute(array(checkInput($_GET['billet']),checkInput($_SESSION['pseudo']), checkInput($_POST['message']), $datetime));


header('Location:commentaires.php?billet='.$_GET['billet']);
}


?>
