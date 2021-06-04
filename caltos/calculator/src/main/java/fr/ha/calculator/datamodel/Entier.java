package fr.ha.calculator.datamodel;

public final class Entier extends Element {
	private final Value<Integer> value;
	
	Entier(Value<Integer> value) {
		this.value = value;
	}

	@Override
	public Integer getValue() {
		return value.getValue();
	}

	public int hashCode() {
        return value.getValue().intValue();
    }

	public boolean equals(Object e) {
		return (e instanceof Entier && 
			((Entier)e).getValue().intValue() == value.getValue().intValue()
		);
	}
}
