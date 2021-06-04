package codelab4.datamodel;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map.Entry;
import java.util.Random;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import codelab4.commons.Tools;
import codelab4.filemanager.FileManager;

public class MapState {

	public static final int MAP_WIDTH = 16;
	public static final int MAP_HEIGHT = 16;

	public static final String STARTING_MAP_STATE = "./src/resources/json/map/start.json";

	private HashMap<Item, ArrayList<Position>> state;
	private HashMap<Item, Integer> inventory;
	private HashMap<Position, Item> positionsHashMap;

	/**
	 * DIRECTION represents the character's orientation and its position equals :
	 * NORD = (0, -1) WEST = (-1, 0) EAST = (1, 0) SOUTH = (0, 1)
	 */
	public enum Item {

		WOOD 		(true, true),
		LEAF		(true, true),
		FRUIT		(true, true),
		BRANCH		(true, true),
		STONE		(true, true),
		TREE		(true, false),
		CHARACTER 	(true, false),
		ARROW		(true, false),
		SHELTER		(true, false),
		HUNGER		(false, false),
		AXE			(false, false),
		DIRECTION	(false, false);

		public static final int MAX_HUNGER = 7;

		private Boolean isRotten = null;

		private boolean isDisplayable;
		private boolean isPickable;

		Item(boolean isDisplayable, boolean isPickable) {
			this.isDisplayable = isDisplayable;
			this.isPickable = isPickable;
		}

		public boolean isDisplayable() {
			return isDisplayable;
		}

		public boolean isPickable() {
			return isPickable;
		}

		public static Item fromString(final String itemString) {
			for (final Item item : Item.values())
				if ( item.toString().equals(itemString.toUpperCase()) )
					return item;

			return null;
		}

		public void setIsRotten(Boolean state) {
			if (this == Item.FRUIT)
				this.isRotten = state;
		}

		public Boolean getIsRotten() {
			return this.isRotten;
		}

		public static int getMaxHunger(){
			return MAX_HUNGER;
		}
	}

	public static final Position NORTH = new Position(0, -1);
	public static final Position WEST = new Position(-1, 0);
	public static final Position EAST = new Position(1, 0);
	public static final Position SOUTH = new Position(0, 1);

	/**
	 * Represents the coordinates of an Item on the Map.
	 */
	public static class Position implements Cloneable {

		private int x;
		private int y;

		public static Position add(final Position p1, final Position p2) {
			assert p1 != null && p2 != null;
			return new Position(p1.x + p2.x, p1.y + p2.y);
		}

		public static boolean isValidDirection(final Position pos) {
			return (pos.equals(NORTH) || pos.equals(EAST) || pos.equals(WEST) || pos.equals(SOUTH));
		}

		public Position(int x) {
			this(x, x);
		}

		public Position(int x, int y) {
			this.x = x;
			this.y = y;
		}

		@Override
		public boolean equals(final Object o) {
			if (o instanceof Position)
				return ((Position) o).x == x && ((Position) o).y == y;
			return false;
		}

		/**
		 * 	 In order to compare (and use equals) 2 objects in a HashMap, we need to
		 *   Override hashCode().
		 * 	 See Joshua Bloch's Effective Java for more informations about 31.
		 */
		@Override
		public int hashCode() {
			return x * 31 + y;
		}

		@Override
		public String toString() {
			return "Pos=[" + x + ", " + y + "]";
		}

		@Override
		public Position clone() {
			return new Position(x, y);
		}

		public boolean isInMap() {
			return x >= 0 && x < MAP_WIDTH - 2 && y >= 0 && y < MAP_HEIGHT - 2;
		}

		public int getX() {
			return x;
		}

		public int getY() {
			return y;
		}

		public void turnRight() {
			if (x == 0) {
				if (y == -1) // NORTH
					x = 1;
				else // SOUTH
					x = -1;
				y = 0;
			} else {
				if (x == -1) // EAST
					y = -1;
				else // WEST
					y = 1;
				x = 0;
			}
		}

		public void turnLeft() {
			if (x == 0) {
				if (y == -1) // NORTH
					x = -1;
				else // SOUTH
					x = 1;
				y = 0;
			} else {
				if (x == -1) // EAST
					y = 1;
				else // WEST
					y = -1;
				x = 0;
			}
		}

		public JSONArray toJSON() {
			final JSONArray positionJSON = new JSONArray();

			positionJSON.put(x);
			positionJSON.put(y);

			return positionJSON;
		}
	}

	/**
	 * Return the MapState initiliazed from the file '/json/map/start.json'.
	 */
	public static MapState createStartingMapState() {
		JSONObject startJSON;

		try {
			startJSON = FileManager.getJson(STARTING_MAP_STATE);

			return new MapState(startJSON);
		}
		catch (final Exception e) {
			System.err.println(e.getMessage());
		}
		return null;
	}

	public MapState() {
		this(new HashMap<Item, ArrayList<Position>>(), new HashMap<Item, Integer>());
	}

	public MapState(MapState mapState) {
		this(mapState.getState(), mapState.getInventory());
	}

	public MapState(
		HashMap<Item, ArrayList<Position>> state,
		HashMap<Item, Integer> inventory
	) {
		this.state = Tools.hashMapWithArrayDeepCopy(state);
		this.inventory = new HashMap<>(inventory);
		initPositionHashMap();
	}

	public MapState(JSONObject json) throws InstantiationException {
		this();
		try {
			Tools.applyToJSONKeys(json.getJSONObject("state"),
				(String key, JSONArray jsonArray) -> {
					addToState(key, jsonArray);
			});
			Tools.applyToJSONKeys(json.getJSONObject("inventory"),
				(String key, Integer value) -> {
					addToInventory(key, value);
			});
			verifyData();
		}
		catch (JSONException e) {
			throw new InstantiationException(
				"<EXCEPTION from MapState> - JSONException thrown."
			);
		}
		initPositionHashMap();
	}

	/**
	 * Verifies that state contains the DIRECTION and CHARACTER item and that both
	 * of them get only one Position.
	 * @throws InstantiationException if is not the case.
	 */
	private void verifyData() throws InstantiationException {
		if (!state.containsKey(Item.DIRECTION) || !state.containsKey(Item.CHARACTER))
			throw new InstantiationException(
				"<EXCEPTION from MapState> - CHARACTER or DIRECTION item is missing."
			);
		else if (
			state.get(Item.DIRECTION).size() != 1 || state.get(Item.CHARACTER).size() != 1
		)
			throw new InstantiationException(
				"<EXCEPTION from MapState> - Not unique Position for CHARACTER or DIRECTION."
			);
	}

	private void addToState(String itemString, JSONArray positions)
	throws JSONException, NullPointerException, InstantiationException {
		for (int i = 0; i < positions.length(); ++i) {
			JSONArray pos = (JSONArray) positions.get(i);
			Position position = new Position((int)pos.get(0), (int)pos.get(1));

			addToState(Item.fromString(itemString), position);
		}
	}

	public void addToState(Item item, Position pos) throws InstantiationException {
		if (pos.isInMap() || (item == Item.DIRECTION && Position.isValidDirection(pos))) {
			if (!positionsHashMap.containsKey(pos)  || item == Item.HUNGER || item == Item.SHELTER) {
				if ( state.containsKey(item) )
					state.get(item).add(pos);
				else
					state.put(item, Tools.createArrayListWith(pos));
				positionsHashMap.put(pos, item);
			}
			else
				throw new InstantiationException(
					"<EXCEPTION from MapState> - Duplicate position = " + pos.toString()
				);
		}
		else
			throw new InstantiationException(
				"<EXCEPTION from MapState> - Position out of boundary."
			);
	}

	private void addToInventory(final String key, int value)
    throws NullPointerException, InstantiationException {
		Item item = Item.fromString(key);

		if (item != null) {
			if (value < 0)
				throw new InstantiationException(
					"<EXCEPTION from MapState> - Invalid inventory value : " + value
				);
			while (value-- > 0)
				addInInventory(item);
		}
		else
			throw new NullPointerException(
				"<EXCEPTION from MapState> - Invalid inventory key : " + key
			);
	}

	public static boolean areEqual(final MapState m1, final MapState m2) {
		boolean statesEqual = m1.getState().equals(m2.getState());
		boolean inventoriesEqual = m1.getInventory().equals(m2.getInventory());

		return statesEqual && inventoriesEqual;
	}

	public boolean contains(final Position pos) {
		return positionsHashMap.containsKey(pos);
	}

	public Item getItemAt(final Position pos) {
		return positionsHashMap.get(pos);
	}

	public void addInInventory(final Item item) {
		addInInventory(item, 1);
	}

	public void addInInventory(final Item item, final int nb) {
		Integer tmp = 0;

		if (inventory.containsKey(item))
			tmp = inventory.get(item);

		tmp += nb;
		inventory.put(item, tmp);
	}

	public boolean isInInventory(final String item, final int expected) {
		if (inventory.size() > 0) {
			final Item searchedItem = Item.valueOf(item);

			return inventory.get(searchedItem) == null ? false :
			inventory.get(searchedItem) >= expected;
		}
		return false;
	}

	public boolean isInCorrectLocation(final String[] pos) {
		final Position pExpected = new Position(
									Integer.parseInt(pos[0]),
									Integer.parseInt(pos[1])
								);
		final Position pActual = getCharacterPos();

		return pActual.equals(pExpected);
	}

	public JSONObject toJSON() {
		JSONObject json = new JSONObject();

		try {
			json.put("state", stateToJSON());
			json.put("inventory", inventoryToJSON());
		}
		catch (final JSONException e) {
			System.err.println(
				"<EXCEPTION from MapState> - Error converting MapSate into a JSONObject."
			);
		}

		return json;
	}

	private JSONObject stateToJSON() throws JSONException {
		JSONObject stateJSON = new JSONObject();

		for (final Entry<Item, ArrayList<Position>> entry : state.entrySet())
			stateJSON.put(
				entry.getKey().toString().toLowerCase(),
				positionsToJSONArray(entry.getValue())
			);

		return stateJSON;
	}

	private JSONArray positionsToJSONArray(final ArrayList<Position> positions) {
		JSONArray positionsJSON = new JSONArray();

		for (final Position p : positions)
			positionsJSON.put(p.toJSON());

		return positionsJSON;
	}

	private JSONObject inventoryToJSON() throws JSONException {
		JSONObject inventoryJSON = new JSONObject();

		for (final Entry<Item, Integer> entry : inventory.entrySet())
			inventoryJSON.put(entry.getKey().toString().toLowerCase(), entry.getValue());

		return inventoryJSON;
	}

	public Item getInventoryKey(final Item item) {
		for (final Item entry : inventory.keySet())
			if (item == entry)
				return entry;

		return null;
	}

	private void setIsRotten(Boolean state) {
		for (final Item entry : inventory.keySet())
			if (Item.FRUIT == entry)
				entry.setIsRotten(state);
	}

	public boolean removeFruitFromInv() {
		final Item item = getInventoryKey(Item.FRUIT);
		final Integer nbInventoryFruit = inventory.get(Item.FRUIT);

		if (nbInventoryFruit != null && nbInventoryFruit > 0 && item != null) {
			inventory.put(Item.FRUIT, nbInventoryFruit - 1);
			setIsRotten(null);

		 	return true;
		}
		return false;
	}

	public boolean removeFromInventory(final Item item, final Integer nbToRemove) {
		final Integer currentNb = inventory.get(item);

		if (currentNb != null && currentNb >= nbToRemove) {
			inventory.put(item, currentNb - nbToRemove);
			if ( inventory.get(item) == 0 )
				inventory.remove(item);

			return true;
		}
		return false;
	}

	public void isGoingRotten() {
		final Integer nbInventoryFruit = inventory.get(Item.FRUIT);

		if (nbInventoryFruit != null && nbInventoryFruit > 0) {
			final Random rand = new Random();
			final int randomNumb = rand.nextInt(10);

			setIsRotten(false);

			if (randomNumb < 4)
				setIsRotten(true);
		}
	}

	public boolean eatFruit() {
		final Integer nbInventoryFruit = inventory.get(Item.FRUIT);

		if ( nbInventoryFruit != null && nbInventoryFruit > 0 ) {
			if ( getInventoryKey(Item.FRUIT).getIsRotten() && !incrementHunger() )
				return false;
			else if ( !getInventoryKey(Item.FRUIT).getIsRotten() && !decrementHunger() )
				return false;

			inventory.put(Item.FRUIT, inventory.get(Item.FRUIT) - 1);
			setIsRotten(null);

			return true;
		}
		return false;
	}

	public boolean incrementHunger() {
		ArrayList<Position> hunger = state.get(Item.HUNGER);

		if ( hunger == null )
			return false;

		hunger.get(0).x += 1;

		return true;
	}

	public boolean decrementHunger() {
		ArrayList<Position> hunger = state.get(Item.HUNGER);

		if ( hunger == null )
			return false;

		if ( getHunger() >= 3 )
			hunger.get(0).x -= 3;

		return true;
	}

	public int getHunger() {
		return state.get(Item.HUNGER).get(0).x;
	}

	public Boolean hasRotten() {
		final Integer nbInventoryFruit = inventory.get(Item.FRUIT);

		if (nbInventoryFruit != null && nbInventoryFruit > 0)
			return getInventoryKey(Item.FRUIT).getIsRotten();

		return null;
	}

	private void initPositionHashFrom(final ArrayList<Position> positions, final Item key) {
		for (final Position p : positions)
			if (!positionsHashMap.containsKey(p) || key.equals(Item.DIRECTION))
				positionsHashMap.put(p, key);
	}

	/**
	 * 	 Initialize postionHashMap with state. It verifies that every position
	 *   is unique and there is no superposition between 2 items.
	 * 	 Print a error message for all superpositions.
	 */
	public void initPositionHashMap() {
		this.positionsHashMap = new HashMap<Position, Item>();

		for (final Entry<Item, ArrayList<Position>> entry : state.entrySet())
			initPositionHashFrom(entry.getValue(), entry.getKey());
	}

	/**
	 * An additionnal item is added to state only if the position (linked to the item)
	 * is not in positionHashMap (contains unique position for different items).
	 * Print a error message for all superpositions.
	 * @param addtionalState is the HashMap additionalItems from Quest.
	 * @throws InstantiationException
	 * @throws NullPointerException
	 */
	public void addAdditionalItems(
		final HashMap<Item, ArrayList<Position>> addtionalState
	) throws NullPointerException, InstantiationException {
		for (final Entry<Item, ArrayList<Position>> entry : addtionalState.entrySet())
			for (final Position p : entry.getValue())
				addToState(entry.getKey(), p);
	}

	public void remove(final Item item, final Position pos) {
		state.get(item).remove(pos);
		positionsHashMap.remove(pos);
	}

	public void removeItem(final Item itemToRemove) {
		if ( state.get(itemToRemove) != null ) {
			final ArrayList<Position> positionsToRemove = new ArrayList<>(
																state.get(itemToRemove)
															);

			for (final Position pos : positionsToRemove)
				remove(itemToRemove, pos);
		}
	}

	public void setCharacterPos(Position newPos) {
		positionsHashMap.remove(getCharacterPos());
		positionsHashMap.put(newPos, Item.CHARACTER);
		state.get(Item.CHARACTER).set(0, newPos);
	}

	public Position getCharacterPos() {
		return state.get(Item.CHARACTER).get(0);
	}

	public Position getDirectionPos() {
		return state.get(Item.DIRECTION).get(0);
	}

	public HashMap<Item, ArrayList<Position>> getState() {
		return state;
	}

	public HashMap<Position, Item> getPositionHashMap() {
		return positionsHashMap;
	}

	public void setState(HashMap<Item, ArrayList<Position>> state) {
		this.state = state;
	}

	public HashMap<Item,Integer> getInventory() {
		return inventory;
	}

	public void setInventory(HashMap<Item,Integer> inventory) {
		this.inventory = inventory;
	}

	@Override
	public String toString() {
		return "MapState [inventory=" + inventory + ", state=" + state + "]";
	}
}
