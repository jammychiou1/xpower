#include "main_lay2.h"

#include <cstdint>
#include <arm_neon.h>

#include "inline/main_lay2.cpp"

namespace xpower::main_lay2 {
  void fwd_inplace(int16_t arr[10][2][9][8]) {
    for (int i = 0; i < 10; i++) {
      for (int k0 = 0; k0 < 2; k0++) {
        int16x8_t f0 = vld1q_s16(&arr[i][k0][0][0]);
        int16x8_t f1 = vld1q_s16(&arr[i][k0][1][0]);
        int16x8_t f2 = vld1q_s16(&arr[i][k0][2][0]);
        int16x8_t f3 = vld1q_s16(&arr[i][k0][3][0]);
        int16x8_t f4 = vld1q_s16(&arr[i][k0][4][0]);
        int16x8_t f5 = vld1q_s16(&arr[i][k0][5][0]);
        int16x8_t f6 = vld1q_s16(&arr[i][k0][6][0]);
        int16x8_t f7 = vld1q_s16(&arr[i][k0][7][0]);
        int16x8_t f8 = vld1q_s16(&arr[i][k0][8][0]);

        int16x8_t h0_2x, h1_2x, h2_2x, h3_2x, h4_2x, h5_2x, h6_2x, h7_2x, h8_2x, h9_2x;
        ntt9_2x(
            f0, f1, f2, f3, f4,
            f5, f6, f7, f8,
            h0_2x, h1_2x, h2_2x, h3_2x, h4_2x,
            h5_2x, h6_2x, h7_2x, h8_2x);

        vst1q_s16(&arr[i][k0][0][0], h0_2x);
        vst1q_s16(&arr[i][k0][1][0], h1_2x);
        vst1q_s16(&arr[i][k0][2][0], h2_2x);
        vst1q_s16(&arr[i][k0][3][0], h3_2x);
        vst1q_s16(&arr[i][k0][4][0], h4_2x);
        vst1q_s16(&arr[i][k0][5][0], h5_2x);
        vst1q_s16(&arr[i][k0][6][0], h6_2x);
        vst1q_s16(&arr[i][k0][7][0], h7_2x);
        vst1q_s16(&arr[i][k0][8][0], h8_2x);
      }
    }
  }

  void bwd_inplace(int16_t arr[10][2][9][8]) {
    for (int i = 0; i < 10; i++) {
      for (int k0 = 0; k0 < 2; k0++) {
        int16x8_t h0 = vld1q_s16(&arr[i][k0][0][0]);
        int16x8_t h1 = vld1q_s16(&arr[i][k0][1][0]);
        int16x8_t h2 = vld1q_s16(&arr[i][k0][2][0]);
        int16x8_t h3 = vld1q_s16(&arr[i][k0][3][0]);
        int16x8_t h4 = vld1q_s16(&arr[i][k0][4][0]);
        int16x8_t h5 = vld1q_s16(&arr[i][k0][5][0]);
        int16x8_t h6 = vld1q_s16(&arr[i][k0][6][0]);
        int16x8_t h7 = vld1q_s16(&arr[i][k0][7][0]);
        int16x8_t h8 = vld1q_s16(&arr[i][k0][8][0]);

        int16x8_t f0_18x, f1_18x, f2_18x, f3_18x, f4_18x, f5_18x, f6_18x, f7_18x, f8_18x, f9_18x;
        intt9_18x(
            h0, h1, h2, h3, h4,
            h5, h6, h7, h8,
            f0_18x, f1_18x, f2_18x, f3_18x, f4_18x,
            f5_18x, f6_18x, f7_18x, f8_18x);

        vst1q_s16(&arr[i][k0][0][0], f0_18x);
        vst1q_s16(&arr[i][k0][1][0], f1_18x);
        vst1q_s16(&arr[i][k0][2][0], f2_18x);
        vst1q_s16(&arr[i][k0][3][0], f3_18x);
        vst1q_s16(&arr[i][k0][4][0], f4_18x);
        vst1q_s16(&arr[i][k0][5][0], f5_18x);
        vst1q_s16(&arr[i][k0][6][0], f6_18x);
        vst1q_s16(&arr[i][k0][7][0], f7_18x);
        vst1q_s16(&arr[i][k0][8][0], f8_18x);
      }
    }
  }
}
