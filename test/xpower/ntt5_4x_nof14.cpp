#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "inline/main_lay1.cpp"

int main() {
  int16_t f0_val, f2_val, f3_val;
  std::cin >> f0_val >> f2_val >> f3_val;

  int16x8_t f0 = vdupq_n_s16(f0_val);
  int16x8_t f2 = vdupq_n_s16(f2_val);
  int16x8_t f3 = vdupq_n_s16(f3_val);

  int16x8_t h0_4x, h1_4x, h2_4x, h3_4x, h4_4x;
  xpower::main_lay1::ntt5_4x_nof14(
      f0, f2, f3,
      h0_4x, h1_4x, h2_4x, h3_4x, h4_4x);

  int16_t h0_4x_val = vduph_laneq_s16(h0_4x, 0);
  int16_t h1_4x_val = vduph_laneq_s16(h1_4x, 0);
  int16_t h2_4x_val = vduph_laneq_s16(h2_4x, 0);
  int16_t h3_4x_val = vduph_laneq_s16(h3_4x, 0);
  int16_t h4_4x_val = vduph_laneq_s16(h4_4x, 0);

  std::cout << h0_4x_val << ' ' << h1_4x_val << ' ' << h2_4x_val << ' ' << h3_4x_val << ' ' << h4_4x_val << '\n';
}
