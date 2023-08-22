#include <cstdint>
#include <array>
#include <utility>
#include <cassert>
#include <iostream>

#include "utils/gen_consts.h"
#include "ntt_ref.h"
#include "mainmul.h"

struct input {
  std::array<int16_t, 768> in1_main;
  std::array<int16_t, 768> in2_main;
};
struct output {
  std::array<int16_t, 1440> out_main;
};
typedef std::pair<input, output> inout;

void dump(inout testcase, output out) {
  std::cerr << "input\n";
  for (int i = 0; i < 761; i++) {
    std::cerr << testcase.first.in1_main[i] << " \n"[i == 760];
  }
  for (int i = 0; i < 761; i++) {
    std::cerr << testcase.first.in2_main[i] << " \n"[i == 760];
  }

  std::cerr << "ref output\n";
  for (int i = 0; i < 1440; i++) {
    std::cerr << testcase.second.out_main[i] << " \n"[i == 1439];
  }

  std::cerr << "output\n";
  for (int i = 0; i < 1440; i++) {
    std::cerr << out.out_main[i] << " \n"[i == 1439];
  }

  std::cerr << "output (center lifted)\n";
  for (int i = 0; i < 1440; i++) {
    std::cerr << sntrup761::utils::center_lift(out.out_main[i]) << " \n"[i == 1439];
  }
}

bool run_testcase(inout testcase) {
  output out = {};
  xpower::mainmul::mainmul(&testcase.first.in1_main[0], &testcase.first.in2_main[0], &out.out_main[0]);

  for (int i = 0; i < 1440; i++) {
    if (sntrup761::utils::center_lift(out.out_main[i]) != testcase.second.out_main[i]) {
      dump(testcase, out);
      return false;
    }
  }

  return true;
}

output mainmul_ref(input in) {
  std::array<int16_t, 1440> in1_pad = {}, in2_pad = {};
  for (int i = 0; i < 761; i++) {
    in1_pad[i] = in.in1_main[i];
    in2_pad[i] = in.in2_main[i];
  }
  std::array<int16_t, 1440> out = weighted_conv_ref<1440>(in1_pad, in2_pad, 1);
  for (int i = 0; i < 1440; i++) {
    out[i] = sntrup761::utils::center_lift(2081 * out[i]);
  }
  return output{out};
}

inout testcase1() {
  inout res = {};
  for (int i = 0; i < 761; i++) {
    res.first.in1_main[i] = 1;
    res.first.in2_main[i] = 1;
  }
  res.second = mainmul_ref(res.first);
  return res;
}

inout testcase2() {
  inout res = {};
  for (int i = 0; i < 380; i++) {
    res.first.in1_main[i] = 2295;
    res.first.in2_main[i] = 2295;
  }
  for (int i = 380; i < 761; i++) {
    res.first.in1_main[i] = -2295;
    res.first.in2_main[i] = -2295;
  }
  res.second = mainmul_ref(res.first);
  return res;
}

int main() {
  assert(run_testcase(testcase1()));
  assert(run_testcase(testcase2()));
}
