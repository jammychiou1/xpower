#ifndef GEN_CONST_H
#define GEN_CONST_H

#include <array>
#include <cmath>
#include <cstdint>

#include "interface/sntrup761.h"

namespace sntrup761::utils {
  constexpr int64_t center_lift(int64_t val) {
    if (val >= 0) {
      val %= sntrup761::q;
    }
    else {
      val = (sntrup761::q - (-val) % sntrup761::q) % sntrup761::q;
    }
    if (val > (sntrup761::q - 1) / 2) {
      val -= sntrup761::q;
    }
    return val;
  }

  constexpr int64_t gen_pow(int64_t base, int pow) {
    int64_t res = 1;
    for (int i = 0; i < pow; i++) {
      res = center_lift(res * base);
    }
    return res;
  }

  template <typename INT_T, size_t SZ>
  constexpr std::array<INT_T, SZ> gen_pows(INT_T base) {
    std::array<INT_T, SZ> pows = {1};
    for (int i = 1; i < SZ; i++) {
      pows[i] = center_lift(int64_t(pows[i - 1]) * base);
    }
    return pows;
  }

  constexpr int16_t gen_bar(int16_t coef) {
    return std::round(double(coef) * (1 << 15) / sntrup761::q);
  }

  template <size_t SZ>
  constexpr std::array<int16_t, SZ> gen_bars(std::array<int16_t, SZ> arr) {
    std::array<int16_t, SZ> bars = {};
    for (int i = 0; i < SZ; i++) {
      bars[i] = gen_bar(arr[i]);
    }
    return bars;
  }
}

#endif // GEN_CONST_H
