package fr.univparis.maljae.datamodel;

import static org.junit.Assert.*;

import org.junit.Test;
import java.io.*;
import java.net.*;

/**
 * Unit test for the class Team.
 */
public class TeamTest {

	@Test
	public void testUpdatePreference() throws Exception {
		URL url = getClass().getResource("/config.json");
		File input = new File(url.getFile());
		Configuration.loadFrom(input);

		URL url1 = getClass().getResource("/rocket-team.json");
		File test = new File(url1.getFile());
		Team rocket = new Team(test);

		// Shoud do nothing because it's not a permutation.
		rocket.updatePreferencesFromString("");
		assertEquals(
			rocket.getPreferences().get(0).toString(),
			"task2\nsome title for task 2\nhttp://somewhereelse.fr\nA great task"
		);
		assertEquals(
			rocket.getPreferences().get(1).toString(),
			"task1\nsome title for task 1\nhttp://somewhere.fr\nA terrific task"
		);

		// Should do nothing because task3 is not in Tasks.
		rocket.updatePreferencesFromString("task1;task2;task3");
		assertEquals(
			rocket.getPreferences().get(0).toString(),
			"task2\nsome title for task 2\nhttp://somewhereelse.fr\nA great task"
		);
		assertEquals(
			rocket.getPreferences().get(1).toString(),
			"task1\nsome title for task 1\nhttp://somewhere.fr\nA terrific task"
		);
		assertEquals( rocket.getPreferences().size(), 2 );

		// Sould do nothing because the argument are not well formated.
		rocket.updatePreferencesFromString( ";task1 ;; task2" );
		assertEquals(
			rocket.getPreferences().get(0).toString(),
			"task2\nsome title for task 2\nhttp://somewhereelse.fr\nA great task"
		);
		assertEquals(
			rocket.getPreferences().get(1).toString(),
			"task1\nsome title for task 1\nhttp://somewhere.fr\nA terrific task"
		);

		// Should invert task1 and task2.
		rocket.updatePreferencesFromString( "task1;task2" );
		assertEquals(
			rocket.getPreferences().get(0).toString(),
			"task1\nsome title for task 1\nhttp://somewhere.fr\nA terrific task"
		);
		assertEquals(
			rocket.getPreferences().get(1).toString(),
			"task2\nsome title for task 2\nhttp://somewhereelse.fr\nA great task"
		);
		assertEquals( rocket.getPreferences().size(), 2 );
	}
}