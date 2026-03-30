#pragma once
#include <cuda_runtime.h>
#include <limits.h>

#include "prng.cu"

constexpr static long pow_int(long a, int b) {
  long out = 1;
  while (b > 0) {
    if (b & 1) out *= a;
    b >>= 1;
    if (b > 0) a *= a;
  }
  return out;
}

constexpr int SEED_LENGTH = 8;
__constant__ char SEED_CHARS[] = "123456789ABCDEFGHIJKLMNPQRSTUVWXYZ";
constexpr int SEED_CHARS_LENGTH = sizeof(SEED_CHARS) - 1;
constexpr long NUM_SEEDS = pow_int(SEED_CHARS_LENGTH, SEED_LENGTH);

constexpr double MATH_PI = 3.14159265358979323846;

// x must be >= 0;
__device__ static double fast_mod_1(double x) {
  return x - trunc(x);
}

__device__ static double pseudohash_partial(int offset, int string_len, const char* string, double num) {
  for (int i = string_len - 1; i >= 0; i--) {
    num = fast_mod_1((1.1239285023 / num) * string[i] * MATH_PI +
                     MATH_PI * (i + offset + 1));
  }
  return num;
}

class RandGen {
 public:
  __device__ double random();
  template <typename T>
  __device__ T rand_item();

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

__device__ double RandGen::random() { return first_rand(this->pseudoseed()); }

template <typename T>
__device__ T RandGen::rand_item() {
  return static_cast<T>(
      static_cast<int>(this->random() * static_cast<int>(T::Count)));
}

class Seed {
 public:
  char seed[SEED_LENGTH + 1];
  __device__ Seed(long seed);
  __device__ RandGen init_rand(int key_len, const char* key) const;
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

__device__ RandGen Seed::init_rand(int key_len, const char* key) const {
  double initial_state = pseudohash_partial(0, key_len, key,
      pseudohash_partial(key_len, SEED_LENGTH, seed,
                         1.0));
  return RandGen(this->hashed_seed[0], initial_state);
}