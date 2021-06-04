package fr.univparis.maljae.datamodel;

import java.io.File;
import java.util.ArrayList;
import java.util.Date;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import org.apache.commons.io.FileUtils;
import org.json.*;

/**
 * This module gives access to the configuration of the maljae instance.
 */
public class Configuration {
	
	/** Version. We following semantic versioning conventions. */
	public static final String version = "1.0";
	private static String dataDirectory;
	/** Dates are supposed to be written with the following format. */
	private static SimpleDateFormat dateFormat = new SimpleDateFormat(
														"yyyy-MM-dd hh:mm:ss"
													);
	/** Opening date. Before this date, nobody can create a team. */
	private static Date openingDate = null;
	/** Closing date. After this date, nobody can change team-related data. */
	private static Date closingDate = null;
	private static int minNbUsersPerTeam = -1;
	private static int maxNbUsersPerTeam = -1;
	private static int defaultNbTeamsPerSubject = -1;
	private static ArrayList<Task> tasks;
	
	public static String getDataDirectory() {
		return dataDirectory;
	}

	/**
	 * Returns the opening date and the closing date.
	 * @return a string representation of the date range.
	 */
	public static String showDateRange() {
		return dateFormat.format(openingDate) + " - " + dateFormat.format(closingDate);
	}

	public static int getMinNbUsersPerTeam() {
		return minNbUsersPerTeam;
	}

	public static int getMaxNbUsersPerTeam() {
		return maxNbUsersPerTeam;
	}

	public static int getDefaultNbTeamsPerSubject() {
		return defaultNbTeamsPerSubject;
	}

	public static ArrayList<Task> getTasks() {
		return tasks;
	}

	/**
	 * Returns a task identified by its id.
	 * @param identifier is the string representation of the task id.
	 * @return the task if the identifier correspond to the task id.
	 * @throws RuntimeException
	 */
	public static Task getTask(String identifier) {
		for (int i = 0; i < tasks.size(); i++) {
			String taskId = tasks.get(i).getIdentifier();

			if ( taskId.equals(identifier) )
				return tasks.get(i);
		}
		throw new RuntimeException( "Task " + identifier + " isn't here." );
	}

	/**
	 * Initializes all Configuration attributes from the json config file.
	 * @param file is the json config file.
	 * @throws Exception
	 */
	public static void loadFrom(File file) throws Exception {
		JSONObject	json = new JSONObject(FileUtils.readFileToString(file, "utf-8"));
		JSONObject	rangeNbUsersPerTeam = json.getJSONObject("nb_users_per_team");
		JSONObject	rangeNbTeamsPerSubject = json.getJSONObject("nb_teams_per_subject");
		JSONArray	json_tasks = json.getJSONArray("tasks");

		loadDatesFrom( json );
		loadRangeNbUsersPerTeamFrom( rangeNbUsersPerTeam );
		defaultNbTeamsPerSubject = rangeNbTeamsPerSubject.getInt("default");
		dataDirectory = json.getString("data_directory");
		loadTaksFrom( json_tasks );
	}

	private static void loadDatesFrom(JSONObject json) throws ParseException {
		openingDate = dateFormat.parse(json.getString("opening_date"));
		closingDate = dateFormat.parse(json.getString("closing_date"));
	}

	private static void loadRangeNbUsersPerTeamFrom(JSONObject rangeNbUsersPerTeam) {
		minNbUsersPerTeam = rangeNbUsersPerTeam.getInt("min");
		maxNbUsersPerTeam = rangeNbUsersPerTeam.getInt("max");
	}

	private static void loadTaksFrom(JSONArray json_tasks) {
		tasks = new ArrayList<Task>();
		tasks.add( new Task(json_tasks.getJSONObject(0)) );

		for (int index = 1; index < json_tasks.length(); ++index) {
			Task tmp = new Task( json_tasks.getJSONObject(index) );

			if ( Configuration.isUnique(tmp) )
				tasks.add(tmp);
		}
	}

	private static boolean isUnique(Task task) {
		for (Task element : tasks)
			if ( element.getIdentifier().equals(task.getIdentifier()) )
				return( false );
		
		return( true );
	}
}