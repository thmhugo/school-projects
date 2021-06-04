package fr.univparis.maljae.webserver;

import fr.univparis.maljae.datamodel.Student;
import fr.univparis.maljae.datamodel.Team;
import java.io.*;

import javax.mail.internet.*;
import javax.mail.*;
import java.util.*; 
import org.json.*;
import org.apache.commons.io.FileUtils;

/**
 * Manages notifications by email.
 */
public class Notifier {

	/**
	 * Sends an email to one person.
	 * @param email is the receiver address.
	 * @param subject is the subject of the email.
	 * @param message is the content of the email.
	 * @throws IOException
	 * @return true if the mail could be sent
     */
	public static boolean sendEmail(String email, String subject, String text)
	throws IOException {
		try {
			/* Initializes connection variables from json */
			File f = new File( "config.json" );
			JSONObject json = new JSONObject( FileUtils.readFileToString(f, "utf-8") );
			String transportProtocol = json.getString("transportProtocol");
			String host = json.getString("host");
			String username = json.getString("username");
			String password = json.getString("password"); 
			int port = json.getInt("port");
			
			Session session = createSession();
			MimeMessage message = createMessage(email, subject, text, session);
		
			/* 3 -> Send mail */
			Transport transport = session.getTransport(transportProtocol);
			transport.connect(host, port, username, password);	        
			transport.sendMessage(message, new Address[] { new InternetAddress(email)});
		    transport.close();
		} catch(Exception e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}

	private static MimeMessage createMessage(
		String email, String subject, String text, Session session
	) throws MessagingException {
		MimeMessage message = new MimeMessage(session);

		message.setText(text);
		message.setSubject(subject);
		message.addRecipients(Message.RecipientType.TO, email);

		return( message );
	}

	private static Session createSession() {
		Properties properties = new Properties();
		properties.setProperty("mail.smtp.starttls.enable", "true");
		return( Session.getInstance(properties) );
	}

	protected static boolean sendTeamCreation(String host, Token token) throws IOException {
		String message =
				"Hello!\n" +
						"You have requested the creation of a team.\n" +
						"Here are the links to perform actions on this team.\n" +
						"- To edit your team information :\n" +
						"  http://" + host + "/team/edit/" + token.toString () + "\n" +
						"- To delete your team :\n" +
						"  http://" + host + "/team/delete/" + token.toString () + "\n";
		return sendEmail(token.getEmail (), "[maljae] Team Creation", message);
	}

	protected static void sendConfirmationLink(
		Token token, Team team, String host, String studentEmail
	) throws IOException {
		String message = "Hello, the team (" + team.getIdentifier()
						 + ") sent you a link to join them : "
						 + "http://" + host + "/team/add/"
						 + token.toString() + "/" + studentEmail + "\n";
		Notifier.sendEmail( studentEmail, "[maljae] Confirmation link", message );
	}

	protected static void sendStudentAdded(Team team, String newStudentEmail) {
		String message = "Hello, a new student has been added to your team :\n"
						+ "\t" + newStudentEmail + "\n";
		
		Notifier.sendToAllTeamStudents( team, "New student added", message );		 
	}
	
	private static void sendToAllTeamStudents(
		Team team, String subject, String message
	) {
		try {
			for (Student student : team.getStudents())
				Notifier.sendEmail(
					student.getEmail(), "[maljae] " + subject, message
				);
		} catch (IOException ioe) {
			System.out.println( ioe );
		}
	}

	protected static void sendStudentRemoved(Team team, String removedStudentEmail) {
		String message = "Hello, a student has been removed of your team :\n"
						+ "\t" + removedStudentEmail + "\n";
		
		Notifier.sendToAllTeamStudents( team, "Student removed", message );

		message = "Hello, you have been removed of the team : " + team.getIdentifier(); 
		try {
			Notifier.sendEmail(
				removedStudentEmail, "You have been removed", message
			);
		} catch (IOException ioe) {
			System.out.println( ioe );
		}
	}

	protected static void sendSecretUpdated(Team team, String newSecret) {
		String message = "Hello, your team's secret has been changed to :\n"
						+ "\t" + newSecret + "\n";
		
		Notifier.sendToAllTeamStudents( team, "Secret updated", message );
	}

	protected static void sendPreferencesUpdated(Team team, String newPreferences) {
		String message = "Hello, your team's preferences has been changed to :\n"
						+ "\t" + newPreferences + "\n";
		
		Notifier.sendToAllTeamStudents( team, "Preferences updated", message );
	}
}