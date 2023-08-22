#include "basemul.h"

#include <cstdint>
#include <arm_neon.h>

#include "inline/basemul.cpp"

namespace xpower::basemul {
  void main_basemul(int16_t in1_arr[10][2][9][8], int16_t in2_arr[10][2][9][8], int16_t out_arr[10][2][9][8]) {
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 9; j++) {
        int16x8_t a0 = vld1q_s16(&in1_arr[i][0][j][0]);
        int16x8_t a1 = vld1q_s16(&in1_arr[i][1][j][0]);
        int16x8_t b0 = vld1q_s16(&in2_arr[i][0][j][0]);
        int16x8_t b1 = vld1q_s16(&in2_arr[i][1][j][0]);

        int16x8_t c0_n1095x, c1_n1095x;
        if (i % 2 == 0) {
          radix2(a0, a1, b0, b1, i, j, c0_n1095x, c1_n1095x);
        }
        else {
          karatsuba(a0, a1, b0, b1, i, j, c0_n1095x, c1_n1095x);
        }

        vst1q_s16(&out_arr[i][0][j][0], c0_n1095x);
        vst1q_s16(&out_arr[i][1][j][0], c1_n1095x);
      }
    }
  }

  void low_basemul(int16_t in1_arr[10][2][8], int16_t in2_arr[10][2][8], int16_t out_arr[10][2][8]) {
    for (int i = 0; i < 10; i++) {
      int16x8_t a0 = vld1q_s16(&in1_arr[i][0][0]);
      int16x8_t a1 = vld1q_s16(&in1_arr[i][1][0]);
      int16x8_t b0 = vld1q_s16(&in2_arr[i][0][0]);
      int16x8_t b1 = vld1q_s16(&in2_arr[i][1][0]);

      int16x8_t c0_n1095x, c1_n1095x;
      if (i % 2 == 0) {
        radix2_twist(a0, a1, b0, b1, i, c0_n1095x, c1_n1095x);
      }
      else {
        karatsuba_twist(a0, a1, b0, b1, i, c0_n1095x, c1_n1095x);
      }

      vst1q_s16(&out_arr[i][0][0], c0_n1095x);
      vst1q_s16(&out_arr[i][1][0], c1_n1095x);
    }
  }
}
