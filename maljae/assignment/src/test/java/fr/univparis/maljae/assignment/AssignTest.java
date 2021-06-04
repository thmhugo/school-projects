package fr.univparis.maljae.assignment;

import fr.univparis.maljae.datamodel.*;

import static org.junit.Assert.*;

import java.io.*;
import java.net.*;
import java.util.ArrayList;
import java.util.HashMap;

import org.apache.commons.io.FileUtils;
import org.json.*;
import org.junit.*;

/**
 * Unit test for assign.
 */
public class AssignTest {
	
	/**
     * Setup console redirection.
	 */
	private final ByteArrayOutputStream outContent = new ByteArrayOutputStream();
	private final ByteArrayOutputStream errContent = new ByteArrayOutputStream();
	private final PrintStream originalOut = System.out;
	private final PrintStream originalErr = System.err;

	@Before
	public void setUpStreams()
	{
		System.setOut(new PrintStream(outContent));
		System.setErr(new PrintStream(errContent));
	}

	@After
	public void restoreStreams()
	{
		System.setOut(originalOut);
		System.setErr(originalErr);
	}

	@Test
	public void testInit() throws Exception {
		ArrayList<Task> configTasks, assignTasks;
		ArrayList<Team> teams, assignTeams;

		File input = getConfigFile("/config1.json");

		Configuration.loadFrom( input );
		Assign.init( input );

		configTasks = Configuration.getTasks();
		assignTasks = Assign.getTasks();
		assertTrue( configTasks.equals(assignTasks) );

		teams = Teams.getTeams();
		assignTeams = Assign.getTeams();
		assertTrue( teams.equals(assignTeams) );
	}
	
	private File getConfigFile(String str) {
		URL url = getClass().getResource(str);
		File file = new File (url.getFile());
		
		return( file );
	}

	@Test
	public void testVerifyParity() throws Exception {
		thereIsEnoughTasks();
		thereIsntEnoughTasks();
	}

	private void thereIsntEnoughTasks() throws Exception {
		Configuration.loadFrom( getConfigFile("/config2.json") );
		Assign.init( getConfigFile("/config2.json") );
		
		assertTrue( !Assign.verifyParity() );
	}

	private void thereIsEnoughTasks() throws Exception {
		Configuration.loadFrom( getConfigFile("/config1.json") );
		Assign.init( getConfigFile("/config1.json") );
		
		assertTrue( Assign.verifyParity() );
	}

	/**
	 * Verifies that the run function works properly.
	 * It should create an assignment.json file and it should
	 * have a trace and an assignment attribute which have for
	 * each team a task assigned.
	 * @throws Exception
	 */
	@Test
	public void testRun() throws Exception {
		/** One team, two tasks. */
		testRunBis( getConfigFile("/config1.json") );
	}

	@Test
	public void testRun2() throws Exception {
		/** If there is no teams in /teams */
		testRunBis( getConfigFile("/config3.json") );
	}
	
	private void testRunBis(File input) throws Exception {
		int verify;
		String msg = "";

		Assign.init( input );
		Assign.run();

		verify = verifyAssignmentFile(input);
		switch ( verify ) {
			case -1 : msg = "The run function dosn't produce an assignment file."; break;
			case 0 : msg = "There is a problem in the assignment file."; break;
			default : msg = "The run function works correctly."; break;
		}

		assertEquals( msg, 1, verify );
	}

	/**
	 * Verifies the assignment file contains everythings needed
	 * and that all teams are assigned.
	 * @param input is the File representation of the json config file of the test.
	 * @return -1 if there is no assignment.json in the Configuration data-directory ;
	 * 0 if there is a file but it contains some failures ;
	 * 1 if there is a file and it's all good.
	 * @throws Exception
	 */
	private int verifyAssignmentFile(File input) throws Exception {
		File assignmentFile;
		String[] assignmentTeams;
		JSONObject assignmentJson;
		ArrayList<Team> teams = Assign.getTeams();

		assignmentFile = getAssignmentFile( input );
		assignmentJson = getJsonObjectFrom( assignmentFile );
		if ( assignmentJson == null )
			return( -1 );

		if ( !traceIsFull(assignmentJson, teams) )
			return( 0 );

		assignmentTeams = getAssignmentTeams( assignmentJson );
		if ( !allTeamsHaveOneTask(assignmentTeams) )
			return( 0 );

		return( 1 );
	}
	
	/**
	 * Verifies that all the teams have one and only one task.
	 * @param assignmentTeams is a String array of all team's id of
	 * the assignment file.
	 * @return true, only if, all the team are assign to exactly one task.
	 */
	private boolean allTeamsHaveOneTask(String[] assignmentTeams) {
		HashMap<String, Integer> tasksPerTeams;
		ArrayList<Team> teams = Assign.getTeams();

		tasksPerTeams = getTasksPerTeams( teams );

		return( exactlyOneTask(tasksPerTeams) );
	}
	
	private HashMap<String, Integer> getTasksPerTeams(ArrayList<Team> teams) {
		HashMap<String, Integer> tasksPerTeams = new HashMap<String, Integer>();

		for(Team t : teams)
			tasksPerTeams.put( t.getIdentifier(), 0 );

		return( tasksPerTeams );
	}
	
	private boolean exactlyOneTask(HashMap<String, Integer> tasksPerTeams) {
		for (String t : tasksPerTeams.keySet())
			if ( tasksPerTeams.get(t) != 1 )
				return( false );

		return( true );
	}
	
	private File getAssignmentFile(File input) {
		File assignmentFile = null;
		
		try {
			Configuration.loadFrom( input );
			assignmentFile = new File( Configuration.getDataDirectory()
										+ "/assignment.json" );
		} catch (Exception e) {
			System.out.println( "File could not be opened." );
			System.out.println( e.getStackTrace() );
		}
		return( assignmentFile );
	}

	private JSONObject getJsonObjectFrom(File assignmentFile) {
		JSONObject assignmentJson = null;

		try {
			assignmentJson= new JSONObject(
									FileUtils.readFileToString(assignmentFile, "utf-8")
								);
			assignmentFile.delete();
		} catch (Exception e) {
			System.out.println( "Could not get json object from assignment file." );
			System.out.println( e.getStackTrace() );
		}

		return( assignmentJson );
	}

	private boolean traceIsFull(JSONObject assignmentJson, ArrayList<Team> teams) {
		JSONArray trace = assignmentJson.getJSONArray( "trace" );
		return( trace.length() == teams.size() );
	}

	private String[] getAssignmentTeams(JSONObject assignmentJson) {
		JSONArray assignment = assignmentJson.getJSONArray( "assignment" );
		String[] assignmentTeams = new String[assignment.length()];

		for(int i = 0; i < assignment.length(); ++i)
			assignmentTeams[i] = assignment.getJSONArray(i).getString(0);

		return( assignmentTeams );
	}

	@Test
	public void testMailSorting() throws Exception {
		ArrayList<String> sortedEmails = getSortedEmails();
		ArrayList<String> orderedEmails = getReferenceOrderedEmails();

		assertTrue( sortedEmails.equals(orderedEmails) );
	}

	private ArrayList<String> getSortedEmails() {
		ArrayList<String> unsortedEmails = new ArrayList<String>( 3 );

		unsortedEmails.add("thisisthelongestmail@mail.com");
		unsortedEmails.add("ashortone@mail.com");
		unsortedEmails.add("one@mails.com");
		
		return( Assign.sortMail(unsortedEmails) );
	}

	private ArrayList<String> getReferenceOrderedEmails() {
		ArrayList<String> orderedmails = new ArrayList<String>( 3 );
		
		orderedmails.add("one@mails.com");
		orderedmails.add("ashortone@mail.com");
		orderedmails.add("thisisthelongestmail@mail.com");

		return( orderedmails );
	}

	@Test
	public void testGetShortestMail() throws Exception{
		Assign.init( getConfigFile("/config2.json") );
		Team t = Teams.getTeam("maljae1465097432");

		assertEquals("a@a", Assign.getShortestEmail(t));
	}

	@Test
	public void testAssociateSeeds(){
		ArrayList<Integer> secrets = new ArrayList<Integer>();
		
		secrets.add(10);
		secrets.add(52);
		secrets.add(1);
		secrets.add(28);

		ArrayList<Integer> seeds = Assign.getSeeds(secrets);

		assertEquals(81, (int)seeds.get(0));
		assertEquals(39, (int)seeds.get(1));
		assertEquals(90, (int)seeds.get(2));
		assertEquals(63, (int)seeds.get(3));
	}

	@Test
	public void testDeleteTeam() throws Exception {
		Assign.init( getConfigFile("/config2.json") );
		assertEquals(6, Assign.getTeams().size());
		Assign.deleteTooSmallTeams();
		assertEquals(3, Assign.getTeams().size());
	}

	@Test
	public void testSorting() throws Exception {
		Assign.init( getConfigFile("/config2.json") );
		Assign.deleteTooSmallTeams();
		
		assertEquals(3, Assign.getTeams().size());

		Assign.teamSorting();
		
		assertEquals("maljae1465097432", Assign.getTeams().get(0).getIdentifier());
		assertEquals("maljae1964485357", Assign.getTeams().get(2).getIdentifier());
		assertEquals("maljae1447015799", Assign.getTeams().get(1).getIdentifier());
	}

	/**
	 * Verify that the whole algorithm works in a normal use.
	 */
	@Test
	public void testAlgorithm() throws Exception {
		Assign.init( getConfigFile("/config2.json") );
		Assign.deleteTooSmallTeams();
		testPermutation();
		testAssignTasks();
	}

	private void testPermutation() {
		Assign.permutation();
		assertEquals("maljae1447015799", Assign.getTeams().get(0).getIdentifier());
		assertEquals("maljae1465097432", Assign.getTeams().get(1).getIdentifier());
		assertEquals("maljae1964485357", Assign.getTeams().get(2).getIdentifier());
	}
	
	private void testAssignTasks() throws IOException {
		String task1 = Configuration.getTask("task1").toString();
		String task2 = Configuration.getTask("task2").toString();

		Assign.assignTasks();
		assertEquals(task2, Assignment.getAssignedTask(Assign.getTeams().get(0)));
		assertEquals(task2, Assignment.getAssignedTask(Assign.getTeams().get(1)));
		assertEquals(task1, Assignment.getAssignedTask(Assign.getTeams().get(2)));
	}

 	/**
  	 * Verify that the algorithm doesn't crash when every team get deleted because
	 * there wasn't enough student.
 	 */
 	@Test
 	public void testAlgorithm2() throws Exception {
		Assign.init( getConfigFile("/config1.json") );
 		Assign.run();
 	}

	/**
	 * There is only one team that got deleted and his only student is erw@ewar/true;
	 */
	@Test
	public void testSaveDeletedStudentTeam() throws Exception {
		Assign.init(getConfigFile( "/config1.json" ));
		Assign.deleteTooSmallTeams();

		File test = new File( Configuration.getDataDirectory()
								+ "/deleted-teams.json" );
		JSONObject json = new JSONObject( FileUtils.readFileToString(test, "utf-8") );
		JSONArray saveTest = json.getJSONArray( "unconfirmed_students" );

		assertEquals(saveTest.getString(0), "erw@ewar/true/9;");
	}
}