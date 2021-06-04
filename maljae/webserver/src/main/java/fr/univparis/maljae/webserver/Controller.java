package fr.univparis.maljae.webserver;

import java.io.IOException;
import java.util.Scanner;

import fr.univparis.maljae.datamodel.*;
import io.javalin.Javalin;
import io.javalin.http.Context;
import io.javalin.plugin.rendering.JavalinRenderer;
import io.javalin.plugin.rendering.template.JavalinFreemarker;
import io.javalin.plugin.rendering.template.TemplateUtil;

/**
 * Controller.
 */
public class Controller {

	/**
	 * Installs Javalin's handlers.
	 * @param app is the Javalin's app.
	 */
	public static void install(Javalin app) {
		JavalinRenderer.register(JavalinFreemarker.INSTANCE, ".ftl");
		installTeamCreate( app );
		installTeamEdit( app );
		installTeamUpdate( app );
	}

	/**
	 * Installs an endpoint-handler for team creation :
	 * 	1. Get by the post method the seized email.
	 * 	2. Creates a new team and a new token.
	 * 	3. Notifies by email the student.
	 * 	4. Redirects to the team-creation-done html page.
	 * @param app is the Javalin's app.
	 */
	public static void installTeamCreate(Javalin app) {
		app.post( "/team/create", ctx -> {
			String email = ctx.formParam( "email" );
			String groupId = ctx.formParam( "groupId" );

			Students.updateAndSaveStudent( new Student(email, true, groupId) );

			Team newTeam = Teams.createTeam( Students.getStudent(email) );
			Token newToken = Token.createToken( newTeam, email );

			if ( Notifier.sendTeamCreation(ctx.host(), newToken) )
				ctx.redirect( "/html/team-creation-done.html" );
			else 
				ctx.redirect( "/html/team-creation-failed.html" );
		});
	}

	/**
	 * Installs an endpoint-handler for team editing.
	 * @param app is the Javalin's app.
	 */
	public static void installTeamEdit(Javalin app) {
		app.get("/team/edit/:token", ctx -> {
			Token token = Token.getToken( ctx.pathParam("token") );
			String teamName = token.getTeam().getIdentifier();
			Team team = Teams.getTeam( teamName );

			ctx.render(
				"/public/freemarker/edit-team.ftl",
				TemplateUtil.model (
					"teamName", teamName,
					"secret", team.getSecret(),
					"students", formatStudentString( team ),
					"preferences", formatTeamPreferencesString( team ),
					"token", token.toString()
				)
			);
		});
	}
	
	/**
	 * Formats the output of Team.preferencesToString() : 
	 *    "task1;task2;...;"
	 * 	  -> <ul id="preferences-list">
	 * 		   <li class="task">task1</li>
	 * 		   <li class="task">tasks2</li>
	 * 		   <li class="task">...</li>
	 * 		 </ul>
	 * @param team is the team which its students string representation
	 * will be formatted.
	 * @return the formatted string.
	 */
	private static String formatTeamPreferencesString(Team team) {
		Scanner scan = new Scanner( team.preferencesToString() );
		String formatted = "<ul id=\"preferences-list\">";

		scan.useDelimiter( ";" );
		while ( scan.hasNext() )
			formatted += "<li class=\"task\">" + scan.next() + "</li>";
		scan.close();
		formatted += "</ul>";

		return( formatted );
	}
	
	/**
	 * Formats the output of Team.studentsToString() :
	 *     "firstname.lastname@etu.univ-paris-diderot.fr/true;...;"
	 * 	   -> <p class="student">firstname lastname | Grp : groupId</p>
	 * 	  	  <p class="student">...</p>
	 * @param team is the team which its students string representation
	 * will be formatted.
	 * @return the formatted string.
	 */
	private static String formatStudentString(Team team) {
		Scanner scan = new Scanner( team.studentsToString() );
		String formatted = "";

		scan.useDelimiter( ";" );
		while( scan.hasNext() ) {
			Scanner adScan = new Scanner( scan.next() );

			adScan.useDelimiter( "@" );
			formatted += "<p class=\"student\">";
			formatted += adScan.next().replace( '.', ' ' );
			formatted += " [Grp : " + team.getGroupId();
			formatted += "]</p>";
			adScan.close();
		}
		scan.close();

		return( formatted );
	}
	
	/**
	 * Installs an endpoint-handler for team updating.
	 * @param app is the current Javalin's app.
	 */
	public static void installTeamUpdate(Javalin app) {
		installAddStudent( app );
		installUpdateSecret( app );
		installRemoveStudent( app );
		installUpdatePreferences( app );
		installSendConfirmLink( app );
		installDeleteTeam(app);
	}

	public static void installDeleteTeam(Javalin app) {
		app.post( "/team/edit/delete-team/:token", ctx -> {
			try {
				Team team = Controller.getTeamFromToken( ctx.pathParam("token") );
				Teams.deleteTeam(team, ctx.pathParam("token"));
				ctx.redirect("/index.html");
			} 
			catch (Exception e) {
				System.out.println(
					"Error in installDeleteTeam() in TeamController.java"
				);
			}
		});
	}

	public static void installSendConfirmLink(Javalin app) {
		app.post( "/team/send/confirmLink/:token", ctx -> {
			try {
				Token token = Token.getToken( ctx.pathParam("token") );
				Team team = Controller.getTeamFromToken( ctx.pathParam("token") );
				String studentEmail = ctx.formParam( "student" ); 
				String groupId = ctx.formParam( "groupId" );
				Student student  = new Student( studentEmail, false, groupId );

				if ( isCanBeAdded(student, team) ) {
					Students.updateAndSaveStudent( student );
					Notifier.sendConfirmationLink(
						token, team, ctx.host(), studentEmail
					);
					ctx.render(
						"/public/freemarker/email-sended.ftl",
						TemplateUtil.model("token", token)
					);
				}
				else
					ctx.render(
						"/public/freemarker/add-student-failed.ftl",
						TemplateUtil.model("token", token, "student", studentEmail)
					);
			} catch (IOException ioe) {
				ioe.printStackTrace();
				ctx.render(
					"/public/freemarker/email-failed.ftl" ,
					TemplateUtil.model("token", ctx.pathParam("token"))
				);
			}
		});		
	}

	/**
	 * A student can be added to a team only if :
	 *   - is not confirmed.
	 *   - he have the same group id of the team. 
	 *	 - the team isn't full.
	 */
	public static boolean isCanBeAdded(Student formStudent, Team team) {
		try {
			Student storedStudent = Students.getStudent( formStudent.getEmail() );

			if ( areFromTheSameGroup(formStudent, team)
				&& !storedStudent.getConfirmed()
				&& !team.isFull()
			 ) {
				/** Allows the student to change his/her groupId. */
				storedStudent.setGroupId( formStudent.getGroupId() );
				return( true );
			}
		} catch (RuntimeException re) {
			if ( formStudent.getGroupId().compareTo(team.getGroupId()) == 0 )
				return( true ); 
		}
		return( false );
	}

	private static boolean areFromTheSameGroup(Student formStudent, Team team) {
		/** groupId is equal to -1, only if, the team is empty. */
		return(
			team.getGroupId().compareTo("-1") == 0
			|| formStudent.getGroupId().compareTo(team.getGroupId()) == 0
		);
	}

	public static void installAddStudent(Javalin app) {
		app.get( "/team/add/:token/:studentEmail", ctx -> {
			try {
				String studentEmail = ctx.pathParam( "studentEmail" ); 
				Team team = Controller.getTeamFromToken( ctx.pathParam("token") );
				String who = Controller.getEmailFromToken( ctx.pathParam("token") );
				Student newStudent = Students.getStudent( studentEmail );

				if ( isCanBeAdded(newStudent, team) ) {
					addStudent( studentEmail, team, who, newStudent );
					ctx.redirect( "/team/edit/" + ctx.pathParam("token") );
				}
				else
					ctx.redirect( "/html/confirmation-link-expired.html" );
			} catch (IOException ioe) {
				System.out.println( ioe.getStackTrace() );
				ctx.redirect( "/html/team-update-failed.html" );
			}
		});
	}

	private static void addStudent(
		String studentEmail, Team team, String who, Student newStudent
	) throws Exception, IOException {
		updateTeam(studentEmail, team, who, newStudent);
		Students.updateAndSaveStudent( team.getStudent(studentEmail) );
		Teams.saveTeam( team );
		Notifier.sendStudentAdded( team, studentEmail );
	}

	private static void updateTeam(
		String studentEmail, Team team, String who, Student newStudent
	) throws Exception {
		String studentsStr = team.studentsToString()
							+ studentEmail + "/true/"+
							newStudent.getGroupId() + ";";

		team.updateStudentsFromString( who, studentsStr );
	}
	
	private static Team getTeamFromToken(String tokenId) throws IOException {
		Token token = Token.getToken( tokenId );
		String teamId = token.getTeam().getIdentifier();
		Team team = Teams.getTeam( teamId );

		return( team );
	}
	
	private static String getEmailFromToken(String tokenId) throws IOException {
		Token token = Token.getToken( tokenId );
		String email = token.getEmail();

		return( email );
	}

	public static void installRemoveStudent(Javalin app) {
		app.post( "/team/rm/:token", ctx -> {
			try {
				String studentEmail = ctx.formParam( "student" ); 
				Team team = Controller.getTeamFromToken( ctx.pathParam("token") );
				Student removedStudent = Students.getStudent( studentEmail );

				removeStudent( studentEmail, team, removedStudent );
				ctx.redirect( "/team/edit/" + ctx.pathParam("token") );
			} catch (IOException ioe) {
				System.out.println( ioe );
				ctx.redirect( "/html/team-update-failed.html" );
			}
		});
	}

	private static void removeStudent(
		String studentEmail, Team team, Student removedStudent
	) throws IOException {
		team.removeStudent( studentEmail );
		removedStudent.setConfirmed( false );
		Students.updateAndSaveStudent( removedStudent );	
		Teams.saveTeam( team );
		Notifier.sendStudentRemoved( team, studentEmail );
	}

	public static void installUpdateSecret(Javalin app) {
		app.post( "team/update/secret/:token", ctx -> {
			try {
				Team team = Controller.getTeamFromToken( ctx.pathParam("token") );
				
				updateSecret( ctx, team );
				ctx.redirect( "/team/edit/" + ctx.pathParam("token") );
			} catch (IOException ioe) {
				System.out.println( ioe );
				ctx.redirect( "/html/team-update-failed.html" );
			}
		});
	}

	private static void updateSecret(Context ctx, Team team) throws IOException {
		team.updateSecretFromString( ctx.formParam("secret") );
		Teams.saveTeam( team );
		Notifier.sendSecretUpdated( team, ctx.formParam("secret") );
	}
	
	public static void installUpdatePreferences(Javalin app) {
		app.post( "/team/update/preferences/:token", ctx -> {
			try {
				Team team = Controller.getTeamFromToken( ctx.pathParam("token") );

				updatePreferences( ctx, team );
				ctx.redirect( "/team/edit/" + ctx.pathParam("token") );
			}
			catch (IOException ioe) {
				System.out.println( ioe );
				ctx.redirect( "/html/team-update-failed.html" );
			}
		});
	}

	private static void updatePreferences(Context ctx, Team team) throws IOException {
		team.updatePreferencesFromString( ctx.formParam("preferences") );
		Teams.saveTeam( team );
		Notifier.sendPreferencesUpdated( team, ctx.formParam("preferences") );
	}
}