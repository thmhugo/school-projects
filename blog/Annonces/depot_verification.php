<?php
require '../verif.php';
	include("../Accueil/entete.php");

	function checkInput($var)
	{
	  $var= trim($var);
	  $var= stripslashes($var);
	  $var= htmlspecialchars($var);
	  return $var;
	}
?>

<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<link rel="stylesheet" href="../style.css" />
		<link rel="stylesheet" href="https://pro.fontawesome.com/releases/v5.7.2/css/all.css" />
    <link rel="stylesheet" href="https://unpkg.com/leaflet@1.3.1/dist/leaflet.css" integrity="sha512-Rksm5RenBEKSKFjgI3a41vrjkw4EVPlJ3+OiI65vTjIdo9brlAacEuKOiQ5OFh7cOI1bkDwLqdLw3Zg0cRJAAQ==" crossorigin="" />
		<script src="https://unpkg.com/leaflet@1.3.1/dist/leaflet.js" integrity="sha512-/Nsx9X4HebavoBvEBuyp3I7od5tA0UzAxs+j83KgC8PU0kgB4XiK4Lfe4y4cgBtaRJQEIFCW+oC506aPT2L1zw==" crossorigin=""></script>
  	<script type="text/javascript">
<?php
$_SESSION['typeAnnonce'] = checkInput($_POST['typeAnnonce']);
$_SESSION['sujetAnnonce'] = checkInput($_POST['sujetAnnonce']);
$_SESSION['villeAnnonce'] = checkInput(strtoupper($_POST['villeAnnonce']));
$_SESSION['titreAnnonce'] = checkInput($_POST['titreAnnonce']);
$_SESSION['messageAnnonce'] = checkInput($_POST['messageAnnonce']);

	require '../database.php';
//carte
		$req = $bdd->prepare('SELECT ville_nom,ville_longitude_deg, ville_latitude_deg FROM villes_france WHERE ville_nom= ?');
		$req->execute(array(strtoupper($_POST['villeAnnonce'])));
		$donnees = $req->fetch();

    $long =$donnees['ville_longitude_deg'];
    $lat =$donnees['ville_latitude_deg'];
?>
  var lat = <?php echo $lat; ?>;
  var lon = <?php echo $long; ?>;
	var macarte = null;

	function initMap()
		{
			macarte = L.map('map').setView([lat, lon], 11);
			L.tileLayer('https://{s}.tile.openstreetmap.fr/osmfr/{z}/{x}/{y}.png',
					{
						attribution: 'données © <a href="//osm.org/copyright">OpenStreetMap</a>/ODbL - rendu <a href="//openstreetmap.fr">OSM France</a>',
							minZoom: 1,
							maxZoom: 20
						}).addTo(macarte);
			var marker = L.marker([<?php echo $lat; ?>,<?php echo $long; ?>]).addTo(macarte);
		}
			window.onload = function()
				{
					initMap();
				};
		</script>

		<style type="text/css">
			#map
			{
				height: 400px;
				width: 400px;
			}
		</style>

		<title> Mon Blog </title>
	</head>

	<body>
		<h1>Validez vos informations</h1>

		<?php
			if($_POST['typeAnnonce'] == 'proposer')
				{
					$typeAnnConjug='proposez votre ';
				}
			else if($_POST['typeAnnonce'] == 'rechercher')
				{
					$typeAnnConjug='recherchez de l\'';
				}
		 ?>

		<h2>Vous <?php echo $typeAnnConjug; ?>aide en <?php echo $_POST['sujetAnnonce'];?></h2>

		<h2>Votre annonce :</h2>
			<p> <strong><?php echo $_POST['titreAnnonce']; ?></strong> <br> <?php echo $_POST['messageAnnonce']; ?></p>

		<h2>Localisation :</h2>
		<p><?php echo $_POST['villeAnnonce']; ?>

		<button id="btnPopup" class="btnPopup" title="Voir sur la carte"><i class="fas fa-map-marker-alt"></i></button></p>
  	<div id="overlay" class="overlay">
			<div id="popup" class="popup">
				<h2>Emplacement sur la carte :<span id="btnClose" class="btnClose">&times;</span></h2>
				<div id="map"> </div>
    	</div>
  	</div>

	<script type="text/javascript" src="script.js"></script>

    <div class="bttonVerif">
        <div>	<a href="depot_post.php"><input type="button" value="poster" /></a>  </div>
        <div>	<a href="depot.php"><input type="button" value="annuler" /></a>	</div>
    </div>
	</body>
</html>
