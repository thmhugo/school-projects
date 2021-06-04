# Review of the Maljae's architecture 

The repository is composed of 4 branches :\
	`develop`\
	`document-usage`\
	`algo`\
	`master`


## The develop branch

This branch contains 3 directories :\
	`assignment`\
	`datamodel`\
	`webserver`

They are composed of one `src` directory divided itself into 2 directories (`main` and `test`) 
and one configuration file for Maven : `pom.xml`. (pom means project object model).

`develop` also contains a Makefile which compile the webserver and the command-line tool.	

### assignment/

#### The main directory

It contains one classe :\
	`Assign` : manages tasks assignement.

#### The test directory

It contains one classe:\
	`AssignTest` : unit test for task assignement.

### datamodel/

It contains source files needed to compile the command-line tool `maljae-assign`.

#### The main directory

It contains 6 classes / modules :\
	`Assignment` : stores tasks assignment (only one task for each team).\
	`Configuration` : manages configuration of the maljae instance from data files.\
	`Student.java` : represent student informations. Stores emails and manages Json objects.\
	`Task` : represents task description. (id, title, url, descritption)\
	`Team` : represents a team of students. (id, preferences, students, secret)\
	`Teams` : manages teams.

#### The test directory

`ConfigurationTest` is a unit test for simple App. It verify :\
	- if the configuration date is in the given range.\
	- if there is 3 or 4 member in each team.\
	- if the default number of project is 3.

All the data needed to implement a test can be found in the directory `resources`.

### webserver/

`webserver` contains source files needed to compile the webserver `maljae-webserver`.

#### The main directory

It's contains 4 classes / modules :\
	`App` : lunch a local server.\
	`Notifier` : notifies when a team is created and allows to send emails.\
	`TeamController` : API for team management. (Javalin)\
	`Token` : manages tokens.

In `ressources` we find HTML files :\
	`edit-team.ftl` : allows to edit members and preferences of a particular team.\
	`index.html` : home page.\
	`team-creation-done.html` : register validation page.\
	`team-update-done` : team update validation page.

#### The test directory 

`AppTest` is a unit test for webserver.

