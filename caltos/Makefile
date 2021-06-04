all: compile

compile:
	cd calculator && mvn clean package

run:
	java -jar calculator/target/calculator-0.1.jar

doc:
	cd calculator && mvn javadoc:javadoc

clean:
	cd calculator && mvn clean ; rm -r apidocs