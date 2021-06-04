package fr.univparis.maljae.datamodel;

import java.io.*;
import java.util.*;

/** 
 * This module manages teams.
 */
public class Teams {

	private static final ArrayList<Team> teams = new ArrayList<Team>();

	/**
	 * Loads teams from a data directory.
	 * @param teamsDataDirectory is the data directory.
	 * @throws IOException
	 */
	public static void loadFrom(File teamsDataDirectory) throws IOException {
		Teams.clear();

		for (File f : teamsDataDirectory.listFiles() )
			if ( Team.isValidTeamFileName(f.getName()) )
				teams.add( new Team(f) );
	}

    /**
	 * Returns a team owning a certain identifier.
	 * @param identifier is the identifier of the desired team.
	 * @return a team if there is a team in teams with the identifier.
	 * @throws RuntimeException
	 */
	public static Team getTeam(String identifier) {
		for (Team team : teams)
			if ( team.getIdentifier ().equals(identifier) )
				return( team );

		throw new RuntimeException(
			"Team " + identifier + " isn't in teams."
		);
	}

	/**
	 * Returns teams.
	 * @return the arrayList representation of teams.
	 */
	public static ArrayList<Team> getTeams() {
		return( Teams.teams );
	}

	/**
	 * Removes a student from a team.
	 * @param email is the student's email.
	 */
	public static void removeFromExistingTeam(String email) throws IOException {
		for (Team team : teams) {
			team.removeStudent( email );
			saveTeam( team );
		}
	}

	/**
	 * Creates a team from a student.
	 * @return the new team.
	 */
	public static Team createTeam(Student student) throws IOException {
		removeFromExistingTeam( student.getEmail() );
		Team newTeam = new Team( student );
		teams.add( newTeam );

		String filename = Configuration.getDataDirectory()
						  + "/teams/"
						  + newTeam.getIdentifier()
						  + "-team.json";
		newTeam.saveTo( new File(filename) );

		return( newTeam );
	}

	/**
	 * Removes a team from the list and delete its file.
	 * @see #deleteTeamFile(Team)
	 * @see #deleteTokenFile(String)
	 * @param team is the team to delete.
	 */
	public static void deleteTeam(Team team, String token) throws IOException {
		teams.remove(team);
		deleteTokenFile(token);
		deleteTeamFile(team);
	}

	private static void deleteTokenFile(String token) throws IOException {
		String tokenFilePath = Configuration.getDataDirectory() 
								+ "/tokens/token" + token + ".json";
		File tokenFile = new File(tokenFilePath);

		tokenFile.delete();
	}

	private static void deleteTeamFile(Team team) throws IOException {
		String teamFilePath = Configuration.getDataDirectory() + "/teams/" 
								+ team.getIdentifier() + "-team.json";
		File teamFile = new File(teamFilePath);

		teamFile.delete();
	}

	/**
	 * Saves a team into a json file.
	 * @param team is the team to save.
	 */
	public static void saveTeam(Team team) throws IOException {
		String filename = Configuration.getDataDirectory () + "/teams/"
							+ team.getIdentifier () + "-team.json";

		team.saveTo( new File(filename) );
	}

	private static void clear() {
		if ( Teams.teams != null )
			Teams.teams.clear();
	}
}