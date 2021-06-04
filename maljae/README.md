# maljae, a project assignment system

## What is the problem solved by maljae?

Imagine that you have a fixed number of teachers to supervise the
programming projects of a bunch of students. Students are regrouped
into small teams and each teacher can supervise a limited number of
teams. In addition, imagine that each teacher only supervises a limited
number of programming tasks, and that the students sometimes prefer some
tasks over others. How to assign teams to each teacher to optimize the
students happiness? That's what `maljae` solves.

## How does maljae work?

The assignment process of `maljae` works in three steps:

1. The team creation: During this first phase, the students must create
   their teams and express which programming tasks they would like to
   get by ordering all the tasks with respect to their preferences.
   There is a deadline for this step. Notice that teams cannot exceed
   a given number of members. They must also contain a minimum number
   of members.

2. The assignment: After the deadline, the students choices are
   processed by an algorithm which decides the task assignment.

3. The reporting: The execution trace of the assignment algorithm
   is described in a report, so that each participant can check that
   its execution has been flawless and conform to its specification.

## What is the assignment algorithm used by maljae?

While giving their task preferences, the teams are also asked to give
a secret integer. After the deadline, a seed is attributed to each team: it
is the sum of the secret integers given by all the other teams.

The assignment algorithm is made of the following steps:

1. The complete teams are ordered according to the smallest email address of
   their members (alphabetical order, characters being ordered with
   ascii order), say from `1` to `k`. The seed of the team `i` is denoted by `n_i`.
   
2. For each team `i` (in the previous order), consider the
   transposition `t_i` that exchanges `i` and `i +
   (n_i%(k-i-1))`. Consider then the permutation `t = t_k o t_{k-1} o
   ... o t_1`.
   
3. The order of the teams is given by the image of `(1,2,...,k)`
   by `t`.
   
4. Consider the teams in the order given in step 3. For each team, the
   assigned task is the first free one in its preference list of tasks.

5. The same steps are followed to order the teams that lack one
   member, and the obtained list is placed after the ordered list of
   complete teams. Then again for the teams that lack two members, etc.

## How maljae is supposed to be used?

maljae is made of two main components:
1. a webserver to collect the team information and to visualize task assignment.
2. a command-line tool to implement the task assignment algorithm.

## How to build maljae ?

First, you need to compile it by running :
```
make
``` 
Then, you need to fill the configuration file : `config.json`
> (An example of configuration file can be found in `datamodel/src/test/resources/config.json`.)

### Webserver

`maljae` server can be run with the following command:

```
make run-server
```

### Command-line tool

When the deadline is reached, the students cannot change their team
descriptions. The files in `data` cannot be modified anymore. They
will be exploited to determine a task assignment.

By running:

```
make run-assign
```

the administrator obtains a file named `assignments.json`. This file
contains the assignments and the execution trace of the
algorithm. Once this file is created, the server can notify the
students of the results using a dedicated web page.

## File formats

The data of `maljae` is stored in human-readable text files. The
administrator can modify these files as long as it respects the format
and the invariants described in this section. Otherwise, there is no
guarantee that `maljae` will work properly. In case of violation of
these expectations, there is always a way to recover a valid state
because the files are stored in a git repository. This git repository
is initialized when the server is launched for the first time.

## Data model

The data model of an instance of `maljae` consists in a single instance of session data record.

### Session data record 

A session data record contains the following data: 

 - a descriptive text
 - the given of a minimal and maximal number of students per team 
 - a default number of slots per task
 - a deadline
 - the schedule of the "Groupes de TD" (so that compatibility of mixed teams can be checked). Alternatively, this could just be a partition of the set of groups. 
 - a list of assignments
 - a list of students 
 - a list of teams
 
 All but the last two items should be set before opening the server to the students and not modified after that.
 
### Assignment

 An assignment consists in a task unique name, a task description (short text) and a number of slots (maximal number of teams who can be assigned to this task). 
  
### Student

 Each student is uniquely identified by its main email address at Paris-Diderot, that is the address having the following form: `firstname.surname@etu.univ-paris-diderot.fr`.
 
 Moreover, each student should input the name of her or his "Groupe de TD".
 
 Finally, each student record also contains the team she or he belongs to, provided the student already conformed being a member of this team. 
 
### Team

A team has a unique identifier (randomly generated at team creation), a list of tasks names in preference order, a list of students (confirmed or not yet confirmed team members) and the secret integer given as an input to the assignment algorithm.
 
A student's membership is confirmed if and only if the team registered for this student is this team.
 
## License

This software is released under the MIT license.
