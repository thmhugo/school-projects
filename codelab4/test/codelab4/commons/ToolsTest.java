package codelab4.commons;

import static org.junit.Assert.assertTrue;

import java.util.HashMap;
import java.util.ArrayList;

import org.junit.Test;

import codelab4.datamodel.MapState;
import codelab4.datamodel.MapState.Item;
import codelab4.datamodel.MapState.Position;

public class ToolsTest {

	@Test
	public void testHashMapWithArrayDeepCopy() {
		HashMap<Item,ArrayList<Position>> stateCopy;
		HashMap<Item,ArrayList<Position>> state = new HashMap<>();
		ArrayList<Position> pos = new ArrayList<>();

		pos.add(new Position(0,-1));
		state.put(Item.DIRECTION, pos);
		stateCopy = Tools.hashMapWithArrayDeepCopy(state);

		state.get(Item.DIRECTION).get(0).turnLeft();

		assertTrue(
			"HashMap's DIRECTIONS should not be equals.",
			!state.get(Item.DIRECTION).get(0).equals(
				stateCopy.get(Item.DIRECTION).get(0)
			)	
		);
	}
	
	@Test
	public void testHashMapWithArrayDeepCopyWithMapStates() {
		HashMap<Item,ArrayList<Position>> state = new HashMap<>();
		ArrayList<Position> pos = new ArrayList<>();

		pos.add(new Position(0,-1));
		state.put(Item.DIRECTION, pos);

		MapState map1 = new MapState(state, new HashMap<Item, Integer>());
		MapState map2 = new MapState(map1);

		map1.getDirectionPos().turnLeft();

		assertTrue(
			"MapStates's DIRECTIONS should not be equals.",
			!map1.getDirectionPos().equals(map2.getDirectionPos())	
		);
	}
}