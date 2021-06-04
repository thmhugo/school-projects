package fr.ha.calculator.calculation;

import static org.junit.Assert.*;
import java.util.*;
import org.junit.*;
import fr.ha.calculator.CommonsTest;
import fr.ha.calculator.datamodel.*;

public class LibraryTest {
	@Test
	public void calculateInt() {
		Library lib = new Library();
		ArrayList<Element> array = new ArrayList<>();
		array.add(CommonsTest.elOfInt(5));
		array.add(CommonsTest.elOfInt(55));

		Optional<? extends Element> elt = lib.calculate(Operation.ADD, Signature.entier, array);

		assertTrue(elt.isPresent());
		assertTrue(60 == (Integer) elt.get().getValue());

		elt = lib.calculate(Operation.SOU, Signature.entier, array);

		assertTrue(elt.isPresent());
		assertTrue(50 == (Integer) elt.get().getValue());

		elt = lib.calculate(Operation.MUL, Signature.entier, array);

		assertTrue(elt.isPresent());
		assertTrue(275 == (Integer) elt.get().getValue());

		elt = lib.calculate(Operation.DIV, Signature.entier, array);

		assertTrue(elt.isPresent());
		assertTrue(11 == (Integer) elt.get().getValue());

		array.remove(1);
		array.add(CommonsTest.elOfInt(2));

		elt = lib.calculate(Operation.POW, Signature.entier, array);

		assertTrue(elt.isPresent());
		assertTrue(32 == (Integer) elt.get().getValue());

		array.remove(1);

		elt = lib.calculate(Operation.FACT, Signature.entier, array);

		assertTrue(elt.isPresent());
		assertTrue(120 == (Integer) elt.get().getValue());
	}

	@Test
	public void calculateDouble() {
		Library lib = new Library();
		ArrayList<Element> array = new ArrayList<>();
		array.add(CommonsTest.elOfDouble(2.0));
		array.add(CommonsTest.elOfDouble(4.5));

		Optional<? extends Element> elt = lib.calculate(Operation.ADD, Signature.decimal, array);

		assertTrue(elt.isPresent());
		assertTrue(6.5 == (Double) elt.get().getValue());

		elt = lib.calculate(Operation.SOU, Signature.decimal, array);

		assertTrue(elt.isPresent());
		assertTrue(2.5 == (Double) elt.get().getValue());

		elt = lib.calculate(Operation.MUL, Signature.decimal, array);

		assertTrue(elt.isPresent());
		assertTrue(9 == (Double) elt.get().getValue());

		elt = lib.calculate(Operation.DIV, Signature.decimal, array);

		assertTrue(elt.isPresent());
		assertTrue(2.25 == (Double) elt.get().getValue());

		elt = lib.calculate(Operation.POW, Signature.decimal, array);

		assertTrue(elt.isPresent());
		assertTrue(20.25 == (Double) elt.get().getValue());
	}

	@Test
	public void calculateBool() {
		Library lib = new Library();
		ArrayList<Element> array = new ArrayList<>();
		array.add(CommonsTest.elOfBool(true));

		Optional<? extends Element> elt = lib.calculate(Operation.NOT, Signature.booleen, array);

		assertTrue(elt.isPresent());
		assertFalse((Boolean) elt.get().getValue());

		array.add(CommonsTest.elOfBool(false));

		elt = lib.calculate(Operation.OR, Signature.booleen, array);

		assertTrue(elt.isPresent());
		assertTrue((Boolean) elt.get().getValue());

		elt = lib.calculate(Operation.AND, Signature.booleen, array);

		assertTrue(elt.isPresent());
		assertFalse((Boolean) elt.get().getValue());

		elt = lib.calculate(Operation.XOR, Signature.booleen, array);

		assertTrue(elt.isPresent());
		assertTrue((Boolean) elt.get().getValue());
	}
}
