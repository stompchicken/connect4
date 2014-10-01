INCLUDES=-Isrc

ifdef DEBUG
OPTFLAGS=-g
CFLAGS=-DDEBUG
else
OPTFLAGS=-O3 -ftree-vectorize -march=native -funroll-loops
CFLAGS=-DNDEBUG
endif


CFLAGS+=-Wno-variadic-macros -Wno-long-long

CFLAGS+=$(OPTFLAGS) -Wall -I/usr/local/opt/ncurses/include -Wextra -pedantic #-Weverything
LDFLAGS=$(OPTFLAGS) -lpthread -lncurses -L/usr/local/opt/ncurses/lib

CONNECT4_SRC=$(wildcard src/*.cpp)
CONNECT4_OBJ=$(patsubst src/%.cpp, build/%.o, $(CONNECT4_SRC))
CONNECT4_OBJ:=$(filter-out build/main.o, $(CONNECT4_OBJ))

UNITTEST_SRC=$(wildcard src/unit_test/*.cpp)
UNITTEST_OBJ=$(patsubst src/unit_test/%.cpp, build/unit_test/%.o, $(UNITTEST_SRC))
UNITTEST_OBJ:=$(filter-out build/unit_test/main.o, $(UNITTEST_OBJ))

PROPTEST_SRC=$(wildcard src/prop_test/*.cpp)
PROPTEST_OBJ=$(patsubst src/prop_test/%.cpp, build/prop_test/%.o, $(PROPTEST_SRC))
PROPTEST_OBJ:=$(filter-out build/prop_test/main.o, $(PROPTEST_OBJ))

BENCH_SRC=$(wildcard src/bench/*.cpp)
BENCH_OBJ=$(patsubst src/bench/%.cpp, build/bench/%.o, $(BENCH_SRC))
BENCH_OBJ:=$(filter-out build/bench/main.o, $(BENCH_OBJ))

.PHONY: all
.DEFAULT: bin/connect4

all: bin/connect4 bin/unit_test bin/prop_test bin/bench

src/%.cpp: src/%.hpp

build/%.o: src/%.cpp
	@mkdir -p "$(@D)"
	$(CXX) -c $(INCLUDES) $(CFLAGS) -o $@ $< 

bin/connect4: $(CONNECT4_OBJ) build/main.o
	@mkdir -p "$(@D)"
	$(CXX) $(CONNECT4_OBJ) build/main.o -o bin/connect4 $(LDFLAGS)

bin/unit_test: $(UNITTEST_OBJ) $(CONNECT4_OBJ) build/unit_test/main.o
	@mkdir -p "$(@D)"
	$(CXX) $(UNITTEST_OBJ) $(CONNECT4_OBJ) build/unit_test/main.o -o bin/unit_test $(LDFLAGS)

bin/prop_test: $(PROPTEST_OBJ) $(CONNECT4_OBJ) build/prop_test/main.o
	@mkdir -p "$(@D)"
	$(CXX) $(PROPTEST_OBJ) $(CONNECT4_OBJ) build/prop_test/main.o -o bin/prop_test $(LDFLAGS)

bin/bench: $(BENCH_OBJ) $(CONNECT4_OBJ) build/bench/main.o
	@mkdir -p "$(@D)"
	$(CXX) $(BENCH_OBJ) $(CONNECT4_OBJ) build/bench/main.o -o bin/bench $(LDFLAGS)

clean:
	@rm -rf bin
	@rm -rf build
