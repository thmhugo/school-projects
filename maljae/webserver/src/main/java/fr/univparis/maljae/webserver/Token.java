package fr.univparis.maljae.webserver;

import java.util.*;
import java.io.*;
import java.lang.Math;
import org.apache.commons.io.FileUtils;
import org.json.*;
import fr.univparis.maljae.datamodel.Configuration;
import fr.univparis.maljae.datamodel.Teams;
import fr.univparis.maljae.datamodel.Team;

/**
 * Manages tokens.
 * Each token stores one team and the student's email who created the team.
 */
public final class Token {

	private static HashMap<Integer, Token> tokens = new HashMap<Integer, Token>();

	/**
	 * raw is the tokens's key for the hash map (= |team.hashCode() + email.hashCode()|)
	 * and the file's name : <data_directory>/token" + raw + ".json"
	 * */
	private Integer raw = 0;
	private Team team;
	private String email;

	/**
	 * Creates a token object.
	 * @param team0 is the team stored.
	 * @param email0 is the email stored.
	 * @param raw0 is the token's identifier used to name the token's file.
	 */
	Token(Team team0, String email0, Integer raw0) {
		team = team0;
		email = email0;
		raw = raw0;
		tokens.put( raw0, this );
	}

	/**
	 * Creates and saves a new token.
	 * @param team0 is the team associates to the new token.
	 * @param email0 is the student's email who creates the team.
	 * @return the token.
	 * @throws IOException
	 */
	public static Token createToken(Team team0, String email0) throws IOException {
		Integer raw = Math.abs( team0.hashCode() + email0.hashCode() );
		Token token = new Token( team0, email0, raw );

		token.saveToFile();

		return( token );
	}

	/**
	 * Returns a token.
	 * If the token isn't in tokens then loads it from it file.
	 * @param raw is the token's hash code.
	 * @return the token.
	 * @throws IOException
	 */
	public static Token getToken(String raw) throws IOException {
		Integer i = Integer.parseInt( raw );
		Token result = null;

		result = tokens.get( i );
		if ( result == null )
			result = loadFromFile( i );

		return( result );
	}

	/**
	 * Creates a token from its json file.
	 * @param raw is the token's hash code.
	 * @return the token.
	 * @throws IOException
	 */
	public static Token loadFromFile(Integer raw) throws IOException {
		File f = new File(
			Configuration.getDataDirectory() + "/tokens/token" + raw + ".json"
		);
		JSONObject json = new JSONObject( FileUtils.readFileToString(f, "utf-8") );
		Team team = Teams.getTeam( json.getString("team") );
		String email = json.getString( "email" );

		return( new Token(team, email, raw) );
	}

	/**
	 * Saves the current token to a json file in data-directory/tokens.
	 * @throws IOException
	 */
	public void saveToFile() throws IOException {
		File f = new File(
			Configuration.getDataDirectory() + "/tokens/token" + raw + ".json"
		);
		FileWriter fw = new FileWriter(f);
		JSONObject json = new JSONObject();

		json.put( "team", team.getIdentifier() );
		json.put( "email", email );
		fw.write( json.toString() );
		fw.close();
	}

	String getEmail() {
		return( email );
	}

	Team getTeam() {
		return( team );
	}

	/**
	 * Returns a string representation of the current token's hash code.
	 * @return the string representation of raw.
	 */
	@Override
	public String toString() {
		return( raw.toString() );
	}
}