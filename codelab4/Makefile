## JavaFX ##

JAVAFX_PATH =  ${PATH_TO_FX}
JAVAFX_MODULES = javafx.controls,javafx.fxml,javafx.graphics,javafx.swing

## JSON ##

JSON = lib/org.json.jar

## COMMONS-IO ##

COMMONS = lib/commons-io-2.0.jar

## JUnit 4 ##

JUNIT = lib/junit-4.13.jar
HAMCREST = lib/hamcrest-core-1.3.jar

TEST_SRC = test/**/*.java test/**/**/*.java
TEST_RUNNER = codelab4.TestsRunner
TEST_RESOURCES = test/resources

## Java sources ##

SRCS = src/**/*.java src/**/**/*.java src/**/**/**/*.java

## Compilation targets ##

BUILD = build
CLASSES = $(BUILD)/classes
JAR = $(BUILD)/codelab4.jar

## Main class ##

MAIN = codelab4.App

define print_header
	@echo "-----------------------------------------------------------------------------"
	@echo "---- "$(1)
	@echo "-----------------------------------------------------------------------------"
endef

all: compile verify package

tmp:

compile:
	$(call print_header, C O M P I L E)
	javac --module-path $(JAVAFX_PATH) \
		--add-modules $(JAVAFX_MODULES) \
		-cp $(JSON):$(COMMONS) \
		$(SRCS) -d $(CLASSES)

verify: compile
	$(call print_header, T E S T)
	javac -cp $(CLASSES):$(JUNIT):$(HAMCREST):$(JSON):$(COMMONS) $(TEST_SRC) -d $(CLASSES)
	java -cp $(CLASSES):$(JUNIT):$(HAMCREST):$(JSON):$(COMMONS):$(TEST_RESOURCES) $(TEST_RUNNER)

package: compile
	$(call print_header, P A C K A G E)
	jar --create --file $(JAR) \
		--main-class $(MAIN) \
		-C $(CLASSES) . src/resources

run:
	$(call print_header, R U N N I N G)
	java --module-path $(JAVAFX_PATH):$(JSON):$(COMMONS) \
		--add-modules $(JAVAFX_MODULES),org.json,commons.io \
		-jar $(JAR)

clean:
	rm -r $(BUILD)
