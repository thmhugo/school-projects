package fr.univparis.maljae.datamodel;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashMap;
import java.util.LinkedList;

import org.apache.commons.io.FileUtils;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

/**
 * This module stores tasks assignment.
 */
public class Assignment {

	/** For each team, there must be exactly one task. */
	private static HashMap<Team, Task> team_tasks = new HashMap<Team, Task>();
	private static LinkedList<String> trace;

	public static LinkedList<String> getTrace() {
		return trace;
	}

	/**
	 * Updates the trace.
	 * @param s is the string representation of a step.
	 */
	public static void addTraceStep(String s)  {
		if (trace == null)
			trace = new LinkedList<String>();
		trace.addLast(s);
	}

	/**
	 * Assigns a task to a team, only if the team doesn't already have one.
	 * And update the trace @see #addTraceStep(String).
	 * @param team is the team.
	 * @param task is the task.
	 */
	public static void assignTask(Team team, Task task) {
		if ( team_tasks.containsKey(team) )
			throw new RuntimeException(
				"Team " + team.getIdentifier() + " already has a task."
			);
		team_tasks.put(team, task);
		addTraceStep(team.getIdentifier() + " is assigned task " + task.getIdentifier());
	}

	/**
	 * Returns a string representation of the current state.
	 * @return the description.
	 */
	public static String show() {
		String description = "";

		description += team_tasks.toString() + "\n";
		for (String step: Assignment.trace)
			description += step+"\n";

		return description ;
	}

	/**
	 * Initializes its attributes from a json file.
	 * @param f a JSON file with two keys : assignment and trace.
	 * @throws IOException
	 * @throws JSONException
	 */
	public static void loadFrom(File f) throws IOException, JSONException {
		JSONObject json = new JSONObject(FileUtils.readFileToString(f, "utf-8"));
		JSONArray assignment_json = json.getJSONArray("assignment");
		JSONArray traceability = json.getJSONArray("trace");

		trace.clear();
		for (int j = 0; j < traceability.length(); ++j)
			trace.addLast(traceability.getString(j));

		for (int i = 0; i < assignment_json.length(); i++) {
			JSONArray team_task = assignment_json.getJSONArray(i);
			Team team = Teams.getTeam(team_task.getString(0));
			Task task = Configuration.getTask(team_task.getString(1));

			assignTask(team, task);
		}
	}

	/**
	 * Saves data (assignment and trace) to a JSON file.
	 * @param f is the file to be saved in.
	 * @throws IOException
	 * @throws JSONException
	 */
	public static void saveTo(File f) throws IOException, JSONException {
		JSONObject json = new JSONObject();
		JSONArray assignment_json = new JSONArray();

		json.put( "trace", trace );
		for (HashMap.Entry<Team, Task> assignment : team_tasks.entrySet()) {
			JSONArray team_task = new JSONArray();

			team_task.put( assignment.getKey().getIdentifier() );
			team_task.put( assignment.getValue().getIdentifier() );
			assignment_json.put( team_task );
		}

		json.put( "assignment", assignment_json );
		FileWriter fw = new FileWriter(f);
		fw.write( json.toString(2) );
		fw.close();
	}

	/** 
	 * Clears team_tasks and trace if they aren't empty.
	 */
	public static void clear() {
		if ( Assignment.team_tasks != null )
			Assignment.team_tasks.clear();
		if ( Assignment.trace != null )
			Assignment.trace.clear();
	}

	/**
	 * Get the task assigned to the team t.
	 * @param t is the team whose task we want to recover.
	 */
	public static String getAssignedTask(Team t) {
		return( Assignment.team_tasks.get(t).toString() );
	}
}