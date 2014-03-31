INCLUDES=-Isrc -Itest
OPTFLAGS=-Ofast -flto

CFLAGS=$(OPTFLAGS) -Wall -I/usr/local/opt/ncurses/include #-DDEBUG -Wextra -pedantic #-Weverything
LDFLAGS=$(OPTFLAGS) -lpthread -lncurses -L/usr/local/opt/ncurses/lib

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
	$(CXX) $(OBJ) -o bin/connect4 $(LDFLAGS)

bin/test: setup $(OBJ) $(TEST_OBJ)
	$(CXX) $(TEST_OBJ) -o bin/test $(LDFLAGS)

build/%.o: src/%.cpp
	$(CXX) -c $(INCLUDES) -o $@ $< $(CFLAGS)

build/%.o: test/%.cpp
	$(CXX) -c $(INCLUDES) -o $@ $< $(CFLAGS)

clean:
	@rm -rf bin/*
	@rm -rf build/*
