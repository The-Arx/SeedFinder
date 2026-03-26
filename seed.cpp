#pragma once
#include <string>
#include <cmath>
#include "prng.cpp"

#define MATH_PI 3.14159265358979323846

double pseudohash(std::string string) {
  double num = 1;
  for (int i = string.length(); i > 0; i--) {
    num = std::fmod(
        (1.1239285023 / num) * string[i - 1] * MATH_PI + MATH_PI * i, 1.0);
  }
  return num;
}

class Seed {
  double hashed_seed;

 public:
  std::string seed;
  Seed(std::string seed);
  double init_rand(std::string key);
  double pseudoseed(double &rand_seed);
  double random(double &rand_seed);
  template <typename T> T rand_item(double &rand_seed);
};

Seed::Seed(std::string seed) {
  this->seed = seed;
  this->hashed_seed = pseudohash(seed);
}

double Seed::init_rand(std::string key) { return pseudohash(key + seed); }

double Seed::pseudoseed(double &rand_seed) {
  rand_seed =
      std::round(std::fmod(2.134453429141 + rand_seed * 1.72431234, 1.0) *
                 std::pow(10.0, 13.0)) /
      std::pow(10.0, 13.0);
  return (rand_seed + hashed_seed) / 2.0;
}

double Seed::random(double &rand_seed) {
  return first_rand(pseudoseed(rand_seed));
}

template <typename T>
T Seed::rand_item(double &rand_seed) {
  return static_cast<T>(static_cast<int>(this->random(rand_seed) * static_cast<int>(T::_Count)));
}