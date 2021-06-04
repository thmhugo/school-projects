package fr.univparis.maljae.datamodel;

import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.concurrent.ThreadLocalRandom;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

import org.apache.commons.io.FileUtils;
import org.json.JSONArray;
import org.json.JSONObject;

/**
 * Represents a team of student.
 */
public class Team {

	private static ArrayList<String> identifierArray = new ArrayList<String>();

	private String identifier;
	private ArrayList<Task> preferences;
	private ArrayList<Student> students;
	private Integer secret;

	public String getIdentifier() {
		return identifier;
	}

	public ArrayList<Task> getPreferences() {
		return preferences;
	}

	public Integer getSecret() {
		return secret;
	}

	public ArrayList<Student> getStudents() {
		return students;
	}

	/**
	 * Returns -1 when the team is empty.
	 */
	public String getGroupId() {
		String groupId = "-1";

		if ( students.size() > 0 )
			groupId = students.get(0).getGroupId();

		return( groupId );
	}

	/**
	 * @param studentMail is the student's email of the wanted student.
	 * @return the student if he's in students.
	 * Else throws a NullPointerException.
	 */
	public Student getStudent(String studentMail) {
		for(int i = 0; i < students.size(); ++i)
			if ( students.get(i).getEmail().compareTo(studentMail) == 0 )
				return( students.get(i) );
		throw new NullPointerException();
	}

	public int getNbStudents() {
		return( students.size() );
	}

	/**
	 * Updates the team's secret.
	 * @param s is the string representation of the new secret.
	 */
	public void updateSecretFromString(String s) {
		secret = Integer.parseInt(s);
	}

	/**
	 * Initializes a team with a student.
	 * @param creator is the first student of the team.
	 */
	Team (Student creator) {
		identifier = generateRandomTeamIdentifier();
		preferences = new ArrayList<Task>(Configuration.getTasks());
		students = new ArrayList<Student>(Configuration.getMaxNbUsersPerTeam());
		students.add(creator);
		secret = ThreadLocalRandom.current().nextInt(10, 100);
	}

	private void read_idendifer(JSONObject json, File f) {
		identifier = json.getString("identifier");

		if ( !f.getName().equals(identifier + "-team.json") )
			throw new RuntimeException(
				"Inconsistency in the data model: " + f.getName()
			);
	}

	private void read_secret(JSONObject json){
		secret = json.getInt("secret");
	}

	private void read_students(JSONObject json){
		JSONArray students_json = json.getJSONArray("students");

		students = new ArrayList<Student>(students_json.length());
		for (int i = 0; i < students_json.length(); i++)
			students.add(i, new Student(students_json.getJSONObject(i)));
	}

	private void read_preferences(JSONObject json){
		JSONArray preferences_json = json.getJSONArray("preferences");
		
		preferences = new ArrayList<Task>(preferences_json.length());
		for (int i = 0; i < preferences_json.length(); i++) {
			String pid = preferences_json.getString(i);

			preferences.add(i, Configuration.getTask(pid));
		}
	}

	/**
	 * Creates a team object from a file.
	 * @param f is the json file which contains team's informations.
	 * @throws IOException
	 */
	Team(File f) throws IOException {
		JSONObject json = new JSONObject(FileUtils.readFileToString(f, "utf-8"));

		read_idendifer(json,f);
		read_preferences(json);
		read_secret(json);
		read_students(json);
	}

	/**
	 * Saves the current team's state to a file.
	 * @param f is the json file.
	 * @throws IOException
	 */
	public void saveTo(File f) throws IOException {
		JSONObject json = new JSONObject();

		putIdentifier(json);
		putSecret(json);
		putPreferences(json);
		putStudent(json);
		writeJson(json,f);
	}

	private void putIdentifier(JSONObject json) {
		json.put( "identifier", identifier );
	}

	private void putSecret(JSONObject json) {
		json.put( "secret", secret );
	}

	private void putPreferences(JSONObject json) {
		JSONArray preferences_json = new JSONArray();

		for (int i = 0; i < preferences.size(); i++)
		  if ( preferences.get(i) != null )
				preferences_json.put( preferences.get(i).getIdentifier () );
		json.put( "preferences", preferences_json );
	}

	private void putStudent(JSONObject json) {
		JSONArray students_json = new JSONArray();

		for (int i = 0; i < students.size (); i++)
			students_json.put( students.get (i).toJSON () );
		json.put( "students", students_json );
	}

	private void writeJson (JSONObject json,File f) throws IOException {
		FileWriter fw = new FileWriter (f);

		fw.write( json.toString (2) );
		fw.close();
	}

	/**
	 * @return the string representation of the team's preferences.
	 */
	public String preferencesToString() {
		String result = "";

		for (int i = 0; i < preferences.size(); i++)
			result += preferences.get(i).getIdentifier() + ";";

		return result;
	}

	/**
	 * Updates preferences from a string.
	 * @param newPrefs is the string representation of new preferences.
	 */
	public void updatePreferencesFromString(String newPrefs) {
		String[] fields = newPrefs.split(";");
		ArrayList<Task> newPreferences = new ArrayList<Task>();
		boolean taskNotPresent = false;

		for (int i = 0; i < fields.length && !taskNotPresent; i++) {
			try {
				newPreferences.add(Configuration.getTask(fields[i]));
			} catch (Exception e) {
				taskNotPresent = true;
			}
		}

		if ( !isPermutation(newPreferences) || taskNotPresent )
			System.out.println("Error : Not a Permutation");
		else
			preferences = newPreferences;
	}

	private boolean isPermutation (ArrayList<Task> permutation){
		if ( permutation.size() != Configuration.getTasks().size() )
			return false;

		for (int i = 0; i < Configuration.getTasks().size(); i++)
				if( !permutation.contains(Configuration.getTasks().get(i)) )
					return false;

		return true;
	}

	/**
	 * @return a string representation of team's students.
	 */
	public String studentsToString() {
		String result = "";

		for (int i = 0; i < students.size(); i++)
			result += students.get(i).toString() + ";";

		return result;
	}

	/**
	 * Updates team's students from a string.
	 * @param who is the email of the student making the update.
	 * @param s is the string representation of the team's students.
	 * @return true if the update succed.
	 * @throws Exception
	 */
	public boolean updateStudentsFromString(String who, String s) throws Exception {
		String[] fields = s.split(";");
		ArrayList<Student> newStudents = new ArrayList<Student>();

		System.out.println(who + " " + s);

		for (int i = 0; i < fields.length; i++) {
			try {
				newStudents.add(Student.fromString(fields[i]));
			} catch (Exception e) {
				return( false );
			}
		}
		students = newStudents;

		return true;
	}

	/**
	 * @return a string representation of the team's informations.
	 */	
	public String toString (){
		String description = identifier + "\n";

		description += preferencesToString() + "\n";
		description += studentsToString() + "\n";
		description += "secret:" + secret;

		return description;
	}

	private static String generateRandomTeamIdentifier() {
		String generatedIdentifier;

		/** Generates a new identifier while isnt's unique. */
		do {
			generatedIdentifier = "maljae";
			generatedIdentifier += ThreadLocalRandom.current()
													.nextInt(10000, Integer.MAX_VALUE);
		} while( !identifierIsUnique(generatedIdentifier) );
		identifierArray.add( generatedIdentifier );

		return( generatedIdentifier );
	}

	/**
	 * Verifies that the generated identifier is unique
	 * in order to assign it to a team.
	 * @param generatedId is the tested identifier.
	 * @return true if the identifer isn't in identifierArray.
	 */
	public static boolean identifierIsUnique(String generatedId) {
		for (String s : identifierArray)
			if ( s.compareTo(generatedId) == 0 )
				return( false );
		return( true );
	}

	/**
	 * Verifies if the parameter is a valid file name.
	 * @param fname is the file name to test.
	 * @return true if the file is found.
	 */
	public static boolean isValidTeamFileName(String fname) {
		Pattern p = Pattern.compile(".*-team.json");
		Matcher m = p.matcher(fname);
		
		return m.find();
	}

	/**
	 * Removes a student from his email.
	 * @param email is the email of the student to remvove.
	 */
	public void removeStudent(String email) {
		for (int i = 0; i < students.size(); ++i)
			if ( students.get(i).getEmail().compareTo(email) == 0 )
				students.remove( i-- );
	}

	/**
	 * Assigns itself with a task with Assignment.assignTask(Team, Task).
	 * @param tasks Is a map that contains in key a task identifier
	 * and in value the number of assignment made with this task.
	 * @throws IOException
	 */
	public void assignTasks(HashMap<Task,Integer> tasks)throws IOException {
		for (Task t : preferences) {
			if (
				tasks.containsKey(t)
				&& tasks.get(t) < Configuration.getDefaultNbTeamsPerSubject()
			) {
				Assignment.assignTask( this, t );
				tasks.put( t, tasks.get(t) + 1 );
				return;
			}
		}
		throw new IOException ("Error no task avalaible for this team !");
	}

	/**
	 * @return true if the team has fewer students than the maximum
	 * number given by Configuration.
	 */
	public boolean isFull() {
		return( getNbStudents() >= Configuration.getMaxNbUsersPerTeam() );
	}
}