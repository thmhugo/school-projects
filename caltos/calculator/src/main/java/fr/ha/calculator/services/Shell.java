package fr.ha.calculator.services;

import java.io.*;
import java.util.*;
import fr.ha.calculator.datamodel.*;
import fr.ha.calculator.calculation.*;

public final class Shell {
	private static boolean isRPN;
	private static boolean isLastOperation;
	private static final BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
	private static final Memory mem = new Memory();

	private Shell() {}

	public static void selectMode() {
		System.out.println(
				"Please choose your calculator's mode between 'RPN' and 'ALG' (Stands for algebraic notation).");
		while (true) {
			printPrompt();
			try {
				String mode = reader.readLine();
				switch (mode) {
					case "RPN":
						isRPN = true;
						return;
					case "ALG":
						isRPN = false;
						return;
					default:
						System.out.println("Wrong mode, please try again with 'RPN' or 'ALG'.");
						break;
				}
			} 
			catch (IOException e) {
				Errors.printError(Errors.READING_ERR, "");
			}
		}
	}

	public static void run() {
		selectMode();
		while (true) {
			printPrompt();
			String line;
			try {
				line = reader.readLine();
				if (line == null || line.length() == 0 || line.trim().length() == 0) {
					Errors.printError(Errors.STR_EMPTY, "");
					continue;
				}
				
				switch (line) {
					case "showStack":
						if (isRPN) {
							RPN.showStack();
						}
						else {
							Errors.printError(Errors.NOT_AVAILABLE_IN, "ALG");
						}
						break;
					case "clear":
						if (isRPN) {
							RPN.clearStack();
						}
						else {
							Errors.printError(Errors.NOT_AVAILABLE_IN, "ALG");
						}
						break;
					case "showHist":
						mem.printAllHist();
						break;
					case "showVar":
						mem.printAllVar();
						break;
					default:
						if (isRPN && Parser.isOneLineInterp(line)) {
							isLastOperation = false;
							lineOperationRPN(line);
						} 
						else if (Parser.isOneLineInterp(line)) {
							isLastOperation = false;
							lineOperation(line);
						} 
						else {
							isLastOperation = true;
							execCmd(line, false);
						}
						break;
				}
			} 
			catch (IOException e) {
				Errors.printError(Errors.READING_ERR, "");
			}
		}
	}

	private static void printPrompt() {
		System.out.print("> ");
	}

	private static boolean execCmd(String line, boolean isLine) {
		if (execCmdMemory(line, isLine)) {
			return true;
		}
		else if (Operation.isOperator(line)) {
			return pushResult(line);
		} 
		else {
			return pushElem(line, isLine);
		}
	}

	private static boolean execCmdMemory(String line, boolean isLine) {
		boolean res = false;

		if (line.contains("pile")) {
			if (!isRPN) {
				Errors.printError(Errors.NOT_AVAILABLE_IN, "ALG");
				return true;
			}
			getStackIndex(line, isLine);
			res = true;
		}
		if (line.contains("hist")) {
			getHistoryIndex(line, isLine);
			res = true;
		}
		if (Parser.isGetVarInterp(line) || Parser.isSetVarInterp(line)){
			execCmdVariable(line, isLine);
			res = true;
		}

		return res;
	}

	private static void doLineOperation(String[] tokens) {
		for (int i = 0; i < tokens.length; i++) {
			if (i == tokens.length - 1) {
				isLastOperation = true;
			}

			if (!execCmd(tokens[i], true)) {
				return;
			}
		}
	}

	private static void lineOperationRPN(String line) {
		String[] tokens = Parser.tokenize(line);

		doLineOperation(tokens);
	}

	private static void lineOperation(String line) {
		Optional<ArrayList<String>> result = RPN.convertInfixToRPN(line);

		if (!result.isPresent()) {
			RPN.clearStack();
			return;
		}

		String[] tokens = result.get().toArray(new String[0]);

		doLineOperation(tokens);
		RPN.clearStack();
	}

	private static boolean pushResult(String line) {
		Optional<? extends Element> result = RPN.computeOperation(line);

		if (result.isPresent()) {
			if (isLastOperation) {
				System.out.println(result.get());
				mem.addHist(result.get());
			}

			return true;
		}

		return false;
	}

	private static boolean pushElem(String line, boolean isLine) {
		Optional<Element> opElement = Parser.strToElement(line);

		if (opElement.isPresent()) {
			if (isRPN || !isLastOperation) {
				RPN.push(opElement.get());
			}

			if (!isLine) {
				mem.addHist(opElement.get());
				System.out.println(opElement.get());
			}

			return true;
		}
		else {
			Errors.printError(Errors.UNKNOWN_CMD, "");
			return false;
		}
	}

	private static void getStackIndex(String line, boolean isLine) {
		Optional<String> elt = Parser.getStackIndexFromLine(line);

		if (!elt.isPresent()) {
			/** error message displayed in getStackIndexFromLine */			
			return;
		}

		pushElem(elt.get(), isLine);
	}

	private static void getHistoryIndex(String line, boolean isLine) {
		Optional <Element> elt = Parser.getHistFromLine(line, mem);
		
		if(!elt.isPresent()) {
			/** error message displayed in getHistFromLine */	
			return;
		}

		pushElem(elt.get().toString(), isLine);
	}

	private static void execCmdVariable(String line, boolean isLine) {
		line = line.trim();

		if (Parser.isSetVarInterp(line)) {
			saveVariable(line.substring(1));
		}

		else if (Parser.isGetVarInterp(line)){
			getSavedVariable(line.substring(1), isLine);
		}

		else {
			Errors.printError(Errors.WRONG_NAME, "");
		}
	}

	private static void saveVariable(String x) {
		if (isRPN) {
			mem.addVarRPN(x);
		}

		else {
			mem.addVarALG(x);
		}
	}

	private static void getSavedVariable(String x, boolean isLine) {
		Optional<Element> e = mem.getVar(x);
		if (!e.isPresent()) {
			Errors.printError(Errors.UNINITIALIZED_VAR, x);
			return;
		}

		String res = e.get().getValue().toString();
		/* When the searched variable is a Set, the brakets should be
		transformed into braces in order to be correctly be parsed. */
		res = res.replaceAll("\\[", "{");
		res = res.replaceAll("\\]", "}");
		res = res.replaceAll(", ", ";");

		pushElem(res, isLine);
	}
}