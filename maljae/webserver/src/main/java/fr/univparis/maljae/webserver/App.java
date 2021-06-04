package fr.univparis.maljae.webserver;

import java.io.File;
import java.io.IOException;

import fr.univparis.maljae.datamodel.Configuration;
import fr.univparis.maljae.datamodel.Students;
import fr.univparis.maljae.datamodel.Teams;
import io.javalin.Javalin;

/**
 * A webserver
 */
public class App {
	/**
	 * Runs a Javalin's app.
	 * @param args is the path of the configuration file.
	 * @throws Exception
	 */
	public static void main (String[] args) {
		Javalin app;

		if ( args.length > 0 ) {
			about();				
			loadConfiguration( args[0] );
			initialize();
			app = launch();
			Controller.install( app );
		}
		else
			System.out.println( "Missing configuration file argument." );
	}

	/**
	 * Prints informations about the current app.
	 */
	public static void about() {
		System.out.println(
			"maljae server version "+ Configuration.version + " running."
		);
	}

	/**
	 * Creates a Javalin's app. 
	 * @return the app.
	 */
	public static Javalin launch() {
		Javalin app = Javalin.create(
			config -> {
				config.addStaticFiles( "public/" );
				config.enableWebjars();
			}
		).start( 8080 );

		return( app );
	}

	/**
	 * Loads configurations from a json file.
	 * @param configFile is the path of the configuration file.
	 * @throws Exception
	 */
	public static void loadConfiguration(String configFile) {
		try {
			Configuration.loadFrom( new File (configFile) );
		} catch (Exception e) {
			System.out.println(
				"An error occured while trying to load Configuration:\n"
			);
			e.printStackTrace();	
		}
	}

	/**
	 * Initializes teams from data-directory's files. 
	 * @throws IOException
	 */
	public static void initialize() {
		createSubDataDirectories();
		try {
			String dataDir = Configuration.getDataDirectory();

			Teams.loadFrom( new File(dataDir + "/teams") );
			Students.loadFrom( new File(dataDir + "/students") );
		} catch (IOException ioe) {
			System.out.println( "An error occured while trying to load Teams :\n" );
			ioe.printStackTrace();
		}
	}

	public static void createSubDataDirectories() {
		String dataDir = Configuration.getDataDirectory();

		new File(dataDir).mkdirs();
		new File(dataDir + "/teams").mkdirs();
		new File(dataDir + "/tokens").mkdirs();
		new File(dataDir + "/students").mkdirs();
	}
}