#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "inline/main_lay2.cpp"

int main() {
  int16_t f0_val, f1_val, f2_val, f3_val, f4_val;
  int16_t f5_val, f6_val, f7_val, f8_val;
  std::cin >> f0_val >> f1_val >> f2_val >> f3_val >> f4_val;
  std::cin >> f5_val >> f6_val >> f7_val >> f8_val;

  int16x8_t f0 = vdupq_n_s16(f0_val);
  int16x8_t f1 = vdupq_n_s16(f1_val);
  int16x8_t f2 = vdupq_n_s16(f2_val);
  int16x8_t f3 = vdupq_n_s16(f3_val);
  int16x8_t f4 = vdupq_n_s16(f4_val);
  int16x8_t f5 = vdupq_n_s16(f5_val);
  int16x8_t f6 = vdupq_n_s16(f6_val);
  int16x8_t f7 = vdupq_n_s16(f7_val);
  int16x8_t f8 = vdupq_n_s16(f8_val);

  int16x8_t h0_2x, h1_2x, h2_2x, h3_2x, h4_2x, h5_2x, h6_2x, h7_2x, h8_2x;
  xpower::main_lay2::ntt9_2x(
      f0, f1, f2, f3, f4,
      f5, f6, f7, f8,
      h0_2x, h1_2x, h2_2x, h3_2x, h4_2x,
      h5_2x, h6_2x, h7_2x, h8_2x);

  int16_t h0_2x_val = vduph_laneq_s16(h0_2x, 0);
  int16_t h1_2x_val = vduph_laneq_s16(h1_2x, 0);
  int16_t h2_2x_val = vduph_laneq_s16(h2_2x, 0);
  int16_t h3_2x_val = vduph_laneq_s16(h3_2x, 0);
  int16_t h4_2x_val = vduph_laneq_s16(h4_2x, 0);
  int16_t h5_2x_val = vduph_laneq_s16(h5_2x, 0);
  int16_t h6_2x_val = vduph_laneq_s16(h6_2x, 0);
  int16_t h7_2x_val = vduph_laneq_s16(h7_2x, 0);
  int16_t h8_2x_val = vduph_laneq_s16(h8_2x, 0);

  std::cout << h0_2x_val << ' ' << h1_2x_val << ' ' << h2_2x_val << ' ' << h3_2x_val << ' ' << h4_2x_val << ' ';
  std::cout << h5_2x_val << ' ' << h6_2x_val << ' ' << h7_2x_val << ' ' << h8_2x_val << '\n';
}
