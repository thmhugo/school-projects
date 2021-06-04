package codelab4.datamodel;

import static org.junit.Assert.*;

import java.util.ArrayList;

import org.json.JSONException;
import org.json.JSONObject;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class ChapterTest {

	private Chapter chapter;

	@Before
	public void init() throws InstantiationException, Exception {
		ArrayList<Quest> quests = createQuest(2);
		try {
			chapter = new Chapter("c1", quests, "Description");
		} catch (InstantiationException e) {
		}
	}

	static ArrayList<Quest> createQuest(int nb) throws InstantiationException, JSONException{
		ArrayList<String> functions = QuestTest.createFunction(2);
		JSONObject test = QuestTest.createTest();
		ArrayList<Quest> quests = new ArrayList<Quest>();

		for (int i = 0; i < nb; ++i) {
			try {
				quests.add(new Quest("c1q" + i, "d", "g" + i, functions, test));
			} catch (InstantiationException e) {
			}
		}

		return quests;
	}

	@After
	public void clean() {
		Chapter.clearIds();
	}

	@Test
	public void testConstructor() {
		assertEquals("c1", chapter.getId());
		assertEquals("Description", chapter.getDescription());
		assertEquals("c1q0", chapter.getQuests().get(0).getId());
		assertEquals("c1q1", chapter.getQuests().get(1).getId());
	}

	@Test(expected = InstantiationException.class)
	public void testUniqueId() throws InstantiationException, JSONException {
		ArrayList<Quest> quests = createQuest(2);
		new Chapter("c1", quests, "Description");
	}

	@Test
	public void testIsAValidFileName() {
		assertTrue(
			"Should be a valid filename. #1",
			Chapter.isAValidFileName("chapter1.json")
		);
		assertTrue(
			"Should be a valid filename. #2",
			Chapter.isAValidFileName("chapter34.json")
		);
		assertTrue(
			"Should be a valid filename. #3",
			Chapter.isAValidFileName("chapter3499.json")
		);
		assertFalse(
			"Should not be a valid filename. #1",
			Chapter.isAValidFileName("chapter.json")
		);
		assertFalse(
			"Should not be a valid filename. #2",
			Chapter.isAValidFileName("")
		);
		assertFalse(
			"Should not be a valid filename. #3",
			Chapter.isAValidFileName("chapter1")
		);
		assertFalse(
			"Should not be a valid filename. #3",
			Chapter.isAValidFileName("chapter1.txt")
		);
		assertFalse(
			"Should not be a valid filename. #4",
			Chapter.isAValidFileName("chapter1.json.te")
		);
		assertFalse(
			"Should not be a valid filename. #5",
			Chapter.isAValidFileName("chapter1.jsonte")
		);
	}
}