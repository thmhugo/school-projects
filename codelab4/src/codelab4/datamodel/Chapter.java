package codelab4.datamodel;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Scanner;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import codelab4.commons.Tools;
import codelab4.datamodel.MapState.Item;
import codelab4.datamodel.MapState.Position;;

public class Chapter {

	private static ArrayList<String> ids = new ArrayList<String>();

	private String id;
	private ArrayList<Quest> quests;
	private String description;
	private DoAtEnd doAtEnd;

	public static class DoAtEnd {

		private Item itemToCreate;
		private HashMap<Item,Integer> recipe;
		private Position positionItemCreated;
		private String message;

		public DoAtEnd(JSONObject doAtEndJSON)
		throws JSONException, InstantiationException {
			this.itemToCreate = getItemToCreateFrom(doAtEndJSON);
			this.recipe = getRecipeFrom(doAtEndJSON);
			this.message = getMessageFrom(doAtEndJSON);
			this.positionItemCreated = getPositionFrom(doAtEndJSON);
		}

		private Item getItemToCreateFrom(JSONObject doAtEndJSON)
		throws JSONException, InstantiationException {
			String itemString = doAtEndJSON.getString("item_to_create");
			Item itemToCreate = Item.fromString(itemString);

			if ( itemToCreate == null ) {
				throw new InstantiationException(
							"<EXCEPTION from Chapter.DoAtEnd> - Item '" + itemString
							+ "' does not exists."
						);
			}
			return itemToCreate;
		}

		private HashMap<Item, Integer> getRecipeFrom(JSONObject doAtEndJSON)
		throws JSONException, InstantiationException {
			HashMap<Item,Integer> recipe = new HashMap<>();
			JSONObject recipeJSON = doAtEndJSON.getJSONObject("recipe");
			Iterator<String> keys = recipeJSON.keys();

			while ( keys.hasNext() ) {
				String key = keys.next();
				Item item = Item.fromString(key);
				Integer numberOf = recipeJSON.getInt(key);

				if ( item != null && numberOf != null )
					recipe.put(item, numberOf);
				else {
					throw new InstantiationException(
								"<EXCEPTION from Chapter.DoAtEnd> - An error occured during"
								+ " instantiation."
							);
				}
			}
			return recipe;
		}

		private String getMessageFrom(JSONObject doAtEndJSON)
		throws InstantiationException, JSONException {
			final String message = doAtEndJSON.getString("message");

			if ( message == null || message.isEmpty() )
				throw new InstantiationException(
					"<EXCEPTION from Chapter.DoAtEnd> - An error occured during"
					+ " instantiation."
				);

			return message;
		}

		private Position getPositionFrom(JSONObject doAtEndJSON)
		throws InstantiationException, JSONException {
			JSONArray pos = null;

			if ( !itemToCreate.isDisplayable() )
				return null;

			try { pos = (JSONArray) doAtEndJSON.get("position"); }
			catch (JSONException jse) { return null; }

			return readPosition(pos);
		}

		private Position readPosition(final JSONArray pos)
		throws JSONException, InstantiationException {
			final Position position = new Position((int)pos.get(0), (int)pos.get(1));

			if (position == null || !position.isInMap()) {
				throw new InstantiationException(
							"<EXCEPTION from Chapter.DoAtEnd> - Position '" + position
							+ "' is incorrect."
						);
			}
			return position;
		}

		public Item getItemToCreate() {
			return itemToCreate;
		}

		public HashMap<Item, Integer> getRecipe() {
			return recipe;
		}

		public String getMessage() {
			return message;
		}

		public Position getPositionItemCreated() {
			return positionItemCreated;
		}
	}

	public static boolean isAValidFileName(final String fileName) {
		Scanner scan = new Scanner(fileName);

		scan.useDelimiter("[.]");
		if ( scan.hasNext() ) {
			final String name = scan.next();

			if ( name.length() < 7 || !name.substring(0,7).equals("chapter") ) {
				scan.close();
				return false;
			}
			if ( !Tools.isAnInteger(name.substring(7)) ) {
				scan.close();
				return false;
			}
			if ( scan.hasNext() ) {
				final String extension = scan.next();
				final boolean hasNext = scan.hasNext();

				scan.close();
				return extension.equals("json") && !hasNext;
			}
		}
		scan.close();

		return false;
	}

	public Chapter(String id, ArrayList<Quest> quests, String description)
	throws InstantiationException {
		this(id, quests, description, null);
	}

	public Chapter(String id, ArrayList<Quest> quests, String description, DoAtEnd doAtEnd)
	throws InstantiationException {
		initID(id);
		this.quests = quests;
		this.description = description;
		this.doAtEnd = doAtEnd;
	}

	private void initID(final String id) throws InstantiationException {
		if ( isUnique(id) ) {
			ids.add(id);
			this.id = id;
		}
		else {
			throw new InstantiationException(
				"<EXCEPTION from Chapter> - Chapter id = " + id + " isn't unique."
			);
		}
	}

	public void sortQuest() {
		quests.sort(
			(quest1, quest2) -> {
				return quest1.getId().compareTo(quest2.getId());
			}
		);
	}

	private static boolean isUnique(final String id) {
		return !ids.contains(id);
	}

	public static void clearIds() {
		Quest.clearIds();
		ids.clear();
	}

	public String getId() {
		return id;
	}

	public int getIdNumber() {
		return Integer.parseInt(String.valueOf(id.charAt(1)));
	}

	public ArrayList<Quest> getQuests() {
		return quests;
	}

	public Quest getQuest(final String id) {
		for (final Quest q : quests)
			if ( q.getId().equals(id) )
				return q;

		return null;
	}

	public Quest getNextQuest(final Quest quest) {
		for (int i = 0; i < quests.size() - 1; ++i)
			if ( quests.get(i).getId().equals(quest.getId()) )
				return quests.get(i+1);

		return null;
	}

	public int getNumberOfQuests(){
		return quests.size();
	}

	public String getDescription() {
		return description;
	}

	public Quest getFirstQuest() {
		return quests.get(0);
	}

	public DoAtEnd getDoAtEnd() {
		return doAtEnd;
	}
}
