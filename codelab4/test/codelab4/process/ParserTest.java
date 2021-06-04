package codelab4.process;

import static org.junit.Assert.*;

import java.util.ArrayList;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import codelab4.commons.Tools;
import codelab4.process.exceptions.SyntaxException;

public class ParserTest {

	private Parser parser;

	@Before
	public void init() {
		parser = new Parser();
	}

	@After
	public void clean() {
	}

	@Test
	public void testSizeActions() throws Exception {
		String str = "avancer()" + "\n" + "avancer()" + "\n" + "ramasser()";
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.AVANCER,
										Token.AVANCER,
										Token.AVANCER
									);
		ArrayList<Token> tokenized = parser.tokenize(str, 3);

		assertEquals(tokenized.size(), expected.size());
	}

	@Test
	public void testActionsTokens() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.AVANCER,
										Token.TOURNER_DROITE,
										Token.TOURNER_GAUCHE,
										Token.RAMASSER
									);
		String str = "avancer()\ntourner_a_droite()\ntourner_a_gauche()\nramasser()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testActionsTokens_2() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.AVANCER,
										Token.AVANCER,
										Token.AVANCER
									);
		String str = "avancer()\navancer()\nramasser()";

		assertFalse(compareStringWithArray(str, expected));
	}

	@Test
	public void testEmpty() throws Exception {
		String str = "";
		ArrayList<Token> tokenized = parser.tokenize(str, 0);

		assertEquals(0, tokenized.size());
	}

	@Test
	public void testSimpleIf() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF, Token.EST_LIBRE,
											Token.AVANCER,
										Token.ENDIF
									);
		String str = "if chemin_est_libre() then\n	avancer()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testSimpleIfElse() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF, Token.EST_LIBRE,
											Token.AVANCER,
										Token.ELSE,
											Token.RAMASSER,
										Token.ENDIF
									);
		String str = "if chemin_est_libre() then\n	avancer()\nelse\n	ramasser()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testIfWithNoElse() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF, Token.EST_LIBRE,
											Token.AVANCER,
										Token.ENDIF,
										Token.RAMASSER
									);
		String str = "if chemin_est_libre() then\n	avancer()\nramasser()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testIfConcatenation() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF, Token.EST_LIBRE,
											Token.AVANCER,
										Token.ENDIF,
										Token.IF, Token.EST_LIBRE,
											Token.AVANCER,
										Token.ENDIF
									);
		String str = "if chemin_est_libre() then\n	avancer()\nif chemin_est_libre() then\n	avancer()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testIfConcatenationWihActionBetween() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF, Token.EST_LIBRE,
											Token.AVANCER,
										Token.ENDIF,
										Token.RAMASSER,
										Token.IF, Token.EST_LIBRE,
											Token.AVANCER,
										Token.ENDIF,
										Token.RAMASSER
									);
		String str = "if chemin_est_libre() then\n	avancer()\nramasser()\n"
				+ "if chemin_est_libre() then\n	avancer()\nramasser()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testSimpleWhile() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.WHILE, Token.EST_LIBRE,
											Token.AVANCER,
										Token.ENDWHILE
									);
		String str = "while chemin_est_libre() do\n	avancer()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testIfInWhile() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.WHILE, Token.EST_LIBRE,
											Token.IF, Token.EST_POURRI,
												Token.RAMASSER,
											Token.ENDIF,
										Token.ENDWHILE
									);
		String str = "while chemin_est_libre() do\n	if fruit_est_pourri() then\n		ramasser()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testIfElseInWhile() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.WHILE, Token.EST_LIBRE,
											Token.IF, Token.EST_POURRI,
												Token.RAMASSER,
											Token.ELSE,
												Token.AVANCER,
											Token.ENDIF,
										Token.ENDWHILE
									);
		String str = "while chemin_est_libre() do\n	if fruit_est_pourri() then\n		ramasser()"
				+ "\n	else\n		avancer()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testWhileInIf() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF, Token.EST_LIBRE,
											Token.WHILE, Token.EST_POURRI,
												Token.RAMASSER,
											Token.ENDWHILE,
										Token.ENDIF
									);
		String str = "if chemin_est_libre() then\n	while fruit_est_pourri() do\n		ramasser()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testIfInIf() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF, Token.EST_LIBRE,
											Token.IF, Token.EST_POURRI,
												Token.RAMASSER,
											Token.ENDIF,
										Token.ENDIF
									);
		String str = "if chemin_est_libre() then\n	if fruit_est_pourri() then\n		ramasser()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testIfElseInIf() {
		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF, Token.EST_LIBRE,
											Token.IF, Token.EST_POURRI,
												Token.RAMASSER,
											Token.ELSE,
												Token.AVANCER,
											Token.ENDIF,
										Token.ENDIF
									);
		String str = "if chemin_est_libre() then\n	if fruit_est_pourri() then\n		ramasser()"
				+ "\n	else\n		avancer()";

		assertTrue(compareStringWithArray(str, expected));

	}

	@Test
	public void testCouper() {
		ArrayList<Token> expected = Tools.createArrayListWith(Token.COUPER);
		String str = "couper()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testJeterFruit() {
		ArrayList<Token> expected = Tools.createArrayListWith(Token.JETER_FRUIT);
		String str = "jeter_fruit()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void testManger() throws SyntaxException {
		ArrayList<Token> expected = Tools.createArrayListWith(Token.MANGER);
		String str = "manger_fruit()";

		assertTrue(compareStringWithArray(str, expected));
	}

	@Test
	public void addEndTokensSizeTest() {
		Parser p = new Parser();

		p.array.add(Token.IF);
		p.array.add(Token.WHILE);
		p.loopStack.push(Token.IF);
		p.loopStack.push(Token.WHILE);
		p.addEndTokens();

		assertEquals(p.array.size(), 4);
	}

	@Test
	public void addEndTokensContentTest() throws Exception {
		Parser p = new Parser();

		p.array.add(Token.IF);
		p.loopStack.push(Token.ENDIF);

		p.array.add(Token.WHILE);
		p.loopStack.push(Token.ENDWHILE);
		p.addEndTokens();

		ArrayList<Token> expected = Tools.createArrayListWith(
										Token.IF,
											Token.WHILE,
											Token.ENDWHILE,
										Token.ENDIF
									);

		assertTrue(expected.equals(p.array));
	}

	private boolean compareStringWithArray(String str, ArrayList<Token> expected) {
		ArrayList<Token> tokenized;
		
		try {
			tokenized = parser.tokenize(str, expected.size());

			for (int i = 0; i < expected.size(); i++) 
				if ( !expected.get(i).equals(tokenized.get(i)) )
					return false;
		}
		catch (Exception e) {
			return false;
		}

		return true;
	}
}
