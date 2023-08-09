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
  int16_t w = sntrup761::utils::gen_pow<int64_t>(prim_root, (sntrup761::q - 1) / SZ);
  std::array<int64_t, SZ> ws = sntrup761::utils::gen_pows<int64_t, SZ>(w);

  std::array<int16_t, SZ> hs = {};
  for (int i = 0; i < SZ; i++) {
    int64_t hi = 0;
    for (int ii = 0; ii < SZ; ii++) {
      hi += ws[ii * i % SZ] * fs[ii];
      hi = sntrup761::utils::center_lift<int16_t>(hi);
    }
    hs[i] = sntrup761::utils::center_lift<int16_t>(hi);
  }
  return hs;
}

#endif // NTT_REF_H
