package fr.ha.calculator;

import fr.ha.calculator.datamodel.*;
import fr.ha.calculator.services.Parser;

public final class CommonsTest {

	/** To facilitate the creation of Element if the parsing is not solicited. */
	public static Element elOfInt(int i) {
		return ElementFactory
				.create(Type.INT, i);
	}

	public static Element elOfDouble(double i) {
		return ElementFactory
				.create(Type.DOUBLE, i);
	}

	public static Element elOfStringSet(String str) {
		return ElementFactory
			.create(Type.SET, Parser.strToSet(str));
	}

	public static Element elOfBool(boolean b) {
		return ElementFactory
				.create(Type.BOOLEAN, b);
	}
}
