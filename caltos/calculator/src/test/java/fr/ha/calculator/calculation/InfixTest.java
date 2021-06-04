package fr.ha.calculator.calculation;

import static org.junit.Assert.*;
import java.util.*;
import org.junit.*;

public final class InfixTest {
	@Test
	public void shouldGiveRightRPNIntExpression() {
		Optional <ArrayList<String>> list = RPN.convertInfixToRPN("3 + 4 * 2");
		
		assertTrue(list.isPresent());
		assertEquals("[3, 4, 2, *, +]", list.get().toString());
	}

	@Test
	public void shouldGiveRightRPNIntExpression2() {
		Optional <ArrayList<String>> list = RPN.convertInfixToRPN("3 ! + 3 + 4 * 2");
		
		assertTrue(list.isPresent());
		assertEquals("[3, !, 3, +, 4, 2, *, +]", list.get().toString());
	}

	@Test
	public void shouldGiveRightRPNIntExpression3() {
		Optional <ArrayList<String>> list = RPN.convertInfixToRPN("4 * ( 2 + 2 )");
		
		assertTrue(list.isPresent());
		assertEquals("[4, 2, 2, +, *]", list.get().toString());
	}

	@Test
	public void shouldGiveRightRPNNegativeIntExpression() {
		Optional <ArrayList<String>> list = RPN.convertInfixToRPN("-3 + 4 * -2");
		
		assertTrue(list.isPresent());
		assertEquals("[-3, 4, -2, *, +]", list.get().toString());
	}

	@Test
	public void shouldGiveRightRPNDoubleExpression() {
		Optional <ArrayList<String>> list = RPN.convertInfixToRPN("3.5 + 10.0 * ( 9.0 + 1.0 )");
		
		assertTrue(list.isPresent());
		assertEquals("[3.5, 10.0, 9.0, 1.0, +, *, +]", list.get().toString());
	}

	@Test
	public void shouldGiveRightRPNNegativeDoubleExpression() {
		Optional <ArrayList<String>> list = RPN.convertInfixToRPN("-3.5 + 10.0 * ( 9.0 - -1.0 )");
		
		assertTrue(list.isPresent());
		assertEquals("[-3.5, 10.0, 9.0, -1.0, -, *, +]", list.get().toString());
	}

	@Test
	public void shouldGiveRightRPNBoolExpression() {
		Optional <ArrayList<String>> list = RPN.convertInfixToRPN("not true or ( true and true )");
		
		assertTrue(list.isPresent());
		assertEquals("[true, not, true, true, and, or]", list.get().toString());
	}

	@Test
	public void shouldGiveRightRPNSetExpression() {
		Optional <ArrayList<String>> list = RPN.convertInfixToRPN("{1;5} U {2;1}");
		
		assertTrue(list.isPresent());
		assertEquals("[{1;5}, {2;1}, U]", list.get().toString());
	}
}
