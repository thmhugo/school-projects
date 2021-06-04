package src.client;

/**
 * Manages the CLI.
 */
public class Printer {
	private static final String RESET_FG = "\u001B[0m";
	private static final String RED_FG = "\u001B[31m";
	private static final String GREEN_FG = "\u001B[32m";
	private static final String YELLOW_FG = "\u001B[33m";
	private static final String BLUE_FG = "\u001B[34m";
	private static final String PURPLE_FG = "\u001B[35m";

	/** The token used to complete strings with hashes. */
	private static final char FILLING_TOKEN = '#';
	/** The prompt symbol. */
	private static final String PROMPT = "$ ";

	private boolean isColorized;

	/**
	 * Builds a printer.
	 *
	 * @param isColorized is the second execution argument.
	 */
	public Printer(Boolean isColorized){
		this.isColorized = isColorized;
	};

	public boolean getColorization() {
		return this.isColorized;
	}

	public void printSuccess(String str) {
		coloredPrint(GREEN_FG,"[+] " + str + "\n");
	}

	public void printError(String str) {
		coloredPrint(RED_FG,"[!] " + str + "\n");
	}

	public void printInfo(String str) {
		coloredPrint(GREEN_FG, str + "\n");
	}

	public void printWarning(String str) {
		coloredPrint(YELLOW_FG, "[~] " + str + "\n");
	}

	public void printQuestion(String str) {
		coloredPrint(PURPLE_FG, "[?] " + str);
	}

	public void printPrompt() {
		coloredPrint(BLUE_FG, PROMPT);
	}

	/**
	 * Prints a string with colors or not depending on the execution args.
	 *
	 * @param color is the sring representation of the terminal color.
	 * @param str is the string to print.
	 */
	private void coloredPrint(String color, String str) {
		if (isColorized) {
			System.out.print(color + str + RESET_FG);
		}
		else {
			System.out.print(str);
		}
	}

	/**
	 * Prints an ITEM message and add multicasters coordinates into multicasters.
	 *
	 * @param i is the index of the packets in the reception flow.
	 * @param message is the message to print.
	 */
	public void printItemMessage(final int i, String message) {
		String[] splited_mess = message.split(Client.P_SEP);

		MulticasterAddress mAdress =
			new MulticasterAddress(
				splited_mess[2],
				Integer.valueOf(splited_mess[3]),
				splited_mess[4],
				Integer.valueOf(splited_mess[5])
			);
		String name = removeHash(splited_mess[1]);

		printMulticasterInfos(i, name, mAdress);

		Client.multicasters.put(name, mAdress);
	}

	private final void printMulticasterInfos(
		final int index,
		final String id,
		final MulticasterAddress address)
	{
		printInfo(
			"[" + index + "] " + id
		);
		printInfo(
			"  Cast @ " + formatAddress(address.getCastAddress(), address.getCastPort())
		);
		printInfo(
			"Listen @ " + formatAddress(address.getComAddress(), address.getComPort())
		);
	}

	/**
	 * Prints a message content (OLDM or MESS).
	 *
	 * @param message is the message to print.
	 */
	public void printMessage(String message) {
		String[] splited_mess = message.split(Client.P_SEP);

		System.out.print(
			"[" + Integer.valueOf(splited_mess[1]) + "] " +
			removeHash(splited_mess[2]) + " : "
		);
		System.out.println(
			removeHash(message.substring(Client.CONTENT_MESS_OFFSET))
		);
	}

	public void printWeatherLine(String message) {
		System.out.println(removeHash(message.substring(4)));
	}

	/**
	 * Prints the multicaster buffer for the command `mAdress`.
	 */
	public void printMulticasterBuffer() {
		int i = 0;
		for (String name : Client.multicasters.keySet()) {
			MulticasterAddress mAddress = Client.multicasters.get(name);
			printMulticasterInfos(i++, name, mAddress);
		}
	}

	/**
	 * Removes the FILLING_TOKEN of the given string.
	 *
	 * @param s is the string to remove FILLING_TOKEN.
	 *
	 * @return the given string without FILLING_TOKEN.
	 */
	public static String removeHash(String s) {
		int i = s.length() - 1;

		while (i >= 0 && FILLING_TOKEN == s.charAt(i)) {
			--i;
		}
		return s.substring(0, i + 1);
	}

	/**
	 * Prints the help.
	 */
	public void printHelp() {
		System.out.println(
			"Available commands:\n" +
			"\n  listen ip1:p1 ... ipn:pn    Launches countinous listening on ipi:pi.\n" +
			"\n  listen @i                   Shortcut for listen (see list & mltc).\n" +
			"\n  mess                        Sends a message to register to a multicaster.\n" +
			"\n  last                        Asks a multicaster its lasts messages.\n" +
			"\n  list                        Requests to a manager the list of its registered multicasters." +
			"\n                              Stores the received data in a buffer (see mltc).\n" +
			"\n  mAdress                     Displays the multicasters buffer." +
			"\n                              Use listen @i to listen to the [i] multicaster.\n" +
			"\n  weather                     Requests to a multicaster the current weather of an location."
		);
	}

	/**
	 * Concatenates the given address, ':' and the given port.
	 *
	 * @param address is address to be concatenated.
	 * @param port is the port to be concatenated.
	 */
	public String formatAddress(String address, Integer port) {
		return address + Client.ARGS_IP_PORT_SEP + port;
	}
}
