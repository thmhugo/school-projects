package codelab4.datamodel;

import static org.junit.Assert.*;

import java.net.URL;
import java.util.ArrayList;

import org.json.JSONObject;
import org.junit.*;

import codelab4.commons.Tools;
import codelab4.datamodel.MapState.Item;
import codelab4.datamodel.MapState.Position;
import codelab4.filemanager.FileManager;

public class SessionTest {
	
	private Session session;

	@Before
	public void init() throws Exception {
		URL url = this.getClass().getResource("/json/chapters-test-2");
		Chapters.init(url.getPath());
	}

	@After
	public void clean() throws Exception {
		session = null;
	}

	@Test
	public void testFromAValidFile() throws Exception {
		JSONObject json = loadTestJSONObject("1");
		MapState excpectedMapState = createFirstMapState();
		
		session = new Session(json);
		assertEquals(
			"Session's id must be equal to 'session-1-test'.",
			"session-1-test", session.getId()
		);
		assertEquals(
			"Session's chapter id must be equal to 'c1'.",
			"c1", session.getCurrentChapter().getId()
		);
		assertEquals(
			"Session's quest id must be equal to 'q1c1'.",
			"q1c1", session.getCurrentQuest().getId()
		);
		assertTrue(
			"Session's mapState must be equal.",
			MapState.areEqual(session.getMapState(), excpectedMapState)
		); 
	}

	/**
	 * In session-2-test.json the map_state attribute does not contain 'direction'.
	 * As a result, the Session constructor should throws an InstantiationException. 
	 */
	@Test(expected = InstantiationException.class)
	public void testFromANonValidFile() throws Exception {
		JSONObject json = loadTestJSONObject("2");
		new Session(json);
	}

	@Test
	public void testProgression() throws Exception {
		JSONObject json = loadTestJSONObject("3");
		session = new Session(json);

		double progress = session.getCurrentProgress();

		assertEquals("First chapter's progression should be at 75%.",0.75,progress,1);
	}

	private MapState createFirstMapState() {
		MapState excpectedMapState = new MapState();
		
		excpectedMapState.addInInventory(Item.FRUIT, 3);
		excpectedMapState.getState().put(
			Item.TREE,
			Tools.createArrayListWith(
				new Position(0,0),new Position(1,1),new Position(2,2)
			)
		);
		excpectedMapState.getState().put(
			Item.LEAF,
			Tools.createArrayListWith(
				new Position(1,2),new Position(2,3)
			)
		);
		excpectedMapState.getState().put(
			Item.DIRECTION,
			Tools.createArrayListWith(
				new Position(0,-1)
			)
		);
		excpectedMapState.getState().put(
			Item.CHARACTER,
			Tools.createArrayListWith(
				new Position(5,5)
			)
		);
		return excpectedMapState;
	}
	
	private JSONObject loadTestJSONObject(String num) throws Exception {
		URL url = getClass().getResource("/json/sessions/session-"+num+"-test.json");
		return FileManager.getJson(url.getPath());
	}
}