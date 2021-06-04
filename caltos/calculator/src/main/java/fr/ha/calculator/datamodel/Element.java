package fr.ha.calculator.datamodel;

public abstract class Element {
	public abstract <R> R getValue();

	public String toString() {
		return ""+getValue();
	}
}
