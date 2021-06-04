package fr.univparis.maljae.datamodel;

import static org.junit.Assert.*;

import org.junit.Test;
import java.io.*;
import java.net.*;

/**
 * Unit test for the class Task.
 */
public class TaskTest {

	/**
	 * Verify that the Update from String of the Preference work well
	 */
	@Test
	public void testUnicityTask() throws Exception {
		URL url = getClass().getResource("/configTestUnicityOfTask.json");
		File input = new File(url.getFile());

		Configuration.loadFrom(input);

		assertEquals(Configuration.getTasks().size(), 2);
	}
}