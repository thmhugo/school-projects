package codelab4.datamodel;

import java.util.ArrayList;
import java.util.Map.Entry;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import codelab4.commons.Tools;
import codelab4.datamodel.Chapter.DoAtEnd;
import codelab4.datamodel.MapState.Item;
import javafx.util.Pair;

public class Session {

	private String id;
	private Chapter currentChapter;
	private Quest currentQuest;
	private MapState currentMapState;
	private ArrayList<String> unlockedFunctions = new ArrayList<>();

	public Session(final JSONObject json) throws InstantiationException {
		try {
			readId(json);
			readChapter(json);
			readQuest(json);
			readMapState(json);
			readUnlockedFunctions(json);
		} catch (Exception e) {
			System.err.println(e.getMessage());
			throw new InstantiationException(
					"<EXCEPTION from Session> - Error loading from a JSON file."
			);
		}
	}

	private void readId(final JSONObject json) throws NullPointerException, JSONException {
		final String id = json.getString("id");

		if (id != null)
			this.id = id;
		else
			throw new NullPointerException(
					"<EXCEPTION from Session> - Can not read the session JSON '" + id + "'."
			);
	}

	private void readChapter(final JSONObject json) throws NullPointerException, JSONException {
		final String chapterId = json.getString("chapter_id");
		final Chapter chapter = Chapters.getChapter(chapterId);

		if (chapter != null)
			this.currentChapter = chapter;
		else
			throw new NullPointerException(
					"<EXCEPTION from Session> - Chapter '" + chapterId + "' does not exist."
			);
	}

	private void readQuest(final JSONObject json) throws JSONException {
		Quest quest = null;
		final String questId = json.getString("quest_id");

		if (currentChapter != null)
			quest = currentChapter.getQuest(questId);
		if (quest != null) {
			this.currentQuest = quest;
			this.unlockedFunctions = currentQuest.getFunctions();
		}
		else
			throw new NullPointerException(
					"<EXCEPTION from Session> - The quest '" + questId + "' does not exist."
			);
	}

	private void readUnlockedFunctions(final JSONObject json) throws JSONException {
		final JSONArray functions = json.getJSONArray("functions");

		unlockedFunctions = new ArrayList<String>();
		if (functions != null)
			for (int j = 0; j < functions.length(); j++)
				unlockedFunctions.add(functions.getString(j));
	}

	private void readMapState(final JSONObject json)
	throws JSONException, InstantiationException {
		final JSONObject mapStateJson = json.getJSONObject("map_state");
		final MapState mapState = new MapState(mapStateJson);

		this.currentMapState = mapState;
		this.currentMapState.getState().putAll(currentQuest.getAdditionalItems());
	}

	public void addAdditionalItems() {
		try {
			this.currentMapState.addAdditionalItems(
					Tools.hashMapWithArrayDeepCopy(currentQuest.getAdditionalItems())
			);
		}
		catch (NullPointerException | InstantiationException e) {
			System.err.println(e.getMessage());
		}
	}

	public JSONObject toJSON() throws JSONException {
		JSONObject json = new JSONObject();

		json.put("id", this.id);
		json.put("chapter_id", this.currentChapter.getId());
		json.put("quest_id", this.currentQuest.getId());
		json.put("map_state", this.currentMapState.toJSON());
		json.put("functions", this.unlockedFunctions);

		return json;
	}

	public Session(String id) {
		this(id, null, null, null);
	}

	public Session(
		String id,
		Chapter actualChapter,
		Quest actualQuest,
		MapState mapState
	) {
		this.id = id;
		this.currentChapter = actualChapter;
		this.currentQuest = actualQuest;
		this.currentMapState = mapState;
	}

	public boolean verify() {
		try {
			return currentQuest.test(currentMapState);
		}
		catch (JSONException e) {
			System.err.println(e.getMessage());
		}
		return false;
	}

	public Pair<String, String> updateToNextQuest() {
		Quest nextQuest = currentChapter.getNextQuest(currentQuest);
		final Chapter nextChapter = Chapters.getNextChapter(currentChapter);
		Pair<String,String> messageAndItemToCreateDoAtEnd = null;

		if ( nextQuest == null && nextChapter == null )
			return new Pair<>("END", "");

		else if ( nextQuest == null && nextChapter != null ) {
			messageAndItemToCreateDoAtEnd = executeChapterDoAtEnd();
			currentChapter = nextChapter;
			nextQuest = currentChapter.getQuests().get(0);
		}
		currentQuest = nextQuest;
		unlockFunctions();

		return messageAndItemToCreateDoAtEnd;
	}

	private Pair<String, String> executeChapterDoAtEnd() {
		final DoAtEnd doAtEnd = currentChapter.getDoAtEnd();

		if (doAtEnd != null) {
			removeRecipeItemsFromInventory(doAtEnd);
			addItemToCreate(doAtEnd);

			return new Pair<>(doAtEnd.getMessage(), doAtEnd.getItemToCreate().toString());
		}
		return null;
	}

	private void addItemToCreate(final DoAtEnd doAtEnd) {
		if ( doAtEnd.getPositionItemCreated() != null ) {
			try {
				currentMapState.addToState(
						doAtEnd.getItemToCreate(),
						doAtEnd.getPositionItemCreated()
						);
			}
			catch (InstantiationException e) {
				e.printStackTrace();
			}
		}
		else
			currentMapState.addInInventory(doAtEnd.getItemToCreate());
	}

	private void removeRecipeItemsFromInventory(final DoAtEnd doAtEnd) {
		for (final Entry<Item, Integer> entry : doAtEnd.getRecipe().entrySet())
			currentMapState.removeFromInventory(entry.getKey(), entry.getValue());
	}

	public void unlockFunctions() {
		if (currentQuest != null)
			for (final String str : currentQuest.getFunctions())
				if(!unlockedFunctions.contains(str))
					this.unlockedFunctions.add(str);
	}

	/**
	 * Get all goals of quests before actualQuest.
	 * @return the goals.
	 */
	public ArrayList<String> getQuestsGoal() {
		ArrayList<String> goals = new ArrayList<String>();
		boolean beforeCurrentQuest = false;

		for (int i = currentChapter.getNumberOfQuests() - 1; i >= 0 ; --i) {
			if ( currentChapter.getQuests().get(i).getId().equals(currentQuest.getId()) )
				beforeCurrentQuest = true;
			if ( beforeCurrentQuest )
				goals.add(currentChapter.getQuests().get(i).getGoal());
		}
		return goals;
	}

	public double getCurrentProgress() {
		final int currentQuestID = currentQuest.getIdNumber() -1;

		return currentQuestID / (double)currentChapter.getNumberOfQuests();
	}

	public double getGlobalProgress() {
		double cpt = 0;
		final int currentChapterID = currentChapter.getIdNumber();

		for (int i = 1; i <= Chapters.chapters.size(); i++) {
			if (currentChapterID == i)
				cpt += getCurrentProgress();
			else if (currentChapterID > i)
				cpt += 1;
		}

		return (cpt / Chapters.chapters.size()) * 100;
	}

	public String getId() {
		return id;
	}

	public void setId(String id) {
		this.id = id;
	}

	public Chapter getCurrentChapter() {
		return currentChapter;
	}

	public void setCurrentChapter(Chapter currentChapter) {
		this.currentChapter = currentChapter;
	}

	public Quest getCurrentQuest() {
		return currentQuest;
	}

	public MapState getMapState() {
		return currentMapState;
	}

	public void setMapState(MapState mapState) {
		this.currentMapState = mapState;
	}

	@Override
	public String toString() {
		return "Session [actualChapter=" + currentChapter + ", actualQuest="
				+ currentQuest + ", id=" + id + ", mapState=" + currentMapState + "]";
	}

	public String getPath() {
		return Sessions.SESSIONS_PATH + "/" + id + ".json";
	}

	public ArrayList<String> getUnlockedFunctions() {
		return this.unlockedFunctions;
	}

	public MapState getCurrentMapState() {
		return currentMapState;
	}
}
