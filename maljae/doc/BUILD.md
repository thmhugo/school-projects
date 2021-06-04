# How to build maljae?

## Requirements

- java JDK (>= 8.0)
- junit    (>= 4.11)
- maven    (>= 3.6)
- javalin  (>= 3.5)

## Instructions

To `build` the project, just type :
```
$ make
```

To run the `run` the server, just type :
```
$ make run-server
```

To run the `run` the command-line tool, just type :
```
$ make run-assign
```

To `clean` generated files type :
```
$ make clean
```

To generate the `documentation files` type :
```
$ make doc 
```
Documentation files (HTML) could be found in : *<directory_name>/target/site/apidocs/*
> (where <directory_name> refers to : assignment, datamodel, webserver)