package codelab4.alert;

import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.ButtonBar;
import javafx.scene.control.ButtonType;
import javafx.scene.image.ImageView;

public class UserAlertChapterAtEnd extends UserAlert {

	private final String itemToCreate;

	public UserAlertChapterAtEnd(String content, String header, String itemToCreate) {
		super("", content, header, AlertType.INFORMATION);
		this.itemToCreate = itemToCreate;
	}

	@Override
	protected void setButton() {
		alert.getButtonTypes().setAll(
			new ButtonType("Fabriquer !", ButtonBar.ButtonData.APPLY)
		);
	}

	@Override
	protected void setGraphic() {
		ImageView png = new ImageView(
							getClass().getResource(
								"/src/resources/png/" + itemToCreate.toLowerCase() + ".png"
							).toExternalForm()
						);

		png.setFitWidth(100);
		png.setFitHeight(100);
		alert.setGraphic(png);
	}
}
