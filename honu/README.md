# Honu 🐢 ![ci][ci] ![tag][tag]

An interpreted programming language and tool chain for turtle graphics.

![Example](/uploads/6be540f55fc47f6eb0f7b45e5b44ed48/Screenshot_from_2021-05-09_17-32-27.png)

## Contents

<!-- vim-markdown-toc GFM -->

* [Documentation](#documentation)
* [Dependencies](#dependencies)
* [How to..](#how-to)
  * [.. compile](#-compile)
  * [.. run tests](#-run-tests)
  * [.. use](#-use)
    * [The interactive mode](#the-interactive-mode)
    * [Compilation target](#compilation-target)
      * [Compilation script](#compilation-script)
    * [Compile documentation of a `Honu` file](#compile-documentation-of-a-honu-file)
  * [.. setup syntax highlighting](#-setup-syntax-highlighting)
    * [for `VS Code`](#for-vs-code)
    * [for `vim`](#for-vim)

<!-- vim-markdown-toc -->

## Documentation

The *./doc* folder contains:

* The [project report](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/blob/master/doc/report.pdf)
* The [subject](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/blob/master/doc/subject.pdf)
* A [preview](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/blob/master/doc/previews.pdf)
of what can be produced with `Honu` programs.

## Dependencies

The interpreter is written in `OCaml` and use several external tools and libraries:

* `ANSITerminal`
* `bimage` and `bimage-unix` (= `0.2.1`)
* `core`
* `dune`
* `graphics`
* `menhir`
* `ocamllex`
* `ounit2`
* `lambda-term`

> They all could be installed with `opam install`.

## How to..

### .. compile

The interpreter could be **compiled** with:

```
make
```

### .. run tests

In order to execute unit tests:

```
make tests
```

### .. use

And could be executed with:

```shell
> ./honu help
Honu, a programming language and toolchain for turtle graphics.

  honu SUBCOMMAND

=== subcommands ===

  compile  Compile the specified Honu file.
  doc      Compile the documentation.
  repl     Launch the interactive mode.
  show     Render the result of the interpreted Honu file.
  version  print version information
  help     explain a given subcommand (perhaps recursively)
```

`honu` flags :

```shell
  [--verbose]     Enable verbose mode.
  [--no-color]    Disable colored output.
  [--no-graphic]  Disable the graphic window.
  [--center]      Use a centered starting position.
```

#### The interactive mode

In the Honu's REPL, you can:

* Exit with `Ctrl-D`.
* Navigate in the history with ⇧ and ⇩.
* Navigate in the input with ⇦ and ⇨.
* Apply autocompletion with `Tab`.
* Do reverse search with `Ctrl-R`.

#### Compilation target

Available compilation targets for the `compile` subcommand are:

* `.ml`
* `.jpg`
* `.png` (default)

##### Compilation script

In order to compile produced `.ml` file, a script is provided:

```shell
./compgraph FILENAME_WITHOUT_ANY_EXTENSION
```

The argument is the path to the targeted OCaml file without any extension.

> For example, `./compgraph out` will compile `out.ml` into `out.exe`.

#### Compile documentation of a `Honu` file

The documentation of a `Honu` file could be compiled into a `Markdown` file
with the subcommand:

```shell
./honu doc filein fileout
```

Only variables and procedures comment lines starting with `~|` are compilable,
with the following tags:

* `note`, will be inserted after a `>` in the `Markdown` file.
* `todo`, will add an item to the TODO list.
* `fixme`, will add an item (in bold) to the FIXME list.
* `param`, will add a description to a specified identifiant.

> The grammar and example could be found in the [report](https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/blob/master/doc/report.pdf).

### .. setup syntax highlighting

#### for `VS Code`

In order to add syntax highlighting use:

```shell
make vscode
```

#### for `vim`

You need to copy the `./syntax-highlight/vim/honu.vim` syntax file into
`$(VIMCONFIG)/syntax` folder.

> Use `.hn` file extension and refresh the window after installation.

[tag]: https://img.shields.io/badge/tag-v2.0-blue
[ci]: https://gaufre.informatique.univ-paris-diderot.fr/EmileRolley/gasp/badges/master/pipeline.svg
