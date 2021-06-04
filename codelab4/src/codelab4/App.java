package codelab4;

import java.io.IOException;
import java.net.URL;

import codelab4.process.Interpreter;
import javafx.application.Application;
import javafx.fxml.FXMLLoader;
import javafx.geometry.Rectangle2D;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.text.Font;
import javafx.stage.Screen;
import javafx.stage.Stage;

public class App extends Application {

	/**
	 * When the user display resolution is under the MAIN_HIGH_H/W,
	 * LOW_RES is set to true;
	 */
	public static boolean LOW_RES = false;

	public static final int LAUNCHER_W = 400;
	public static final int LAUNCHER_H = 600;
	public static final int MAIN_HIGH_W = 1400;
	public static final int MAIN_HIGH_H = 800;
	public static final int MAIN_LOW_W = 960;
	public static final int MAIN_LOW_H = 540;

	private static final String LAUNCHER_FXML = "/src/resources/fxml/launcher.fxml";
	private static final String LAUNCHER_CSS = "/src/resources/css/launcher.css";
	private static final String FONTS_DIR = "/src/resources/font/";

	public static void main(String[] args) throws Exception {
		initResolution();
		launch(args);
	}

	@Override
	public void start(final Stage primaryStage) throws IOException {
		final Parent root = loadFXML(getClass().getResource(LAUNCHER_FXML));
		final Scene mainScene = initMainScene(root);

		initFonts();
		initPrimaryStage(primaryStage, mainScene);
		Interpreter.init();
	}

	private static void initResolution() throws Exception {
		final Rectangle2D screenDim = Screen.getPrimary().getBounds();

		if (
			screenDim.getHeight() <  MAIN_HIGH_H || screenDim.getWidth() < MAIN_HIGH_W
		) {
			if (
				screenDim.getHeight() >= MAIN_LOW_H && screenDim.getWidth() >= MAIN_LOW_W
			)
				LOW_RES = true;
			else
				throw new Exception("Sorry, your screen resolution is to low.");
		}
	}

	protected static Parent loadFXML(final URL location) throws IOException {
		FXMLLoader loader = new FXMLLoader();
		loader.setLocation(location);

		return loader.load();
	}

	private void initFonts() {
		loadFont("A_Love_of_Thunder");
		loadFont("Hack_Bold");
		loadFont("Earth_Quest");
	}

	private void loadFont(final String fontName) {
		final String fontPath = FONTS_DIR + fontName + ".ttf";

		Font.loadFont(getClass().getResource(fontPath).toExternalForm(), 12);
	}

	private Scene initMainScene(final Parent root) {
		Scene mainScene = new Scene(root, LAUNCHER_W, LAUNCHER_H);
		mainScene.getStylesheets().add(LAUNCHER_CSS);

		return mainScene;
	}

	private void initPrimaryStage(Stage primaryStage, final Scene mainScene) {
		primaryStage.setTitle("Launcher");
		primaryStage.setScene(mainScene);
		primaryStage.setMinHeight(LAUNCHER_H);
		primaryStage.setMinWidth(LAUNCHER_W);
		primaryStage.setMaxHeight(LAUNCHER_H);
		primaryStage.setMaxWidth(LAUNCHER_W);
		primaryStage.centerOnScreen();
		primaryStage.show();
	}
}
