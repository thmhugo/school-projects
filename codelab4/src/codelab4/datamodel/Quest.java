package codelab4.datamodel;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import codelab4.datamodel.MapState.Item;
import codelab4.datamodel.MapState.Position;

public class Quest {

	private static ArrayList<String> ids = new ArrayList<String>();

	private String id;
	private String description;
	private String goal;
	private ArrayList<String> functions;
	private JSONObject test;
	private JSONObject items;
	private int instructionCount;
	private HashMap<Item, ArrayList<Position>> additionalItems;

	public Quest(
		String id,
		String description,
		String goal,
		ArrayList<String> functions,
		JSONObject test,
		int instructionCount,
		JSONObject items
	) throws InstantiationException {
		initID(id);
		this.description = description;
		this.goal = goal;
		this.functions = functions;
		this.test = test;
		this.items = items;
		this.instructionCount = instructionCount;
		if (items!=null)
			this.additionalItems = initAdditionalItems();
	}

	public Quest(
		String id,
		String description,
		String goal,
		ArrayList<String> functions,
		JSONObject test
	) throws InstantiationException {
		this(id, description, goal, functions, test, -1, null);
	}

	private void initID(final String id) throws InstantiationException {
		if (isUnique(id)) {
			ids.add(id);
			this.id = id;
		} else {
			throw new InstantiationException(
				"<EXCEPTION from Quest> - Quest id = " + id + " isn't unique."
			);
		}
	}

	private static boolean isUnique(final String id) {
		return !ids.contains(id);
	}

	public static void clearIds() {
		ids.clear();
	}

	public boolean test(final MapState mapState) throws JSONException {
		final String item = test.getString("item");

		switch (item) {
			case "CHARACTER": return testCharacter(mapState);
			case "HUNGER": return testHunger(mapState);
			default: return testDefault(item, mapState);
		}
	}

	private boolean testHunger(final MapState mapState) throws JSONException {
		final int expectedHunger = test.getInt("expected");

		return mapState.getHunger() <= expectedHunger;
	}

	private boolean testCharacter(final MapState mapState) throws JSONException {
		final String[] pos = test.getString("expected").split(",");

		return mapState.isInCorrectLocation(pos);
	}

	private boolean testDefault(final String item, final MapState mapState)
	throws JSONException {
		final int expected = test.getInt("expected");

		return mapState.isInInventory(item, expected);
	}

	public static ArrayList<Quest> createQuests(final JSONArray json)
	throws JSONException, InstantiationException {
		ArrayList<Quest> quests = new ArrayList<Quest>();

		for (int i = 0; i < json.length(); i++)
			quests.add(Quest.createQuest(json.getJSONObject(i)));

		return quests;
	}

	public static Quest createQuest(final JSONObject json)
	throws JSONException, InstantiationException {
		final ArrayList<String> functions = getJSONFunctionValues(json);
		final JSONObject test = getJSONTestValues(json);
		final JSONObject items = getJSONItems(json);
		final int count = getInstructionCount(json);

		return new Quest (
					json.getString("id"),
					json.getString("description"),
					json.getString("goal"),
					functions, test, count, items
				);
	}

	private static ArrayList<String> getJSONFunctionValues(final JSONObject json)
	throws JSONException {
		ArrayList<String> functions = new ArrayList<String>();
		final JSONArray functionsQuest = json.getJSONArray("function");

		for (int j = 0; j < functionsQuest.length(); j++)
			functions.add(functionsQuest.getString(j));

		return functions;
	}

	private HashMap<Item, ArrayList<Position>> initAdditionalItems() {
		HashMap<Item, ArrayList<Position>> states = new HashMap<Item, ArrayList<Position>>();
		final Iterator iterator = this.items.keys();

		while (iterator.hasNext()) {
			final String key = (String)iterator.next();
			final Item item = Item.fromString(key);
			final ArrayList<Position> itemPositions = getPositionsFromJSONObject(this.items,key);

			states.put(item, itemPositions);
		}
		return states;
	}

	private ArrayList<Position> getPositionsFromJSONObject(
		final JSONObject json, final String key
	) {
		ArrayList<Position> listOfPosition = new ArrayList<Position>();
		ArrayList<JSONArray> listOfPositionsJSON  = new ArrayList<JSONArray>();
		JSONArray itemPos;

		try {
			itemPos = json.getJSONArray(key);

			if (itemPos != null)
				listOfPositionsJSON = getListOfPositionsFromJSONArray(itemPos);

			if (listOfPosition!=null)
				listOfPosition = getListOfPosition(listOfPositionsJSON);

		}
		catch (JSONException e) {
			System.err.println(e.getMessage());
		}
		return listOfPosition;
	}

	private ArrayList<Position> getListOfPosition(
		final ArrayList<JSONArray> listOfPositionsJSON
	) throws JSONException {
		ArrayList<Position> listOfPosition = new ArrayList<Position>();

		for (final JSONArray j : listOfPositionsJSON)
			listOfPosition.add(new Position(j.getInt(0),j.getInt(1)));

		return listOfPosition;
	}

	private ArrayList<JSONArray> getListOfPositionsFromJSONArray(final JSONArray itemPos)
	throws JSONException {
		ArrayList<JSONArray> listOfPositionsJSON  = new ArrayList<JSONArray>();

		for (int i=0;i<itemPos.length();i++)
			listOfPositionsJSON.add(itemPos.getJSONArray(i));

		return listOfPositionsJSON;
	}

	private static JSONObject getJSONTestValues(final JSONObject json)
 	throws JSONException {
		return json.getJSONObject("test");
	}

	private static JSONObject getJSONItems(final JSONObject json)
	throws JSONException {
		return json.getJSONObject("items");
	}

	public String getExpectedResult() throws JSONException {
		return test.get("expected").toString();
	}

	private static int getInstructionCount(final JSONObject json) {
		try {
			return json.getInt("nb_instructions_max");
		}
		catch (Exception e) {}
		return -1;
	}

	public String getId() {
		return id;
	}

	public int getIdNumber() {
		return Integer.parseInt(String.valueOf(id.charAt(1)));
	}

	public String getDescription() {
		return description;
	}

	public String getGoal() {
		return goal;
	}

	public ArrayList<String> getFunctions() {
		return functions;
	}

	public JSONObject getTest() {
		return test;
	}

	public JSONObject getItems() {
		return items;
	}

	public int getInstructionCount(){
		return this.instructionCount;
	}

	public HashMap<Item, ArrayList<Position>> getAdditionalItems() {
		return additionalItems;
	}
}
