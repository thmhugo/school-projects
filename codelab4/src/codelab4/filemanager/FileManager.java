package codelab4.filemanager;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import org.apache.commons.io.FileUtils;
import org.json.JSONException;
import org.json.JSONObject;

import javafx.stage.FileChooser;
import javafx.stage.FileChooser.ExtensionFilter;

public class FileManager {

	public static final int FILENAME_MAX_LEN = 100;

	public static boolean save(final String path, final String content) {
		FileWriter fw;

		try {
			fw = new FileWriter(path);
			fw.write(content);
			fw.close();
		}
		catch (IOException e) {
			e.printStackTrace();
			return false;
		}
        return true;
    }

    public static boolean save(final String path, final JSONObject content) {
        return save(path, content.toString());
	}

	public static void deleteFile(final String path) {
		final File fileToDelete = new File(path);

		if ( fileToDelete != null ) {
			if ( fileToDelete.delete() )
				System.out.println("<INFO> - '" + path + "' deleted.");
			else {
				System.out.println(
					"<INFO> - An error occured while trying to remove '"
					+ path + "' deleted."
				);
			}
		}
	}

    public static JSONObject getJson(final String path) throws IOException, JSONException {
        final File file = new File(path);
        final JSONObject json = new JSONObject(FileUtils.readFileToString(file, "utf-8"));

		return json;
	}

	public static boolean isAValidFileName(final String fileName) {
		return !fileName.contains(" ") && fileName.length() < FILENAME_MAX_LEN;
	}

	public static void saveCode(final String code) throws Exception {
		FileChooser fileChooser = createFileChooser();
		fileChooser.setInitialFileName(".txt");
		final File file = fileChooser.showSaveDialog(null);

        save(file.getAbsolutePath(), code);
	}

	private static FileChooser createFileChooser() {
		ExtensionFilter extensionFilter = new ExtensionFilter("Plain text", "*.txt");
		final FileChooser fileChooser = new FileChooser();

		fileChooser.getExtensionFilters().add(extensionFilter);

		return fileChooser;
	}

	public static String loadCodeFile() throws Exception {
		FileChooser fileChooser = createFileChooser();
		final File file = fileChooser.showOpenDialog(null);

        fileChooser.setTitle("Open Code File");

		return getFileContent(file);
	}

	private static String getFileContent(final File file) throws Exception {
		final BufferedReader br = new BufferedReader(new FileReader(file));
		StringBuilder sb = new StringBuilder();
		String line = br.readLine();

		while (line != null) {
			sb.append(line);
			sb.append(System.lineSeparator());
			line = br.readLine();
		}
		br.close();

		return sb.toString();
	}
}
