package fr.ha.calculator.datamodel;

import java.util.HashSet;

public final class ElementFactory {
	public static <R> Element create(Type t, R value) {
		switch (t) {
			case INT:
				return new Entier(new Value<Integer>((Integer) value));
			case DOUBLE:
				return new Decimal(new Value<Double>((Double) value));
			case BOOLEAN:
				return new Booleen(new Value<Boolean>((Boolean) value));
			case SET:
				return new Ensemble(new Value<HashSet<Element>>((HashSet<Element>) value));

			default: throw new IllegalArgumentException("Wrong type");
		}
	}
}
