#include <cstdint>
#include <arm_neon.h>

#include "inline/main_lay1.cpp"

namespace xpower::main_lay1 {
  void inplace(int16_t arr[10][2][9][8]) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        int16x8_t f0 = vld1q_s16(&arr[0][k0][j][0]);
        int16x8_t f1 = vld1q_s16(&arr[1][k0][j][0]);
        int16x8_t f2 = vld1q_s16(&arr[2][k0][j][0]);
        int16x8_t f3 = vld1q_s16(&arr[3][k0][j][0]);
        int16x8_t f4 = vld1q_s16(&arr[4][k0][j][0]);
        int16x8_t f5 = vld1q_s16(&arr[5][k0][j][0]);
        int16x8_t f6 = vld1q_s16(&arr[6][k0][j][0]);
        int16x8_t f7 = vld1q_s16(&arr[7][k0][j][0]);
        int16x8_t f8 = vld1q_s16(&arr[8][k0][j][0]);
        int16x8_t f9 = vld1q_s16(&arr[9][k0][j][0]);

        int16x8_t h0_4x, h1_4x, h2_4x, h3_4x, h4_4x, h5_4x, h6_4x, h7_4x, h8_4x, h9_4x;
        ntt10_4x_nof3546(
            f0, f1, f2, f7, f8, f9,
            h0_4x, h1_4x, h2_4x, h3_4x, h4_4x,
            h5_4x, h6_4x, h7_4x, h8_4x, h9_4x);

        vst1q_s16(&arr[0][k0][j][0], h0_4x);
        vst1q_s16(&arr[1][k0][j][0], h1_4x);
        vst1q_s16(&arr[2][k0][j][0], h2_4x);
        vst1q_s16(&arr[3][k0][j][0], h3_4x);
        vst1q_s16(&arr[4][k0][j][0], h4_4x);
        vst1q_s16(&arr[5][k0][j][0], h5_4x);
        vst1q_s16(&arr[6][k0][j][0], h6_4x);
        vst1q_s16(&arr[7][k0][j][0], h7_4x);
        vst1q_s16(&arr[8][k0][j][0], h8_4x);
        vst1q_s16(&arr[9][k0][j][0], h9_4x);
      }
    }
  }
}
