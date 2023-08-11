#ifndef NTT_REF_H
#define NTT_REF_H

#include <array>
#include <cmath>
#include <cstdint>

#include "utils/gen_consts.h"

constexpr int16_t prim_root = 11;

template <int SZ>
std::array<int16_t, SZ> ntt_ref(std::array<int16_t, SZ> fs) {
  static_assert((sntrup761::q - 1) % SZ == 0);
  int16_t w = sntrup761::utils::gen_pow(prim_root, (sntrup761::q - 1) / SZ);
  std::array<int64_t, SZ> ws = sntrup761::utils::gen_pows<int64_t, SZ>(w);

  std::array<int16_t, SZ> hs = {};
  for (int i = 0; i < SZ; i++) {
    int64_t hi = 0;
    for (int ii = 0; ii < SZ; ii++) {
      hi += ws[ii * i % SZ] * fs[ii];
      hi = sntrup761::utils::center_lift(hi);
    }
    hs[i] = hi;
  }
  return hs;
}

template <int SZ>
std::array<int16_t, SZ> weighted_conv_ref(std::array<int16_t, SZ> a, std::array<int16_t, SZ> b, int16_t weight) {
  std::array<int16_t, SZ> c = {};

  for (int k = 0; k < SZ; k++) {
    int64_t ck = 0;
    for (int dk = 0; dk < SZ; dk++) {
      int k_a = (k + dk) % 16;
      int k_b = (16 - dk) % 16;
      if (k_a + k_b < 16) {
        ck += int64_t(a[k_a]) * b[k_b];
      }
      else {
        ck += int64_t(a[k_a]) * b[k_b] * weight;
      }
      ck = sntrup761::utils::center_lift(ck);
    }
    c[k] = ck;
  }
  return c;
}

#endif // NTT_REF_H
