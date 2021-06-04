package fr.univparis.maljae.datamodel;

import static org.junit.Assert.*;

import org.junit.Test;
import java.io.*;
import java.net.*;

/**
 * Unit test for Configuration.
 */
public class ConfigurationTest {
	
	@Test
	public void versionMustBeAccessible() {
		assertTrue( !Configuration.version.equals("") );
	}

	/**
	 * Reading config.json must update the appropriate global variables.
	 */
	@Test
	public void testLoadFrom() throws Exception {
		URL url = getClass().getResource("/config.json");
		File input = new File( url.getFile() );

		Configuration.loadFrom( input );

		assertEquals(
			Configuration.showDateRange(), "2017-01-19 05:00:00 - 2017-01-23 12:00:00"
		);
		assertEquals( Configuration.getMinNbUsersPerTeam(), 3 );
		assertEquals( Configuration.getMaxNbUsersPerTeam(), 4 );
		assertEquals( Configuration.getDefaultNbTeamsPerSubject(), 3 );
		assertEquals(
			Configuration.getTask("task1").toString(),
			"task1\nsome title for task 1\nhttp://somewhere.fr\nA terrific task"
		);
		assertEquals(
			Configuration.getTask("task2").toString(),
			"task2\nsome title for task 2\nhttp://somewhereelse.fr\nA great task"
		);
		assertEquals( Configuration.getDataDirectory(), "./maljae-data" );
	}
}