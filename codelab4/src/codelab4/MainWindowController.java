package codelab4;

import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.ResourceBundle;

import codelab4.alert.UserAlertChapterAtEnd;
import codelab4.alert.UserAlertEndOfGame;
import codelab4.alert.UserAlertQuestDescription;
import codelab4.commons.Tools;
import codelab4.datamodel.Chapters;
import codelab4.datamodel.MapState;
import codelab4.datamodel.MapState.Item;
import codelab4.filemanager.FileManager;
import codelab4.process.Interpreter;
import codelab4.process.Parser;
import codelab4.process.Token;
import codelab4.vizualisation.MapGenerator;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.Button;
import javafx.scene.control.Label;
import javafx.scene.control.ListView;
import javafx.scene.control.ProgressBar;
import javafx.scene.control.TextArea;
import javafx.scene.image.Image;
import javafx.scene.image.ImageView;
import javafx.scene.layout.FlowPane;
import javafx.scene.layout.GridPane;
import javafx.scene.layout.HBox;
import javafx.scene.layout.Pane;
import javafx.scene.layout.VBox;
import javafx.stage.Stage;
import javafx.util.Pair;

public class MainWindowController implements Initializable {

	public static final String SYNTAX_ERR_COLOR = "#ff3600";
	public static final String LOGIC_ERR_COLOR =  "#ff3600";
	public static final String INTERPRETATION_ERR_COLOR = "#ffa600";
	public static final String SUCCESS_COLOR = "#5a9258";

	private static final String PNG_PATH = "/src/resources/png/";

	@FXML
	GridPane vizualisationGridPane;
	@FXML
	Pane mapDisplayPane;
	@FXML
	FlowPane progressBox;
	@FXML
	Label progressIndicator;
	@FXML
	ListView inventoryList;
	@FXML
	TextArea textArea;
	@FXML
	ListView questList;
	@FXML
	VBox list_actions;
	@FXML
	VBox list_conditions;
	@FXML
	VBox list_boucles;
	@FXML
	Label errorLabel;
	@FXML
	HBox hungerBox;
	@FXML
	Label fileStatusLabel;

	ArrayList<HBox> inventoryStringSet = new ArrayList<HBox>();
	ArrayList<String> questStringSet = new ArrayList<String>();
	ObservableList obsList = FXCollections.observableArrayList();
	ObservableList obsQuestList = FXCollections.observableArrayList();

	private Instance instance;
	private MapGenerator mapGenerator;
	private Stage thisStage;
	private Thread interpreterThread;

	@Override
	public void initialize(URL location, ResourceBundle resources) {}

	private void initMapGenerator() {
		this.mapGenerator = new MapGenerator(instance.getCurrentMapState(), mapDisplayPane);
	}

	public void init(final Instance instance) {
		this.instance = instance;
		initMapGenerator();
		initProgressBar();
		initInventoryList();
		initQuestList();
		instance.getCurrentSession().unlockFunctions();
		showQuestDescription();
		refreshFunctions();
		refreshView();
	}

	private void refreshMapState() {
		instance.setCurrentMapState(new MapState(instance.getNotModifiedMapState()));
	}

	private void refreshProgressBar() {
		progressBox.getChildren().clear();
		initProgressBar();
	}

	private ProgressBar createProgressBar(final double progress) {
		ProgressBar newBar = new ProgressBar(progress);
		final int maxWidth = App.LOW_RES ? 250 : 400;
		final int maxHeight = App.LOW_RES ? 13 : 20;

		newBar.setPrefWidth(maxWidth / Chapters.chapters.size());
		newBar.setMaxHeight(maxHeight);
		newBar.getStyleClass().add("progress-bar");
		progressBox.getChildren().add(newBar);

		return newBar;
	}

	private void initProgressBar() {
		for (int i = 1; i <= Chapters.chapters.size(); i++) {
			if (instance.getCurrentChapter().getIdNumber() == i)
				createProgressBar(instance.getCurrentSession().getCurrentProgress());
			else if (instance.getCurrentChapter().getIdNumber() > i)
				createProgressBar(1);
			else
				createProgressBar(0);
		}

		progressIndicator.setText(
			(int) instance.getCurrentSession().getGlobalProgress() + "%"
		);
	}

	private void initInventoryList() {
		final HashMap<Item, Integer> inventoryHash = instance.getCurrentMapState().getInventory();

		obsList = FXCollections.observableArrayList();
		inventoryStringSet.removeAll(inventoryStringSet);

		for (final Entry<Item, Integer> entry : inventoryHash.entrySet())
			addItemToInventory(entry);

		obsList.setAll(inventoryStringSet);
		inventoryList.setPrefWidth(inventoryHash.size() * 125);
		inventoryList.setItems(obsList);
	}

	private void addItemToInventory(final Entry<Item, Integer> entry) {
		HBox box = new HBox(5);
		final Image img = Tools.loadImageFromResources(
								PNG_PATH + entry.getKey().toString().toLowerCase() + ".png",
								(App.LOW_RES) ? 25 : 40
							);
		final ImageView imageView = new ImageView(img);
		final String str = "(x" + entry.getValue() + ")";

		box.getChildren().addAll(imageView, new Label(str));
		inventoryStringSet.add(box);
	}

	private void initQuestList() {
		obsQuestList.setAll(instance.getCurrentSession().getQuestsGoal());
		questList.setItems(obsQuestList);
	}

	public void refreshView() {
		mapDisplayPane.getChildren().clear();
		mapGenerator.generateMap();
		initInventoryList();
		displayHunger();
	}

	private void displayHunger() {
		hungerBox.getChildren().clear();
		try {
			final int pngSize = App.LOW_RES ? 30 : 45;
			final int hunger = instance.getCurrentMapState().getHunger();
			final int maxHunger = MapState.Item.getMaxHunger();

			if (hunger == maxHunger)
				setErrorLabel(
					"Attention ! \nN'oublies pas de manger !",
					INTERPRETATION_ERR_COLOR
				);

			displayRightFruit(hunger, maxHunger, pngSize);
			hungerBox.setMaxWidth(pngSize * maxHunger);
			hungerBox.setVisible(true);
		}
		catch (NullPointerException e) {
			hungerBox.setVisible(false);
		}
	}

	private void setErrorLabel(final String text, final String color) {
		errorLabel.setMinHeight(App.LOW_RES ? 25 : 50);
		errorLabel.setStyle("-fx-text-fill: " + color);
		errorLabel.setText(text);
	}

	private void displayRightFruit(
		final int hunger, final int maxHunger, final int pngSize
	) {
		final Image apple;
		final Image blackApple;

		apple = Tools.loadImageFromResources(PNG_PATH + "fruit.png", pngSize);
		blackApple = Tools.loadImageFromResources(PNG_PATH + "black-apple.png", pngSize);
		for (int i = 0; i < maxHunger; i++) {
			if (i < maxHunger - hunger)
				addToHungerBox(apple);
			else
				addToHungerBox(blackApple);
		}
	}

	private void addToHungerBox(final Image img){
		hungerBox.getChildren().add(new ImageView(img));
	}

	public void refreshFunctions() {
		try {
			clearFunctionsList();
			addFunctionsToList();
		}
		catch(NullPointerException e ) {
			e.printStackTrace();
		}
	}

	private void clearFunctionsList() {
		list_actions.getChildren().clear();
		list_conditions.getChildren().clear();
		list_boucles.getChildren().clear();
	}

	private void addFunctionsToList() {
		for (final String str : instance.getCurrentSession().getUnlockedFunctions()) {
			final Token t = Token.fromString(str);

			switch (Token.getType(t)) {
				case "action":
						addButton(list_actions, str, true);
					break;
				case "condition":
						addButton(list_conditions, str, false);
					break;
				default:
					addButton(list_boucles, str, str.equals("else"));
			}
		}
	}

	public void addButton(VBox box, final String str, final boolean needNewLine) {
		Button functionButton = new Button(str);
		functionButton.setMnemonicParsing(false);

		functionButton.setOnAction(
			(event) -> {
				textArea.appendText(
					functionButton.getText()
					+ (needNewLine ? "\n" : " ")
				);
			}
		);
		box.getChildren().add(functionButton);
	}

	@FXML
	public void interpret() {
		final Interpreter interpreter;
		final ArrayList<Token> tokens;

		reinitialize();
		try {
			tokens = new Parser().tokenize(
							textArea.getText(),
							instance.getCurrentQuest().getInstructionCount()
						);
			interpreter = new Interpreter(
									tokens,
									instance.getCurrentMapState(),
									instance
								);
			interpreterThread = new Thread(() -> interpreterRun(interpreter));
			interpreterThread.start();
		}
		catch (Exception e) {
			displayException(e);
		}
	}

	private void interpreterRun(final Interpreter interpreter) {
		try {
			interpreter.run();
		}
		catch (Exception e) {
			Platform.runLater(() -> displayException(e));
		}
	}

	private void displayException(final Exception e) {
		String exceptionColor = "";

		switch (e.getClass().toString()) {
			case "class codelab4.process.exceptions.SyntaxException" :
			 		exceptionColor = SYNTAX_ERR_COLOR;
				break;
			default :
				exceptionColor = LOGIC_ERR_COLOR;
		}
		try {
			setErrorLabel(e.getMessage(), exceptionColor);
		}
		catch (IllegalStateException g) {}
	}

	private void hideError() {
		errorLabel.setMinHeight(0);
		errorLabel.setText("");
		fileStatusLabel.setText("");
	}

	public void verify() {
		if (instance.verify()) {
			instance.updateToNextQuest();
			if ( !instance.getIsOver() ) {
				obsQuestList.add(0, instance.getCurrentSession().getCurrentQuest().getGoal());
				showQuestDescription();
				instance.getCurrentSession().getCurrentMapState().removeItem(Item.ARROW);
				instance.saveSession();
				instance.getCurrentSession().addAdditionalItems();
				refreshFunctions();
				refreshProgressBar();
				textArea.setText("");
				refreshView();
			}
		}
		else
			setErrorLabel(
				"Ce programme ne valide pas la quête...\nRecommence !",
				INTERPRETATION_ERR_COLOR
			);
	}

	private void showQuestDescription() {
		final String content = instance.getCurrentQuest().getDescription();
		final String header = instance.getCurrentChapter().getDescription();

		new UserAlertQuestDescription(content, header).showAndWait();
	}

	public void showDoAtEndMessage(final Pair<String, String> messageAndItemToCreateDoAtEnd) {
		if (messageAndItemToCreateDoAtEnd.getKey().equals("END")) {
			new UserAlertEndOfGame().showAndWait();
			instance.setIsOver(true);
			thisStage.close();
		}
		else {
			new UserAlertChapterAtEnd(
				"", messageAndItemToCreateDoAtEnd.getKey(),
				messageAndItemToCreateDoAtEnd.getValue()
			).showAndWait();
		}
		clearQuestList();
	}

	private void clearQuestList() {
		obsQuestList.clear();
	}

	@FXML
	public void reinitialize() {
		stopInterpreterThread();
		hideError();
		refreshMapState();
		mapGenerator.setMapState(instance.getCurrentMapState());
		refreshView();
	}

	private void stopInterpreterThread() {
		if (interpreterThread != null)
			interpreterThread.stop();
	}

	@FXML
	public void saveCode() {
		try {
			FileManager.saveCode(textArea.getText());

			setFileStatusLabel(true);
		}
		catch(Exception e) {
			setFileStatusLabel(false);
		}
	}

	@FXML
	public void loadCode() {
		try {
			final String code = FileManager.loadCodeFile();

			textArea.setText("");
			textArea.setText(code);
			setFileStatusLabel(true);
		}
		catch(Exception e) {
			setFileStatusLabel(false);
		}
	}

	private void setFileStatusLabel(final boolean succes) {
		if( succes ) {
			fileStatusLabel.setText("Succès !");
			fileStatusLabel.setStyle("-fx-text-fill:" + SUCCESS_COLOR);
		}
		else {
			fileStatusLabel.setText("Une erreur a eu lieu...");
			fileStatusLabel.setStyle("-fx-text-fill:" + LOGIC_ERR_COLOR);
		}
	}

	public void setThisStage(Stage stage) {
		this.thisStage = stage;
	}
}
