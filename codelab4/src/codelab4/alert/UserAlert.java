package codelab4.alert;

import java.net.URL;

import codelab4.App;
import javafx.scene.control.Alert;
import javafx.scene.control.Alert.AlertType;
import javafx.scene.control.Label;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

public abstract class UserAlert {

	private static final String CSS_PATH = "/src/resources/css/user-alert-";

	protected String title;
	protected String header;
	protected String content;
	protected Alert alert;

	abstract protected void setButton();
	abstract protected void setGraphic();

	public UserAlert(String title, String content, String header, AlertType alertType) {
		this.title = title;
		this.header = header;
		this.content = content;
		this.alert = new Alert(alertType);
	}

	private void init() {
		alert.setTitle(title);
		alert.setHeaderText(header);
		setContent();
		setButton();
		setGraphic();
		applyStyleSheets();
		setInFront();
	}

	void show() {
		init();
		alert.show();
	}

	public void showAndWait() {
		init();
		alert.showAndWait();
	}

	protected URL getResourcesAccordingAppRES(final String prefix, final String suffix) {
		return getClass().getResource(
			prefix + (App.LOW_RES ? "low" : "high") + suffix
		);
	}

	private void setContent() {
		Label contentLabel = new Label(this.content);

		contentLabel.setWrapText(true);
		alert.getDialogPane().setContent(contentLabel);
	}

	private void applyStyleSheets() {
		alert.getDialogPane().getStylesheets().add(
			getResourcesAccordingAppRES(CSS_PATH, ".css")
			.toExternalForm()
		);
	}

	private void setInFront() {
		Stage userAlertStage = (Stage) alert.getDialogPane().getScene().getWindow();

		userAlertStage.setAlwaysOnTop(true);
		userAlertStage.initStyle(StageStyle.TRANSPARENT);
		alert.getDialogPane().setMaxWidth(500);
		alert.getDialogPane().setMinHeight(250);
	}
}
