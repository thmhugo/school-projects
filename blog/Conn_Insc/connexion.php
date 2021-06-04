<?php
if(isset($_GET['cd']))
{
	switch($_GET['cd'])
	{
		case 5:
			$message='Mot de passe ou identifiant incorect';
			break;
		case 6:
			$message='Pseudo inconnu, veuillez vous inscrire en cliquant <a href="inscription.php"> ici </i></a>';
	}
}
?>

<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<link rel="stylesheet" href="../style.css" />
		<!-- <link rel="stylesheet" media="screen and (max-width: 800px)" href="../responstyle.css" /> -->
		<title>Connexion</title>
	</head>

	<body>
	  <form class="box" action="connexion_post.php" method="post">
			<div class="formulaire">
				<h1>Connexion</h1>
        <input type="text" name="pseudo" placeholder="pseudo" id="pseudo" class="inputFormulaire" required/>
        <input type="password" name="password" id="password" placeholder="mot de passe" class="inputFormulaire" required />
				<input type="submit" value="login" />
				<p><?php if(isset($message)){echo $message;} ?></p>
				<p class="pmt"> Pas encore de compte  <a class="pmt" href="inscription.php">Inscrivez-vous !</a> </p>
		  </div>
		</form>
	</body>
</html>
