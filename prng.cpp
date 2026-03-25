#include <cstdint>

class PRNG {
  uint64_t u[4];
  uint64_t prng_u64();

 public:
  void seed(double d);
  double random();
};

#define TW223_GEN(rs, z, r, i, k, q, s)                 \
  z = rs[i];                                            \
  z = (((z << q) ^ z) >> (k - s)) ^                     \
      ((z & ((uint64_t)(int64_t)-1 << (64 - k))) << s); \
  r ^= z;                                               \
  rs[i] = z;

#define TW223_STEP(rs, z, r)         \
  TW223_GEN(rs, z, r, 0, 63, 31, 18) \
  TW223_GEN(rs, z, r, 1, 58, 19, 28) \
  TW223_GEN(rs, z, r, 2, 55, 24, 7)  \
  TW223_GEN(rs, z, r, 3, 47, 21, 8)

typedef union {
  uint64_t u64;
  double d;
} U64double;

double first_rand(double seed) {
  uint64_t rs[4];
  uint32_t r = 0x11090601; /* 64-k[i] as four 8 bit constants. */
  int i;
  for (i = 0; i < 4; i++) {
    U64double u;
    uint32_t m = 1u << (r & 255);
    r >>= 8;
    u.d = seed = seed * 3.14159265358979323846 + 2.7182818284590452354;
    if (u.u64 < m) u.u64 += m; /* Ensure k[i] MSB of u[i] are non-zero. */
    rs[i] = u.u64;
  }
  uint64_t z, r2;
  for (i = 0; i < 11; i++) {
    z, r2 = 0;
    TW223_STEP(rs, z, r2)
  }
  U64double u;
  u.u64 = (r2 & 0x000fffffffffffff) | 0x3ff0000000000000;
  return u.d - 1.0;
}
