package fr.ha.calculator.calculation;

import java.util.*;
import fr.ha.calculator.datamodel.*;

public final class Memory {
	private final Map<String, Element> variables;
	private final LinkedList<Element> calculation_history;

	public Memory() {
		variables = new HashMap<>();
		calculation_history = new LinkedList<>();
	}

	private boolean addVar(String s, Optional <Element> elt) {
		if (!elt.isPresent()){
			Errors.printError(Errors.NO_CALCULATION, "");
			return false;
		} 
		if (s.length() != 1) {
			Errors.printError(Errors.WRONG_FORMAT, s);
			return false;
		}

		variables.put(s, elt.get());
		return true;
	}

	/**
	 * Associates s with the actual top of the stack. (Only for RPN)
	 * 
	 * @param s is the name's variable
	 * @return true if it could be added and false there is nothing to be associated.
	 */
	public boolean addVarRPN(String s) {
		Optional <Element> elt = RPN.pop();
		return addVar(s, elt);
	}

	/**
	 * Associates s with the last result in the history. (Only for ALG)
	 * 
	 * @param s is the name's variable
	 * @return true if it could be added and false if the history is empty.
	 */
	public boolean addVarALG(String s) {
		try {
			Optional <Element> elt = Optional.of(calculation_history.getLast());

			return addVar(s, elt);
		}
		catch (NoSuchElementException e) {
			Errors.printError(Errors.NO_CALCULATION, "");
			return false;
		}
	}

	public boolean isVarPresent(String s) {
		return variables.containsKey(s);
	}

	public Optional<Element> getVar(String s) {
		if(isVarPresent(s)) {
			return Optional.of(variables.get(s));
		}
		
		return Optional.empty();
	}

	public void addHist(Element e) {
		calculation_history.add(e);
	}

	public Optional <Element> getHist(int index) {
		if (index >= calculation_history.size()) {
			return Optional.empty();
		}

		if (index < 0) {
			index = calculation_history.size() + index;

			if (index < 0) {
				return Optional.empty();
			}
		}

		return Optional.of(calculation_history.get(index));
	}

	public void printAllVar() {
		System.out.println("["+variables.size()+"]");

		for (Map.Entry<String, Element> entry: variables.entrySet()) {
			System.out.print(entry.getKey()+"="+entry.getValue()+" ");
		}
		System.out.println();
	}

	public void printAllHist() {
		int index = 0;
		System.out.print("Current history: [");
		for (Element elt: calculation_history) {
			if (index == calculation_history.size() - 1) {
				System.out.print("("+index+") "+elt.getValue());
			}
			else {
				System.out.print("("+index+") "+elt.getValue()+", ");
			}
			index ++;
		}
		System.out.println("]");
	}
}
