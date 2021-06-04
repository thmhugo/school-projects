package fr.ha.calculator.datamodel;

public final class Decimal extends Element{
	private final Value<Double> value;
	
	Decimal(Value<Double> value){
		this.value = value;
	}

	@Override
	public Double getValue() {
		return value.getValue().doubleValue();
	}

	@Override
	public int hashCode() {
        return value.getValue().intValue();
    }

	public boolean equals(Object e) {
		return (e instanceof Decimal &&
			((Decimal)e).getValue().doubleValue() == value.getValue().doubleValue()
		);
	}
}
