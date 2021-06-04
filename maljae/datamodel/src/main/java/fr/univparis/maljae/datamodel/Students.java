package fr.univparis.maljae.datamodel;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class Students {

	private static final ArrayList<Student> students = new ArrayList<Student>();

	/**
	 * Loads students from students json file stored in data-directory/students.
	 * @param studentsDataDirectory is the directory in which the students
	 * files are stored.
	 */
	public static void loadFrom(File studentsDataDirectory) {
		Students.clear();

		for (File f : studentsDataDirectory.listFiles() )
			if ( Students.isValidName(f.getName()) )
				students.add( new Student(f) );
	}

	public static Student getStudent(String studentEmail) {
		for (Student student : students)
			if ( student.getEmail().compareTo(studentEmail) == 0 )
				return( student );
		
		throw new RuntimeException(
					"Student" + studentEmail + " isn't loaded."
				);
	}

	/**
	 * Verifies that the student's name is in the form :
	 * <student_name>@etu.univ-paris-diderot.fr-student.json 		
	 * @param fileName is the student's name.
	 * @return true if the file's name is valid.
	 */
	public static boolean isValidName(String fileName) {
		Pattern pattern;
		Matcher matcher;
		
		pattern = Pattern.compile( ".*@etu.univ-paris-diderot.fr-student.json" );
		matcher= pattern.matcher( fileName );

		return( matcher.find() );
	}

	/**
	 * Updates the confirmed attribut of the student.
	 * If the student already exists, then its attribut is updated
	 * and its json file is overwritten.
	 * Else, the student is added to students and a new json file is saved.
	 * @param student is the new state of the student.
	 */
	public static void updateAndSaveStudent(Student student) {
		updateStudent( student );
		try {
			saveStudent( student );
		} catch (IOException ioe) {
			System.out.println(
				"An error occured while saving student (" + student + ")"
			);
			ioe.printStackTrace();
		}
	}

	private static void saveStudent(Student student) throws IOException {
		String filename = Configuration.getDataDirectory () + "/students/"
						+ student.getEmail() + "-student.json";

    	student.saveTo( new File(filename) );
	}

	private static void updateStudent(Student student) {
		try {
			getStudent( student.getEmail() );
		} catch (RuntimeException rte) {
			students.add( student );
		} finally {
			getStudent(student.getEmail()).setConfirmed( student.getConfirmed() );
		}
	}
	
	private static void clear() {
		if ( students != null )
			students.clear();
	}
}