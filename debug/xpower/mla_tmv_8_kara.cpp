#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "inline/basemul.cpp"

int16_t a_arr[8];
int16_t b_arr[16];
int32_t c_arr[8];

int main() {
  for (int k = 0; k < 8; k++) {
    std::cin >> a_arr[k];
  }
  for (int k = 0; k < 16; k++) {
    std::cin >> b_arr[k];
  }

  int16x8_t a = vld1q_s16(&a_arr[0]);
  int16x8_t b0 = vld1q_s16(&b_arr[0]);
  int16x8_t b1 = vld1q_s16(&b_arr[8]);

  int32x4_t c0, c1;
  xpower::basemul::mla_tmv_8_kara(b0, b1, a, c0, c1);

  vst1q_s32(&c_arr[0], c0);
  vst1q_s32(&c_arr[4], c1);

  for (int k = 0; k < 8; k++) {
    std::cout << c_arr[k] << " \n"[k == 7];
  }
}
