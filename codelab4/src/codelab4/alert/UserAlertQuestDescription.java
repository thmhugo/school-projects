package codelab4.alert;

import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.image.ImageView;

public class UserAlertQuestDescription extends UserAlert {

	public UserAlertQuestDescription(String content, String header) {
		super("", content, header, AlertType.INFORMATION);
	}

	@Override
	protected void setButton() {
		alert.getButtonTypes().setAll(
			new ButtonType("Compris !", ButtonBar.ButtonData.APPLY)
		);
	}

	@Override
	protected void setGraphic() {
		alert.setGraphic(
			new ImageView(
				getResourcesAccordingAppRES("/src/resources/icon/quest-", ".png")
				.toExternalForm()
			)
		);
	}
}
