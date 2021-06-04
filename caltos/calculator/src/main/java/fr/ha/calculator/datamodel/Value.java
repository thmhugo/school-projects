package fr.ha.calculator.datamodel;

public final class Value<R> {
	private final R value;
	
	Value(R value) {
		this.value = value;
	}

	public R getValue() {
		return this.value;
	}
}
