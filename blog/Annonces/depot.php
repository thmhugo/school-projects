<?php
require '../verif.php';
include("../Accueil/entete.php") ?>
<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<link rel="stylesheet" href="../style.css" />
		<link rel="stylesheet" href="https://pro.fontawesome.com/releases/v5.7.2/css/all.css">
		<title> Dépot </title>
	</head>

	<body>

<header>
	<form class="boxAjout" action="depot_verification.php" method="post">
		<h1>Déposer une annonce </h1>
    <p>Je veux <select name="typeAnnonce">
              	<option value="proposer">proposer</option>
              	<option value="rechercher">rechercher</option>
            	</select> de l'aide.
    </p><br>
		<p> </p>
    <p>Sujet:  <select name="sujetAnnonce">
              	<option value="quotidien">Quotidien</option>
              	<option value="informatique">Informatique</option>
								<option value="sante">Santé</option>
								<option value="loisirs">Loisirs</option>
								<option value="devoirs">Devoirs</option>
								<option value="orientation">Orientation</option>
            	</select>
    </p><br>

    <p>
      <label for="villeAnnonce">Localisation :</label><input type="text" name="villeAnnonce" placeholder="Ville" required/>
			<label for="titreAnnonce">Titre :</label><input type="text" name="titreAnnonce" placeholder="Titre" required/>
    </p>

		<p>Votre annonce :</p>
        <textarea rows=9 name="messageAnnonce" placeholder="Annonce" required></textarea>
				<input type="submit" value="Valider" id="poster" /> <br>
	</form>
</div>
</body>
</html>
