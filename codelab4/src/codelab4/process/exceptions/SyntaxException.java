package codelab4.process.exceptions;

public class SyntaxException extends Exception {

	private static final long serialVersionUID = 1L;

	private final String message;

	public SyntaxException(final int line, final String str) {
		this.message = "Il y a une erreur a la ligne " + line + " :\n@ \"" + str + "\"";
	}

	public String getMessage() {
		return this.message;
	}
}
