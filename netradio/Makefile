.PHONY: all

CC = gcc
CFLAGS = -Wall -pthread -g

JC = javac

JCFLAGS = -XDignore.symbol.file=true

SRC = src
TEST = test

LIBNET_NAME = net
LIBCURL_NAME = curl
BUILD_D = build
NET_F = $(SRC)/$(LIBNET_NAME)/*.c
MANAGER_F = $(SRC)/manager/*
MULTICASTER_F = $(SRC)/multicaster/*
ADD_LIB = -L$(BUILD_D) -l$(LIBNET_NAME) -l$(LIBCURL_NAME)

C_TEST_RUNNER_SRC = $(TEST)/test_runner.c
C_TEST_RUNNER = $(BUILD_D)/c_test_runner


all:    create_dirs \
		net.o \
		libnet.a \
		manager \
		multicaster \
		client \
		wipe

# General
create_dirs:
	mkdir -p $(BUILD_D)

wipe:
	rm $(BUILD_D)/*.a $(BUILD_D)/*.o

clean:
	rm -r $(BUILD_D)

# C
manager:
	$(CC) $(CFLAGS) $(MANAGER_F) $(ADD_LIB) -o $(BUILD_D)/manager

multicaster:
	$(CC) $(CFLAGS) $(MULTICASTER_F) $(ADD_LIB) -o $(BUILD_D)/multicaster

net.o: $(SRC)/$(LIBNET_NAME)/$(LIBNET_NAME).c
	$(CC) -O -c $^ -o $(BUILD_D)/$(LIBNET_NAME).o

$(BUILD_D)/errors.o: $(SRC)/$(LIBNET_NAME)/errors.c
	$(CC) -O -c $^ -o $(BUILD_D)/errors.o

$(BUILD_D)/log.o: $(SRC)/$(LIBNET_NAME)/log.c
	$(CC) -O -c $^ -o $(BUILD_D)/log.o -DLOG_USE_COLOR

$(BUILD_D)/messages.o: $(SRC)/$(LIBNET_NAME)/messages.c
	$(CC) -O -c $^ -o $(BUILD_D)/messages.o

libnet.a: $(BUILD_D)/messages.o $(BUILD_D)/errors.o $(BUILD_D)/log.o $(BUILD_D)/$(LIBNET_NAME).o
	ar rcs $(BUILD_D)/$@ $^

# Java
client: $(BUILD_D)
	$(JC) -cp $^ -d $^ $(SRC)/client/*.java $(JCFLAGS)

#Tests
tests:  create_dirs \
		c-compile-tests \
		c-run-tests

c-compile-tests:
	$(CC) $(C_TEST_RUNNER_SRC) $(NET_F) -I . -o $(C_TEST_RUNNER)

c-run-tests:
	./$(C_TEST_RUNNER)
