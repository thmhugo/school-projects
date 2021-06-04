package codelab4.datamodel;

import static org.junit.Assert.*;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

public class SessionsTest {

	@Before
	public void start() {
		Chapters.init(getClass().getResource("/json/chapters-test-2").getPath());
	}

	@After
	public void clean() {
		Sessions.clear();
		Chapters.clear();
	}

	@Test
	public void testLoad() {
		assertTrue(
			"Sessions should be loaded.",
			Sessions.load(getClass().getResource("/json/sessions").getPath())
		);
		assertTrue(
			"Sessions should loads valid json file. #1",
			Sessions.contains("session-1-test")
		);
		assertTrue(
			"Sessions should loads valid json file. #2",
			Sessions.contains("session-3-test")
		);
		assertTrue(
			"Sessions should have loaded only 2 Session.",
			Sessions.sessions.size() == 2
		);
	}

	private void sessionEqualsToFirstSession(String sessionId) {
		Session session = Sessions.getSession(sessionId);

		assertNotNull(
			"Session should not be equals to null.",
			session
		);
		assertTrue(
			"Session id should be equals to '" + sessionId + "'.",
			session.getId().equals(sessionId)
		);
		assertTrue(
			"Session's chapter id should be equals to 'c1'.",
			session.getCurrentChapter().getId().equals("c1")	
		);
		assertTrue(
			"Session's quest id should be equals to 'q1c1'.",
			session.getCurrentQuest().getId().equals("q1c1")	
		);
		assertTrue(
			"Session map state should be equals to the starting map state.",
			MapState.areEqual(MapState.createStartingMapState(), session.getMapState())	
		);
	}

	@Test
	public void testCreateUnvalid() {
		assertFalse(
			"Session should not be create.",
			Sessions.create("new session")
		);
	}
	
	@Test
	public void testCreateValid() {
		assertTrue(
			"Session should be create.",
			Sessions.create("new-session1231231312")
		);
		Sessions.delete("new-session1231231312");
	}
}