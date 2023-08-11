#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "inline/basemul.cpp"

int16_t a_arr[16];
int16_t b_arr[16];
int16_t c_arr[16];

int main() {
  for (int k = 0; k < 16; k++) {
    std::cin >> a_arr[k];
  }
  for (int k = 0; k < 16; k++) {
    std::cin >> b_arr[k];
  }

  int16x8_t a0 = vld1q_s16(&a_arr[0]);
  int16x8_t a1 = vld1q_s16(&a_arr[8]);
  int16x8_t b0 = vld1q_s16(&b_arr[0]);
  int16x8_t b1 = vld1q_s16(&b_arr[8]);

  int16x8_t c0_n1095x, c1_n1095x;
  xpower::basemul::karatsuba(a0, a1, b0, b1, 0, 0, c0_n1095x, c1_n1095x);

  vst1q_s16(&c_arr[0], c0_n1095x);
  vst1q_s16(&c_arr[8], c1_n1095x);

  for (int k = 0; k < 16; k++) {
    std::cout << c_arr[k] << " \n"[k == 15];
  }
}
