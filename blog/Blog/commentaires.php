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
		<header>
			<?php
				function checkInput($var)
					{
			 			$var= trim($var);
			 			$var= stripslashes($var);
			 			$var= htmlspecialchars($var);
			     return $var;
				 	}

				require '../database.php';
				include("../Accueil/entete.php")	 ;
			?>
		</header>

<div class="page">
<?php

				$req = $bdd->prepare('SELECT id, titre, contenu, DATE_FORMAT(date_creation, \'%d/%m/%Y à %Hh%imin%ss\') AS date_creation_fr FROM billets WHERE id = ?');
				$req->execute(array($_GET['billet']));
				$donnees = $req->fetch(); ?>

				<div class="news">
    			<h3>	<?php echo checkInput($donnees['titre']); ?>	<em>le <?php echo $donnees['date_creation_fr']; ?>	</em>	</h3>

    			<p> <?php echo checkInput($donnees['contenu']);	?>	</p>
				</div>

		<?php

			$id_billet_comm = $_GET['billet'];
			$_SESSION['nbbillet']=$_GET['billet'];
?>  <?php

		$commParPage= 5;
		$totalCommReq = $bdd->query('SELECT id FROM commentaires WHERE id_billet='.$_GET['billet']);
		$totalComm = $totalCommReq->rowCount();
		$nbPages = ceil($totalComm/$commParPage);

		if(isset($_GET['page']) && !empty($_GET['page']) && $_GET['page'] >0)
		{
			$pageActuelle = intval($_GET['page']);
		}else {
			$pageActuelle=1;
		}

		$debut = ($pageActuelle-1)*$commParPage;

		$reponse = $bdd->query('SELECT * FROM commentaires  WHERE id_billet='.$_GET['billet'].' ORDER BY id DESC LIMIT '.$debut.','.$commParPage);

			while($donnees = $reponse->fetch())
			{
						echo '<div class="commentaire"><p><strong>'.  $donnees['auteur']. '</strong> : '.$donnees['date_commentaire'].'<br>' .$donnees['commentaire']. '</p></div> <div class="ln"> </div>' ;
			}
			echo '<div class="lignePagi">';
			for($i=1;$i<=$nbPages;$i++)
			{
				if($i==$pageActuelle){
					echo '<div class="pageAct">'.$i.'</div>';
				}else{
					echo '<div class="pagination"><a href="commentaires.php?billet='.$_SESSION['nbbillet'].'&page='.$i.'">'.$i.'</a></div>';
				}
			}
			echo '</div>';

		$reponse->closeCursor();


	?>
	<form class="boxAjout" action="commentaires_post.php?billet=<?php echo $_GET['billet'] ?>" method="post">
		<p>	<label for="saisie">Ajouter un commentaire : </label> </p>
 				<textarea rows=9 name="message" id="message" required></textarea>
				<input type="submit" value="Poster" id="poster" /> <br>
				<a href="index.php" > <i class="fas fa-arrow-circle-left"></i></a>
	</form>
</div>
	</body>
</html>
