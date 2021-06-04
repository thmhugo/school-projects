package codelab4.datamodel;

import static org.junit.Assert.*;

import java.net.URL;
import java.util.ArrayList;

import org.json.JSONException;
import org.json.JSONObject;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import codelab4.filemanager.FileManager;

public class QuestTest {

	private Quest quest;

	@Before
	public void init() {
		ArrayList<String> functions = createFunction(2);
		try {
			JSONObject test = createTest();
			this.quest = new Quest("q1c1", "Description", "Goal", functions, test);
		} catch (InstantiationException ie) {
		} catch (Exception e) {
		}
	}

	static ArrayList<String> createFunction(int nb) {
		ArrayList<String> functions = new ArrayList<>();

		for (int i = 0; i < nb; ++i)
			functions.add("function" + i);

		return functions;
	}

	static JSONObject createTest()
	throws InstantiationException, JSONException {
		JSONObject test = new JSONObject();

		test.put("item", "WOOD");
		test.put("expected", 1);

		return test;
	}

	@After
	public void clean() {
		Quest.clearIds();
	}

	@Test
	public void testConstructor() {
		assertEquals("q1c1", quest.getId());
		assertEquals("Description", quest.getDescription());
		assertEquals("Goal", quest.getGoal());
		assertEquals("function0", quest.getFunctions().get(0));
		assertEquals("function1", quest.getFunctions().get(1));
	}

	@Test(expected = InstantiationException.class)
	public void testUniqueId() throws InstantiationException, Exception {
		ArrayList<String> functions = createFunction(2);
		JSONObject test = createTest();
		new Quest("q1c1", "Description", "Goal", functions, test);
	}

	@Test
	public void testTest() throws Exception {
		MapState map = MapStateTest.createMapState();
		assertTrue(quest.test(map));
	}

	/** Missing an attribute (description here) */
	@Test(expected = JSONException.class)
	public void missingKeyInQuest()
	throws Exception {
		URL url = this.getClass().getResource("/json/chapters/chapterTest2.json");
		String testPath = url.getPath();
		JSONObject json = FileManager.getJson(testPath);

		Quest.createQuest(json);
	}

	/** Wrong value in the JSON file: int instead of a String for the description. */
	@Test(expected = JSONException.class)
	public void notValueExpectedInJSON()
	throws Exception {
		URL url = this.getClass().getResource("/json/chapters/chapterTest3.json");
		String testPath = url.getPath();
		JSONObject json = FileManager.getJson(testPath);

		Quest.createQuest(json);
	}

	@Test(expected = JSONException.class)
	public void missingValueInJSON()
	throws Exception {
		URL url = this.getClass().getResource("/json/chapters/chapterTest4.json");
		String testPath = url.getPath();
		JSONObject json = FileManager.getJson(testPath);

		Quest.createQuest(json);
	}

	@Test
	public void expectedResult() throws JSONException {
		String res = quest.getExpectedResult();
		assertTrue(res.equals("1"));
	}
}
