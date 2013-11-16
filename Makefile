GTEST=gtest-1.7.0

CFLAGS=-c -O3 -Wall -Isrc -Itest
CFLAGS_NOOB=$(CFLAGS) -Weffc++ -pedantic -Wextra
CFLAGS_TEST=$(CFLAGS) -Isrc -Ibuild/$(GTEST)/include

LDFLAGS=
LDFLAGS_TEST=$(LDFLAGS) -Lbuild/$(GTEST)/lib -lgtest

.PHONY: gtest setup

all: bin/noob bin/test_noob bin/slowtest_noob bin/bench_noob

setup:
	mkdir -p build/noob
	mkdir -p bin

bin/noob: setup build/noob/noob.o build/noob/connect4.o build/noob/cache.o build/noob/game.o
	$(CXX) $(LDFLAGS) build/noob/noob.o build/noob/connect4.o build/noob/game.o build/noob/cache.o -o bin/noob

build/noob/noob.o: src/noob.cpp
	$(CXX) $(CFLAGS_NOOB) src/noob.cpp -o build/noob/noob.o

build/noob/connect4.o: src/connect4.cpp
	$(CXX) $(CFLAGS_NOOB) src/connect4.cpp -o build/noob/connect4.o

build/noob/cache.o: src/cache.cpp
	$(CXX) $(CFLAGS_NOOB) src/cache.cpp -o build/noob/cache.o

build/noob/game.o: src/game.cpp
	$(CXX) $(CFLAGS_NOOB) src/game.cpp -o build/noob/game.o

# Unit tests

bin/test_noob: setup gtest build/noob/test_noob.o build/noob/test_connect4.o build/noob/test_cache.o build/noob/game.o build/noob/connect4.o build/noob/cache.o
	$(CXX) $(LDFLAGS_TEST) build/noob/test_noob.o build/noob/test_connect4.o build/noob/test_cache.o build/noob/connect4.o build/noob/cache.o build/noob/game.o -o bin/test_noob

build/noob/test_noob.o: test/test_noob.cpp
	$(CXX) $(CFLAGS_TEST) test/test_noob.cpp -o build/noob/test_noob.o

build/noob/test_connect4.o: test/test_connect4.cpp
	$(CXX) $(CFLAGS_TEST) test/test_connect4.cpp -o build/noob/test_connect4.o

build/noob/test_cache.o: test/test_cache.cpp
	$(CXX) $(CFLAGS_TEST) test/test_cache.cpp -o build/noob/test_cache.o

# Slow tests

bin/slowtest_noob: setup gtest build/noob/slowtest_noob.o build/noob/slowtest_game.o build/noob/connect4.o build/noob/cache.o build/noob/game.o
	$(CXX) $(LDFLAGS_TEST) build/noob/slowtest_noob.o build/noob/slowtest_game.o build/noob/connect4.o build/noob/cache.o build/noob/game.o -o bin/slowtest_noob

build/noob/slowtest_noob.o: slowtest/slowtest_noob.cpp
	$(CXX) $(CFLAGS_TEST) slowtest/slowtest_noob.cpp -o build/noob/slowtest_noob.o

build/noob/slowtest_game.o: slowtest/slowtest_game.cpp
	$(CXX) $(CFLAGS_TEST) slowtest/slowtest_game.cpp -o build/noob/slowtest_game.o

# Benchmarks

bin/bench_noob: setup build/noob/bench_noob.o build/noob/bench_pruning.o build/noob/bench_heuristic.o build/noob/connect4.o build/noob/cache.o build/noob/game.o
	$(CXX) $(LDFLAGS) build/noob/bench_noob.o build/noob/bench_pruning.o build/noob/bench_heuristic.o build/noob/connect4.o build/noob/cache.o build/noob/game.o -o bin/bench_noob

build/noob/bench_noob.o: benchmark/bench_noob.cpp
	$(CXX) $(CFLAGS) benchmark/bench_noob.cpp -o build/noob/bench_noob.o

build/noob/bench_pruning.o: benchmark/bench_pruning.cpp
	$(CXX) $(CFLAGS) benchmark/bench_pruning.cpp -o build/noob/bench_pruning.o

build/noob/bench_heuristic.o: benchmark/bench_heuristic.cpp
	$(CXX) $(CFLAGS) benchmark/bench_heuristic.cpp -o build/noob/bench_heuristic.o

gtest: third-party/$(GTEST)/make/Makefile
	mkdir -p build/$(GTEST)/lib
	mkdir -p build/$(GTEST)/include
	make -C third-party/$(GTEST)/make
	cp third-party/$(GTEST)/make/gtest_main.a build/$(GTEST)/lib/libgtest.a
	cp -rf third-party/$(GTEST)/include/gtest build/$(GTEST)/include

clean:
	rm -rf bin/*
	rm -rf build/*
	make -C third-party/$(GTEST)/make clean

