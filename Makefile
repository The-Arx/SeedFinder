CXX = g++
NVCC = nvcc

CXXFLAGS = -O3 -std=c++20 -Wall
NVFLAGS = -fmad=false -O3 --std=c++20

.PHONY: all cpu gpu clean

cpu: seed_find
seed_find: main.cpp searchers.cpp seed.cu.cc util.cu.cc
	$(CXX) $(CXXFLAGS) -o $@ $<

gpu: gpu_seed_find
gpu_seed_find: main.cu seed.cu.cc util.cu.cc
	$(NVCC) $(NVFLAGS) -o $@ $<

seed.cu.cc: prng.cu.cc pseudohash.cu.cc cuda.h
util.cu.cc: items.h cuda.h