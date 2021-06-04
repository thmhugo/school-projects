package fr.ha.calculator.datamodel;

import java.util.List;
import java.util.Optional;

public enum Operation {
	ADD(2, true, 0),
	SOU(2, false, 0), 
	MUL(2, true, 1), 
	DIV(2, false, 1),
	FACT(1, false, 2),
	POW(2, false, 3),


	UNION(2, true, 0),
	INTER(2, true, 0),
	DIFF(2, true, 0),
	
	AND(2, true, 1),
	OR(2, true, 0),
	XOR(2, true, 0),
	NOT(1, true, 1);
	
	private final int arite;
	private final boolean left_associativity;
	private final int priority;

	public static final List<String> numberOperators = 
		List.of(
			"+", "-", "*", "/", "!", "^"
		);

	public static final List<String> setOperators = 
		List.of(
			"U", "I", "S"
		);

	public static final List<String> booleanOperators =
		List.of(
			"and",
			"not",
			"or",
			"xor"
		);
	

	private Operation(int arite, boolean associativity, int priority) {
		this.arite = arite;
		this.left_associativity = associativity;
		this.priority = priority;
	}

	public int getArite() {
		return arite;
	}

	public int getPriority() {
		return priority;
	}

	public boolean getLeftAssociativity() {
		return left_associativity;
	}

	public static boolean isFact(String s) {
		return s.equals("!");
	}

	public static boolean isOperator(String s) {
		return numberOperators.contains(s) 
		|| setOperators.contains(s)
		|| booleanOperators.contains(s) ;
	}

	public static Optional<Operation> ofString(String s) {
		switch (s) {
			case "+" : return Optional.of(ADD);
			case "-" : return Optional.of(SOU);
			case "*" : return Optional.of(MUL);
			case "/" : return Optional.of(DIV);
			case "!" : return Optional.of(FACT);
			case "^" : return Optional.of(POW);

			case "U" : return Optional.of(UNION);
			case "I" : return Optional.of(INTER);
			case "S" : return Optional.of(DIFF);

			case "and" : return Optional.of(AND);
			case "or" :  return Optional.of(OR);
			case "xor" : return Optional.of(XOR);
			case "not" : return Optional.of(NOT);
		}
		return Optional.empty();
	}
}
