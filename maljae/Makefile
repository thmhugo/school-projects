CONFIG = ./config.json

.PHONY: all datamodel webserver assignment clean

all: datamodel webserver assignment

datamodel:
	cd datamodel/src/test/resources && \
	sed -i 's@"<maljae_path>"@"$(shell readlink -f datamodel/src/test/resources/data-students)"@g' config-students.json
	cd datamodel && mvn package install

webserver:
	cd webserver/src/test/resources && \
	sed -i 's@"<maljae_path>"@"$(shell readlink -f webserver/src/test/resources/data-tests)"@g' config-test.json
	cd webserver && mvn compile test assembly:single
	cp webserver/target/webserver-1.0-jar-with-dependencies.jar \
	   maljae-webserver.jar

assignment:
	cd assignment/src/test/resources && \
	sed -i 's@"<maljae_path>"@"$(shell readlink -f assignment/src/test/resources/data-test1)"@g' config1.json
	cd assignment/src/test/resources && \
	sed -i 's@"<maljae_path>"@"$(shell readlink -f assignment/src/test/resources/data-test2)"@g' config2.json
	cd assignment/src/test/resources && \
	sed -i 's@"<maljae_path>"@"$(shell readlink -f assignment/src/test/resources/data-test3)"@g' config3.json
	cd assignment && mvn compile test assembly:single
	cp assignment/target/assignment-1.0-jar-with-dependencies.jar \
	   maljae-assign.jar

run-server:
	java -jar maljae-webserver.jar $(CONFIG)

run-assign:
	java -jar maljae-assign.jar $(CONFIG)

clean:
	cd datamodel  && mvn clean
	cd assignment && mvn clean
	cd webserver  && mvn clean

clean-data:	
	rm -rf maljae-data

generate-doc:
	cd datamodel  && mvn site 
	cd assignment && mvn site 
	cd webserver  && mvn site 
