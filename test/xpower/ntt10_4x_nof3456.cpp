#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "inline/main_lay1.cpp"

int main() {
  int16_t f0_val, f1_val, f2_val, f7_val, f8_val, f9_val;
  std::cin >> f0_val >> f1_val >> f2_val >> f7_val >> f8_val >> f9_val;

  int16x8_t f0 = vdupq_n_s16(f0_val);
  int16x8_t f1 = vdupq_n_s16(f1_val);
  int16x8_t f2 = vdupq_n_s16(f2_val);
  int16x8_t f7 = vdupq_n_s16(f7_val);
  int16x8_t f8 = vdupq_n_s16(f8_val);
  int16x8_t f9 = vdupq_n_s16(f9_val);

  int16x8_t h0_4x, h1_4x, h2_4x, h3_4x, h4_4x, h5_4x, h6_4x, h7_4x, h8_4x, h9_4x;
  xpower::main_lay1::ntt10_4x_nof3546(
      f0, f1, f2, f7, f8, f9,
      h0_4x, h1_4x, h2_4x, h3_4x, h4_4x,
      h5_4x, h6_4x, h7_4x, h8_4x, h9_4x);

  int16_t h0_4x_val = vduph_laneq_s16(h0_4x, 0);
  int16_t h1_4x_val = vduph_laneq_s16(h1_4x, 0);
  int16_t h2_4x_val = vduph_laneq_s16(h2_4x, 0);
  int16_t h3_4x_val = vduph_laneq_s16(h3_4x, 0);
  int16_t h4_4x_val = vduph_laneq_s16(h4_4x, 0);
  int16_t h5_4x_val = vduph_laneq_s16(h5_4x, 0);
  int16_t h6_4x_val = vduph_laneq_s16(h6_4x, 0);
  int16_t h7_4x_val = vduph_laneq_s16(h7_4x, 0);
  int16_t h8_4x_val = vduph_laneq_s16(h8_4x, 0);
  int16_t h9_4x_val = vduph_laneq_s16(h9_4x, 0);

  std::cout << h0_4x_val << ' ' << h1_4x_val << ' ' << h2_4x_val << ' ' << h3_4x_val << ' ' << h4_4x_val << ' ';
  std::cout << h5_4x_val << ' ' << h6_4x_val << ' ' << h7_4x_val << ' ' << h8_4x_val << ' ' << h9_4x_val << '\n';
}
