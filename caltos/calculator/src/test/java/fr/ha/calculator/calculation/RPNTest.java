package fr.ha.calculator.calculation;

import static org.junit.Assert.*;
import java.util.Optional;
import org.junit.*;
import fr.ha.calculator.CommonsTest;
import fr.ha.calculator.datamodel.*;

public final class RPNTest {
    @Before
	public void initRPN() {
		new RPN();
	}

	@After
	public void resetRPN() {
		RPN.clearStack();
    }
    
    @Test
    public void sumOfEntierTest() {
        RPN.push(CommonsTest.elOfInt(1));
        RPN.push(CommonsTest.elOfInt(1));

        Optional<? extends Element> opElement = RPN.computeOperation("+");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Entier);
        assertTrue(2 == ((Entier)opElement.get()).getValue().intValue());
    }

    @Test
    public void sumOfNegativeEntierTest() {
        RPN.push(CommonsTest.elOfInt(-1));
        RPN.push(CommonsTest.elOfInt(5));

        Optional<? extends Element> opElement = RPN.computeOperation("+");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Entier);
        assertTrue(4 == ((Entier)opElement.get()).getValue().intValue());
    }

    @Test
    public void subOfNegativeEntierTest() {
        RPN.push(CommonsTest.elOfInt(-1));
        RPN.push(CommonsTest.elOfInt(5));

        Optional<? extends Element> opElement = RPN.computeOperation("-");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Entier);
        assertTrue(-6 == ((Entier)opElement.get()).getValue().intValue());
    }

    @Test
    public void divOfNegativeEntierTest() {
        RPN.push(CommonsTest.elOfInt(-500));
        RPN.push(CommonsTest.elOfInt(4));

        Optional<? extends Element> opElement = RPN.computeOperation("/");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Entier);
        assertTrue(-125 == ((Entier)opElement.get()).getValue().intValue());
    }

    @Test
    public void sumOfDecimalTest() {  
        RPN.push(CommonsTest.elOfDouble(1.5));
        RPN.push(CommonsTest.elOfDouble(1.5));

        Optional<? extends Element> opElement = RPN.computeOperation("+");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Decimal);
        assertTrue(3.0 == ((Decimal)opElement.get()).getValue().doubleValue());
    }

    @Test
    public void sumOfDecimalAndEntierIsDecimalTest() {
        RPN.push(CommonsTest.elOfInt(1));
        RPN.push(CommonsTest.elOfDouble(1.5));

        Optional<? extends Element> opElement = RPN.computeOperation("+");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Decimal);
        assertTrue(2.5 == ((Decimal)opElement.get()).getValue().doubleValue());
    }

    @Test
    public void timesOfNegativeDecimalAndEntierTest() {
        RPN.push(CommonsTest.elOfInt(5));
        RPN.push(CommonsTest.elOfDouble(-2.0));

        Optional<? extends Element> opElement = RPN.computeOperation("*");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Decimal);
        assertTrue(-10.0 == ((Decimal)opElement.get()).getValue().doubleValue());
    }

    @Test
    public void timesOfNegativeDecimalAndEntierTest2() {
        RPN.push(CommonsTest.elOfInt(-5));
        RPN.push(CommonsTest.elOfDouble(-2.0));

        Optional<? extends Element> opElement = RPN.computeOperation("*");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Decimal);
        assertTrue(10.0 == ((Decimal)opElement.get()).getValue().doubleValue());
    }

    @Test
    public void booleanOperationOrTest() {
        RPN.push(CommonsTest.elOfBool(true));
        RPN.push(CommonsTest.elOfBool(false));

        Optional<? extends Element> opElement = RPN.computeOperation("or");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Booleen);
        assertTrue(true == ((Booleen)opElement.get()).getValue().booleanValue());
    }

    @Test
    public void wrongEntierOperatorTest() {
        RPN.push(CommonsTest.elOfDouble(1.5));
        RPN.push(CommonsTest.elOfDouble(1.5));

        Optional<? extends Element> opElement = RPN.computeOperation("or");
        assertFalse(opElement.isPresent());
    }

    @Test
    public void wrongBooleanOperatorTest() {
        RPN.push(CommonsTest.elOfBool(true));
        RPN.push(CommonsTest.elOfBool(false));

        Optional<? extends Element> opElement = RPN.computeOperation("+");
        assertFalse(opElement.isPresent());
    }

    @Test
    public void wrongSetOperatorTest() {
        RPN.push(CommonsTest.elOfStringSet("{1}"));
        RPN.push(CommonsTest.elOfStringSet("{2}"));

        Optional<? extends Element> opElement = RPN.computeOperation("+");
        assertFalse(opElement.isPresent());
    }

    @Test
    public void divisionByZeroReturnEmpty() {
        RPN.push(CommonsTest.elOfInt(1));
        RPN.push(CommonsTest.elOfInt(0));

        Optional<? extends Element> opElement = RPN.computeOperation("/");
        assertFalse(opElement.isPresent());
    }

    @Test
    public void testSetUnion() {
        RPN.push(CommonsTest.elOfStringSet("{1;2}"));
        RPN.push(CommonsTest.elOfStringSet("{2;3;4}"));
        
        Optional<? extends Element> opElement = RPN.computeOperation("U");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Ensemble);
        assertTrue("{1;2;3;4}".equals(opElement.get().toString()));
    } 

    @Test
    public void testSetIntersection() {
        RPN.push(CommonsTest.elOfStringSet("{1;2}"));
        RPN.push(CommonsTest.elOfStringSet("{2;3;4}"));
        
        Optional<? extends Element> opElement = RPN.computeOperation("I");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Ensemble);
        assertTrue("{2}".equals(opElement.get().toString()));
    }

    @Test
    public void testSetIntersectionEmpty() {
        RPN.push(CommonsTest.elOfStringSet("{1;2}"));
        RPN.push(CommonsTest.elOfStringSet("{3;4}"));
        
        Optional<? extends Element> opElement = RPN.computeOperation("I");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Ensemble);
        assertTrue("{}".equals(opElement.get().toString()));
    } 

    @Test
    public void testFact() {
        RPN.push(CommonsTest.elOfInt(4));
        
        Optional<? extends Element> opElement = RPN.computeOperation("!");
        assertTrue(opElement.isPresent());
        assertTrue(24 == (Integer) opElement.get().getValue());
    }

    @Test
    public void testDoubleFact() {
        RPN.push(CommonsTest.elOfDouble(4.0));
        
        Optional<? extends Element> opElement = RPN.computeOperation("!");
        assertTrue(!opElement.isPresent());
    } 

    @Test
    public void testBooleanFact() {
        RPN.push(CommonsTest.elOfBool(true));
        
        Optional<? extends Element> opElement = RPN.computeOperation("!");
        assertTrue(!opElement.isPresent());
    } 

    @Test
    public void testIntPower() {
        RPN.push(CommonsTest.elOfInt(4));
        RPN.push(CommonsTest.elOfInt(2));
        
        Optional<? extends Element> opElement = RPN.computeOperation("^");
        assertTrue(opElement.isPresent());
        assertTrue(16 == (Integer) opElement.get().getValue());
    }

    @Test
    public void testDoublePower() {
        RPN.push(CommonsTest.elOfDouble(2));
        RPN.push(CommonsTest.elOfDouble(5.0));
        
        Optional<? extends Element> opElement = RPN.computeOperation("^");
        assertTrue(opElement.isPresent());
        assertTrue(32.0 == (Double) opElement.get().getValue());
    }

    @Test
    public void testSubIntersection() {
        RPN.push(CommonsTest.elOfStringSet("{1;2}"));
        RPN.push(CommonsTest.elOfStringSet("{2;3;4}"));
        
        Optional<? extends Element> opElement = RPN.computeOperation("S");
        assertTrue(opElement.isPresent());
        assertTrue(opElement.get() instanceof Ensemble);
        assertTrue("{1}".equals(opElement.get().toString()));
    }
}
