.PHONY: all clean doc

CC = gcc
FLAGS = -Wall

BUILD_DIR = build
SRC_DIR = src
INC_DIR = inc
TSH_EXEC = tsh

SRCS = $(wildcard src/*.c)

TEST_RUNNER = $(BUILD_DIR)/test_runner
TEST_RUNNER_SRC = test/test_runner.c
TEST_COMMONS = test/test_commons_aux.c
TEST_RESOURCES_BCKP_DIR = test/resources/backup-test-tar-files
TEST_RESOURCES_CURR_DIR = test/resources/current-test-tar-files

DOXYGEN_CONFIG = Doxyfile

define print_header
	@echo "\n\033[0m-----------------------------------------------------------------------------"
	@echo "---- \033[1m\033[32m" $(1) "\033[0m\033[22m"
	@echo "-----------------------------------------------------------------------------\033[29m\n"
endef

all: create_dirs tsh compile_test copy_test_resources run_test clean_test_resources

create_dirs:
	$(call print_header, C O M P I L E)
	mkdir -p $(BUILD_DIR)

tsh: $(SRCS)
	$(CC) $(FLAGS) -o $(BUILD_DIR)/$@ $^

compile_test: $(TEST_RUNNER_SRC) $(filter-out src/tsh.c, $(SRCS)) $(wildcard test/*aux.c) $(TEST_COMMONS)
	$(CC) $^ -I . -o $(TEST_RUNNER)

copy_test_resources:
	if [ -d $(TEST_RESOURCES_CURR_DIR) ]; then rm -r $(TEST_RESOURCES_CURR_DIR); fi
	cp -r $(TEST_RESOURCES_BCKP_DIR) $(TEST_RESOURCES_CURR_DIR)

run_test:
	$(call print_header, T E S T)
	./$(TEST_RUNNER)

doc:
	doxygen $(DOXYGEN_CONFIG)

clean_test_resources:
	rm -r $(TEST_RESOURCES_CURR_DIR)

clean:
	rm -r $(BUILD_DIR)
