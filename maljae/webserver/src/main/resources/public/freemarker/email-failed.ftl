<!DOCTYPE html>
<html lang="en">
	<head>
		<meta charset="UTF-8">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<meta http-equiv="X-UA-Compatible" content="ie=edge">
		<link rel="stylesheet" href="/../css/edit-team.css"/>

		<title>maljae - email failed</title>
	</head>
	<body>
		<main>
			<h1 class="error">Oups ! An error occured while sending email...</h1>

			<form id="back" action="/team/edit/${token}" method="get">
				<input class="button" name="button" type="submit" value="Go back"/>	
			</form>

		</main>	
	</body>
</html>