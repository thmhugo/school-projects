package fr.ha.calculator.calculation;

import java.util.*;
import fr.ha.calculator.datamodel.*;

public class Library {
	private Map<Operation, Map<Signature, OperationCalc<? extends Object>>> lib = new HashMap<>();

	Library() {
		for (Operation o : Operation.values()){
			lib.put(o, createSignatureList(o));
		}
	}

	/**
	 * Functional interface used to calculate an operation.
	 */
	private interface OperationCalc<R> {
		public R calculate (ArrayList<R> el);
	}

	/**
	 * Convert an operator and the type to an operation in order to calculate
	 * various Elements.
	 * 
	 * @param o is the operator to be used.
	 * @param type is the type of the calculation.
	 * @return the corresponding lambda expression calculation.
	 */
	private OperationCalc<? extends Object> convert(Operation o, Type type) {
		OperationCalc<Integer> entier;
		OperationCalc<Double> decimal;
		OperationCalc<Boolean> booleen;
		OperationCalc<HashSet<Element>> set;

		switch (type) {
			case INT : switch (o) {
				/** op's length is checked in RPN.  */
				case ADD: entier = (op) -> op.get(0) + op.get(1); return entier;
				case SOU: entier = (op) -> op.get(1) - op.get(0); return entier;
				case MUL: entier = (op) -> op.get(0) * op.get(1); return entier;
				case FACT: entier = (op) -> factorial(op.get(0)); return entier;
				case POW : entier = (op) -> (int)Math.pow(op.get(1),op.get(0)); return entier;
				default : entier = (op) -> op.get(1) / op.get(0); return entier;
			}
		
			case DOUBLE : switch (o) {
				case ADD: decimal = (op) -> op.get(0) + op.get(1); return decimal;
				case SOU: decimal = (op) -> op.get(1) - op.get(0); return decimal;
				case MUL: decimal = (op) -> op.get(0) * op.get(1); return decimal;
				case POW : decimal = (op) -> Math.pow(op.get(1),op.get(0)); return decimal;
				default : decimal = (op) -> {
						if (op.get(0) == 0.0) {
							throw new ArithmeticException();
						}

						return op.get(1) / op.get(0);
				}; return decimal;
			}
			
			case SET : switch (o) {
				case UNION : set = (op) -> {
						HashSet<Element> s = new HashSet<Element>();
						s.addAll(op.get(0));
						s.addAll(op.get(1));
						return s;
					}; 
					return set;
				case INTER : set = (op) -> {
						HashSet<Element> s = new HashSet<Element>();
						HashSet<Element> bigger = 
							op.get(0).size() >=  op.get(1).size() ? op.get(0) : op.get(1);
						HashSet<Element> smaller = 
							op.get(0).size() <  op.get(1).size() ? op.get(0) : op.get(1);
						s.addAll(smaller);
						s.retainAll(bigger);
						return s;
					}; 
					return set;
				case DIFF : set = (op) -> {
					HashSet<Element> source = op.get(1);
					source.removeAll(op.get(0));
					return source;
				};
				return set; 
			}
			
			default : switch (o) {
				case AND: booleen =(op) -> op.get(0) && op.get(1); return booleen;
				case OR: booleen = (op) -> op.get(0) || op.get(1); return booleen;
				case XOR : booleen = (op) -> op.get(0) ^ op.get(1); return booleen;
				default: booleen = (op) -> !op.get(0); return booleen;
			}
		}
	}

	private Map<Signature, OperationCalc<? extends Object>> createSignatureList(Operation o) {
		Map<Signature, OperationCalc<? extends Object>> liste = new HashMap<>();

		for (Type t : Type.values()){
			Signature s;
			switch (t) {
				case INT : s = Signature.entier; break;
				case DOUBLE : s = Signature.decimal; break;
				case SET : s = Signature.set; break;
				default : s = Signature.booleen; break;
			}
			liste.put(s, convert(o, t));
		}

		/* Add special cases: */
		liste.put(new Signature(Type.DOUBLE, Type.INT), convert(o, Type.DOUBLE));
		liste.put(new Signature(Type.INT, Type.DOUBLE), convert(o, Type.DOUBLE));

		return liste;
	}

	/**
	 * Calculates the result given an operator, a signature and operands.
	 * 
	 * @param operator is the operator to be used.
	 * @param signature is the signature of the calculation.
	 * @param el are the operands.
	 * @return an Optional that contains the result.
	 */

	public <R> Optional<? extends Element> calculate (Operation operator, Signature signature, ArrayList<Element> el) {
		Map<Signature, OperationCalc<? extends Object>> sub_result = lib.get(operator);
		Object op = sub_result.get(signature);
		
		if (sub_result == null || op == null) {
			return Optional.empty(); 
		} 

		ArrayList<R> values = new ArrayList<R>();
		el.stream().forEach((e) -> values.add(e.getValue()));

		try {
			R res =((OperationCalc<R>) op).calculate(values);

			return Optional.of(
				ElementFactory.create(signature.getSignatureType(), res)
			);
		} 
		catch (ClassCastException | ArithmeticException | IndexOutOfBoundsException e) {
			/** ArithmeticException -> among others the division by 0 */
			/** ClassCastException -> wrong operator like : true false + */
			/** IndexOutOfBoundsException -> not supported operation in the given type */
			Errors.printError(Errors.NOT_SUPPORTED_OP, "");
			return Optional.empty();
		} 
		catch (IllegalArgumentException e) {
			/** IllegalArgumentException -> factorial of an int bigger than 12 */
			Errors.printError(Errors.WRONG_FORMAT2, e.getMessage());
			return Optional.empty();
		}
	}

	private int factorial(int n){    
		if (n > 12) {
			throw new IllegalArgumentException(" Too big integer : "+n);
		}
		if (n == 0) {
			return 1;    
		}    
		else {
			return(n * factorial(n-1));
		}
	}  

}
