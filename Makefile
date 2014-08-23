INCLUDES=-Isrc -Itest

CFLAGS=$(OPTFLAGS) -Wall -I/usr/local/opt/ncurses/include -Wextra -pedantic #-Weverything
LDFLAGS=$(OPTFLAGS) -lpthread -lncurses -L/usr/local/opt/ncurses/lib

SRC=$(wildcard src/*.cpp)
OBJ=$(patsubst src/%.cpp,build/%.o,$(SRC))

TEST_SRC=$(wildcard test/*.cpp)
TEST_OBJ=$(patsubst test/%.cpp,build/%.o,$(TEST_SRC))
TEST_OBJ+=$(OBJ)
TEST_OBJ:=$(filter-out build/main.o, $(TEST_OBJ))

BENCH_SRC=$(wildcard bench/*.cpp)
BENCH_OBJ=$(patsubst bench/%.cpp,build/%.o,$(BENCH_SRC))
BENCH_OBJ+=$(OBJ)
BENCH_OBJ:=$(filter-out build/main.o, $(BENCH_OBJ))

.PHONY: setup

ifdef DEBUG
	$(info 1)
	OPTFLAGS=-O1 -g
	CFLAGS+=-DDEBUG
else
	$(info 2)
	OPTFLAGS=-O3 -ftree-vectorize -msse2 -funroll-loops
endif

all: bin/connect4 bin/test bin/bench

setup:
	@mkdir -p build
	@mkdir -p bin

bin/connect4: setup $(OBJ)
	$(CXX) $(OBJ) -o bin/connect4 $(LDFLAGS)

bin/test: setup $(TEST_OBJ)
	$(CXX) $(TEST_OBJ) -o bin/test $(LDFLAGS)

bin/bench: setup $(BENCH_OBJ)
	$(CXX) $(BENCH_OBJ) -o bin/bench $(LDFLAGS)

build/%.o: src/%.cpp
	$(CXX) -c $(INCLUDES) -o $@ $< $(CFLAGS)

build/%.o: test/%.cpp
	$(CXX) -c $(INCLUDES) -o $@ $< $(CFLAGS)

build/%.o: bench/%.cpp
	$(CXX) -c $(INCLUDES) -o $@ $< $(CFLAGS)

clean:
	@rm -rf bin
	@rm -rf build
