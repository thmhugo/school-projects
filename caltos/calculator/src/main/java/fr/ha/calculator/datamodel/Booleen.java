package fr.ha.calculator.datamodel;

public final class Booleen extends Element {
	private final Value<Boolean> value;
	
	Booleen(Value<Boolean> value) {
		this.value = value;
	}

	@Override
	public Boolean getValue() {
		return value.getValue();
	}

	@Override
	public int hashCode() {
        return  value.getValue().booleanValue() == true ? 1 : 0;
    }
	
	public boolean equals(Object e) {
		return (e instanceof Booleen &&
			((Booleen)e).getValue().booleanValue() == value.getValue().booleanValue()
		);
	}
}
