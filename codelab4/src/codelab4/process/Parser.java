package codelab4.process;

import java.util.ArrayList;
import java.util.Stack;
import java.util.StringTokenizer;

import codelab4.process.exceptions.InstructionCountException;
import codelab4.process.exceptions.SyntaxException;

public class Parser {

	private static final String TAB = "	";
	private static final String END_WHILE = "do";
	private static final String END_IF = "then";

	protected Stack<Token> loopStack = new Stack<Token>();
	protected ArrayList<Token> array = new ArrayList<>();

	private StringTokenizer tokenizer ;
	private int currentLineNumber = 0;
	private int depth = 0;
	private boolean inLoop = false;

	public ArrayList<Token> tokenize(
		final String str, final int expectedTokenCount
	) throws SyntaxException, InstructionCountException {
		tokenizer = new StringTokenizer(str, "\n");

		checkCount(tokenizer.countTokens(), expectedTokenCount);

        while (tokenizer.hasMoreTokens()) {
			final String next = tokenizer.nextToken();
			final String line = lineTreatment(next);

			this.currentLineNumber++;
			switch (line) {
				case "avancer()" :
						array.add(Token.AVANCER);
					break;
				case "ramasser()" :
						array.add(Token.RAMASSER);
					break;
				case "tourner_a_droite()" :
						array.add(Token.TOURNER_DROITE);
					break;
				case "tourner_a_gauche()" :
						array.add(Token.TOURNER_GAUCHE);
					break;
				case "jeter_fruit()" :
						array.add(Token.JETER_FRUIT);
					break;
				case "couper()" :
						array.add(Token.COUPER);
					break;
				case "manger_fruit()" :
						array.add(Token.MANGER);
					break;
				case "else" :
						array.add(Token.ELSE);
						indent();
					break;
				default :
					if ( ! this.inLoop )
						throw new SyntaxException(this.currentLineNumber,line);
					break;
			}
			this.inLoop = false;
		}
		addEndTokens();

        return array;
	}

	private void checkCount(final int count, final int expected)
	throws InstructionCountException {
		if (count > expected && expected >=0)
			throw new InstructionCountException(count, expected);
	}

	protected String lineTreatment(final String str) throws SyntaxException {
		final int tabs = countTabs(str);

		if ( tabs < this.depth && !str.contains("else") )
			for (int i = 0; i < this.depth - tabs; i++)
				array.add(loopStack.pop());

		this.depth = tabs;

		final StringTokenizer stringTokenizer = new StringTokenizer(str, " ");

		if ( stringTokenizer.countTokens() == 1 )
			return str.trim();

		else {
			statementTreatment(str.trim());
			return "";
		}
	}

	private static int countTabs(final String str) {
		int nbTab = 0;
		int i = 0;

		while (String.valueOf((str.charAt(i++))).equals(TAB))
			nbTab++;

		return nbTab;
	}

	private void statementTreatment(final String line) throws SyntaxException {
		final StringTokenizer stringTokenizer = new StringTokenizer(line, " ");
		final String statement = stringTokenizer.nextToken();

		switch (statement) {
			case "if" :
					this.inLoop = true;
					addIfToArray(line);
				break;
			case "while" :
					this.inLoop = true;
					addWhileToArray(line);
				break;
			default :
				throw new SyntaxException(currentLineNumber, statement);
		}
	}

	private void addIfToArray(final String next) throws SyntaxException {
		array.add(Token.IF);
		array.add(parseLoopCondition(next));
		loopStack.push(Token.ENDIF);
	}

	public void addWhileToArray(final String next) throws SyntaxException {
		array.add(Token.WHILE);
		array.add(parseLoopCondition(next));
		loopStack.push(Token.ENDWHILE);
	}

    public Token parseLoopCondition(final String str) throws SyntaxException {
		final ArrayList<String> parsedString = getParsedStatement(str);

		switchOnStatement(parsedString);

		return getTokenizedCondition(parsedString);
	}

	private Token getTokenizedCondition(final ArrayList<String> parsedString)
	throws SyntaxException {
		final String condition = parsedString.get(1);

		switch (condition) {
			case "chemin_est_libre()": return Token.EST_LIBRE;
			case "fruit_est_pourri()": return Token.EST_POURRI;
			default:  throw new SyntaxException(currentLineNumber,condition);
		}
	}

	private void switchOnStatement(final ArrayList<String> parsedString)
	throws SyntaxException {
		final String statement = parsedString.get(0);

		switch (statement) {
			case "if" :
					if ( !parsedString.get(2).equals(END_IF) )
						throw new SyntaxException(currentLineNumber,statement);
				break;
			case "while" :
					if ( !parsedString.get(2).equals(END_WHILE) )
						throw new SyntaxException(currentLineNumber,statement);
				break;
			default :
				throw new SyntaxException(currentLineNumber,statement);
		}
	}

	private ArrayList<String> getParsedStatement(final String str)
	throws SyntaxException {
		final StringTokenizer conditionParser = new StringTokenizer(str," ");
		final ArrayList<String> parsedString = new ArrayList<>();

		if (conditionParser.countTokens() != 3)
			throw new SyntaxException(currentLineNumber,str);

		while (conditionParser.hasMoreTokens())
			parsedString.add(conditionParser.nextToken());

		return parsedString;
	}

	public void addEndTokens(){
		while ( !loopStack.empty() )
			array.add(loopStack.pop());
	}

	public Parser() { }

	public int getCurrentLineNumber(){
		return this.currentLineNumber;
	}

	public void indent(){
		this.depth++;
	}

	public void unindent(){
		this.depth--;
	}
}
