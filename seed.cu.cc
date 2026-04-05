#pragma once
#include <limits.h>
#include <utility>

#ifndef __CUDACC__
#include <string_view>
#include <stdexcept>
#include <sstream>
#endif

#include "cuda.h"
#include "prng.cu.cc"
#include "pseudohash.cu.cc"

constexpr long pow_int(long a, int b) {
  long out = 1;
  while (b > 0) {
    if (b & 1) out *= a;
    b >>= 1;
    if (b > 0) a *= a;
  }
  return out;
}

constexpr int SEED_LENGTH = 8;
__constant__ constexpr char SEED_CHARS[] = "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
constexpr int SEED_CHARS_LENGTH = sizeof(SEED_CHARS) - 1;
constexpr long NUM_SEEDS = pow_int(SEED_CHARS_LENGTH, SEED_LENGTH);

class RandGen {
 public:
  __device__ double random();
  __device__ void skip();
  template <typename T>
  __device__ T rand_item();
  __device__ bool is_bugged();

 private:
  double hashed_seed;
  double state;
  __device__ RandGen(double hashed_seed, double state);
  __device__ double pseudoseed();

  friend class Seed;
};

__device__ RandGen::RandGen(double hashed_seed, double state) {
  this->hashed_seed = hashed_seed;
  this->state = state;
}

__device__ double RandGen::pseudoseed() {
  this->state = round(
    fast_mod_1(2.134453429141 + this->state * 1.72431234)
    * pow(10.0, 13.0)) / pow(10.0, 13.0);
  return (this->state + hashed_seed) / 2.0;
}

__device__ void RandGen::skip() {
  this->pseudoseed();
}

__device__ double RandGen::random() { return first_rand(this->pseudoseed()); }

template <typename T>
__device__ T RandGen::rand_item() {
  return static_cast<T>(
      static_cast<int>(this->random() * static_cast<int>(T::Count)));
}

__device__ bool RandGen::is_bugged() {
  return std::isnan(this->state);
}

class Seed {
 public:
  char seed[SEED_LENGTH + 1];
  __device__ Seed(long seed);
#ifndef __CUDACC__
  __device__ Seed(std::string_view seed_str);
#endif
  template <typename... Args>
  __device__ RandGen init_rand(Args&& ...args) const;
  __device__ long to_long() const;
  __device__ void next();

 private:
  unsigned int seed_num[SEED_LENGTH];
  double hashed_seed[SEED_LENGTH + 1];
  __device__ void partial_hash_seed(int start);
};

__device__ Seed::Seed(long seed_long) {
  for (int i = 0; i < SEED_LENGTH; i++) {
    int index = seed_long % SEED_CHARS_LENGTH;
    seed_long /= SEED_CHARS_LENGTH;
    seed_num[i] = index;
    seed[i] = SEED_CHARS[index];
  }
  hashed_seed[SEED_LENGTH] = 1.0;
  seed[SEED_LENGTH] = '\0';
  partial_hash_seed(SEED_LENGTH - 1);
}

#ifndef __CUDACC__
Seed::Seed(std::string_view seed_str) {
  if (seed_str.size() != SEED_LENGTH) {
    std::ostringstream oss;
    oss << "Seed must be exactly " << SEED_LENGTH << " characters (was " << seed_str.size() << ")";
    throw std::runtime_error(oss.str());
  }
  for (int i = 0; i < SEED_LENGTH; i++) {
    auto index = std::string_view(SEED_CHARS).find(seed_str[i]);
    if (index == std::string_view::npos) {
      std::ostringstream oss;
      oss << "Unexpected character '" << seed_str[i] << "' in initial seed";
      throw std::runtime_error(oss.str());
    }
    seed_num[i] = index;
    seed[i] = seed_str[i];
  }
  hashed_seed[SEED_LENGTH] = 1.0;
  seed[SEED_LENGTH] = '\0';
  partial_hash_seed(SEED_LENGTH - 1);
}
#endif

__device__ long Seed::to_long() const {
  long num = 0;
  for (int i = SEED_LENGTH - 1; i >= 0; i--) {
    num *= SEED_CHARS_LENGTH;
    num += seed_num[i];
  }
  return num;
}

__device__ void Seed::partial_hash_seed(int start) {
  double num = hashed_seed[start + 1];
  for (int i = start; i >= 0; i--) {
    num = fast_mod_1((1.1239285023 / num) * seed[i] * MATH_PI +
                     MATH_PI * (i + 1));
    hashed_seed[i] = num;
  }
}

__device__ void Seed::next() {
  for (int i = 0; i < 8; i++) {
    seed_num[i] += 1;
    if (seed_num[i] != SEED_CHARS_LENGTH) {
      seed[i] = SEED_CHARS[seed_num[i]];
      partial_hash_seed(i);
      break;
    }
    seed_num[i] = 0;
    seed[i] = SEED_CHARS[0];
  }
}

template <typename... Args>
__device__ RandGen Seed::init_rand(Args&& ...args) const {
  double initial_state = pseudohash(std::forward<Args>(args)..., this->seed);
  return RandGen(this->hashed_seed[0], initial_state);
}