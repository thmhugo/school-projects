<?php
require '../verif.php';
?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<link rel="stylesheet" href="../style.css" />
		<link rel="stylesheet" href="https://pro.fontawesome.com/releases/v5.7.2/css/all.css" />
		<title> Mon Blog </title>
	</head>

	<body>

	<?php include("../Accueil/entete.php")	 ; ?>

<h1> Les dernières annonces </h1>

<?php
require '../database.php';


			$reponse = $bdd->query('SELECT * FROM billets ORDER BY id DESC LIMIT 0, 5 ');
			//$id_billet_sent = $donnees['id'];

		while($donnees = $reponse->fetch())
			{
				echo '<div class="news"><p><strong>'.  htmlspecialchars($donnees['titre']). '</strong> ' . 'le : '. htmlspecialchars($donnees['date_creation']).'<br>'.htmlspecialchars($donnees['contenu']). '</p>' ;?>
				<a href="commentaires.php?billet=<?php echo $donnees['id'] ?>&page=1"><i id="iconcommentaire" class="far fa-comments"></i></a></div> <?php
			}

		$reponse->closeCursor();


	?>

	</body>
</html>
