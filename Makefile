INCLUDES=-Isrc -Itest
OPTFLAGS=-O3

CFLAGS=$(OPTFLAGS) -Wall
CFLAGS_DEV=$(CFLAGS) -DDEBUG -Wextra -pedantic #-Weverything
LDFLAGS=$(OPTFLAGS)

SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp,build/%.o,$(SRC))

TEST_SRC=$(wildcard test/*.cpp)
TEST_OBJ=$(patsubst test/%.cpp,build/%.o,$(TEST_SRC))
TEST_OBJ+=$(OBJ)
TEST_OBJ:=$(filter-out build/main.o, $(TEST_OBJ))

.PHONY: setup

all: bin/connect4 bin/test

setup:
	@mkdir -p build
	@mkdir -p bin

bin/connect4: setup $(OBJ)
	$(CXX) $(LDFLAGS) $(OBJ) -o bin/connect4

bin/test: setup $(OBJ) $(TEST_OBJ)
	$(CXX) $(LDFLAGS) $(TEST_OBJ) -o bin/test

build/%.o: src/%.cpp
	$(CXX) -c $(INCLUDES) -o $@ $< $(CFLAGS)

build/%.o: test/%.cpp
	$(CXX) -c $(INCLUDES) -o $@ $< $(CFLAGS)

clean:
	rm -rf bin/*
	rm -rf build/*
