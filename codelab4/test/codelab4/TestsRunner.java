package codelab4;

import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import java.lang.Error;

import codelab4.datamodel.*;
import codelab4.filemanager.FileManagerTest;
import codelab4.process.*;
import codelab4.vizualisation.*;
import codelab4.commons.*;

public class TestsRunner {

	static String RED = "";
	static String GREEN = "";
	static String NORMAL = "";

	public static void main(String[] args) {
		Result result = JUnitCore.runClasses(
			SessionTest.class,
			SessionsTest.class,
			QuestTest.class,
			ChapterTest.class,
			ChapterDoAtEndTest.class,
			ChaptersTest.class,
			MapStateTest.class,
			FileManagerTest.class,
			InterpreterTest.class,
			ParserTest.class,
			ToolsTest.class
		);

		System.out.println("\n==================================================\n");
		printResult(result);
		if ( !result.wasSuccessful() ) {
			printFailures(result);
			printSummary(result);
			throw new Error();
		}
		System.out.println("\n");
		System.out.println("==================================================\n");
	}

	private static void printResult(Result result) {
		String succed = result.wasSuccessful() ? GREEN + "TRUE" : RED + "FALSE";
	
		printHeader(result);
		System.out.println("\n  Tests succed : " + succed + NORMAL);
	}

	private static void printHeader(Result result) {
		printSummary(result);
	}

	private static void printSummary(Result result) {
		System.out.println( 
			"\033[0m\nTests run : " + result.getRunCount()
			+ ", Failures : " + result.getFailureCount()
			+ ", Skipped : " + result.getIgnoreCount()
		);
	}

	private static void printFailures(Result result) {
		System.out.println( "\n  Failed tests : \n");

		for (Failure failure : result.getFailures()) {
			System.err.println(
				"    " + RED + failure.toString() + "\033[00m"
			);
		}
		System.out.println("\n");
	}

}
