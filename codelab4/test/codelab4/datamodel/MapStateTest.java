package codelab4.datamodel;

import static org.junit.Assert.*;

import java.net.URL;
import java.util.ArrayList;
import java.util.HashMap;
import org.json.JSONObject;
import org.junit.*;
import codelab4.commons.Tools;
import codelab4.datamodel.MapState.*;
import codelab4.filemanager.FileManager;

public class MapStateTest {

	private MapState map;

	@Before
	public void init() {
		map = new MapState();
	}

	@After
	public void clean() {
	}

	@Test
	public void testConstructor() {
		ArrayList<Position> pos1 = new ArrayList<>();
		ArrayList<Position> pos2 = new ArrayList<>();

		pos1.add(new Position(2, 2));
		pos2.add(new Position(0, 0));
		map = createMapState();

		assertEquals(2, (int) map.getInventory().get(Item.LEAF));
		assertEquals(1, (int) map.getInventory().get(Item.FRUIT));

		assertTrue(map.getState().get(Item.CHARACTER).equals(pos1));
		assertTrue(map.getState().get(Item.DIRECTION).equals(pos2));
	}

	@Test
	public void testAreEqual() {
		MapState m1 = createMapState();
		MapState m2 = createMapState();

		assertTrue(
			"Map states should be equal.", MapState.areEqual(m1, m2)
		);

		m2.getState().get(Item.DIRECTION).add(new Position(1,1));
		assertFalse(
			"Map states should not be equal.", MapState.areEqual(m1, m2)
		);
		
		m2.getState().get(Item.DIRECTION).remove(new Position(1,1));
		m2.getInventory().put(Item.WOOD, 2);
		assertFalse(
			"Map states should not be equal.", MapState.areEqual(m1, m2)
		);
	}

	static MapState createMapState() {
		HashMap<MapState.Item, Integer> inventory = new HashMap<>();
		inventory.put(Item.LEAF, 2);
		inventory.put(Item.FRUIT, 1);
		inventory.put(Item.WOOD, 1);

		HashMap<MapState.Item, ArrayList<Position>> state = new HashMap<>();
		ArrayList<Position> pos1 = new ArrayList<>();
		ArrayList<Position> pos2 = new ArrayList<>();

		pos1.add(new Position(2, 2));
		pos2.add(new Position(0, 0));

		state.put(Item.CHARACTER, pos1);
		state.put(Item.DIRECTION, pos2);

		return new MapState(state, inventory);
	}

	@Test
	public void testFromJSON() throws Exception {
		JSONObject correctJSON = loadTestJSONObject("1");
		MapState mapStateExpected = createFirstMapStateExpected(false);
		MapState mapStateFromJson = new MapState(correctJSON);

		assertTrue(
			"MapStates should be equal.",
			MapState.areEqual(mapStateExpected, mapStateFromJson)
		);
	}

	private JSONObject loadTestJSONObject(String num) throws Exception {
		URL url = getClass().getResource("/json/map-states/map-state-"+num+"-test.json");
		return FileManager.getJson(url.getPath());
	}

	@Test
	public void testFromJSON2() throws Exception {
		JSONObject emptyInventoryJSON = loadTestJSONObject("2");
		MapState mapStateExpected = createFirstMapStateExpected(true);
		MapState mapStateFromJson = new MapState(emptyInventoryJSON);

		assertTrue(
			"MapStates should be equal.",
			MapState.areEqual(mapStateExpected, mapStateFromJson)
		);
	}

	@Test(expected = InstantiationException.class)
	public void testFromJSON3() throws Exception {
		JSONObject positionsOutBoundaryJSON = loadTestJSONObject("3");
		new MapState(positionsOutBoundaryJSON);
	}

	@Test(expected = InstantiationException.class)
	public void testFromJSON4() throws Exception {
		JSONObject noneDirectionAttributeJSON = loadTestJSONObject("4");
		new MapState(noneDirectionAttributeJSON);
	}
	
	@Test(expected = InstantiationException.class)
	public void testFromJSON5() throws Exception {
		JSONObject noneCharacterAttributeJSON = loadTestJSONObject("5");
		new MapState(noneCharacterAttributeJSON);
	}
	
	@Test(expected = InstantiationException.class)
	public void testFromJSON6() throws Exception {
		JSONObject moreThanOneCharacterPositionJSON = loadTestJSONObject("6");
		new MapState(moreThanOneCharacterPositionJSON);
	}
	
	@Test(expected = InstantiationException.class)
	public void testFromJSON7() throws Exception {
		JSONObject noneCharacterPositionJSON = loadTestJSONObject("7");
		new MapState(noneCharacterPositionJSON);
	}

	@Test(expected = InstantiationException.class)
	public void testFromJSON8() throws Exception {
		JSONObject moreThanOneDirectionPositionJSON = loadTestJSONObject("8");
		new MapState(moreThanOneDirectionPositionJSON);
	}
	
	@Test(expected = InstantiationException.class)
	public void testFromJSON9() throws Exception {
		JSONObject noneDirectionPositionJSON = loadTestJSONObject("9");
		new MapState(noneDirectionPositionJSON);
	}
	
	@Test(expected = InstantiationException.class)
	public void testFromJSON10() throws Exception {
		JSONObject nonValidDirectionPositionJSON = loadTestJSONObject("10");
		new MapState(nonValidDirectionPositionJSON);
	}
	
	@Test(expected = InstantiationException.class)
	public void testFromJSON11() throws Exception {
		JSONObject positionsOutBoundary2JSON = loadTestJSONObject("11");
		new MapState(positionsOutBoundary2JSON);
	}
	
	@Test(expected = InstantiationException.class)
	public void testFromJSON12() throws Exception {
		JSONObject inventoryNoneValidValuesJSON = loadTestJSONObject("12");
		new MapState(inventoryNoneValidValuesJSON);
	}

	@Test
	public void testUpdateHunger() {
		ArrayList<Position> pos = new ArrayList<>();
		pos.add(new Position(0));
		HashMap<Item, ArrayList<Position>> state = new HashMap<>();
		state.put(Item.HUNGER, pos);

		MapState mapState = new MapState(state, new HashMap<Item, Integer>());

		assertTrue(
			"Hunger should be updated. #1",
			mapState.incrementHunger()
		);

		assertEquals(mapState.getHunger(), 1);
	}
	
	@Test
	public void testUpdateHunger2() {
		MapState mapState = new MapState(
								new HashMap<Item, ArrayList<Position>>(),
								new HashMap<Item, Integer>()
							);

		assertFalse(
			"Hunger should not be updated.",
			mapState.incrementHunger()
		);
	}

	@Test
	public void testUpdateHunger3() {
		ArrayList<Position> pos = new ArrayList<>();
		pos.add(new Position(5));
		HashMap<Item, ArrayList<Position>> state = new HashMap<>();
		state.put(Item.HUNGER, pos);

		MapState mapState = new MapState(state, new HashMap<Item, Integer>());

		assertTrue(
			"Hunger should be updated. #2",
			mapState.incrementHunger()
		);

		assertEquals(mapState.getHunger(), 6);
	}

	@Test
	public void testItemFromString() {
		assertEquals(Item.fromString("HUNGER"), Item.HUNGER);
		assertEquals(Item.fromString("branch"), Item.BRANCH);
		assertEquals(Item.fromString(""), null);
		assertEquals(Item.fromString("asfa"), null);
	}

	private MapState createFirstMapStateExpected(boolean emptyInventory) {
		HashMap<Item,ArrayList<Position>> state = new HashMap<>();
		state.put(
			Item.TREE,
			Tools.createArrayListWith(
				new Position(0,0), new Position(1,1), new Position(2,2)
			) 
		);
		state.put(
			Item.DIRECTION,
			Tools.createArrayListWith(
				new Position(0,-1)
			) 
		);
		state.put(
			Item.CHARACTER,
			Tools.createArrayListWith(
				new Position(5,5)
			) 
		);
		
		HashMap<Item,Integer> inventory = new HashMap<>();
		if ( !emptyInventory )
			inventory.put(Item.LEAF, 1);

		MapState mapStateExpected = new MapState(state, inventory);
		return mapStateExpected;
	}

}