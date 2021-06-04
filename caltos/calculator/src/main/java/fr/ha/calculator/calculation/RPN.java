package fr.ha.calculator.calculation;

import java.util.*;
import fr.ha.calculator.datamodel.*;
import fr.ha.calculator.services.*;

public final class RPN {
    private static Stack<Element> stack = new Stack<Element>();
    private static Library b = new Library();

    /**
	 * Given an operator, compute the result using the RPN algorithm
	 * 
	 * @param operator is the String representation of the operator
	 * @return an Optional that contains the result.
	 */
    public static Optional<? extends Element> computeOperation(String operator) {
        Optional<Operation> optionalOp = Operation.ofString(operator);
        Operation op;
        ArrayList<Element> args = new ArrayList<Element>();

        if (!optionalOp.isPresent()) {
            return Optional.empty();
        }

        op = optionalOp.get();
       
        if (op.getArite() > stack.size()) {
            Errors.printError(Errors.NOT_ENOUGH_OPERANDS, "");

            return Optional.empty();
        }

        for (int i = 0; i < op.getArite(); i++) {
            args.add(stack.pop());
        }
        
        Type operandsType = getTypeFromArgs(args);

        if (operandsType == Type.DOUBLE) {
            try{
                args = argsToDecimal(args);
            } 
            catch (NumberFormatException e) {
                Errors.printError(Errors.INCOMPATIBLE_OPERANDS, args.toString());
                return Optional.empty();
            }
        }

        Optional<? extends Element> elt = b.calculate(op, Signature.ofOperation(op, operandsType), args);

        if (elt.isPresent()) {
            push(elt.get());
        }

        return elt;
    }

    private static Type getTypeFromArgs(ArrayList<Element> args) {
        if (argsOfEntier(args)) {
            return Type.INT;
        }
        if (argsOfBooleen(args)) {
            return Type.BOOLEAN;
        }
        if (argsOfEnsemble(args)) {
            return Type.SET;
        }
        return Type.DOUBLE;
    }

    public static void push(Element e) {
        stack.push(e);
    }

    public static Optional<Element> pop() {
        if (stack.isEmpty()) {
            return Optional.empty();
        }

        return Optional.of(stack.pop());
    }

    public static void clearStack() {
		stack = new Stack<Element>();
    }

    public static void showStack() {
        int index = 0;
		System.out.print("Current stack: [");
		for (Element elt: stack) {
			if (index == stack.size() - 1) {
				System.out.print("("+index+") "+elt.getValue());
			}
			else {
				System.out.print("("+index+") "+elt.getValue()+", ");
			}
			index ++;
		}
		System.out.println("]");
	}

    private static boolean argsOfEntier(ArrayList<Element> args) {
        for (int i =0; i<args.size(); i++) {
            if (!(args.get(i) instanceof Entier)) {
                return false;
            }
        }
        return true;
    }

    private static boolean argsOfBooleen(ArrayList<Element> args) {
        for (int i =0; i<args.size(); i++) {
            if (!(args.get(i) instanceof Booleen)) {
                return false;
            }
        }
        return true;
    }
    private static boolean argsOfEnsemble(ArrayList<Element> args) {
        for (int i =0; i<args.size(); i++) {
            if (!(args.get(i) instanceof Ensemble)) {
                return false;
            }
        }
        return true;
    }

    private static ArrayList<Element> argsToDecimal(ArrayList<Element> args) {
        ArrayList<Element> decimalArgs = new ArrayList<>();
        for (Element el : args) {
            decimalArgs.add(
                ElementFactory
                    .create(
                        Type.DOUBLE,
                        Double.parseDouble(el.getValue().toString())
                    )
            );
        }
        return decimalArgs;
    }

    public static Optional<String> getIndexInStack(int index) {
        /* Negative index starting from the top and positive index from the bottom. */

        if (stack.isEmpty()) {
            Errors.printError(Errors.EMPTY_STACK, "");
            return Optional.empty();
        }

        if (index < stack.size()) {
            if (index < 0) {
                index = stack.size() + index;

                if (index > stack.size() || index < 0) {
                    Errors.printError(Errors.INCORRECT_INDEX, "");
                    return Optional.empty();
                }
            }

            return Optional.of(stack.get(index).toString());
        }
        
        Errors.printError(Errors.INCORRECT_INDEX, "");
        return Optional.empty();
    }

    /**
	 * Converts an Infix expression to the Reverse Polish Notation.
     * It's an implementation of the Shunting-Yard algorithm.
	 * 
	 * @param line is the String to be converted.
	 * @return a list of tokens in RPN order.
	 */
	public static Optional<ArrayList<String>> convertInfixToRPN (String line) {
        String tokens[] = Parser.tokenize(line);
		Stack<String> stack = new Stack<>();
        ArrayList<String> result = new ArrayList<>();

		for (String str : tokens) {
			if (Operation.isOperator(str)) {
				/* If operator o2 is in top of the stack */
				if (!stack.isEmpty()) {
					String top_stack = stack.peek();
					if (Operation.isOperator(top_stack)) {
						Operation op1 = Operation.ofString(str).get();
						Operation op2 = Operation.ofString(top_stack).get();
	
						/* Compares the associativity of the operators. */
						if ((op1.getLeftAssociativity() && op1.getPriority() <= op2.getPriority()) || 
							(!op1.getLeftAssociativity() && op1.getPriority() <= op2.getPriority())
						) {
							stack.pop();
							result.add(top_stack);
						}
					}
				}
				stack.add(str);
			}
			else if (str.equals("(")) {
				stack.add(str);
			}
			else if (str.equals(")")) {
				String poped = "";

				/* Pop the content of the stack until the closing parenthesis is found. */
				while (!poped.equals("(")) {
					if (stack.isEmpty()) {
						Errors.printError(Errors.INFIX_RPN_MISSING, "");
						return Optional.empty();
					}
					poped = stack.pop();

					if (!poped.equals("(")) {
						result.add(poped);
					}
				}
			}
			else {
				result.add(str);
			}
		}
		
		/* Empty out the remaining operators in the stack. */
		while (!stack.isEmpty()) {
			String poped = stack.pop();

			if (poped.equals("(")) {
				Errors.printError(Errors.INFIX_RPN_ILLEGAL, "");
				return Optional.empty();
			}

			result.add(poped);
        }
        
        if (result.equals(Arrays.asList(tokens)) && !result.get(result.size()-1).equals("!")) {
            Errors.printError(Errors.UNKNOWN_CMD, "");

            return Optional.empty();
        }

		return Optional.of(result);
	}
}
