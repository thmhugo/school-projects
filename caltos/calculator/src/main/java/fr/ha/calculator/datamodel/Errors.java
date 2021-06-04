package fr.ha.calculator.datamodel;

public enum Errors {
    MISSING_OPERAND         ("Missing operands..."),
    WRONG_MODE              ("Wrong mode, should be RPN!"),
    UNKNOWN_TYPE            ("Unknown type "),
    INCORRECT_INDEX         ("The given index is incorrect."),
    EMPTY_STACK             ("The stack might be empty"),
    UNINITIALIZED_VAR       ("This variable was not initialized : "),
    NO_CALCULATION          ("No previous calculation detected !"),
    READING_ERR             ("An error occurred while reading"),
    NOT_ENOUGH_OPERANDS     ("Not enough operands"),
    NOT_AVAILABLE_IN        ("This is not available in "),
    NOT_SUPPORTED_OP        ("Not supported operation with the given type(s)."),
    WRONG_FORMAT            ("Wrong variable format : "),
    WRONG_FORMAT2           ("Wrong format !"),
    INCOMPATIBLE_OPERANDS   ("Operands are not compatible : "),
    WRONG_NAME              ("Wrong variable name"),
    INCOMPATIBLE_TYPES      ("The types are incompatible !"),
    INFIX_RPN_MISSING       ("Infix to RPN: '(' was not found in the formula."),
    INFIX_RPN_ILLEGAL       ("Infix to RPN: Illegal '(' found."),
    STR_EMPTY               ("The input is empty, please try again"),
    UNKNOWN_CMD             ("Unknown command.");


    private final String message;
    private static final String RED = "\u001B[31m";
    private static final String RESET = "\u001B[0m";

    Errors(String message) {
        this.message = message;
    }

    private String getMessage() {
        return this.message;
    }

    public static void printError(Errors e , String after) {
        System.out.println(RED + e.getMessage() + after + RESET);
    }
}
