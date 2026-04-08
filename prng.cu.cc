#pragma once
#include <cstdint>

#include "cuda.h"

#define TW223_GEN(rs, z, i, k, q, s)                    \
  z = rs[i];                                            \
  z = (((z << q) ^ z) >> (k - s)) ^                     \
      ((z & ((uint64_t)(int64_t)-1 << (64 - k))) << s); \
  rs[i] = z;

#define TW223_STEP(rs, z)         \
  TW223_GEN(rs, z, 0, 63, 31, 18) \
  TW223_GEN(rs, z, 1, 58, 19, 28) \
  TW223_GEN(rs, z, 2, 55, 24, 7)  \
  TW223_GEN(rs, z, 3, 47, 21, 8)

typedef union {
  uint64_t u64;
  double d;
} U64double;

class PRNG {
  public:
    __device__ PRNG(double seed);
    __device__ double random();
    __device__ void skip();
    template <typename T>
    __device__ T rand_item();

  private:
    uint64_t rs[4];
};

__device__ PRNG::PRNG(double seed) {
  uint32_t r = 0x11090601; /* 64-k[i] as four 8 bit constants. */
  int i;
  for (i = 0; i < 4; i++) {
    U64double u;
    uint32_t m = 1u << (r & 255);
    r >>= 8;
    u.d = seed = seed * 3.14159265358979323846 + 2.7182818284590452354;
    if (u.u64 < m) u.u64 += m; /* Ensure k[i] MSB of u[i] are non-zero. */
    this->rs[i] = u.u64;
  }
  for (i = 0; i < 10; i++) {
    this->skip();
  }
}

__device__ double PRNG::random() {
  this->skip();
  U64double u;
  u.u64 = ((rs[0] ^ rs[1] ^ rs[2] ^ rs[3]) & 0x000fffffffffffff) | 0x3ff0000000000000;
  return u.d - 1.0;
}

__device__ void PRNG::skip() {
  uint64_t z;
  TW223_STEP(this->rs, z)
}

template <typename T>
__device__ T PRNG::rand_item() {
  return static_cast<T>(
      static_cast<int>(this->random() * static_cast<int>(T::Count)));
}
