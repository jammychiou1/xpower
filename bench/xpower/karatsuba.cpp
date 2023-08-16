#include <cstdint>
#include <arm_neon.h>

#include "inline/basemul.cpp"

int16_t in1_arr[10][2][9][8];
int16_t in2_arr[10][2][9][8];
int16_t out_arr[10][2][9][8];

int main() {
  for (int t = 0; t < 1000000; t++) {

    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 9; j++) {
        int16x8_t a0 = vld1q_s16(&in1_arr[i][0][j][0]);
        int16x8_t a1 = vld1q_s16(&in1_arr[i][1][j][0]);
        int16x8_t b0 = vld1q_s16(&in2_arr[i][0][j][0]);
        int16x8_t b1 = vld1q_s16(&in2_arr[i][1][j][0]);

        int16x8_t c0_n2190x, c1_n2190x;
        xpower::basemul::karatsuba(a0, a1, b0, b1, i, j, c0_n2190x, c1_n2190x);

        vst1q_s16(&out_arr[i][0][j][0], c0_n2190x);
        vst1q_s16(&out_arr[i][1][j][0], c1_n2190x);
      }
    }

  }
}
