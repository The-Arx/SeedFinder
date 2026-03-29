CC = gcc
CXX = g++
CXXFLAGS = -O3 --std=c++20 -Wall

seed_find: main.o
	g++ $(LDFLAGS) -o $@ $< $(LDLIBS)

main.o: seed.cpp searchers.cpp prng.cpp util.cpp