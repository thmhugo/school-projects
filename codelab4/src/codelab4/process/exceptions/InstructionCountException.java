package codelab4.process.exceptions;

public class InstructionCountException extends Exception {

	private static final long serialVersionUID = 1L;

	private final String message;

	public InstructionCountException(final int count, final int expected) {
        this.message = "Attention au nombre d'instructions !\n"
						+ "Tu en utilises " + (count - expected) + " de trop !";
	}

	public String getMessage() {
		return this.message;
	}
}
