package fr.univparis.maljae.datamodel;

import org.apache.commons.io.FileUtils;
import org.json.*;
import java.io.*;

/**
 * Represents a student.
 */
public class Student {

	private String email;
	private Boolean confirmed;
	private String groupId;

	Student (JSONObject json) {
		if ( json != null ) {
			email = json.getString( "email" );
			confirmed = json.getBoolean( "confirmed" );
			groupId = json.getString( "groupId" );
		}
	}

	/**
	 * @param email is a string representation of the email.
	 * @param confirmed says if the student has confirmed that he is in a team.
	 * @param groupId is a string representation of the group identifier.
	 */
	public Student (String email, Boolean confirmed, String groupId) {
		this.email = email;
		this.confirmed = confirmed;
		this.groupId = groupId;
	}

	/**
	 * @param file is the json files that contains student's informations.
	 */
	public Student(File file) {
		this( getJsonFromFile(file) );
	}

	private static JSONObject getJsonFromFile(File file) {
		JSONObject json = new JSONObject();

		try {
			json = new JSONObject( FileUtils.readFileToString(file,"utf-8") );
		} catch (Exception e) {
			e.printStackTrace();
			System.out.println(
				"An error occured"+ "while loadding json object from : " + file
			);
		}
		return( json );
	}

	public String getEmail() {
		return email;
	}

	public Boolean getConfirmed() {
		return confirmed;
	}

	public String getGroupId() {
		return groupId;
	}

	public void setGroupId(String groupId) {
		this.groupId = groupId;
	}

	public void setConfirmed(Boolean confirmed) {
		this.confirmed = confirmed;
	}

	/**
	 * @return a string representation of the student's informations.
	 */
	public String toString () {
		return email + "/" + confirmed + "/" + groupId;
	}

	/**
	 * Returns a new student initializes from a string.
	 * @param s is a string representation of a student's informations.
	 * @return a new student.
	 */
	public static Student fromString (String s) {
		String[] fields = s.split ("/");

		if ( fields.length == 3 )
			return(
 				new Student(fields[0], Boolean.parseBoolean(fields[1]), fields[2])
			);
		else
			throw new RuntimeException("Bad format");
	}

	/**
	 * @return a JSONObject representation of the student's informations.
	 */
	public JSONObject toJSON() {
		JSONObject json = new JSONObject();

		json.put( "email", email );
		json.put( "confirmed", confirmed );
		json.put( "groupId", groupId );

		return( json );
	}

	/**
	 * Saves the student's informations into a json file.
	 * @param file is the json file.
	 * @throws IOException
	 */
	public void saveTo(File file) throws IOException {
		JSONObject json = this.toJSON();
		FileWriter fw = new FileWriter( file );

		fw.write( json.toString(2) );
		fw.close();
	}
}