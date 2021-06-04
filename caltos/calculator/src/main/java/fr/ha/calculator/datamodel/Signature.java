package fr.ha.calculator.datamodel;

import java.util.Arrays;

public final class Signature {
	private final int arite;
	private final Type[] types;

	public final static Signature entier = new Signature(Type.INT, Type.INT);
	public final static Signature decimal = new Signature(Type.DOUBLE, Type.DOUBLE);
	public final static Signature booleen = new Signature(Type.BOOLEAN, Type.BOOLEAN);
	public final static Signature set = new Signature(Type.SET, Type.SET);

	public Signature (Type ... elt) {
		types = Arrays.copyOf(elt, elt.length);
		arite = elt.length;
	}

	@Override
	public boolean equals(Object o) { 
		if (o == this) {
			return true;
		}
		if (!(o instanceof Signature)) {
			return false;
		}

		Signature s = (Signature) o;
		if (this.arite != s.arite) {
			return false;
		}

		for (int i = 0; i < arite; i++) {
			if (s.types[i] != this.types[i]) {
				return false;
			}
		}

		return true;
	}

	@Override
	public int hashCode() {
	   return (int)types[0].toString().charAt(0)+(int)types[0].toString().charAt(types[0].toString().length()-1);
	}

	public Type getTypeAt(int index) {
		if (index >= arite || index < 0) {
			throw new IndexOutOfBoundsException();
		}
		return types[index]; 
	}

	public boolean isEntier() {
		for (int i = 0; i < arite; i++) {
			if (types[i] != Type.INT) {
				return false;
			}
		}

		return true;
	}

	public static Signature ofOperation(Operation op, Type t) {
		switch (op) {
			case ADD:
			case SOU:
			case MUL:
			case FACT:
			case POW :
			case DIV: return (t.equals(Type.INT) ? entier : decimal);
			case UNION : 
			case DIFF : 
			case INTER : return set;
			default : return booleen;
		}
	}

	public Type getSignatureType() {
		if (this.isEntier()) {
			return Type.INT;
		}
		if (this.isBoolean()) {
			return Type.BOOLEAN;
		}
		if (this.isSet()) {
			return Type.SET;
		}

		return Type.DOUBLE;
	}

	public boolean isBoolean() {
		for (int i = 0; i < arite; i++) {
			if (types[i] != Type.BOOLEAN) {
				return false;
			}
		}

		return true;
	}

	public boolean isDecimal() {
		for (int i = 0; i < arite; i++) {
			if (types[i] != Type.DOUBLE) {
				return false;
			}
		}

		return true;
	}

	public boolean isSet() {
		for (int i = 0; i < arite; i++) {
			if (types[i] != Type.SET) {
				return false;
			}
		}
		return true;
	}

	public String toString() {
		String res = "";
		for (Type t : types) {
			res += t.toString()+" ";
		}
		return res;
	}


	public int getArite() {
		return this.arite;
	}
}