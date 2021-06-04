package fr.univparis.maljae.datamodel;

import org.json.JSONObject;

/** This class of objects represent task description. */
public class Task {

	private String identifier;
	private String title;
	private String url;
	private String description;

	/**
	 * Initializes a task from a JSONObject.
	 * @param jsonObject contains the task's informations needed.
	 */
	Task (JSONObject jsonObject) {
		identifier = jsonObject.getString( "identifier" );
		title = jsonObject.getString( "title" );
		url = jsonObject.getString( "url" );
		description = jsonObject.getString( "description" );
	}

	public String toString() {
		return(
			identifier + "\n" +
			title + "\n" +
			url + "\n" +
			description
		);
	}

	/**
	 * @return the task's identifer.
	 */
	public String getIdentifier () {
		return identifier;
	}
}