package fr.ha.calculator.calculation;

import static org.junit.Assert.*;
import java.util.Optional;
import org.junit.*;
import fr.ha.calculator.CommonsTest;
import fr.ha.calculator.datamodel.*;

public final class MemoryTest {
	@Before
	public void initRPN() {
		new RPN();
	}

	@After
	public void resetRPN() {
		RPN.clearStack();
	}

	@Test
    public void shouldSaveUniqueVarInt() {
		Memory mem = new Memory();

		RPN.push(CommonsTest.elOfInt(50));
		mem.addVarRPN("x");
		
		assertEquals(Optional.empty(), RPN.pop());
		assertTrue(mem.getVar("x").isPresent());
		assertTrue(50 == (Integer) mem.getVar("x").get().getValue());
	}

	@Test
    public void shouldSaveUniqueVarBool() {
		Memory mem = new Memory();

		RPN.push(CommonsTest.elOfBool(true));
		mem.addVarRPN("y");

		assertEquals(Optional.empty(), RPN.pop());
		assertTrue(mem.getVar("y").isPresent());
		assertEquals(true, (Boolean) mem.getVar("y").get().getValue());
	}

	@Test
    public void shouldSaveUniqueVarDouble() {
		Memory mem = new Memory();

		RPN.push(CommonsTest.elOfDouble(10.6));
		mem.addVarRPN("t");

		assertEquals(Optional.empty(), RPN.pop());
		assertTrue(mem.getVar("t").isPresent());
		assertTrue(10.6 == (Double) mem.getVar("t").get().getValue());
	}

	@Test
    public void shouldSaveUniqueVarSet() {
		Memory mem = new Memory();

		RPN.push(CommonsTest.elOfStringSet("{5;4;3;2;1}"));
		mem.addVarRPN("z");

		assertEquals(Optional.empty(), RPN.pop());
		assertTrue(mem.getVar("z").isPresent());
		// FIXME: To change when Ensemble will be fixed
	}

	@Test
	public void shouldSaveVariousVar() {
		Memory mem = new Memory();

		RPN.push(CommonsTest.elOfInt(50));
		mem.addVarRPN("x");
		RPN.push(CommonsTest.elOfBool(false));
		mem.addVarRPN("y");

		assertTrue(mem.getVar("x").isPresent());
		assertTrue(50 == (Integer) mem.getVar("x").get().getValue());

		assertTrue(mem.getVar("y").isPresent());
		assertEquals(false, (Boolean) mem.getVar("y").get().getValue());

		assertEquals(Optional.empty(), RPN.pop());
	}

	@Test
	public void shouldSaveSameTypeVar() {
		Memory mem = new Memory();

		RPN.push(CommonsTest.elOfInt(50));
		mem.addVarRPN("x");
		RPN.push(CommonsTest.elOfInt(14));
		mem.addVarRPN("y");

		assertTrue(mem.getVar("x").isPresent());
		assertTrue(50 == (Integer) mem.getVar("x").get().getValue());

		assertTrue(mem.getVar("y").isPresent());
		assertTrue(14 == (Integer) mem.getVar("y").get().getValue());

		assertEquals(Optional.empty(), RPN.pop());
	}

	@Test
	public void shouldHaveCorrectHistory() {
		Memory mem = new Memory();
		Optional<Element> empty = Optional.empty();

		mem.addHist(CommonsTest.elOfInt(5));
		mem.addHist(CommonsTest.elOfInt(50));
		mem.addHist(CommonsTest.elOfInt(10));

		assertTrue((Integer) mem.getHist(0).get().getValue() == 5);
		assertTrue((Integer) mem.getHist(1).get().getValue() == 50);
		assertTrue((Integer) mem.getHist(2).get().getValue() == 10);
		assertTrue(mem.getHist(3) == empty);
	}
	
	@Test
	public void shouldNotGetVar() {
		Memory mem = new Memory();
		mem.addHist(CommonsTest.elOfInt(5));
		mem.addVarRPN("xxx");
		
		assertFalse(mem.getVar("x").isPresent());
		assertFalse(mem.getVar("xxx").isPresent());
	}
}
