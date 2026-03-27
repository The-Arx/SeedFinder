#pragma once
#include <cmath>
#include <string>

#include "prng.cpp"

#define MATH_PI 3.14159265358979323846

// WARNING: Won't work on bugged seeds
inline double fast_mod_1(double x) { return x - static_cast<long>(x); }

double pseudohash(std::string_view string) {
  double num = 1;
  for (int i = string.length(); i > 0; i--) {
    num = fast_mod_1((1.1239285023 / num) * string[i - 1] * MATH_PI +
                     MATH_PI * i);
  }
  return num;
}

double pseudohash_partial(int offset, std::string_view string, double num) {
  for (int i = string.length() - 1; i >= 0; i--) {
    num = fast_mod_1((1.1239285023 / num) * string[i] * MATH_PI +
                     MATH_PI * (i + offset + 1));
  }
  return num;
}

class RandGen {
 public:
  double random();
  template <typename T>
  T rand_item();

 private:
  double hashed_seed;
  double state;
  RandGen(double hashed_seed, double state);
  double pseudoseed();

  friend class Seed;
};

RandGen::RandGen(double hashed_seed, double state) {
  this->hashed_seed = hashed_seed;
  this->state = state;
}

double RandGen::pseudoseed() {
  this->state =
      std::round(std::fmod(2.134453429141 + this->state * 1.72431234, 1.0) *
                 std::pow(10.0, 13.0)) /
      std::pow(10.0, 13.0);
  return (this->state + hashed_seed) / 2.0;
}

double RandGen::random() { return first_rand(this->pseudoseed()); }

template <typename T>
T RandGen::rand_item() {
  return static_cast<T>(
      static_cast<int>(this->random() * static_cast<int>(T::Count)));
}

class Seed {
 public:
  const std::string seed;
  Seed(const std::string &seed);
  RandGen init_rand(std::string_view key);

 private:
  double hashed_seed;
};

Seed::Seed(const std::string &seed)
    : seed(seed), hashed_seed(pseudohash(seed)) {}

RandGen Seed::init_rand(std::string_view key) {
  double initial_state =
      pseudohash_partial(0, key, pseudohash_partial(key.size(), seed, 1.0));
  // double initial_state = pseudohash(key + seed);
  return RandGen(this->hashed_seed, initial_state);
}