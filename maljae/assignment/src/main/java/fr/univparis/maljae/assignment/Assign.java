package fr.univparis.maljae.assignment;

import java.io.*;
import java.util.*;
import org.json.*;

import fr.univparis.maljae.datamodel.*;

/**
 * Source code of the command-line tool.
 */
public class Assign {
	
	private static ArrayList<Team> teams;
	private static ArrayList<Task> tasks;

	public static void main(String[] args) throws Exception {
		if ( args.length > 0 ) {
			Assign.init( new File(args[0]) );
			Assign.run();
		}
	}

	public static ArrayList<Task> getTasks() {
		return( tasks );
	}

	public static ArrayList<Team> getTeams() {
		return( teams );
	}

	/**
	 * Initializes Configuration and Teams for the task assignment.
	 * @throws Exception
	 */
	public static void init(File configFile) throws Exception {
		Assignment.clear();
		Configuration.loadFrom( configFile );
		Assign.tasks = Configuration.getTasks();
		loadRegisteredTeams();
	}

	private static void loadRegisteredTeams() throws IOException {
		String dataDirectory = Configuration.getDataDirectory() + "/teams";
		File teamsLocation = new File( dataDirectory );

		Teams.loadFrom( teamsLocation );
		Assign.teams = Teams.getTeams();
	}

	/**
	 * Runs the algorithm after removing the teams that were too small.
	 * @throws IOException
	 * @throws JSONException
	 */
	public static void run() throws IOException, JSONException {
		Assign.deleteTooSmallTeams();
		if ( Assign.verifyParity() ) {
			Assign.permutation();
			Assign.assignTasks();
			Assign.save();
		}
		else
			System.out.println( "L'assignation n'a pas pu etre realisee." );
	}
	 
	/**
	 * Deletes all teams that have less than the minimum number of students
	 * and saves them in deleted-teams.json.
	 */
	public static void deleteTooSmallTeams() throws IOException, JSONException{
		ArrayList<Team> deletedTeams = new ArrayList<Team>();

		for (int i = 0; i < Assign.teams.size(); ++i)
			if ( !Assign.goodNumberOfStudent(Assign.teams.get(i)) )
				deletedTeams.add( Assign.teams.remove(i--) );
		Assign.saveDeletedStudent( deletedTeams );
	}
	
	protected static void saveDeletedStudent(ArrayList<Team> deletedTeams)
	throws IOException, JSONException {
		String filePath = Configuration.getDataDirectory()
							+ "/deleted-teams.json";
		File f = new File(filePath);
		JSONObject json = Assign.putSaveInJson(deletedTeams);
		FileWriter fw = new FileWriter(f);

		fw.write( json.toString(2) );
		fw.close();
	}

	protected static void permutation() {
		teamSorting();
		ArrayList<Integer> secrets = Assign.getSecrets();
		ArrayList<Integer> seeds = Assign.getSeeds(secrets);
		teamPermutationFromSeeds(seeds);
	}

	protected static void teamSorting() {
		HashMap<String, Team> teamsIdByTheShortestEmail = new HashMap<String, Team>();
		ArrayList <String> mails = new ArrayList<String>();
		ArrayList<Team> sortedTeams = new ArrayList<Team>();

		fill( teamsIdByTheShortestEmail );
		fillMails( mails );
		fillSortedTeams( teamsIdByTheShortestEmail, mails, sortedTeams );
	}

	private static void fillSortedTeams(
		HashMap<String, Team> teamsIdByTheShortestEmail,
		ArrayList<String> mails,
		ArrayList<Team> sortedTeams
	) {
		for (int k = 0; k < teams.size() ; k++)
			sortedTeams.add(teamsIdByTheShortestEmail.get(mails.get(k)));
		teams =  sortedTeams;
	}

	private static void fillMails(ArrayList<String> mails) {
		for (int i = 0; i < teams.size(); i++)
			mails.add(Assign.getShortestEmail(teams.get(i)));
		mails = Assign.sortMail(mails);
	}

	private static void fill(HashMap<String, Team> hashM) {
		for (Team t : teams)
			hashM.put(Assign.getShortestEmail(t), t);
	}
	
	private static void teamPermutationFromSeeds(ArrayList<Integer> seeds) {
		/** k refers to the one given in the algorithm. */
		int k = teams.size() - 1;

		for (int i = 0; i < teams.size(); ++i) {
			int m = i + seeds.get(i) % (k - i + 1);
			swap(teams, i, m);
		}
	}
	
	/**
	 * Swaps the element at position i with the one at position j.
	 */
	private static <T> void swap(ArrayList<T> table, int i, int j) {
		if ( i < table.size() && j < table.size() ) {
			T tmp = table.get(i);
			table.set(i, table.get(j));
			table.set(j, tmp);
		}
	}

	/**
	 * Verifies there are enough tasks for all teams. 
	 */
	protected static boolean verifyParity() {
		int nbTeamsPerSubject = Configuration.getDefaultNbTeamsPerSubject();

		return( nbTeamsPerSubject * Assign.tasks.size() >= Assign.teams.size() );
	}

	protected static void assignTasks() throws IOException {
		HashMap<Task,Integer> tasksDistribution;

		tasksDistribution = new HashMap <Task,Integer>( tasks.size() );

		for (int i = 0; i < Assign.tasks.size(); ++i)
			tasksDistribution.put(Assign.tasks.get(i), 0);

		for (int i = 0; i < Assign.teams.size(); ++i)
			Assign.teams.get(i).assignTasks(tasksDistribution);
	}

	private static void save() throws IOException, JSONException {
		String filePath = Configuration.getDataDirectory() + "/assignment.json";
		
		Assignment.saveTo( new File(filePath) );
		System.out.println( "Assignment file saved at " + filePath );
	}

	protected static String getShortestEmail(Team t) {
		ArrayList<Student> studentList = t.getStudents();
		Student shortest = studentList.get(0);

		for (Student s : studentList) 
			if ( s.getEmail().length() < shortest.getEmail().length() )
				shortest = s;

		return( shortest.getEmail() );
	}

	protected static ArrayList<String> sortMail(ArrayList<String> mails) {
		mails.sort(
				new Comparator<String>() {
					public int compare(String mail1, String mail2) {
						return( mail1.length() - mail2.length() );
					};
				}
			);

		return( mails );
	}

	/**
	 * @return an arraylist that contains the secret of each team.
	 */
	protected static ArrayList<Integer> getSecrets() {
		ArrayList<Integer> secrets = new ArrayList<Integer>(teams.size());

		for (int i = 0; i < teams.size(); ++i)
			secrets.add(teams.get(i).getSecret());

		return( secrets );
	}

	protected static ArrayList<Integer> getSeeds(ArrayList<Integer> secrets){
		ArrayList<Integer> seeds = new ArrayList<Integer>();

		for (int i = 0; i < secrets.size(); ++i) {
			int calculatedSeed = 0;

			for (int j = 0 ; j < secrets.size(); ++j)
				if ( j != i )
					calculatedSeed += secrets.get(j);

			seeds.add(i,calculatedSeed);
		}

		return( seeds );
	}

	private static JSONObject putSaveInJson(ArrayList<Team> deletedTeams) {
		JSONObject json = new JSONObject();
		JSONArray teamsJson = new JSONArray();

		for (Team t : deletedTeams)
			teamsJson.put(t.studentsToString());
		json.put( "unconfirmed_students", teamsJson );

		return json;
	}

	private static boolean goodNumberOfStudent(Team team) {
		int min = Configuration.getMinNbUsersPerTeam();
		int max =Configuration.getMaxNbUsersPerTeam();

		return( team.getStudents().size() >= min
				&& team.getStudents().size() <= max );
	}
}