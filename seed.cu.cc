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

#define SEED_CHARS_STR "123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
constexpr int SEED_LENGTH = 8;

constexpr char HOST_SEED_CHARS[] = SEED_CHARS_STR;

constexpr long pow_int(long a, int b) {
  long out = 1;
  while (b > 0) {
    if (b & 1) out *= a;
    b >>= 1;
    if (b > 0) a *= a;
  }
  return out;
}


constexpr int SEED_CHARS_LENGTH = sizeof(HOST_SEED_CHARS) - 1;
constexpr long NUM_SEEDS = pow_int(SEED_CHARS_LENGTH, SEED_LENGTH);
__constant__ constexpr char DEVICE_SEED_CHARS[] = SEED_CHARS_STR;

struct CharLookupTable {
  char next[256];
  int index[256];
};
__constant__ constexpr CharLookupTable SEED_CHAR_LOOKUP = [] () {
  CharLookupTable table;
  for (int i = 0; i < 256; i++) {
    table.next[i] = HOST_SEED_CHARS[0];
    table.index[i] = 0;
  }
  for (int i = 0; i < SEED_CHARS_LENGTH; i++) {
    unsigned char c = HOST_SEED_CHARS[i];
    if (i < SEED_CHARS_LENGTH - 1) table.next[c] = HOST_SEED_CHARS[i + 1];
    table.index[c] = i;
  }
  return table;
}();

class RandGen {
 public:
  __device__ double random();
  __device__ PRNG prng();
  __device__ void skip();
  __device__ uint32_t rand_int(uint32_t max);
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

__device__ PRNG RandGen::prng() {
  return PRNG(this->pseudoseed());
}

__device__ RandGen::RandGen(double hashed_seed, double state) {
  this->hashed_seed = hashed_seed;
  this->state = state;
}

__device__ double RandGen::pseudoseed() {
  this->state = round(
    fast_mod_1(2.134453429141 + this->state * 1.72431234) * 1e13) / 1e13;
  return (this->state + hashed_seed) / 2.0;
}

__device__ void RandGen::skip() {
  this->pseudoseed();
}

__device__ double RandGen::random() {
  PRNG prng(this->pseudoseed());
  return prng.random();
}

__device__ uint32_t RandGen::rand_int(uint32_t max) {
  PRNG prng(this->pseudoseed());
  return prng.rand_int(max);
}

template <typename T>
__device__ T RandGen::rand_item() {
  return static_cast<T>(
      static_cast<int>(this->rand_int(static_cast<uint32_t>(T::Count))));
}

__device__ bool RandGen::is_bugged() {
  return isnan(this->state);
}

class Seed {
 public:
  char seed[SEED_LENGTH + 1];
  __device__ Seed(long seed);
  __device__ Seed(const char* seed_str);
#ifndef __CUDACC__
  __device__ Seed(std::string_view seed_str);
#endif
  template <typename... Args>
  __device__ RandGen init_rand(Args&& ...args) const;
  __device__ long to_long() const;
  __device__ void next();

 private:
  double hashed_seed[SEED_LENGTH + 1];
  __device__ void partial_hash_seed(int start);
  __device__ void init_vars();
};

__device__ Seed::Seed(long seed_long) {
  for (int i = 0; i < SEED_LENGTH; i++) {
    int index = seed_long % SEED_CHARS_LENGTH;
    seed_long /= SEED_CHARS_LENGTH;
    seed[i] = DEVICE_SEED_CHARS[index];
  }
  init_vars();
}

#ifndef __CUDACC__
Seed::Seed(std::string_view seed_str) {
  if (seed_str.size() != SEED_LENGTH) {
    std::ostringstream oss;
    oss << "Seed must be exactly " << SEED_LENGTH << " characters (was " << seed_str.size() << ")";
    throw std::runtime_error(oss.str());
  }
  for (int i = 0; i < SEED_LENGTH; i++) {
    // auto index = std::string_view(HOST_SEED_CHARS).find(seed_str[i]);
    // if (index == std::string_view::npos) {
    //   std::ostringstream oss;
    //   oss << "Unexpected character '" << seed_str[i] << "' in initial seed";
    //   throw std::runtime_error(oss.str());
    // }
    seed[i] = seed_str[i];
  }
  init_vars();
}
#endif

__device__ Seed::Seed(const char* seed_str) {
  for (int i = 0; i < SEED_LENGTH; i++) {
    seed[i] = seed_str[i];
  }
  init_vars();
}

__device__ void Seed::init_vars() {
  hashed_seed[SEED_LENGTH] = 1.0;
  seed[SEED_LENGTH] = '\0';
  partial_hash_seed(SEED_LENGTH - 1);
}

__device__ long Seed::to_long() const {
  long num = 0;
  for (int i = SEED_LENGTH - 1; i >= 0; i--) {
    num *= SEED_CHARS_LENGTH;
    num += SEED_CHAR_LOOKUP.index[static_cast<unsigned char>(seed[i])];
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
  for (int i = 0; i < SEED_LENGTH; i++) {
    seed[i] = SEED_CHAR_LOOKUP.next[seed[i]];
    if (seed[i] != DEVICE_SEED_CHARS[0]) {
      partial_hash_seed(i);
      break;
    }
    seed[i] = DEVICE_SEED_CHARS[0];
  }
}

template <typename... Args>
__device__ RandGen Seed::init_rand(Args&& ...args) const {
  double initial_state = pseudohash(std::forward<Args>(args)..., this->seed);
  return RandGen(this->hashed_seed[0], initial_state);
}
