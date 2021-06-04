package fr.univparis.maljae.webserver;

import java.net.URL;
import java.io.File;
import java.io.IOException;
import org.junit.Test;

import fr.univparis.maljae.datamodel.*;

import static org.junit.Assert.*;

/**
 * Unit test for TeamController.
 */
public class ControllerTest {

	@Test
	public void testIsCanBeAdded() {
		init();
		studentCanBeAdded();
		studentCannotBeAdded();
	}

	private void studentCanBeAdded() {
		Team team = Teams.getTeam( "maljae1437854598" );
		Student student = Students.getStudent( "test1@etu.univ-paris-diderot.fr" );

		assertTrue( Controller.isCanBeAdded(student, team) );
	}

	private void studentCannotBeAdded() {
		Team team = Teams.getTeam( "maljae1437854598" );
		Student student = Students.getStudent( "test2@etu.univ-paris-diderot.fr" );

		assertFalse( Controller.isCanBeAdded(student, team) );
		
		student = Students.getStudent( "test4@etu.univ-paris-diderot.fr" );
		assertFalse( Controller.isCanBeAdded(student, team) );
		
		student = Students.getStudent( "test1@etu.univ-paris-diderot.fr" );
		team = Teams.getTeam( "maljae100534132" );
		assertFalse( Controller.isCanBeAdded(student, team) );
	}

	private void init() {
		String dataDirectory;

		loadConfiguration( "/config-test.json" );

		dataDirectory = Configuration.getDataDirectory();
		try {
			Teams.loadFrom( new File(dataDirectory + "/teams") );
		} catch (IOException ioe) {
			ioe.printStackTrace();
			System.out.println( "An error occured while loading teams for test in : "
								+ "TeamControllerTest.java l.44" );
		}
		Students.loadFrom( new File(dataDirectory + "/students") );
	}

	private void loadConfiguration(String configFile) {
		try {
			Configuration.loadFrom( getFile(configFile) );
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println( "Error while loading Configuration." );
		}
	}

	private File getFile(String filePath) {
		URL url = getClass().getResource( filePath );
		File input = new File(url.getFile());
		
		return( input );
	}
}