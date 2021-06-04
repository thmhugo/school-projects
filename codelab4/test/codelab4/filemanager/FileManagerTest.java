package codelab4.filemanager;

import static org.junit.Assert.*;
import java.net.URL;
import org.json.JSONObject;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

public class FileManagerTest {
	private String testPath;
	private JSONObject json;

	@Before
	public void init() throws Exception{
		URL url = this.getClass().getResource("/json/chapters/chapterTest.json");
		testPath = url.getPath();
		json = FileManager.getJson(testPath);
	}

	@After
	public void clean() {}

	/**
	 * Tests the FileManager.load(String) function.
	 */
	@Test
	public void testLoad() throws Exception {
		assertNotNull("The path is incorrect or the file doesn't exist.",json);
	}

	@Test(expected = NullPointerException.class)
	public void testLoadBadPath() throws Exception {
		URL url = this.getClass().getResource("/json/chapters/chapterTestff.json");
		testPath = url.getPath();
		json = FileManager.getJson(testPath);
	}

	@Test
	public void testIsAValidFileName() {
		assertTrue(FileManager.isAValidFileName("filename"));
		assertTrue(FileManager.isAValidFileName("filename.extention"));
		assertFalse(FileManager.isAValidFileName("filename ext"));
		assertFalse(
			FileManager.isAValidFileName(
				"ddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd"
			)
		);
	}
}