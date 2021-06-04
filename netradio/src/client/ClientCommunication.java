package src.client;

import java.io.IOException;
import java.io.PrintWriter;
import java.io.BufferedReader;
import java.net.Socket;

public interface ClientCommunication {
	/**
	 * Describes a communication with another entity.
	 *
	 * @param pw is the PrintWriter initialized from the recipientSocket.
	 * @param br is the BufferedReader initialized from the recipientSocket.
	 * @param recipientSocket is the socket of the other entity to communicate with.
	 *
	 * @throws IOException if the socket initialization fails.
	 * @throws NullPointerException if the other entity unexpectedly closes the connection.
	 *
	 * @note these exceptions are voluntarily thrown to be managed in Client.process.
	 */
	void
	communicate(PrintWriter pw, BufferedReader br, Socket recipientSocket)
	throws IOException, NullPointerException;
}
