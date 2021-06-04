EXEC=honu
SRC=src
BUILD=$(SRC)/_build/default

.PHONY=all clean
all: link_exe

compile:
	cd $(SRC) && dune build

link_exe: compile
	ln -sf $(BUILD)/honu.exe $(EXEC)

tests:
	cd $(SRC) && dune runtest -f

vscode: ${CURDIR}/syntax-highlight/setup_vscode.sh
	chmod +x $<
	$<

clean:
	cd $(SRC) && dune clean
	rm $(EXEC)
