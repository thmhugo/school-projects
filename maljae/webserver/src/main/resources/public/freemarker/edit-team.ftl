<!DOCTYPE html>
<html>
	<head>
		<meta charset="utf-8">
		<meta content="IE=edge" http-equiv="X-UA-Compatible">
		<meta name="viewport" content="width=device-width, initial-scale=1.0">
		<link rel="stylesheet" href="/../css/edit-team.css"/>
	  
		<script src="https://code.jquery.com/jquery-3.4.1.js"></script>
		<script src="https://code.jquery.com/ui/1.12.1/jquery-ui.js"></script>
		<script>
			$(
				function() {
					$( "#preferences-list" ).sortable();
					$( "#preferences-list" ).disableSelection();
				}
			);
		</script>

		<title>maljae - Team editing</title>
	</head>

	<body>
		<main>
			<div id="div-logo">
				<img src="/images/univ-logo.png" alt="Logo paris" title="Logo paris">
			</div>
			
			<h1>${teamName}</h1>

			<div class="div-form" id="secret">
				<h2>Secret</h2>
				<form id="update-secret" action="/team/update/secret/${token}" method="post">
					<label for="email">Enter your secret</label>
					<input type="number" min="-2147483648" max="2147483647" name="secret" value="${secret}" required/>
					<input class="button" type="submit" value="Change"/>
				</form>
			</div>

			<div class="div-form" id="students-forms">
				<h2>Team members</h2>
				<div id="students">
					${students}
				</div>
				<form id="add-student" action="/team/send/confirmLink/${token}" method="post">
					<label for="students">Add a member to your team</label>
					<input type="text" name="student" pattern=".+@etu.univ-paris-diderot.fr" value="@etu.univ-paris-diderot.fr" required>
					<input type="number" name="groupId" placeholder="Group number" required>
					<input class="button" type="submit" value="Add"/>
				</form>
				<form id="rm-student" action="/team/rm/${token}" method="post">
					<label for="students">Remove a student from your team</label>
					<input type="text" name="student" pattern=".+@etu.univ-paris-diderot.fr" value="@etu.univ-paris-diderot.fr" required>
					<input class="button" type="submit" value="Remove"/>
				</form>
			</div>
				
			<div class="div-form" id="preferences-form">
				<h2>Preferences</h2>
				${preferences}
				<form id="update-preferences" action="/team/update/preferences/${token}" method="post">
					<input class="button" type="submit" value="Update"/>
				</form>
			</div>

			<div class="div-form" id="delete">
				<h2>Delete Team</h2>

				<script>
					function display() {
						var revealButton = document.getElementById("delete-reveal");
						var confirmButton = document.getElementById("delete-confirm");
						
						if (confirmButton.style.display == "none") {
							confirmButton.style.display = "block";
							revealButton.value = "Cancel";
						}
						else {
							confirmButton.style.display = "none";
							revealButton.value = "Delete ?";
						}
					}
				</script>


				<input id="delete-reveal" class="button" onclick="display()" value="Delete ?" style="text-align: center" readonly="readonly"/>

				<form id="delete-confirm" action="/team/edit/delete-team/${token}" method="post" style="display: none;">
					<input class="button" type="submit" value="Delete !"/>
				</form>
			</div>
		</main>

		

		<script>
			$( "#update-preferences" ).submit( function( event ) {
				event.preventDefault();
				var pref = "";
				
				$( "li" ).each(function() {
					pref += $( this ).text() + ";";
				});

				var url = $( this ).attr( "action" );
				var posting = $.post( url, { 'preferences': pref } );
			});
		</script>

	</body>
</html>
