package codelab4.process;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Stack;
import java.util.function.Predicate;

import codelab4.Instance;
import codelab4.datamodel.MapState;
import codelab4.datamodel.MapState.Item;
import codelab4.datamodel.MapState.Position;
import codelab4.process.exceptions.InfiniteLoopException;
import javafx.application.Platform;

/**
 * Module that modifies the instance's MapState from a list of tokens.
 */
public class Interpreter {

	static HashMap<Token, Action> actions = new HashMap<>();
	static HashMap<Token, Condition> conditions = new HashMap<>();
	static HashMap<Token, Loop> loops = new HashMap<>();
	static boolean test = false;

	private static final int MAX_LOOP_COUNT = 50;

	/** Represents the number of actions needed to decrement Item.HUNGER */
	private static final int INIT_COUNT_ACTION = 5;
	private static int countActions;

	private Stack<Token> tokens;
	private MapState currentMapState;
	private Instance instance;

	/**
	 * Modifies the input MapState.
	 */
	@FunctionalInterface
	private interface Action {
		public boolean run(MapState input);
	}

	/**
	 * Evaluates a predicate based on an input.
	 */
	@FunctionalInterface
	private interface Condition extends Predicate<MapState> {
	}

	/**
	 * Modifies several times input.
	 */
	@FunctionalInterface
	private interface Loop {
		public boolean run (
			Instance instance, MapState input, Condition prop, Stack<Token> args
		) throws InfiniteLoopException;
	}

	public static void init() {
		initActions();
		initConditions();
		initLoops();
	}

	private static void initActions() {
		actions.put(
			Token.AVANCER, (currentMapState) -> {
				if ( conditions.get(Token.EST_LIBRE).test(currentMapState)) {
					final Position nextPos = getNextPos(currentMapState);
					currentMapState.setCharacterPos(nextPos);

					return true;
				}
				return false;
			}
		);
		actions.put(
			Token.TOURNER_DROITE, (currentMapState) -> {
				currentMapState.getDirectionPos().turnRight();

				return true;
			}
		);
		actions.put(
			Token.TOURNER_GAUCHE, (currentMapState) -> {
				currentMapState.getDirectionPos().turnLeft();

				return true;
			}
		);
		actions.put(
			Token.RAMASSER, (currentMapState) -> {
				final Position nextPos = getNextPos(currentMapState);
				final Item item = currentMapState.getItemAt(nextPos);

				if ( item != null && item.isPickable()) {
					currentMapState.remove(item, nextPos);
					currentMapState.addInInventory(item);
				}
				return true;
			}
		);
		actions.put(
			Token.COUPER, (currentMapState) -> {
				final Position nextPos = getNextPos(currentMapState);
				final Item item = currentMapState.getItemAt(nextPos);

				if (Item.TREE == item && currentMapState.isInInventory("AXE", 1)) {
					currentMapState.remove(item, nextPos);
					currentMapState.addInInventory(Item.WOOD);

					return true;
				}
				return false;
			}
		);
		actions.put(
			Token.MANGER, (currentMapState) -> {
				if (currentMapState.hasRotten() == null)
					currentMapState.isGoingRotten();

				return currentMapState.eatFruit();
			}
		);
		actions.put(
			Token.JETER_FRUIT, (currentMapState) -> {
				return currentMapState.removeFruitFromInv();
			}
		);
	}

	/**
	 * @param input represents the current MapState.
	 * @return the Position just in front of the character.
	 */
	private static Position getNextPos(final MapState input) {
		final Position currentPos = input.getCharacterPos();
		final Position direction = input.getDirectionPos();

		return Position.add(currentPos, direction);
	}

	private static void initConditions() {
		conditions.put(
			Token.EST_LIBRE, (currentMapState) -> {
				final Position nextPos = getNextPos(currentMapState);

				return nextPos.isInMap() && !currentMapState.contains(nextPos)
					|| !currentMapState.getItemAt(nextPos).isDisplayable()
					|| currentMapState.getItemAt(nextPos) == Item.SHELTER;
			}
		);
		conditions.put(
			Token.EST_POURRI, (currentMapState) -> {
				if (currentMapState.hasRotten() == null)
					currentMapState.isGoingRotten();

				return currentMapState.hasRotten();
			});
	}

	private static void initLoops() {
		loops.put(
			Token.IF, (instance, currentMapState, condition, tokens) -> {
				Token tokenFound = null;

				if (condition.test(currentMapState)) {
					if (!interpretUntilOR(instance, currentMapState, tokens, Token.ELSE, Token.ENDIF))
						return false;
					if (tokens.peek() == Token.ELSE)
						tokenFound = popUntil(tokens, Token.ENDIF);
				}
				else {
					tokenFound = popUntilOR(tokens, Token.ELSE, Token.ENDIF);
					if (tokenFound == Token.ELSE)
						if (!interpretUntil(instance, currentMapState, tokens, Token.ENDIF))
							return false;
				}
				if (tokenFound != Token.ENDIF)
					tokens.pop();

				return true;
			}
		);
		loops.put(
			Token.WHILE, (instance, currentMapState, condition, tokens) -> {
				Stack<Token> tokensTmp = null;
				Token tokenFound = null;
				int currentCount = 0;

				while (condition.test(currentMapState)) {
					tokensTmp = (Stack<Token>) tokens.clone();
					if (!interpretUntil(instance, currentMapState, tokensTmp, Token.ENDWHILE))
						return false;
					tokensTmp = null;
					testCount(currentCount);
					currentCount++;
				}

				if (tokensTmp == null)
					tokenFound = popUntilOR(tokens, Token.ENDWHILE);

				if (tokenFound != Token.ENDWHILE)
					return false;

				return true;
			}
		);
	}

	private static void testCount(final int currentCount) throws InfiniteLoopException {
		if (currentCount > MAX_LOOP_COUNT)
			throw new InfiniteLoopException();
	}

	private static boolean interpretUntil(
		Instance instance,
		MapState current,
		Stack<Token> tokens,
		final Token endToken
	) throws InfiniteLoopException {
		return interpretUntilOR(instance, current, tokens, endToken, endToken);
	}

	private static boolean interpretUntilOR(
		Instance instance,
		MapState current,
		Stack<Token> tokens,
		final Token ... endTokens
	) throws InfiniteLoopException {
		while (!tokens.isEmpty() && !Arrays.asList(endTokens).contains(tokens.peek()))
			if (!interpret(instance, current, tokens))
				return false;

		return true;
	}

	private static Token popUntil(Stack<Token> tokens, final Token endToken) {
		return popUntilOR(tokens, endToken);
	}

	/**
	 * @param tokens    is a Token stack.
	 * @param firstEnd  is the first token to look.
	 * @param secondEnd is the second token to look.
	 * @return the first corresponding token, null if neither is found.
	 */
	private static Token popUntilOR(Stack<Token> tokens, final Token... endTokens) {
		Token tokenPopped = null;

		while (!tokens.isEmpty()) {
			tokenPopped = tokens.pop();
			if (Arrays.asList(endTokens).contains(tokenPopped))
				return tokenPopped;
		}

		return tokenPopped;
	}

	public Interpreter(
		ArrayList<Token> tokens,
		MapState current,
		Instance instance
	) {
		initTokens(tokens);
		this.currentMapState = current;
		this.instance = instance;
		countActions = INIT_COUNT_ACTION;
	}

	private void initTokens(final ArrayList<Token> tokens) {
		this.tokens = new Stack<>();
		for (int i = tokens.size() - 1; i >= 0; --i)
			this.tokens.push(tokens.get(i));
	}

	public boolean run() throws InfiniteLoopException {
		while (!tokens.isEmpty())
			if (!interpret(instance, currentMapState, tokens))
				return false;

		if (!test)
			Platform.runLater( () -> instance.getMainController().verify() );

		return true;
	}

	private static boolean interpret(
		Instance instance, MapState current, Stack<Token> tokens
	) throws InfiniteLoopException {
		assert !tokens.isEmpty();
		final Token token = tokens.pop();

		switch (token.type) {
			case "action":
				return interpretAction(instance, current, token);
			case "loop":
				return interpretLoop(instance, current, tokens, token);
		}
		return false;
	}

	private static boolean interpretAction(
		Instance instance, MapState current, Token token
	) {
		if ( actions.get(token).run(current) ) {
			if (!updateActionsCount(current))
				return false;

			if (!test) {
				Platform.runLater(instance::refreshView);
				try {
					Thread.sleep(500);
				}
				catch (Exception e) {}
			}

			if (
				current.getState().containsKey(Item.HUNGER)
				&& current.getHunger() == Item.getMaxHunger()
			)
				return false;

			return true;
		}
		return false;
	}

	private static boolean updateActionsCount(MapState current) {
		--countActions;
		if ( countActions == 0 ) {
			countActions = INIT_COUNT_ACTION;
			if ( current.incrementHunger() && current.getHunger() > Item.MAX_HUNGER )
				return false;
		}
		return true;
	}

	private static boolean interpretLoop(
		Instance instance,
		MapState current,
		Stack<Token> tokens,
		final Token token
	) throws InfiniteLoopException {
		if ( tokens.peek().type.equals("condition") )
			return loops.get(token).run(
										instance,
										current,
										conditions.get(tokens.pop()),
										tokens
									);

		return false;
	}

	MapState getCurrentMapState() {
		return currentMapState;
	}

	public void setTokens(Stack<Token> tokens) {
		this.tokens = tokens;
	}
}
