#pragma once
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

#include "prng.cpp"

constexpr int SEED_LENGTH = 8;
const std::string SEED_CHARS = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const std::string NEXT_SEED_CHAR = SEED_CHARS.substr(1) + SEED_CHARS[0];

constexpr double MATH_PI = 3.14159265358979323846;

// x must be >= 0;
inline double fast_mod_1(double x) {
  // Won't work on bugged seeds
  // return x - static_cast<long>(x);

  // Works on all seeds but adds about 50ns per call
  return (x - static_cast<long>(x)) * (x < std::numeric_limits<long>::max());

  // Untested
  return x - std::trunc(x);
}

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
  Seed(std::string_view seed);
  Seed(long seed);
  RandGen init_rand(std::string_view key) const;
  long to_long() const;
  std::string_view to_string() const;
  void next();

 private:
  std::array<char, SEED_LENGTH> seed;
  std::array<unsigned int, SEED_LENGTH> seed_num;
  std::array<double, SEED_LENGTH + 1> hashed_seed;
  void partial_hash_seed(int start);
};

Seed::Seed(std::string_view seed_str) {
  if (seed_str.size() != SEED_LENGTH) {
    throw std::runtime_error(std::string("Seed must be exactly ") +
                             std::to_string(SEED_LENGTH) + " characters (was " +
                             std::to_string(seed_str.size()) + ")");
  }
  for (int i = 0; i < SEED_LENGTH; i++) {
    auto index = SEED_CHARS.find(seed_str[i]);
    if (index == std::string::npos) {
      throw std::runtime_error(std::string("Unexpected character ") +
                               seed_str[i] + " in initial seed");
    }
    seed_num[i] = index;
    seed[i] = seed_str[i];
  }
  hashed_seed[SEED_LENGTH] = 1.0;
  partial_hash_seed(SEED_LENGTH - 1);
}

Seed::Seed(long seed_long) {
  for (int i = 0; i < SEED_LENGTH; i++) {
    int index = seed_long % 35;
    seed_long /= 35;
    seed_num[i] = index;
    seed[i] = SEED_CHARS[index];
  }
  hashed_seed[SEED_LENGTH] = 1.0;
  partial_hash_seed(SEED_LENGTH - 1);
}

std::string_view Seed::to_string() const {
  return std::string_view(seed.data(), seed.size());
}

std::ostream &operator<<(std::ostream &os, const Seed &seed) {
  os << "Seed(" << seed.to_string() << ")";
  return os;
}

void Seed::partial_hash_seed(int start) {
  double num = hashed_seed[start + 1];
  for (int i = start; i >= 0; i--) {
    num = fast_mod_1((1.1239285023 / num) * seed[i] * MATH_PI +
                     MATH_PI * (i + 1));
    hashed_seed[i] = num;
  }
}

void Seed::next() {
  for (int i = 0; i < 8; i++) {
    seed[i] = NEXT_SEED_CHAR[seed_num[i]];
    seed_num[i] += 1;
    if (seed_num[i] != SEED_CHARS.length()) {
      partial_hash_seed(i);
      break;
    }
    seed_num[i] = 0;
  }
}

RandGen Seed::init_rand(std::string_view key) const {
  double initial_state = pseudohash_partial(
      0, key,
      pseudohash_partial(key.size(), std::string_view(seed.data(), seed.size()),
                         1.0));
  // double initial_state = pseudohash(key + seed);
  return RandGen(this->hashed_seed[0], initial_state);
}