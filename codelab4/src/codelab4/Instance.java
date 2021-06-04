package codelab4;

import codelab4.datamodel.Chapter;
import codelab4.datamodel.MapState;
import codelab4.datamodel.Quest;
import codelab4.datamodel.Session;
import codelab4.datamodel.Sessions;
import javafx.util.Pair;

public class Instance {

	private MainWindowController mainController;
	private Session currentSession;
	private MapState notModifiedMapState;
	// It's set to true when the last quest is done.
	private boolean isOver;

	public Instance() {}

	public Instance(MainWindowController mainController, Session session) {
		this.currentSession = session;
		this.mainController = mainController;
		this.notModifiedMapState = session.getMapState();
		this.isOver = false;
	}

	public boolean verify() {
		return currentSession.verify();
	}

	public void updateToNextQuest() {
		final Pair<String, String> messageAndItemToCreateAtEnd;

		messageAndItemToCreateAtEnd = currentSession.updateToNextQuest();
		this.notModifiedMapState = currentSession.getMapState();
		if ( messageAndItemToCreateAtEnd != null )
			mainController.showDoAtEndMessage(messageAndItemToCreateAtEnd);
	}

	public void refreshView() {
		mainController.refreshView();
	}

	public Session getCurrentSession() {
		return currentSession;
	}

	public void setCurrentSession(Session currentSession) {
		this.currentSession = currentSession;
	}

	public MapState getCurrentMapState() {
		return currentSession.getMapState();
	}

	public MapState getNotModifiedMapState() {
		return this.notModifiedMapState;
	}

	public Quest getCurrentQuest() {
		return currentSession.getCurrentQuest();
	}

	public Chapter getCurrentChapter() {
		return currentSession.getCurrentChapter();
	}

	public void setCurrentMapState(MapState currentMapState) {
		currentSession.setMapState(currentMapState);
	}

	public MainWindowController getMainController() {
		return mainController;
	}

	public void saveSession() {
		if ( Sessions.save(currentSession) ) {
			System.out.println(
				"<INFO> - Session '" + currentSession.getId() + "' saved."
			);
		}
		else {
			System.out.println(
				"<EXCEPTION> - The session '" + currentSession.getId()
				+ "' could not be saved."
			);
		}
	}

	public boolean getIsOver() {
		return isOver;
	}

	public void setIsOver(boolean isOver) {
		this.isOver = isOver;
	}
}
