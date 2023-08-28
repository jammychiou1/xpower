#include <cstdint>
#include <arm_neon.h>
#include <iostream>
#include <random>
#include <algorithm>

#include "inline/main_lay2.cpp"

const int in_range = 4000;

int16_t roll() {
  static std::default_random_engine generator;
  static std::uniform_int_distribution<int16_t> distribution(-in_range, in_range);
  return distribution(generator);
}

void gen_input(int16_t fjs[9][8]) {
  for (int j = 0; j < 9; j++) {
    for (int k = 0; k < 8; k++) {
      fjs[j][k] = roll();
    }
  }
}

int main() {
  int16_t fjs[9][8];
  int16_t hj_2xs[9][8];
  int16_t mxs[9] = {};
  int16_t mns[9] = {};
  for (int t = 0; t < 1000000; t++) {
    gen_input(fjs);

    int16x8_t f0 = vld1q_s16(&fjs[0][0]);
    int16x8_t f1 = vld1q_s16(&fjs[1][0]);
    int16x8_t f2 = vld1q_s16(&fjs[2][0]);
    int16x8_t f3 = vld1q_s16(&fjs[3][0]);
    int16x8_t f4 = vld1q_s16(&fjs[4][0]);
    int16x8_t f5 = vld1q_s16(&fjs[5][0]);
    int16x8_t f6 = vld1q_s16(&fjs[6][0]);
    int16x8_t f7 = vld1q_s16(&fjs[7][0]);
    int16x8_t f8 = vld1q_s16(&fjs[8][0]);

    int16x8_t h0_2x, h1_2x, h2_2x, h3_2x, h4_2x, h5_2x, h6_2x, h7_2x, h8_2x;
    xpower::main_lay2::ntt9_2x(
        f0, f1, f2, f3, f4,
        f5, f6, f7, f8,
        h0_2x, h1_2x, h2_2x, h3_2x, h4_2x,
        h5_2x, h6_2x, h7_2x, h8_2x);

    vst1q_s16(&hj_2xs[0][0], h0_2x);
    vst1q_s16(&hj_2xs[1][0], h1_2x);
    vst1q_s16(&hj_2xs[2][0], h2_2x);
    vst1q_s16(&hj_2xs[3][0], h3_2x);
    vst1q_s16(&hj_2xs[4][0], h4_2x);
    vst1q_s16(&hj_2xs[5][0], h5_2x);
    vst1q_s16(&hj_2xs[6][0], h6_2x);
    vst1q_s16(&hj_2xs[7][0], h7_2x);
    vst1q_s16(&hj_2xs[8][0], h8_2x);

    for (int j = 0; j < 9; j++) {
      for (int k = 0; k < 8; k++) {
        mxs[j] = std::max(mxs[j], hj_2xs[j][k]);
        mns[j] = std::min(mns[j], hj_2xs[j][k]);
      }
    }
  }

  for (int j = 0; j < 9; j++) {
    std::cout << j << ": " << mxs[j] << ' ' << mns[j] << '\n';
  }
}
