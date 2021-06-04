package codelab4.process;

import static org.junit.Assert.*;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import org.junit.Ignore;
import codelab4.Instance;
import codelab4.datamodel.MapState;
import codelab4.datamodel.MapState.*;

public class InterpreterTest {

	private Interpreter interpreter;

	@Before
	public void init() {
		Interpreter.init();
		Interpreter.test = true;
	}

	@After
	public void clean() {
		interpreter = null;
		Interpreter.test = false;
	}

	/**
	 * Verifies that all the Tokens are associated with a lambda expression.
	 */
	@Test
	public void testInit() {
		int nbInstructions = 0;

		nbInstructions += Interpreter.actions.size();
		nbInstructions += Interpreter.conditions.size();
		nbInstructions += Interpreter.loops.size();

		/**
		 * 3 is the number of end_loop Token type.
		 * They don't have associated functional interfaces.
		 */
		assertEquals( nbInstructions, Token.values().length - 3 );
	}

	@Test
	public void testAVANCER() {
		MapState first = createFirstMapStateWith(Item.TREE, new Position(5, 3));
		MapState currentMapState;

		interpreterShouldRunWith(
			true, "Interpreter should run",
			first, Token.AVANCER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertTrue(
			"Character should step forward.",
			currentMapState.getCharacterPos().equals(new Position(5, 4))
		);

		interpreterShouldRunWith(
			false, "Character shouldn't step forward.",
			currentMapState,
			Token.AVANCER, Token.AVANCER
		);
	}

	@Test
	public void testAVANCER2() {
		MapState first = createFirstMapStateWith(Item.HUNGER, new Position(5, 4));
		MapState currentMapState;

		interpreterShouldRunWith(
			true, "Interpreter should run",
			first, Token.AVANCER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertTrue(
			"Character should step forward.",
			currentMapState.getCharacterPos().equals(new Position(5, 4))
		);
	}

	@Test
	public void testTOURNER() {
		MapState prev;
		MapState first = createMapState();
		ArrayList<Position> directionsPos = new ArrayList<Position>();

		directionsPos.add(MapState.NORTH);
		first.getState().put(Item.DIRECTION, directionsPos);

		interpreterShouldRunWith(
			true, "Program sould be correct. #1",
			first,
			Token.TOURNER_DROITE
		);

		prev = interpreter.getCurrentMapState();
		assertTrue(
			"Direction should be equals to WEST(1, 0)",
			MapState.EAST.equals(prev.getDirectionPos())
		);

		interpreterShouldRunWith(
			true, "Program sould be correct. #2",
			prev,
			Token.TOURNER_GAUCHE
		);

		prev = interpreter.getCurrentMapState();
		assertTrue(
			"Direction should be equals to NORTH(0, -1)",
			MapState.NORTH.equals(prev.getDirectionPos())
		);

		interpreterShouldRunWith(
			true, "Program sould be correct. #3",
			prev,
			Token.TOURNER_GAUCHE, Token.TOURNER_GAUCHE
		);

		prev = interpreter.getCurrentMapState();
		assertTrue(
			"Direction should be equals to SOUTH(0, 1)",
			MapState.SOUTH.equals(prev.getDirectionPos())
		);

		interpreterShouldRunWith(
			true, "Program should be correct. #4",
			prev,
			Token.TOURNER_GAUCHE, Token.TOURNER_GAUCHE, Token.TOURNER_DROITE
		);

		prev = interpreter.getCurrentMapState();
		assertTrue(
			"Direction should be equals to WEST(1, 0)",
			MapState.EAST.equals(prev.getDirectionPos())
		);
	}

	@Test
	public void testIF() {
		MapState first = createFirstMapStateWith(Item.TREE, new Position(5, 2));
		MapState currentMapState;

		interpreterShouldRunWith(
			false, "Program should not be considered as correct. #1",
			first,
			Token.IF, Token.AVANCER, Token.ENDIF
		);

		currentMapState = interpreter.getCurrentMapState();
		interpreterShouldRunWith(
			true, "Program should be considered as correct. #1",
			currentMapState,
			Token.IF, Token.EST_LIBRE, Token.AVANCER, Token.ENDIF, Token.AVANCER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertTrue(
			"Character should step forward.",
			currentMapState.getCharacterPos().equals(new Position(5, 3))
		);

		interpreterShouldRunWith(
			true, "Program should be considered as correct. #2",
			currentMapState,
			Token.IF, Token.EST_LIBRE, Token.AVANCER, Token.ENDIF
		);

		currentMapState = interpreter.getCurrentMapState();
		assertTrue(
			"Character should not step forward.",
			currentMapState.getCharacterPos().equals(new Position(5, 3))
		);
	}

	@Test
	public void testELSE() {
		MapState currentMapState;
		MapState first = createFirstMapStateWith(Item.TREE, new Position(5, 4));

		interpreterShouldRunWith(
			true, "Program should be correct. #1",
			first,
			Token.IF, Token.EST_LIBRE,
				Token.AVANCER,
			Token.ELSE,
				Token.TOURNER_DROITE,
			Token.ENDIF,
			Token.AVANCER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertTrue(
			"Character should be in Position(6, 5)",
			currentMapState.getCharacterPos().equals(new Position(6, 5))
		);

		Item item = Item.FRUIT;
		first.addInInventory(item);

		interpreterShouldRunWith(
			true, "Program should be correct.",
			first,
			Token.IF, Token.EST_POURRI,
				Token.JETER_FRUIT,
			Token.ELSE,
				Token.MANGER,
			Token.ENDIF
		);

		assertEquals(
			"Inventory should have 0 fruit left.",
			0, (int) first.getInventory().get(Item.FRUIT)
		);
	}

	@Test
	public void testWHILE() {
		MapState first = createFirstMapStateWith(Item.TREE, new Position(5, 1));
		MapState currentMapState;

		interpreterShouldRunWith(
			true, "Program should be correct. #1",
			first,
			Token.WHILE, Token.EST_LIBRE,
				Token.AVANCER,
			Token.ENDWHILE
		);

		currentMapState = interpreter.getCurrentMapState();
		assertTrue(
			"Character should be in Position(6, 5)",
			currentMapState.getCharacterPos().equals(new Position(5, 2))
		);
	}

	@Test
	public void testRAMASSER() {
		MapState first = createFirstMapStateWith(Item.FRUIT, new Position(5, 4));
		MapState currentMapState;

		interpreterShouldRunWith(
			true, "Program should be correct. #1",
			first,
			Token.RAMASSER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertEquals(
			"Inventory should get a fruit.",
			1, (int) currentMapState.getInventory().get(Item.FRUIT)
		);

		interpreterShouldRunWith(
			true, "Program should be correct. #2",
			currentMapState,
			Token.RAMASSER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertEquals(
			"Inventory should not get one more fruit.",
			1, (int) currentMapState.getInventory().get(Item.FRUIT)
		);
	}

	@Test
	public void testJETER_FRUIT() {
		MapState first = createMapState();
		MapState currentMapState;

		Item item = Item.FRUIT;
		item.setIsRotten(true);

		first.addInInventory(Item.FRUIT, 2);

		interpreterShouldRunWith(
			true, "Program should be correct.",
			first,
			Token.JETER_FRUIT
		);

		currentMapState = interpreter.getCurrentMapState();
		assertEquals(
			"Inventory should have 1 fruit left.",
			1, (int) currentMapState.getInventory().get(Item.FRUIT)
		);

		assertEquals(
			"Fruit from inventory should not be rotten.",
			null, currentMapState.getInventoryKey(Item.FRUIT).getIsRotten()
		);

		currentMapState = interpreter.getCurrentMapState();
		interpreterShouldRunWith(
			true, "Program should be correct.",
			currentMapState,
			Token.JETER_FRUIT
		);

		assertEquals(
			"Inventory should have 0 fruit left.",
			0, (int) currentMapState.getInventory().get(Item.FRUIT)
		);

		currentMapState = interpreter.getCurrentMapState();
		interpreterShouldRunWith(
			false, "Program should not be correct.",
			currentMapState,
			Token.JETER_FRUIT
		);
	}

	@Test
	public void testCOUPER() {
		MapState first = createFirstMapStateWith(Item.TREE, new Position(5, 4));
		MapState currentMapState;

		first.addInInventory(Item.AXE);

		interpreterShouldRunWith(
			true, "Program should be correct.",
			first,
			Token.COUPER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertEquals(
			"Inventory should get a wood.",
			1, (int) currentMapState.getInventory().get(Item.WOOD)
		);

		interpreterShouldRunWith(
			false, "Program should not be correct.",
			currentMapState,
			Token.COUPER
		);

		assertEquals(
			"Inventory should not get a wood.",
			1, (int) currentMapState.getInventory().get(Item.WOOD)
		);
	}

	@Test
	public void testMANGER() {
		MapState first = createMapState();
		MapState currentMapState;

		ArrayList<Position> hungerPos = new ArrayList<>();
		hungerPos.add(new Position(4));
		first.getState().put(Item.HUNGER, hungerPos);

		first.addInInventory(Item.FRUIT);
		interpreterShouldRunWith(
			true, "Program should be correct. #1",
			first,
			Token.MANGER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertEquals(
			"Inventory should have 0 fruit left.",
			0, (int) currentMapState.getInventory().get(Item.FRUIT)
		);

		Item item = Item.FRUIT;
		item.setIsRotten(true);
		currentMapState.addInInventory(item);

		interpreterShouldRunWith(
			true, "Program should be correct. #2",
			currentMapState,
			Token.MANGER
		);

		assertEquals(
			"Inventory should have 0 fruit left.",
			0, (int) currentMapState.getInventory().get(Item.FRUIT)
		);
	}

	@Test
	public void testBUGCHARACTER() {
		MapState first = createFirstMapStateWith(Item.TREE, new Position(5, 1));
		MapState currentMapState;

		interpreterShouldRunWith(
			true, "Interpreter should run",
			first,
			Token.AVANCER,
			Token.AVANCER,
			Token.TOURNER_DROITE,
			Token.TOURNER_DROITE,
			Token.AVANCER,
			Token.AVANCER,
			Token.AVANCER
		);

		currentMapState = interpreter.getCurrentMapState();
		assertTrue(
			"Character should step forward.",
			currentMapState.getCharacterPos().equals(new Position(5, 6))
		);
	}

	/**
	 * Creates a MapState with CHARACTER = (5, 5) and DIRECTION = NORTH (0, -1).
	 * @param item is the Item to add into the new MapState's state.
	 * @param pos is Positions of the Item.
	 * @return the new MapState.
	 */
	public static MapState createFirstMapStateWith(Item item, Position ... pos) {
		MapState first = createMapState();
		ArrayList<Position> characterPos = new ArrayList<Position>();
		ArrayList<Position> directionsPos = new ArrayList<Position>();
		ArrayList<Position> hungerPos = new ArrayList<Position>();
		ArrayList<Position> itemPos = new ArrayList<Position>();

		characterPos.add(new Position(5, 5));
		directionsPos.add(MapState.NORTH.clone());
		hungerPos.add(new Position(3));
		itemPos.addAll(Arrays.asList(pos));

		first.getState().put(Item.CHARACTER, characterPos);
		first.getState().put(Item.DIRECTION, directionsPos);
		first.getState().put(Item.HUNGER, hungerPos);
		first.getState().put(item, itemPos);

		first.initPositionHashMap();

		return first;
	}


	private void interpreterShouldRunWith(
		boolean cond, String msg, MapState first, Token...tokens
	) {
		interpreter = new Interpreter(
						new ArrayList<Token>(Arrays.asList(tokens)), first, new Instance()
					);
		try {
			if ( cond )
				assertTrue( msg, interpreter.run() );
			else
				assertFalse( msg, interpreter.run() );
		}
		catch (Exception e) {}
	}

	private static MapState createMapState() {
		HashMap<Item,ArrayList<Position>> state = new HashMap<>();
		HashMap<Item,Integer> inventory = new HashMap<>();
		return new MapState(state, inventory);
	}
}
