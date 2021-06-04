package fr.univparis.maljae.datamodel;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertTrue;

import org.junit.Test;
import java.net.URL;
import java.io.File;

public class StudentsTest {

	@Test
	public void testStudentsLoad() throws Exception {
		loadConfiguration( "/config-students.json" );
		loadStudents();

		Student student = Students.getStudent( "jean.dupont@etu.univ-paris-diderot.fr" );
		assertEquals( student.getEmail(), "jean.dupont@etu.univ-paris-diderot.fr" );
		assertTrue( student.getConfirmed() );
	}

	private void loadConfiguration(String configFile) {
		try {
			Configuration.loadFrom( getFile(configFile) );
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println( "Error while loading Configuration." );
		}
	}

	private void loadStudents() {
		String studentsDirectory = Configuration.getDataDirectory()
													+ "/students";
		Students.loadFrom( new File(studentsDirectory) );
	}

	private File getFile(String filePath) {
		URL url = getClass().getResource( filePath );
		File input = new File(url.getFile());

		return( input );
	}
}