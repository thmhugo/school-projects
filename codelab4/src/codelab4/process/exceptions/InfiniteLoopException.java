package codelab4.process.exceptions;

public class InfiniteLoopException extends Exception {

	private static final long serialVersionUID = 1L;

	private final String message;

	public InfiniteLoopException() {
		this.message = "Il semblerait que tu ais une boucle\ninfinie.";
	}

	public String getMessage() {
		return this.message;
	}
}
