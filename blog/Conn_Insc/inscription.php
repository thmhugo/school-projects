<?php
$message='Merci de compléter entièrement le formulaire';


if(isset($_GET['cd']))
{
	switch($_GET['cd'])
	{
		case 0:
			$message='Les mots de passe sont différents';
			?> <style>#mdp{border:2px dotted red; } #Vmdp{border-color:red} </style> <?php
			break;

		case 1:
			$message='L\'adresse mail est invalide';
			?> <style>#email{border-color:red} </style> <?php
			break;

		case 2:
			$message='Le pseudo est déjà utilisé';
			?> <style>#pseudo{border-color:red}</style> <?php
			break;

		case 3;
			$message='Veuillez entrer un pseudo valide';
			?> <style>#pseudo{border-color:red}</style> <?php
			break;

		case 4:
		$message='Adresse mail déjè utilisée';
		?> <style>#email{border-color:red} </style> <?php
			break;
	}
}

?>

<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<link rel="stylesheet" href="../style.css" />
		<link rel="stylesheet" href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" integrity="sha384-fnmOCqbTlWIlj8LyTjo7mOUStjsKC4pOpQbqyi7RrhN7udi9RwhKkMHpvLbHG9Sr" crossorigin="anonymous">
		<title>Inscription</title>
	</head>

	<body>
	  <form class="box" action="inscription_post.php" method="post">
      <div class="formulaire">
				<h1>Inscription</h1>
        <span class="t"><label class="textFormulaire">Pseudo  : </label>  <input type="text" name="pseudo" id="pseudo" class="inputFormulaire" placeholder="pseudo" title="pseudo composé de 2 à 10 caractères alphanumériques sans caractères spéciaux" required/></span><br />
        <span class="t"><label class="textFormulaire"> Mot de passe :</label> <input type="password" name="password" class="inputFormulaire" id="mdp" placeholder="Mot de passe" required /></span><br />
        <span class="t"><label class="textFormulaire">Retapez votre mot de passe : </label> <input type="password" name="Vpassword" id="Vmdp" class="inputFormulaire" placeholder="Confirmation du mot de passe" required /></span><br />
        <span class="t"><label class="textFormulaire">Adresse email : </label> <input type="email" name="email" class="inputFormulaire" id="email" placeholder="e-mail" required/> </span> <br / />
				<input type="submit" value="Valider" id="valider" />
				<p><?php echo $message; ?></p>
				<p> Déjà inscrit ? Connectez-vous : <a href="connexion.php"><i class="fas fa-bookmark"></i></a> </a> </p>
      </div>


    </form>
	</body>
</html>
