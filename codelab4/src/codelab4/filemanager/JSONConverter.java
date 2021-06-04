package codelab4.filemanager;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

import org.json.JSONObject;

/**
 * Module to load a JSONobject from a jar archive.
 */
public class JSONConverter {

	private String JSONPath;

	public JSONConverter(String JSONPath) {
    	this.JSONPath = JSONPath;
	}

	public JSONObject getJSONObject() {
		final InputStream inputStreamObject = this.getClass().getResourceAsStream(JSONPath);

		try {
			final InputStreamReader inStreamReader = new InputStreamReader(
															inputStreamObject, "UTF-8"
														);
			final BufferedReader streamReader = new BufferedReader(inStreamReader);
			StringBuilder responseStrBuilder = new StringBuilder();
			String inputStr;

			while ((inputStr = streamReader.readLine()) != null)
				responseStrBuilder.append(inputStr);

			return new JSONObject(responseStrBuilder.toString());
		}
		catch (Exception e) {
			e.printStackTrace();
		}
		return null;
	}
}
