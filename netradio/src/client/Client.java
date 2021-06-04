package src.client;

import java.io.*;
import java.net.*;
import java.util.*;

import sun.misc.Signal;
import sun.misc.SignalHandler;

public class Client {

	/** SIGING signal in sun.misc. */
	private static final String SIGINT = "INT";

	/** Maximum amount of arguments. */
	private static final int MAX_ARGS = 3;
	/** Flag to print without color. */
	private static final String NO_COLOR_FLAG = "--no-color";

	/** The number of bytes of NbMess. */
	private static final int NB_MESS_SIZE = 3;
	/** The bytes size of the message's type. */
	private static final int TYPE_SIZE = 4;
	/** The length of the name in a message. */
	private static final int NAME_SIZE = 8;
	/** The bytes size of NumMess. */
	private static final int NUM_MESS_SIZE = 4;
	/** The bytes size of the message content. */
	public static final int CONTENT_MESS_SIZE = 140;
	/** The offset before the content of a message starts. */
	public static final int
		CONTENT_MESS_OFFSET = TYPE_SIZE + NUM_MESS_SIZE + NAME_SIZE + 3;
	/** The maximum of messages a multicaster can store. */
	private static final int MAX_MESS_REQ = 999;
	/** The number of elements in an OLDM and DIFF message. */
	private static final int OLDM_DIFF_NB_ELEM = 4;
	/** The number of elements in an ITEM message. */
	private static final int ITEM_NB_ELEM = 6;
	/** Length of the location in a RWET message. */
	private static final int LOCATION_SIZE = 50;
	/** Number of elements expected in a valid WETC message. */
	private static final int WETC_NB_ELEM = 2;

	/** The token used to complete strings with hashes. */
	private static final String FILLING_TOKEN = "#";
	/** The token used to complete a number. */
	private static final String TO_STR_TOKEN = "0";
	/** The delimiter of each part of a message. */
	public static final String P_SEP = " ";
	/** The termination sequence of a message. */
	private static final String P_TERM = "\r\n";

	/** The shortcut of the listen command. */
	private static final char LISTEN_SHORTCUT_CHAR = '@';
	/** The delimiter between an ip and a port in the normal listen command
	 * (and used to display coordinates). */
	public static final String ARGS_IP_PORT_SEP = ":";

	public static HashMap<String, MulticasterAddress> multicasters = new HashMap<>();

	/** Used to reset the signal handler when the reception is cancelled. */
	private static SignalHandler resetSignal = (Signal sig) -> System.exit(0);

	/** Prints in color in the terminal. */
	private static Printer printer;

	/** The username given on execution. */
	private static String clientName;

	public static void main(String[] args) {
		if (!manageArgs(args)) {
			System.exit(0);
		}
		runREPL();
	}

	/**
	 * 	Sets the client's username and initializes the printer.
	 *
	 * @param args is the execution args.
	 *
	 * @return true if the client's name was correctly set and the amount of
	 * arguments is less than 2, otherwise false.
	 */
	private static boolean manageArgs(String[] args) {
		boolean colored = true;
		String name = "";

		if (0 == args.length) {
			System.out.println("You must specify your name.");
			return false;
		}

		if (MAX_ARGS < args.length) {
			System.out.println(
				"Too much arguments, expecting less than 2, got (" + args.length + ")."
			);
			return false;
		}

		for (int i = 0; i < args.length; i++) {
			if (args[i].equals(NO_COLOR_FLAG)) {
				colored = false;
			}
			else {
				name = args[i];
			}
		}

		printer = new Printer(colored);

		return setClientName(name);
	}

	/**
	 * Manages the client's name given as paramater on execution.
	 *
	 * @param arg is the client's name.
	 *
	 * @return true if the argument format is only a string and
	 * 			smaller that NAME_SIZE else false.
	 */
	private static boolean setClientName(String arg) {
		String name = "";

		if (arg.length() == 0) {
			System.out.println("You must specify your name !");
			return false;
		}

		if (NAME_SIZE < arg.length()) {
			name = arg.substring(0, NAME_SIZE);
		}
		else {
			name = arg;
		}

		System.out.println("Hello " + name + " !");
		clientName = completeWithHashes(name, NAME_SIZE);

		return true;
	}

	/**
	 * Launches the console to manage the user interactions.
	 */
	private static void runREPL() {
		Scanner commandInput = new Scanner(System.in);

		while (true) {
			printer.printPrompt();
			String input = commandInput.nextLine().trim();
			Command command;

			try {
				command = Command.valueOf(input.split(P_SEP)[0].toUpperCase());
			}
			catch (IllegalArgumentException iae) {
				printer.printWarning("Unknown command.");
				command = Command.NONE;
			}

			switch (command) {
				case LISTEN: {
					try {
						List<MulticasterAddress> msoList =
							makeMSOListfromUserInput(input);
						startListening(msoList);
					}
					catch (IllegalArgumentException iae) {
						printer.printError("An address seems to be misspelled.");
					}
					catch (IndexOutOfBoundsException iobe) {
						printer.printError("Incorrect index.");
					}

					break;
				}
				case MESS: {
					process(getMessCommunication());
					break;
				}
				case LAST: {
					process(getLastCommunication());
					break;
				}
				case LIST: {
					process(getListCommunication());
					break;
				}
				case WEATHER: {
					process(getWeatherCommunication());
					break;
				}
				case MLTC: {
					printer.printMulticasterBuffer();
					break;
				}
				case HELP: {
					printer.printHelp();
					break;
				}
				case EXIT: {
					commandInput.close();
					return;
				}
				default: {
					break;
				}
			}
		}
	}

	/**
	 * Listens messages sent by multicasters.
	 *
	 * @param msoList is the list of multicasters to listen to
	 * where a multicaster is represented by an Entry<String, Integer>
	 * where the String is the ip address and the Integer is the port
	 * of the multicaster.
	 */
	private static void startListening(List<MulticasterAddress> msoList) {
		try {
			List<Receiver> receivers = new ArrayList<>();

			for (MulticasterAddress addr : msoList) {
				try {
					if (null != addr.getComAddress() &&
						null != addr.getComPort() &&
						!processRUOK(addr.getComAddress(), addr.getComPort()))
					{
						/** If the RUOK process went wrong, the multicaster is
						 * removed from the list. */
						multicasters.entrySet().removeIf(entryAddr -> {
							if (addr.equals(entryAddr.getValue())) {
								String id = entryAddr.getKey();

								printer.printWarning(
									"The specified multicaster " + id +
									" is no longer alive: can't listen to " +
									printer.formatAddress(addr.getCastAddress(), addr.getCastPort())
								);
								printer.printWarning("Multicaster " + id + " removed from the list.");

								return true;
							}
							return false;
						});
					}
					else {
						MulticastSocket mso = new MulticastSocket(addr.getCastPort());
						mso.joinGroup(InetAddress.getByName(addr.getCastAddress()));

						Receiver r = new Receiver(mso, printer.getColorization());
						receivers.add(r);
						r.start();

						/** Initialize SignalHandler while thread is starting. */
						SignalHandler interruptionSignal = (Signal sig) -> {
							for (Receiver rec : receivers) {
								rec.cancel();
							}
							/** Reset the [ctrl-c] handling. */
							Signal.handle(new Signal(SIGINT), resetSignal);
						};

						Signal.handle(new Signal(SIGINT), interruptionSignal);
					}
				}
				catch (IOException ioe) {
					printer.printError(
						"Unable to join " +
						printer.formatAddress(addr.getCastAddress(), addr.getCastPort()) +
						"."
					);
				}
			}
			for (Receiver rec : receivers) {
				rec.join();
			}
		}
		catch (InterruptedException ie) {
			printer.printError("Serious failure, thread interrupted.");
		}
	}

	/**
	 * Processes a communication by calling communication.communicate.
	 *
	 * @param communication is the communication to execute.
	 */
	private static void process(ClientCommunication communication) {
		Optional<Socket> recipientSocketOpt = makeRecipientSocket();

		if (recipientSocketOpt.isPresent()) {
			Socket recipientSocket = recipientSocketOpt.get();

			try {
				PrintWriter pw =
					new PrintWriter(
						new OutputStreamWriter(recipientSocket.getOutputStream())
					);
				BufferedReader br =
					new BufferedReader(
						new InputStreamReader(recipientSocket.getInputStream())
					);

				communication.communicate(pw, br, recipientSocket);

				br.close();
				pw.close();
				recipientSocket.close();
			}
			catch (IOException e) {
				printer.printError("Serious failure on socket initialization.");
			}
			catch (NullPointerException npe) {
				printer.printError("An error occured with the server connection.");
			}
		}
		else {
			printer.printWarning("Operation cancelled.");
		}
	}

	/**
	 * Processes a RUOK communication with the multicaster at the given address.
	 *
	 * @param ip is the multicaster's ip.
	 * @param port is the multicaster's port.
	 *
	 * @return true if the communication went well, false otherwise.
	 */
	private static boolean processRUOK(String ip, int port) {
		try {
			Socket multicasterSocket = new Socket(ip, port);
			PrintWriter pw =
				new PrintWriter(
					new OutputStreamWriter(multicasterSocket.getOutputStream())
				);
			BufferedReader br =
				new BufferedReader(
					new InputStreamReader(multicasterSocket.getInputStream())
				);
			String message = makeRUOKMessage();

			reliableSend(message, pw);
			String receivedMessage = br.readLine();

			if (!isIMOKMessage(receivedMessage)) {
				return false;
			}

			br.close();
			pw.close();
			multicasterSocket.close();
		}
		catch (Exception e) {
			return false;
		}
		return true;
	}

	/**
	 * Returns an implementation of ClientCommunication that
	 * registers a new message to cast at a specified multicaster's
	 * address.
	 *
	 * @return the ClientCommunication implementation.
	 */
	private static ClientCommunication getMessCommunication() {
		return (pw, br, recipientSocket) -> {
			String message = makeMESSMessage();
			reliableSend(message, pw);
			String receivedMessage = br.readLine();

			if (isACKMMessage(receivedMessage)) {
				printer.printSuccess(
					"The server received the message correctly !"
				);
			}
			else {
				printer.printWarning(
					"The server did not receive the message, please try again."
				);
			}
		};
	}

	/**
	 * Returns an implementation of ClientCommunication that
	 * requests a multicaster the n lasts messages stored.
	 *
	 * @return the ClientCommunication implementation.
	 */
	private static ClientCommunication getLastCommunication() {
		return (pw, br, recipientSocket) -> {
			String message = makeLASTMessage();
			String receivedMessage = "";

			reliableSend(message, pw);

			/** It's in charge of the multicaster to send the right number of messages. */
			while (!isENDMMessage(receivedMessage = br.readLine())) {
				if (!messageHasRightFormat(receivedMessage)) {
					/**
					 *  Gives back control to the user if a received message has not the
					 *  right format.
					 */
					printer.printError("Unexpected message: " + receivedMessage);
					break;
				}
				else {
					printer.printMessage(receivedMessage);
				}
			}
		};
	}

	/**
	 * Returns an implementation of ClientCommunication that
	 * asks a manager the list of multicasters stored.
	 *
	 * @return the ClientCommunication implementation.
	 */
	private static ClientCommunication getListCommunication() {
		return (pw, br, recipientSocket) -> {
			String message = makeLISTMessage();

			reliableSend(message, pw);

			int count = getNumberOfMulticasters(br.readLine());
			for (int i = 0; i < count; i++) {
				String receivedMessage = br.readLine();

				if(!ITEMMessHasRightFormat(receivedMessage)) {
					printer.printError(
						"Unexpected message (expected ITEM): " +
						receivedMessage
					);
					break;
				}
				else {
					printer.printItemMessage(i, receivedMessage);
				}

			}
		};
	}

	/**
	 * Returns an implementation of ClientCommunication that
	 * asks to a multicaster the weather at the given location.
	 *
	 * @return the ClientCommunication implementation.
	 */
	private static ClientCommunication getWeatherCommunication() {
		return (pw, br, recipientSocket) -> {
			String message = makeRWETMessage();
			reliableSend(message, pw);

			String receivedMessage = "";
			while (!isPEOFMessage(receivedMessage = br.readLine())) {
				if (isWERRMessage(receivedMessage)) {
					printer.printError("The server failed to get the weather.");
					break;
				}
				else if (!WETCMessHasRightFormat(receivedMessage)) {
					printer.printError("Unexpected message: '" + receivedMessage);
					break;
				}
				else {
					printer.printWeatherLine(receivedMessage);
				}
			}
		};
	}

	/**
	 * Checks the format of an ITEM message.
	 *
	 * @param message is the message to check the format.
	 *
	 * @return true if the message has the expected format else false.
	 */
	private static boolean ITEMMessHasRightFormat(String message) {
		String[] splitedMess = message.split(P_SEP);

		return isRightStringValue(splitedMess[0], Type.ITEM) &&
			ITEM_NB_ELEM == splitedMess.length ;
	}

	/**
	 * Checks the format of an ACKM message.
	 *
	 * @param message is the message to check the format.
	 *
	 * @return true if the message has the expected format else false.
	 */
	private static boolean isACKMMessage(String message) {
		return isRightStringValue(message, Type.ACKM);
	}

	/**
	 * Checks the format of an ENDM message.
	 *
	 * @param message is the message to check the format.
	 *
	 * @return true if the message has the expected format else false.
	 */
	private static boolean isENDMMessage(String message) {
		return isRightStringValue(message, Type.ENDM);
	}

	/**
	 * Checks the format of an PEOF message.
	 *
	 * @param message is the message to check the format.
	 *
	 * @return true if the message has the expected format else false.
	 */
	private static boolean isPEOFMessage(String message) {
		return isRightStringValue(message, Type.PEOF);
	}

	/**
	 * Checks the format of an WERR message.
	 *
	 * @param message is the message to check the format.
	 *
	 * @return true if the message has the expected format else false.
	 */
	private static boolean isWERRMessage(String message) {
		return isRightStringValue(message, Type.WERR);
	}

	/**
	 * Checks the format of an IMOK message.
	 *
	 * @param message is the message to check the format.
	 *
	 * @return true if the message has the expected format else false.
	 */
	private static boolean isIMOKMessage(String message) {
		return isRightStringValue(message, Type.IMOK);
	}

	/**
	 * Same as Type.valueOf(s).equals(t) and catch
	 *  the eventual exception of Type.valueOf.
	 *
	 * @param s is the exected string value of t.
	 * @param t is the expetec Type.
	 *
	 * @return true if the string value of t is s.
	 */
	private static boolean isRightStringValue(String s, Type t) {
		try {
			return Type.valueOf(s).equals(t);
		}
		catch (IllegalArgumentException iae) {
			return false;
		}
	}

	/**
	 * Checks if a message with the type OLDM or MESS has the right format.
	 *
	 * @param message is the message to check the format.
	 *
	 * @return true if the message has the expected format else false.
	 */
	public static boolean messageHasRightFormat(String message) {
		String[] splitedMess = message.split(P_SEP);

		try {
			return (
				(Type.valueOf(splitedMess[0]).equals(Type.OLDM) ||
				Type.valueOf(splitedMess[0]).equals(Type.DIFF)) &&
				OLDM_DIFF_NB_ELEM <= splitedMess.length
			);
		}
		catch (IllegalArgumentException iae) {
			return false;
		}
	}

	/**
	 * Checks if a WETC message has the expected format.
	 *
	 * @param message is the message to check the format.
	 *
	 * @return true if the message has the expected format else false.
	 */
	private static boolean WETCMessHasRightFormat(String message) {
		String[] splitedMess = message.split(P_SEP);

		try {
			return (
				WETC_NB_ELEM <= splitedMess.length &&
				Type.valueOf(splitedMess[0]).equals(Type.WETC)
			);
		}
		catch (IllegalArgumentException e) {
			return false;
		}
	}

	/**
	 * Manages the arguments of the command *listen*.
	 *
	 * @param input is the argument of the *listen* command.
	 *
	 * @return a List of Entry that contains multicaster informations formed by the arguments.
	 *
	 * @throws IllegalArgumentException if an argument has not the right format.
	 * @throws IndexOutOfBoundsException if the requested index in multicasters is out of bounds.
	 */
	private static List<MulticasterAddress>
		makeMSOListfromUserInput(String input)
		throws IllegalArgumentException, IndexOutOfBoundsException
	{
		List<MulticasterAddress> addresses = new ArrayList<>();
		String[] splitedInput = input.split(P_SEP);

		/** input[0] is the command so we begin at input[1]. */
		for (int i = 1; i < splitedInput.length; i++) {
			String destination = splitedInput[i];
			/** Get a multicaster using its index. */
			if (LISTEN_SHORTCUT_CHAR == destination.charAt(0)) {
				/** Get the value at an index in a HashMap. */
				int bufferIndex = Integer.valueOf(splitedInput[i].substring(1));
				String name = (String)multicasters.keySet().toArray()[bufferIndex];

				addresses.add(multicasters.get(name));
			}
			else if (null != multicasters.get(destination)) {
				addresses.add(multicasters.get(destination));
			}
			else {
				String[] splitedDestination = destination.split(ARGS_IP_PORT_SEP);

				if (2 != splitedDestination.length) {
					throw new IllegalArgumentException();
				}
				addresses.add(
					new MulticasterAddress(
						splitedDestination[0],
						Integer.valueOf(splitedDestination[1])
					)
				);
			}
		}
		return addresses;
	}

	/**
	 * Asks informations to the user to make a MESS message.
	 *
	 * @return the MESS message generated following user given details.
	 */
	private static String makeMESSMessage() {
		Scanner sc = new Scanner(System.in);
		String userMessage;

		printer.printQuestion("Your message: ");
		userMessage = completeWithHashes(sc.nextLine().trim(), CONTENT_MESS_SIZE);

		return Type.MESS.name() + P_SEP + clientName + P_SEP + userMessage + P_TERM;
	}

	/**
	 * Asks informations to the user to make a LAST message.
	 *
	 * @return the LAST message generated following user given details.
	 */
	private static String makeLASTMessage() {
		Scanner sc = new Scanner(System.in);
		String nbMess;

		try {
			printer.printQuestion("How many messages: ");
			int n = sc.nextInt();
			if (MAX_MESS_REQ < n  || 0 > n) {
				throw new NumberFormatException();
			}
			nbMess = nToString(n, NB_MESS_SIZE);

			return Type.LAST.name() + P_SEP + nbMess + P_TERM;
		}
		catch (Exception e) {
			printer.printWarning("Invalid number format !");

			return makeLASTMessage();
		}
	}

	/**
	 * Generates a LIST message.
	 *
	 * @return the LIST message generated.
	 */
	private static String makeLISTMessage() {
		return Type.LIST.name() + P_TERM;
	}

	/**
	 * Generates a RUOK message.
	 *
	 * @return the RUOK message generated.
	 */
	private static String makeRUOKMessage() {
		return Type.RUOK.name() + P_TERM;
	}

	/**
	 * Generates a RWET message.
	 *
	 * @return the RWET message generated following user given details.
	 */
	private static String makeRWETMessage() {
		Scanner sc = new Scanner(System.in);
		String location;

		printer.printQuestion("Location: ");
		location = completeWithHashes(sc.nextLine().trim(), LOCATION_SIZE);

		return Type.RWET.name() + P_SEP + location + P_TERM;
	}
	/**
	 * Gets the number of multicasters from a LINB message.
	 *
	 * @param linb is the LINB message.
	 *
	 * @return the number of messages (Ndf) of a LINB message.
	 */
	private static int getNumberOfMulticasters(String linb) {
		try {
			/**
			 * Following the protocol, a LINB message as the following format :
			 *		LINB __
			 * We can get the integer value of __ by this way.
			 */
			return Integer.valueOf(linb.split(P_SEP)[1]);
		}
		catch (Exception nfe) {
			printer.printError(
				"The received message hasn't the expected format. Please try again."
			);
			return 0;
		}
	}

	/**
	 * Converts an int into a String following the protocol.
	 *
	 * @param n is the int to convert.
	 * @param expectedSize is the expected size of the final string.
	 *
	 * @return n as a String preceded by TO_STR_TOKEN to reach expectedSize.
	 */
	private static String nToString(int n, int expectedSize) {
		String nStr = String.valueOf(n);

		while (expectedSize != nStr.length()) {
			nStr = TO_STR_TOKEN + nStr;
		}

		return nStr;
	}

	/**
	 * Completes the string given in argument with the FILLING_TOKEN.
	 *
	 * @param s is the string to complete.
	 * @param expectedSize is the expected final length of the string.
	 *
	 * @return s filled with FILLING_TOKEN.
	 */
	private static String completeWithHashes(String s, int expectedSize) {
		StringBuilder builder = new StringBuilder(s);

		for (int i = 0; i < expectedSize - s.length(); i++) {
			builder.append(FILLING_TOKEN);
		}

		return builder.toString();
	}

	/**
	 * Asks informations to the user in order to create a recipient socket.
	 *
	 * @return an Optional that contains the socket if all is right
	 * 			else an empty Optional.
	 */
	private static Optional<Socket> makeRecipientSocket() {
		Scanner sc = new Scanner(System.in);

		try {
			printer.printQuestion("Server IP address (or name): ");

			String ip = sc.nextLine().trim();
			printer.printQuestion("Server port: ");

			int port = sc.nextInt();
			Socket s = new Socket(ip, port);

			return Optional.of(s);
		}
		catch (ConnectException ce) {
			printer.printError("Connection refused...");

			return Optional.empty();
		}
		catch (UnknownHostException uhe) {
			printer.printWarning("Unknown host... Please, try again.");

			return makeRecipientSocket();
		}
		catch (IllegalArgumentException |   /** Port out of range. */
				InputMismatchException |  	/** Wrong input format. */
				IOException ioe				/** Socket instanciation failure. */
		) {
			printer.printWarning("Something went wrong...");

			return makeRecipientSocket();
		}
	}

	/**
	 * Used to send a message and be sure that the printwriter is flushed.
	 *
	 * @param message is the message to send.
	 * @param pw is the PrintWriter that prints the message.
	 */
	private static void reliableSend(String message, PrintWriter pw) {
		pw.print(message);
		pw.flush();
	}

	/**
	 * Removes useless zeros of a String representation of an ip address.
	 *
	 * @param addr is the string address to lighten.
	 *
	 * @return addr without the useless zeros.
	 */
	public static String lightenAddress(String addr) {
		StringBuilder sb = new StringBuilder();
		String[] splitedAddr = addr.split("\\.");

		for (int i = 0; i < splitedAddr.length; i++) {
			sb.append(Integer.valueOf(splitedAddr[i]));
			if (i < splitedAddr.length - 1) {
				sb.append(".");
			}
		}
		return sb.toString();
	}
}
