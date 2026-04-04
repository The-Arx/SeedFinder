#pragma once
#include <cuda_runtime.h>
#include <utility>

constexpr double MATH_PI = 3.14159265358979323846;

// x must be >= 0;
__device__ double fast_mod_1(double x) {
  return x - trunc(x);
}

__device__ double pseudohash_partial(int offset, int string_len, const char* string, double num) {
  for (int i = string_len - 1; i >= 0; i--) {
    num = fast_mod_1((1.1239285023 / num) * string[i] * MATH_PI +
                     MATH_PI * (i + offset + 1));
  }
  return num;
}

__device__  double pseudohash_partial(int offset, int number_length, int number, double num) {
  for (int i = number_length + offset; i > offset; i--) {
    num = fast_mod_1((1.1239285023 / num) * (number % 10 + '0') * MATH_PI +
                     MATH_PI * i);
    number /= 10;
  }
  return num;
}

__device__ int int_length(int num) {
  int len = 0;
  do {
      len++;
      num /= 10;
  } while (num > 0);
  return len;
}

__device__ double pseudohash_impl(int offset);

template <typename... Rest>
__device__ double pseudohash_impl(int offset, int val, Rest&&... rest);

template <size_t N, typename... Rest>
__device__ double pseudohash_impl(int offset, const char (&str)[N], Rest&&... rest);

__device__ double pseudohash_impl(int offset) {
    return 1.0;
}

template <typename... Rest>
__device__ double pseudohash_impl(int offset, int val, Rest&&... rest) {
    const int current_len = int_length(val);
    
    double num = pseudohash_impl(offset + current_len, std::forward<Rest>(rest)...);
    
    return pseudohash_partial(offset, current_len, val, num);
}

template <size_t N, typename... Rest>
__device__ double pseudohash_impl(int offset, const char (&str)[N], Rest&&... rest) {
    const int current_len = static_cast<int>(N) - 1;

    double num = pseudohash_impl(offset + current_len, std::forward<Rest>(rest)...);

    return pseudohash_partial(offset, current_len, str, num);
}

template <typename... Args>
__device__ double pseudohash(Args&& ...args) {
    return pseudohash_impl(0, std::forward<Args>(args)...);
}