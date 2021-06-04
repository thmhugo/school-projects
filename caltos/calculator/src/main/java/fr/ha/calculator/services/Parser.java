package fr.ha.calculator.services;

import java.util.*;
import java.util.regex.*;
import fr.ha.calculator.calculation.*;
import fr.ha.calculator.datamodel.*;

public final class Parser {
    private enum Patterns {
        /* All the patterns used to analyse the line. */
        GETVARIABLE(Pattern.compile("\\?[a-zA-Z]")),
        SETVARIABLE(Pattern.compile("\\![a-zA-Z]")),
        INT(Pattern.compile("-?\\d*")),
        DOUBLE(Pattern.compile("(-?\\d*)\\.(\\d*)")),
        BOOL(Pattern.compile("true|false")),
        SETINT(Pattern.compile("\\{((true|false);)*(true|false)\\}")),
        SETDOUBLE(Pattern.compile("\\{((-?\\d*);)*(-?\\d*)\\}")),
        SETBOOL(Pattern.compile("\\{((-?\\d*\\.\\d*);)*(-?\\d*\\.\\d*)\\}"));

        private final Pattern pattern;

        private Patterns(Pattern p) {
            pattern = p;
        }
    }

    private static boolean matches(Pattern p, String line) {
        return p.matcher(line).matches();
    }

    private Parser() {}

    public static String[] tokenize(String line) {
        line = line.trim();
		String[] tokens = line.split("\\s+", 0);
		
		return tokens;
    }

    public static HashSet<Element> strToSet(String line) {
        HashSet<Element> set = new HashSet<Element>();
        line = line.replaceAll("\\s+","");
        line = line.substring(1, line.length()-1);

        String[] tab = line.split(";");
        for (int i = 0; i < tab.length; i++) {
            Optional<Element> elt = strToElement(tab[i]);
            if (elt.isPresent()) {
                set.add(elt.get());
            }
        }

        return set;
    }

    public static boolean isOneLineInterp(String line) {
        line = line.trim();
        String tokens[] = tokenize(line);
        
        for (String str : tokens){
            if (Operation.isOperator(str)) {
                return true;
            }
        }
        
        return false;
    }

    public static boolean isGetVarInterp(String line) {
        return matches(Patterns.GETVARIABLE.pattern, line);
    }

    public static boolean isSetVarInterp(String line) {
        return matches(Patterns.SETVARIABLE.pattern, line);
    }

    public static boolean containsSetOperators(String line) {
        for(String s : Operation.setOperators) {
            if (line.contains(s)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Parses a line and creates an Element from it.
     * 
     * @param line is the String to be parsed.
     * @return an empty Optional if the type of the element is not accepted and
     * Optional with the corresponding Element if it succeed.
     */
    public static Optional<Element> strToElement(String line) {
        line = line.trim();
        Element elt = null;

        if (matches(Patterns.INT.pattern, line)) {
            elt = ElementFactory.create(Type.INT, Integer.parseInt(line));
        }
        else if (matches(Patterns.DOUBLE.pattern, line)) {
            elt = ElementFactory.create(Type.DOUBLE, Double.parseDouble(line));
        }
        else if (matches(Patterns.BOOL.pattern, line)) {
            elt = ElementFactory.create(Type.BOOLEAN, Boolean.parseBoolean(line));
        }
        else if (matches(Patterns.SETINT.pattern, line) || 
            matches(Patterns.SETDOUBLE.pattern, line) ||
            matches(Patterns.SETBOOL.pattern, line)
        ) {
            elt = ElementFactory.create(Type.SET, strToSet(line));
        }
        else {
            return Optional.empty();
        }

        return Optional.of(elt);
    }

    public static Optional<Element> getHistFromLine(String line, Memory mem) {
        String[] str = line.split("[(]|[)]", 0);

		if (str[0].compareTo("hist") != 0 || str.length > 2 || str.length <= 1) {
            Errors.printError(Errors.UNKNOWN_TYPE, line);
            return Optional.empty();
        }
        
        try {
			int val = Integer.parseInt(str[1]);
			Optional <Element> elt  = mem.getHist(val);

			if(!elt.isPresent()) {
                Errors.printError(Errors.INCORRECT_INDEX, "");
				return Optional.empty();
			}

            return elt;
		}
		catch(NumberFormatException e) {
            Errors.printError(Errors.UNKNOWN_TYPE, str[1]);
            return Optional.empty();
		}
    }

    public static Optional<String> getStackIndexFromLine(String line) {
        String[] str = line.split("[(]|[)]", 0);

		if (str[0].compareTo("pile") != 0 || str.length > 2 || str.length <= 1) {
            Errors.printError(Errors.UNKNOWN_TYPE, line);
            return Optional.empty();
		}
		
		try {
			int val = Integer.parseInt(str[1]);
			Optional<String> elt = RPN.getIndexInStack(val);
			if (!elt.isPresent()) {
                /** error message displayed in getIndexInStack */
                return Optional.empty();
			}

			return elt;
		}
		catch(NumberFormatException e) {
            Errors.printError(Errors.UNKNOWN_TYPE, str[1]);
			return Optional.empty();
		}
    }
}
