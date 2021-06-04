package codelab4;

import java.io.IOException;
import java.net.URL;
import java.util.ResourceBundle;

import codelab4.datamodel.Chapters;
import codelab4.datamodel.Session;
import codelab4.datamodel.Sessions;
import javafx.event.ActionEvent;
import javafx.fxml.FXML;
import javafx.fxml.FXMLLoader;
import javafx.fxml.Initializable;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ListView;
import javafx.scene.control.TextField;
import javafx.stage.Stage;

public class LauncherController implements Initializable {

	static final String MAIN_FXML_HIGH = "/src/resources/fxml/main-high.fxml";
	static final String MAIN_FXML_LOW = "/src/resources/fxml/main-low.fxml";

	static int MAIN_WIDTH;
	static int MAIN_HEIGHT;

	@FXML
	private ListView<String> listView;
	@FXML
	private Button launchButton;
	@FXML
	private Button deleteButton;
	@FXML
	private Button submitButton;
	@FXML
	private TextField newSessionTextField;

	private MainWindowController mainController;

	@Override
	public void initialize(URL location, ResourceBundle resources) {
		initDimensions();
		Chapters.init();
		Sessions.init();
		refreshSessionsListView();
	}

	private void refreshSessionsListView() {
		listView.getItems().clear();
		listView.getItems().setAll(Sessions.getIds());
	}

	private void initDimensions() {
		MAIN_WIDTH = App.LOW_RES ? App.MAIN_LOW_W : App.MAIN_HIGH_W;
		MAIN_HEIGHT = App.LOW_RES ? App.MAIN_LOW_H : App.MAIN_HIGH_H;
	}

	@FXML
	public void launch(final ActionEvent event) {
		final String selectedItem = listView.getSelectionModel().getSelectedItem();

		if ( selectedItem != null ) {
			final Session selectedSession = Sessions.getSession(selectedItem);

			if ( selectedSession != null )
				launchMainWindow(selectedSession);
		}
	}

	private void launchMainWindow(final Session selectedSession) {
		final Parent mainWindowRoot = tryToCreateMainWindowRoot(selectedSession);

		if ( mainWindowRoot != null ) {
			final Scene mainScene = initMainScene(mainWindowRoot);
			launchMainStage(mainScene);
		}
	}

	private Parent tryToCreateMainWindowRoot(final Session selectedSession) {
		Parent mainWindowRoot = null;

		try {
			mainWindowRoot = createMainWindowRoot(selectedSession);
		}
		catch (IOException e) {
			e.printStackTrace();
		}
		return mainWindowRoot;
	}

	private Parent createMainWindowRoot(final Session selectedSession)
	throws IOException {
		final FXMLLoader loader = createFXMLLoader();
		final Parent root = loader.load();

		initMainWindowController(selectedSession, loader);

		return root;
	}

	private void initMainWindowController(
		final Session selectedSession, final FXMLLoader loader
	) {
		this.mainController = loader.getController();
		mainController.init(new Instance(mainController, selectedSession));
	}

	/**
	 * Creates a FXMLLoader according the user's screen resolution.
	 * @return the FXMLLoader.
	 */
	private FXMLLoader createFXMLLoader() {
		FXMLLoader loader = new FXMLLoader();

		loader.setLocation(
		    getClass().getResource(
		       App.LOW_RES ? MAIN_FXML_LOW : MAIN_FXML_HIGH
		    )
		);

		return loader;
	}

	private Scene initMainScene(final Parent root) {
		return new Scene(root, MAIN_WIDTH, MAIN_HEIGHT);
	}

	private void launchMainStage(final Scene mainScene) {
		Stage mainWindow = new Stage();
		this.mainController.setThisStage(mainWindow);

		mainWindow.setTitle("CodeLab 4 - L'aventure");
		mainWindow.setScene(mainScene);
		mainWindow.setHeight(MAIN_HEIGHT);
		mainWindow.setWidth(MAIN_WIDTH);
		mainWindow.centerOnScreen();
		mainWindow.show();
	}

	@FXML
	public void delete(ActionEvent event) {
		final String selectedItem = listView.getSelectionModel().getSelectedItem();

		if ( selectedItem != null ) {
			Sessions.delete(selectedItem);
			refreshSessionsListView();
		}
	}

	@FXML
	public void submit(ActionEvent event) {
		final String newSessionId = newSessionTextField.getText();

		newSessionTextField.setText("");
		if ( Sessions.create(newSessionId) ) {
			System.out.println("<INFO> - Session '" + newSessionId + "' created.");
			refreshSessionsListView();
		}
		else {
			System.out.println(
				"<INFO> - An error occured while creating '" + newSessionId + "' session."
			);
		}
	}
}
