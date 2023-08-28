#include <cstdint>
#include <arm_neon.h>
#include <array>
#include <utility>
#include <cassert>
#include <iostream>

#include "inline/main_lay2.cpp"
#include "utils/gen_consts.h"
#include "ntt_ref.h"

typedef std::array<std::array<int16_t, 8>, 9> input;
typedef std::array<std::array<int16_t, 8>, 9> output;
typedef std::pair<input, output> inout;

void dump(inout testcase, output out) {
  std::cerr << "input\n";
  for (int i = 0; i < 9; i++) {
    for (int k = 0; k < 8; k++) {
      std::cerr << testcase.first[i][k] << " \n"[k == 7];
    }
  }

  std::cerr << "ref output\n";
  for (int i = 0; i < 9; i++) {
    for (int k = 0; k < 8; k++) {
      std::cerr << testcase.second[i][k] << " \n"[k == 7];
    }
  }

  std::cerr << "output\n";
  for (int i = 0; i < 9; i++) {
    for (int k = 0; k < 8; k++) {
      std::cerr << out[i][k] << " \n"[k == 7];
    }
  }

  std::cerr << "output (center lifted)\n";
  for (int i = 0; i < 9; i++) {
    for (int k = 0; k < 8; k++) {
      std::cerr << sntrup761::utils::center_lift(out[i][k]) << " \n"[k == 7];
    }
  }
}

bool run_testcase(inout testcase) {
  int16x8_t f0 = vld1q_s16(&testcase.first[0][0]);
  int16x8_t f1 = vld1q_s16(&testcase.first[1][0]);
  int16x8_t f2 = vld1q_s16(&testcase.first[2][0]);
  int16x8_t f3 = vld1q_s16(&testcase.first[3][0]);
  int16x8_t f4 = vld1q_s16(&testcase.first[4][0]);
  int16x8_t f5 = vld1q_s16(&testcase.first[5][0]);
  int16x8_t f6 = vld1q_s16(&testcase.first[6][0]);
  int16x8_t f7 = vld1q_s16(&testcase.first[7][0]);
  int16x8_t f8 = vld1q_s16(&testcase.first[8][0]);

  int16x8_t h0_2x, h1_2x, h2_2x, h3_2x, h4_2x, h5_2x, h6_2x, h7_2x, h8_2x;
  xpower::main_lay2::ntt9_2x(
      f0, f1, f2, f3, f4,
      f5, f6, f7, f8,
      h0_2x, h1_2x, h2_2x, h3_2x, h4_2x,
      h5_2x, h6_2x, h7_2x, h8_2x);

  output out = {};

  vst1q_s16(&out[0][0], h0_2x);
  vst1q_s16(&out[1][0], h1_2x);
  vst1q_s16(&out[2][0], h2_2x);
  vst1q_s16(&out[3][0], h3_2x);
  vst1q_s16(&out[4][0], h4_2x);
  vst1q_s16(&out[5][0], h5_2x);
  vst1q_s16(&out[6][0], h6_2x);
  vst1q_s16(&out[7][0], h7_2x);
  vst1q_s16(&out[8][0], h8_2x);

  for (int i = 0; i < 9; i++) {
    for (int k = 0; k < 8; k++) {
      if (sntrup761::utils::center_lift(out[i][k]) != testcase.second[i][k]) {
        dump(testcase, out);
        return false;
      }
    }
  }

  return true;
}

inout testcase1(int idx) {
  std::array<int16_t, 9> fs = {};
  fs[idx] = 1;
  std::array<int16_t, 9> hs = ntt_ref<9>(fs);

  inout res;
  for (int k = 0; k < 8; k++) {
    res.first[0][k] = fs[0];
    res.first[1][k] = fs[1];
    res.first[2][k] = fs[2];
    res.first[3][k] = fs[3];
    res.first[4][k] = fs[4];
    res.first[5][k] = fs[5];
    res.first[6][k] = fs[6];
    res.first[7][k] = fs[7];
    res.first[8][k] = fs[8];

    for (int i = 0; i < 9; i++) {
      res.second[i][k] = sntrup761::utils::center_lift(2 * int64_t(hs[i]));
    }
  }
  return res;
}

int main() {
  std::array<int, 9> indices = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  for (int idx : indices) {
    assert(run_testcase(testcase1(idx)));
  }
}
