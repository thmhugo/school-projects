package codelab4.datamodel;

import java.io.File;
import java.util.ArrayList;

import org.apache.commons.io.FileUtils;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

public class Chapters {

	public final static String DIR_PATH = "./src/resources/json/chapters";

	public static ArrayList<Chapter> chapters = new ArrayList<Chapter>();

	public static boolean init() {
		final boolean initialized = init(DIR_PATH);

		if ( initialized ) {
			sortChapters();
			sortQuests();
		}

		return initialized;
	}

	private static void sortChapters() {
		chapters.sort(
			(chapter1, chapter2) -> {
				return chapter1.getId().compareTo(chapter2.getId());
			}
		);
	}

	private static void sortQuests() {
		for (final Chapter c : chapters)
			c.sortQuest();
	}

	public static boolean init(final String chaptersDirPath) {
		return init(new File(chaptersDirPath));
	}

	public static boolean init(final File chaptersDir) {
		Chapters.clear();
		if ( !chaptersDir.exists() )
			return false;

		for (final File f : chaptersDir.listFiles())
			if (Chapter.isAValidFileName(f.getName()))
				if (!addChapterFromFile(f))
					return false;

		return true;
	}

	public static void clear() {
		chapters.clear();
		Quest.clearIds();
		Chapter.clearIds();
	}

	public static boolean addChapterFromFile(final File f) {
		try {
			final JSONObject chapterJSON = new JSONObject(
				FileUtils.readFileToString(f, "utf-8")
			);

			return addChapterFromJSON(chapterJSON);
		} catch (Exception e) {
			System.err.println(e.getMessage());
			return false;
		}
	}

	public static boolean addChapterFromJSON(final JSONObject json) {
		try {
			tryToAddChapterFromJSON(json);
		}
		catch (Exception e) {
			System.err.println(e.getMessage());
			return false;
		}
		return true;
	}

	private static void tryToAddChapterFromJSON(final JSONObject json)
	throws JSONException, InstantiationException {
		final String idChapter = json.getString("id");
		final String descriptionChapter = json.getString("description");
		final JSONArray listQuests = json.getJSONArray("quests");
		JSONObject doAtEndJSON = null;

		try {
			doAtEndJSON = json.getJSONObject("do_at_end");
			chapters.add(
				new Chapter(
					idChapter,
					Quest.createQuests(listQuests),
					descriptionChapter,
					new Chapter.DoAtEnd(doAtEndJSON)
				)
			);
		}
		catch (JSONException je) {
			chapters.add(
				new Chapter(
					idChapter,
					Quest.createQuests(listQuests),
					descriptionChapter
				)
			);
		}
	}

	public static Chapter getChapter(final String id) {
		for (final Chapter c : chapters)
			if ( c.getId().equals(id) )
				return c;

		return null;
	}

	public static Chapter getNextChapter(final Chapter chapter) {
		for (int i = 0; i < chapters.size() - 1; ++i)
			if ( chapters.get(i).getId().equals(chapter.getId()) )
				return chapters.get(i+1);

		return null;
	}

	public static Chapter getFirstChapter() {
		return chapters.get(0);
	}

	public static Quest getFirstQuest() {
		return getFirstChapter().getFirstQuest();
	}
}
