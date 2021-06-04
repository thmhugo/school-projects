package fr.ha.calculator.services;

import static org.junit.Assert.*;
import java.util.Optional;
import org.junit.Test;
import fr.ha.calculator.CommonsTest;
import fr.ha.calculator.datamodel.*;
import fr.ha.calculator.calculation.Memory;

public final class ParserTest {
    @Test
    public void givenStringMustBeEntier() {
        Optional<Element> op = Parser.strToElement("1");
        assertTrue(op.isPresent());
        assertTrue(op.get() instanceof Entier);
    }

    @Test
    public void givenStringMustBeDecimal() {
        Optional<Element> op = Parser.strToElement("1.1");
        assertTrue(op.isPresent());
        assertTrue(op.get() instanceof Decimal);
    }

    @Test
    public void givenStringMustBeBooleen() {
        Optional<Element> op = Parser.strToElement("true");
        assertTrue(op.isPresent());
        assertTrue(op.get() instanceof Booleen);
    }

    @Test
    public void givenStringMustBeEnsemble() {
        Optional<Element> op = Parser.strToElement("{1;2;3}");
        assertTrue(op.isPresent());
        assertTrue(op.get() instanceof Ensemble);
    }

    @Test
    public void givenStringMustBeBoolEnsemble() {
        Optional<Element> op = Parser.strToElement("{true;true;false}");
        assertTrue(op.isPresent());
        assertTrue(op.get() instanceof Ensemble);
    }

    @Test
    public void givenStringMustBeDoubleEnsemble() {
        Optional<Element> op = Parser.strToElement("{1.0;2.0;3.99}");
        assertTrue(op.isPresent());
        assertTrue(op.get() instanceof Ensemble);
    }

    @Test
    public void givenStringIsUnknown1() {
        Optional<Element> op = Parser.strToElement("unknown");
        assertFalse(op.isPresent());
    }

    @Test
    public void setElementMustBeSeparatedByASemicolon() {
        Optional<Element> op = Parser.strToElement("{1,2,3}");
        assertFalse(op.isPresent());
    }

    @Test
    public void setElementMustBeOfTheSameType() {
        Optional<Element> op = Parser.strToElement("{1;2;true}");
        assertFalse(op.isPresent());
    }

    @Test
    public void isOneLineInterp() {
        assertTrue(Parser.isOneLineInterp("3 4 5 +"));
        assertTrue(Parser.isOneLineInterp("10 *"));
        assertFalse(Parser.isOneLineInterp("3"));
        assertFalse(Parser.isOneLineInterp(""));
    }

    @Test
    public void getHistFromLineTest(){
        Memory mem = new Memory();
        mem.addHist(CommonsTest.elOfInt(16));
        mem.addHist(CommonsTest.elOfInt(9999));

        assertEquals(CommonsTest.elOfInt(16), Parser.getHistFromLine("hist(0)", mem).get());
        assertEquals(CommonsTest.elOfInt(9999), Parser.getHistFromLine("hist(1)", mem).get());
        assertEquals(CommonsTest.elOfInt(9999), Parser.getHistFromLine("hist(-1)", mem).get());
        assertEquals(CommonsTest.elOfInt(16), Parser.getHistFromLine("hist(-2)", mem).get());
        assertFalse(Parser.getHistFromLine("hist(4)", mem).isPresent());
        assertFalse(Parser.getHistFromLine("hist(-3)", mem).isPresent());
    }
}
