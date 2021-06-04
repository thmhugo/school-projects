package codelab4.datamodel;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

import org.apache.commons.io.FileUtils;
import org.json.JSONException;
import org.json.JSONObject;

import codelab4.filemanager.FileManager;

public class Sessions {

	public static final String SESSIONS_PATH = "./sessions";

	public static ArrayList<Session> sessions = new ArrayList<>();

	public static boolean init() {
		return load(SESSIONS_PATH);
	}

	public static boolean load(final String sessionsDirPath) {
		final File sessionsDir = new File(sessionsDirPath);

		Sessions.clear();
		if ( !sessionsDir.exists() ) {
			sessionsDir.mkdir();
			return false;
		}

		for (final File f : sessionsDir.listFiles())
			addSessionFromFile(f);

		return true;
	}

	private static void addSessionFromFile(final File f) {
		try {
			final JSONObject sessionJSON = new JSONObject(
												FileUtils.readFileToString(f, "utf-8")
											);
			sessions.add(new Session(sessionJSON));
		}
		catch (Exception e) {
			System.err.println(e.getMessage());
		}
	}

	public static boolean create(final String newSessionId) {
		Session newSession;

		if ( isAValidId(newSessionId) ) {
			newSession = new Session(
				newSessionId,
				Chapters.getFirstChapter(),
				Chapters.getFirstQuest(),
				MapState.createStartingMapState()
			);
			newSession.addAdditionalItems();
			newSession.unlockFunctions();
			sessions.add(newSession);

			return Sessions.save(newSession);
		}
		return false;
	}

	private static boolean isAValidId(final String newSessionId) {
		return getSession(newSessionId) == null
				&& !newSessionId.equals("")
				&& FileManager.isAValidFileName(newSessionId);
	}

	public static void clear() {
		Sessions.sessions.clear();
	}

	public static boolean save(final Session session) {
		try {
			FileManager.save(session.getPath(), session.toJSON());
			return true;
		}
		catch (JSONException e) {
			System.err.println(e.getMessage());
		}
		return false;
	}

	public static ArrayList<Session> getSessions() {
		return sessions;
	}

	public static Session getSession(final String sessionId) {
		for (final Session s : sessions)
			if ( s.getId().equals(sessionId) )
				return s;

		return null;
	}

	public static List<String> getIds() {
		List<String> ids = new ArrayList<>();

		for (final Session s : sessions)
			ids.add(s.getId());

		return ids;
	}

	public static boolean contains(final String sessionId) {
		return Sessions.getSession(sessionId) != null;
	}

	public static void delete(final String selectedItem) {
		final Session removedSession = getSession(selectedItem);

		if ( removedSession != null ) {
			FileManager.deleteFile(
				SESSIONS_PATH + "/" + removedSession.getId() + ".json"
			);
			sessions.remove(removedSession);
		}
	}
}
