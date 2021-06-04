# NetRadio ![build][master-build]

A broadcasting service of text messages written in C and Java.

Project for the course *Programmation Réseau*.

## Contents

<!-- vim-markdown-toc GFM -->

* [Documentation](#documentation)
* [How to..](#how-to)
  * [compile](#compile)
  * [run tests](#run-tests)
  * [execute](#execute)
    * [A manager](#a-manager)
    * [A multicaster](#a-multicaster)
    * [A client](#a-client)
* [Extensions](#extensions)
  * [Manager can propagate `MESS` messages](#manager-can-propagate-mess-messages)
  * [Client test if a multicaster is alive before listening to it](#client-test-if-a-multicaster-is-alive-before-listening-to-it)
  * [Multicasters can cast the weather](#multicasters-can-cast-the-weather)
* [Architecture](#architecture)

<!-- vim-markdown-toc -->

## Documentation

The project documentation could be find in [./doc](https://gitlab.com/EmileRolley/netradio/-/tree/master/doc)
and contains the following files:

* The project [subject](https://gitlab.com/EmileRolley/netradio/-/blob/master/doc/pdf/subject.pdf).
* The [specifications](https://gitlab.com/EmileRolley/netradio/-/blob/master/doc/pdf/specifications.pdf).
It's summarize and describe in detail the internet protocol used in the project.
* The [Sequence diagrams](https://gitlab.com/EmileRolley/netradio/-/blob/master/doc/md/sequence-diagram.md) of a:
  * [manager](https://gitlab.com/EmileRolley/netradio/-/blob/master/doc/pdf/manager-seqdia.pdf)
  * [multicaster](https://gitlab.com/EmileRolley/netradio/-/blob/master/doc/pdf/multicaster-seqdia.pdf)
  * [client](https://gitlab.com/EmileRolley/netradio/-/blob/master/doc/pdf/client-seqdia.pdf)

> All the documentation can also be found in the [wiki](https://gitlab.com/EmileRolley/netradio/-/wikis/home).

## How to..

### compile

In order to compile the project run:

```
make
```

All binaries and `.class` will be stored in `./build/`.

### run tests

In order to run the unit test suite, run:

```
make tests
```

> Tests aren't exhaustive at all and are only here for the C library.

### execute

#### A manager

A manager could be launched with the following command:

```shell
> ./build/manager comport nb_maxmulticasters
```

Where:

* `comport` is the manager's communication port. (`0 <= comport <= 9999`)
* `nb_maxmulticasters` is the maximum number of multicasters handleable by the manager.
 (`0 <= nb_maxmulticasters <= 99`)

#### A multicaster

A multicaster could be launched with the following command:

```shell
> ./multicast id comport castaddr:castport manageraddr:managerport messlist [--quiet]
```

Where:

* `id` is the multicaster's id, (up to 8 characters).
* `comport` is the multicaster's communication port. (`0 <= comport <= 9999`)
* `castaddr` is the multicaster's IPv4 multicast address.
* `castport` is the multicaster's multicast port. (`0 <= castport <= 9999`)
* `manageraddr` is the manager's IPv4 address to register with.
* `messlist` is the path to the text file containing the multicaster's message list.
* `--quiet` turn off logging messages.

> For `messlit`, every first 140 characters of each lines will be consider as one message.

#### A client

A client could be launched with the following command:

```shell
> ./run-client.sh id [--no-color]
```

Where:

* `id` is the client's id, (up to 8 characters).
* `--no-color` to disable colored output.

## Extensions

### Manager can propagate `MESS` messages

As suggested in the subject, we added the possibility to send a `MESS` typed
message to a manager, which will propagate it to all its registered multicasters.

### Client test if a multicaster is alive before listening to it

Before listening to a registered multicaster, the client sends a `RUOK` typed message
to the multicaster.
If no responds, the multicaster alias (see the `mltc` command) is removed.

### Multicasters can cast the weather

As a second extension, we have extended the protocol by allowing to a client to ask
a multicaster to broadcast the current weather for a certain location.

First, the client sends a message \[`RWET`␣`localisation`\], where `localisation`
is a string of 50 characters long (filled with `#` if needed) which corresponds to a city name.
Secondly, the manager use the [wttr](wttr.in/) API to get the weather before storing it
to a file.
Then, the manager responds back messages of the form \[`WETC`␣`content`\] to the client,
where `content` is a string of 140 characters long (filled with `#` if needed)
which corresponds to a block of the file.
Once the entire content of the file is sent, the multicaster sends a last message \[`PEOF`\]
before closing the connection and removing the file.

Following this, the multicaster reuse the [wttr](wttr.in/) API in order to access to
a condensed weather format before adding it to its list of messages to cast,
with `WEATHER` used as id.

> The protocol is detailed in the [specifications](./doc/pdf/specifications.pdf).

## Architecture

We have chosen to implement the multicaster and the manager in `C` because they are
servers and therefore, must be reliable and efficient.
However, for the client which needed to be portable, we have selected `Java`.

By having two servers written in `C`, we could easily factorize the common code
in a static library.
It's stored in the [./src/net/](https://gitlab.com/EmileRolley/netradio/-/tree/master/src/net) folder
and contains all commons functions to manage:

* [messages](https://gitlab.com/EmileRolley/netradio/-/blob/master/src/net/messages.h)
* [internet protocols](https://gitlab.com/EmileRolley/netradio/-/blob/master/src/net/net.h)
* [logging messages](https://gitlab.com/EmileRolley/netradio/-/blob/master/src/net/log.h)
 (we used the library [log.c](https://github.com/rxi/log.c))
* [error messages](https://gitlab.com/EmileRolley/netradio/-/blob/master/src/net/errors.h)

Because, the manager and the multicaster implementations depends on the library, we have
chosen to setup some tests for it.
They can be found in [./test/net/](https://gitlab.com/EmileRolley/netradio/-/tree/master/test/net).

> **Note**: they don't have full coverage, but remains interesting to ensure
> good management of the messages for example.

The manager and the multicaster are respectively implemented in
[./src/manager/manager.c](https://gitlab.com/EmileRolley/netradio/-/blob/master/src/manager/manager.c)
and
[./src/multicaster/multicaster.c](https://gitlab.com/EmileRolley/netradio/-/blob/master/src/multicaster/multicaster.c).

The client's classes are stored in [./src/client/](https://gitlab.com/EmileRolley/netradio/-/tree/master/src/client).

[master-build]: https://img.shields.io/gitlab/pipeline/EmileRolley/netradio?label=build&style=flat-square
