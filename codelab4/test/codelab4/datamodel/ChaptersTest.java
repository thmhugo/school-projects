package codelab4.datamodel;

import static org.junit.Assert.*;
import org.json.JSONObject;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;
import codelab4.filemanager.FileManager;
import java.net.*;

public class ChaptersTest {
	
	private JSONObject json;

	@Before
	public void init() throws Exception {
		URL url = this.getClass().getResource("/json/chapters/chapterTest.json");
		json = FileManager.getJson(url.getPath());
	}

	@After
	public void clean() {
		Chapters.clear();
	}

	@Test
	public void testAddChapterFromJson() throws Exception {
		assertTrue(Chapters.addChapterFromJSON(json));

		assertEquals(
			"The id of the Chapter should be \"ct\".",
			"ct", Chapters.chapters.get(0).getId()
		);
		assertEquals(
			"The description of the Chapter should be \"Chapitre Test.\".",
			"Chapitre Test.", Chapters.chapters.get(0).getDescription())
		;
		assertEquals(
			"The id of the first quest should be \"q1c1\".",
			"q1c1", Chapters.chapters.get(0).getQuests().get(0).getId()
		);
		assertEquals(
			"The description of the second quest should be \"Description de test.\".",
			"Description de test.",
			Chapters.chapters.get(0).getQuests().get(1).getDescription()
		);
		assertEquals(
			"The goal of the second quest should be \"Ce test doit fonctionner (bis).\".",
			"Ce test doit fonctionner (bis).", 
			Chapters.chapters.get(0).getQuests().get(1).getGoal()
		);
		assertEquals(
			"avancer()",
			Chapters.chapters.get(0).getQuests().get(0).getFunctions().get(0)
		);
		assertEquals(
			"ramasser()",
			Chapters.chapters.get(0).getQuests().get(0).getFunctions().get(1)
		);
	}

	@Test
	public void testInit() {
		assertTrue(
			"Chapters should be able to init from /json/chapters-test-2",
			Chapters.init(getClass().getResource("/json/chapters-test-2").getPath())
		);

		assertTrue(
			"Chapter 'c1' should be loaded.",
			Chapters.getChapter("c1") != null
		);
		assertTrue(
			"Chapter 'c1' should be loaded.",
			Chapters.getChapter("c2") != null
		);
		assertTrue(
			"Quest 'q3c2' should be loaded.",
			Chapters.getChapter("c2").getQuest("q3c2") != null
		);
		assertTrue(
			"Quest 'q2c1' should be loaded.",
			Chapters.getChapter("c1").getQuest("q2c1") != null
		);
		assertTrue(
			"Quest 'q3c1' should be loaded.",
			Chapters.getChapter("c1").getQuest("q3c1") != null
		);
	}
}