package codelab4.alert;

import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.image.ImageView;

public class UserAlertEndOfGame extends UserAlert {

	public UserAlertEndOfGame() {
		super(
				"",
				"En résolvant ces quatres premiers chapitres tu viens d'apprendre les bases de la programmation.\n\n"
				+ "La suite des chapitres est en préparation.\n"
				+ "En attendant, repose toi dans ton nouvel abris !\n",
				"Félicitation !",
				AlertType.INFORMATION
		);
	}

	@Override
	protected void setButton() {
		alert.getButtonTypes().setAll(
			new ButtonType("Fermer", ButtonBar.ButtonData.APPLY)
		);
	}

	@Override
	protected void setGraphic() {
		alert.setGraphic(
			new ImageView(
				getResourcesAccordingAppRES("/src/resources/icon/codelab-logo-", ".png")
				.toExternalForm()
			)
		);
	}
}
