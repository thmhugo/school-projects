# Currently working commands

## `ls`

```shell
tsh/ > ls
tsh/ > ls test.tar
tsh/ > ls src
tsh/ > ls test.tar/src src test.tar/src/../src/inc
tsh/ > ls test.tar/src src -l test.tar/src/../src/inc
tsh/test.tar/ > ls ..
tsh/test.tar/ > ls ../test.tar
tsh/test.tar/ > ls -l ../src
tsh/test.tar/ > ls -la ../src
tsh/test.tar/ > ls -l ../src -a
tsh/test.tar/ > ls .
```

## `cat`

```shell
tsh/ > cat
tsh/ > cat README.md
tsh/ > cat README.md Makefile
tsh/ > cat test.tar/src/parser.c
tsh/ > cat test.tar/src/parser.c Makefile
tsh/ > cat . # Is a directory
```

## `pwd`

```shell
tsh/ > pwd
```

## `cd`

```shell
tsh/ > cd src
tsh/ > cd /home/
tsh/ > cd ../tsh/..
tsh/ > cd test.tar
tsh/ > cd test.tar/src/..
tsh/test.tar/ > cd .
```

## `cp`

```shell
tsh/ > cp Makefile test.tar/make
tsh/ > cp file1 file2 fileN test.tar/
tsh/test.tar/ > cp ../file1 ../file2 .
tsh/test.tar/ > cp folder/file1 folder/file2 .
tsh/test.tar/ > cp folder/file1 folder/file2 ../outside_folder
tsh/ > cp -r src srccpy
tsh/ > cp -r test.tar/src srccpy
tsh/ > cp -r src test.tar/srccpy
tsh/test.tar/ > cp src/cd.c cdcpy
tsh/test.tar/ > cp src/cd.c ../cdcpy
tsh/test.tar/ > cp ../TODO.md todocpy
tsh/test.tar/ > cp -r src/inc inccpy
tsh/test.tar/ > cp -r ../src srccpy
tsh/test.tar/ > cp -r src ../srccpy
tsh/ > cp file tar.tar
tsh/ > cp -r folder tar.tar
tsh/test.tar > cp folder new_folder #-r not specified
tsh/test.tar/folder > cp . ../new_folder #-r not specified
tsh/test.tar/folder > cp -r . ../new_folder
tsh/test.tar/folder > cp -r . new_folder # cannot copy a directory, "." into itself.
```

## `rm`

```shell
tsh/test.tar/ > rm file
tsh/test.tar/ > rm file folder/file
tsh/ > rm outside_file tar.tar/inside_folder/inside_file
tsh/test.tar/ > rm -r inside_folder/
tsh/ > rm -r tar.tar/inside_folder
tsh/test.tar/ > rm -r inside_folder/ inside_file
tsh/ > rm -r oustside_folder/ tar.tar/inside_folder
tsh/ > rm # missing operands
tsh/ > rm -r # missing operands
tsh//test.tar/ > rm file_that_does_not_exist # No such file or directory
tsh/test.tar > rm . # Is a directory
tsh/test.tar/folder > rm . # Is a directory
```

## `rmdir`

```shell
tsh/test.tar/ > rmdir inside_folder
tsh/test.tar/ > rmdir inside_folder1 inside_folder2
tsh/ > rmdir not_empty_folder # Directory not empty
tsh/test.tar/ > rmdir ../outside_folder
tsh/ > rmdir tar.tar/../outside_file
tsh/test.tar > rmdir . # Invalid argument
tsh/test.tar/folder > rmdir . # Invalid argument
```

## `mkdir`

```shell
tsh/test.tar/ > mkdir folder
tsh/ > mkdir tar.tar/folder
tsh/test.tar/ > mkdir ../outside_folder
tsh/test.tar/ > mkdir existing_folder # File exists
tsh/test.tar/ > mkdir inside_folder1 inside_folder2
tsh/test.tar/ > mkdir inside_folder1 ../outside_folder
tsh/test.tar/ > mkdir . # File exists
tsh/test.tar/ > mkdir ./new_folder
tsh/test.tar/folder/ > mkdir .././new_folder
```

## `mv`

```shell
tsh/ > mv outside_file test.tar/file
tsh/test.tar/ > mv file new_file
tsh/test.tar/ > mv folder new_folder
tsh/test.tar/ > mv inside_file ../outside_file
tsh/test.tar/ > mv file existing_folder
tsh/test.tar/ > mv file1 file1 existing_folder
tsh/test.tar/ > mv folder ./new_folder
tsh/test.tar/ > mv folder/. new_folder # Not a directory
```

## Redirections

```shell
tsh/ > ls > lsout
tsh/ > cat unknownfile 2> lsout
tsh/ > cat Makefile >> lsout
tsh/ > echo src > lsin
tsh/ > ls < lsin
tsh/ > ls < lsin > lsout
tsh/ > wc -l < Makefile
tsh/ > wc -l < Makefile README.md
tsh/ > ls > test.tar/file
tsh/ > cat < test.tar/file
tsh/test.tar/ > ls > file
tsh/test.tar/ > ls 2> file
tsh/ > ls test.tar/non_existing_dir 2>> err
tst/test.tar/ > ls -l folder >> ../outside_folder 
```

## Pipes

```shell
tsh/ > ls -l | cat | grep .md | wc -l
tsh/ > ls -l src unknownfile 2> lserr | cat
tsh/ > ls -l src | cat < Makefile
tsh/test.tar > ls -l src | wc -l
```
