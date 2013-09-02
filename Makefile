GTEST=gtest-1.6.0

CXX=clang++

CFLAGS=-c -g -Wall -Weffc++ -pedantic -Wextra -Itest -std=c++11 -stdlib=libc++
CFLAGS_TEST=$(CFLAGS) -Isrc -Ibuild/$(GTEST)/include -DGTEST_USE_OWN_TR1_TUPLE=1

LDFLAGS=-stdlib=libc++
LDFLAGS_TEST=$(LDFLAGS) -Lbuild/$(GTEST)/lib -lgtest

.PHONY: gtest setup

all: bin/noob bin/test_noob

setup:
	mkdir -p build/noob
	mkdir -p bin

bin/noob: setup build/noob/noob.o build/noob/connect4.o build/noob/cache.o build/noob/game.o build/noob/perfect.o
	$(CXX) $(LDFLAGS) build/noob/noob.o build/noob/connect4.o build/noob/game.o build/noob/cache.o build/noob/perfect.o -o bin/noob

build/noob/noob.o: src/noob.cpp
	$(CXX) $(CFLAGS) src/noob.cpp -o build/noob/noob.o

build/noob/connect4.o: src/connect4.cpp
	$(CXX) $(CFLAGS) src/connect4.cpp -o build/noob/connect4.o

build/noob/cache.o: src/cache.cpp
	$(CXX) $(CFLAGS) src/cache.cpp -o build/noob/cache.o

build/noob/game.o: src/game.cpp
	$(CXX) $(CFLAGS) src/game.cpp -o build/noob/game.o

build/noob/perfect.o: src/perfect.cpp
	$(CXX) $(CFLAGS) src/perfect.cpp -o build/noob/perfect.o

bin/test_noob: setup gtest build/noob/test_noob.o build/noob/test_connect4.o build/noob/test_cache.o build/noob/game.o build/noob/connect4.o build/noob/cache.o build/noob/test_perfect.o build/noob/perfect.o
	$(CXX) $(LDFLAGS_TEST) build/noob/test_noob.o build/noob/test_connect4.o build/noob/test_cache.o build/noob/connect4.o build/noob/cache.o build/noob/game.o build/noob/test_perfect.o build/noob/perfect.o -o bin/test_noob

build/noob/test_noob.o: test/test_noob.cpp
	$(CXX) $(CFLAGS_TEST) test/test_noob.cpp -o build/noob/test_noob.o

build/noob/test_connect4.o: test/test_connect4.cpp
	$(CXX) $(CFLAGS_TEST) test/test_connect4.cpp -o build/noob/test_connect4.o

build/noob/test_cache.o: test/test_cache.cpp
	$(CXX) $(CFLAGS_TEST) test/test_cache.cpp -o build/noob/test_cache.o

build/noob/test_perfect.o: test/test_perfect.cpp
	$(CXX) $(CFLAGS_TEST) test/test_perfect.cpp -o build/noob/test_perfect.o

gtest: third-party/$(GTEST)/make/Makefile
	mkdir -p build/$(GTEST)/lib
	mkdir -p build/$(GTEST)/include
	make -C third-party/$(GTEST)/make
	cp third-party/$(GTEST)/make/gtest_main.a build/$(GTEST)/lib/libgtest.a
	cp -rf third-party/gtest-1.6.0/include/gtest build/gtest-1.6.0/include

clean:
	rm -rf bin/*
	rm -rf build/*
	make -C third-party/$(GTEST)/make clean

