package codelab4.process;

public enum Token {

	IF				( "loop" ),
	WHILE			( "loop" ),

	ELSE			( "mid_loop" ),

	ENDIF			( "end_loop" ),
	ENDWHILE		( "end_loop" ),

	EST_LIBRE		( "condition" ),
	EST_POURRI		( "condition" ),

	TOURNER_DROITE	( "action" ),
	TOURNER_GAUCHE	( "action" ),
	AVANCER			( "action" ),
	RAMASSER		( "action" ),
	MANGER			( "action" ),
	JETER_FRUIT		( "action" ),
	COUPER			( "action" );

	final String type;

	private Token(String type) {
		this.type = type;
	}

	public static Token fromString(final String str) {
		switch (str) {
			case "if":					return Token.IF;
			case "while":				return Token.WHILE;
			case "else":				return Token.ELSE;
			case "fruit_est_pourri()":	return Token.EST_POURRI;
			case "chemin_est_libre()":	return Token.EST_LIBRE;
			case "tourner_a_droite()":	return Token.TOURNER_DROITE;
			case "tourner_a_gauche()":	return Token.TOURNER_GAUCHE;
			case "jeter_fruit()": 		return Token.JETER_FRUIT;
			case "avancer()": 			return Token.AVANCER;
			case "couper()": 			return Token.COUPER;
			case "ramasser()":	 		return Token.RAMASSER;
			case "manger_fruit()":		return Token.MANGER;
			default: return null;
		}
	}

	public static String getType(final Token t) {
		return t.type;
	}
}
