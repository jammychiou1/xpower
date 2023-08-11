#include <cstdint>
#include <arm_neon.h>
#include <cassert>
#include <iostream>

#include "inline/basemul.cpp"
#include "utils/gen_consts.h"
#include "ntt_ref.h"

struct input {
  std::array<int16_t, 16> a;
  std::array<int16_t, 16> b;
  int i, j;
};
typedef std::array<int16_t, 16> output;
typedef std::pair<input, output> inout;

void dump_arr(std::array<int16_t, 16> arr) {
  for (int k = 0; k < 16; k++) {
    std::cerr << arr[k] << " \n"[k == 15];
  }
}

void dump(inout testcase, output out) {
  dump_arr(testcase.first.a);
  dump_arr(testcase.first.b);
  std::cerr << testcase.first.i << ' ' << testcase.first.j << '\n';
  dump_arr(testcase.second);
  dump_arr(out);
  for (int k = 0; k < 16; k++) {
    std::cerr << sntrup761::utils::center_lift(out[k]) << " \n"[k == 15];
  }
}

bool run_testcase(inout testcase) {
  int16x8_t a0 = vld1q_s16(&testcase.first.a[0]);
  int16x8_t a1 = vld1q_s16(&testcase.first.a[8]);
  int16x8_t b0 = vld1q_s16(&testcase.first.b[0]);
  int16x8_t b1 = vld1q_s16(&testcase.first.b[8]);
  int i = testcase.first.i;
  int j = testcase.first.j;

  int16x8_t c0_n1095x, c1_n1095x;
  xpower::basemul::main_karatsuba_kara(a0, a1, b0, b1, i, j, c0_n1095x, c1_n1095x);

  output out = {};
  vst1q_s16(&out[0], c0_n1095x);
  vst1q_s16(&out[8], c1_n1095x);

  for (int k = 0; k < 16; k++) {
    if (sntrup761::utils::center_lift(out[k]) != testcase.second[k]) {
      dump(testcase, out);
      return false;
    }
  }

  return true;
}

inout factory(std::array<int16_t, 16> a, std::array<int16_t, 16> b, int i, int j) {
  inout testcase;
  testcase.first = {a, b, i, j};
  int16_t prim_root = 11;
  int16_t w10 = sntrup761::utils::gen_pow(prim_root, (sntrup761::q - 1) / 10);
  int16_t w9 = sntrup761::utils::gen_pow(prim_root, (sntrup761::q - 1) / 9);
  int16_t weight = sntrup761::utils::center_lift(sntrup761::utils::gen_pow(w10, i) * sntrup761::utils::gen_pow(w9, j));
  std::array<int16_t, 16> c = weighted_conv_ref<16>(a, b, weight);
  for (int k = 0; k < 16; k++) {
    testcase.second[k] = sntrup761::utils::center_lift(-1095 * c[k]);
  }
  return testcase;
}

inout testcase0() {
  std::array<int16_t, 16> a = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };
  std::array<int16_t, 16> b = {
    0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0
  };
  return factory(a, b, 0, 0);
}

inout testcase1(int i, int j) {
  std::array<int16_t, 16> a = {
    101, 201, 301, 401, 501, 601, 701, 801,
    -102, -202, -302, -402, -502, -602, -702, -802
  };
  std::array<int16_t, 16> b = {
    103, 203, 303, 403, 503, 603, 703, 803,
    -104, -204, -304, -404, -504, -604, -704, -804
  };
  return factory(a, b, i, j);
}

inout testcase2(int i, int j) {
  std::array<int16_t, 16> a = {
    30101, 30201, 30301, 30401, 30501, 30601, 30701, 30801,
    -30102, -30202, -30302, -30402, -30502, -30602, -30702, -30802
  };
  std::array<int16_t, 16> b = {
    30103, 30203, 30303, 30403, 30503, 30603, 30703, 30803,
    -30104, -30204, -30304, -30404, -30504, -30604, -30704, -30804
  };
  return factory(a, b, i, j);
}

int main() {
  assert(run_testcase(testcase0()));
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 9; j++) {
      assert(run_testcase(testcase1(i, j)));
    }
  }
  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 9; j++) {
      assert(run_testcase(testcase2(i, j)));
    }
  }
}
