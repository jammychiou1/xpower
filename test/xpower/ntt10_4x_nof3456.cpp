#include <cstdint>
#include <arm_neon.h>
#include <array>
#include <utility>
#include <cassert>

#include "inline/main_lay1.cpp"
#include "utils/gen_consts.h"
#include "ntt_ref.h"

typedef std::array<std::array<int16_t, 8>, 6> input;
typedef std::array<std::array<int16_t, 8>, 10> output;
typedef std::pair<input, output> inout;

bool run_testcase(inout testcase) {
  int16x8_t f0 = vld1q_s16(&testcase.first[0][0]);
  int16x8_t f1 = vld1q_s16(&testcase.first[1][0]);
  int16x8_t f2 = vld1q_s16(&testcase.first[2][0]);
  int16x8_t f7 = vld1q_s16(&testcase.first[3][0]);
  int16x8_t f8 = vld1q_s16(&testcase.first[4][0]);
  int16x8_t f9 = vld1q_s16(&testcase.first[5][0]);

  int16x8_t h0_4x, h1_4x, h2_4x, h3_4x, h4_4x, h5_4x, h6_4x, h7_4x, h8_4x, h9_4x;
  xpower::main_lay1::ntt10_4x_nof3546(
      f0, f1, f2, f7, f8, f9,
      h0_4x, h1_4x, h2_4x, h3_4x, h4_4x,
      h5_4x, h6_4x, h7_4x, h8_4x, h9_4x);

  output out = {};

  vst1q_s16(&out[0][0], h0_4x);
  vst1q_s16(&out[1][0], h1_4x);
  vst1q_s16(&out[2][0], h2_4x);
  vst1q_s16(&out[3][0], h3_4x);
  vst1q_s16(&out[4][0], h4_4x);
  vst1q_s16(&out[5][0], h5_4x);
  vst1q_s16(&out[6][0], h6_4x);
  vst1q_s16(&out[7][0], h7_4x);
  vst1q_s16(&out[8][0], h8_4x);
  vst1q_s16(&out[9][0], h9_4x);

  for (int i = 0; i < 10; i++) {
    for (int k = 0; k < 8; k++) {
      if (sntrup761::utils::center_lift(out[i][k]) != testcase.second[i][k]) {
        return false;
      }
    }
  }

  return true;
}

inout testcase1(int idx) {
  std::array<int16_t, 10> fs = {};
  fs[idx] = 1;
  std::array<int16_t, 10> hs = ntt_ref<10>(fs);

  inout res;
  for (int k = 0; k < 8; k++) {
    res.first[0][k] = fs[0];
    res.first[1][k] = fs[1];
    res.first[2][k] = fs[2];
    res.first[3][k] = fs[7];
    res.first[4][k] = fs[8];
    res.first[5][k] = fs[9];
    for (int i = 0; i < 10; i++) {
      res.second[i][k] = sntrup761::utils::center_lift(4 * int64_t(hs[i]));
    }
  }
  return res;
}

int main() {
  std::array<int, 6> indices = {0, 1, 2, 7, 8, 9};
  for (int idx : indices) {
    assert(run_testcase(testcase1(idx)));
  }
}
