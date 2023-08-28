#include <cstdint>
#include <array>
#include <utility>
#include <cassert>
#include <iostream>

#include "utils/gen_consts.h"
#include "ntt_ref.h"
#include "lowmul.h"

struct input {
  std::array<int16_t, 81> in1_low;
  std::array<int16_t, 81> in2_low;
};
struct output {
  std::array<int16_t, 96> out_low;
};
typedef std::pair<input, output> inout;

void dump(inout testcase, output out) {
  std::cerr << "input\n";
  for (int i = 0; i < 81; i++) {
    std::cerr << testcase.first.in1_low[i] << " \n"[i == 80];
  }
  for (int i = 0; i < 81; i++) {
    std::cerr << testcase.first.in2_low[i] << " \n"[i == 80];
  }

  std::cerr << "ref output\n";
  for (int i = 0; i < 81; i++) {
    std::cerr << testcase.second.out_low[i] << " \n"[i == 80];
  }

  std::cerr << "output\n";
  for (int i = 0; i < 81; i++) {
    std::cerr << out.out_low[i] << " \n"[i == 80];
  }

  std::cerr << "output (center lifted)\n";
  for (int i = 0; i < 81; i++) {
    std::cerr << sntrup761::utils::center_lift(out.out_low[i]) << " \n"[i == 80];
  }
}

bool run_testcase(inout testcase) {
  output out = {};
  xpower::lowmul::lowmul(&testcase.first.in1_low[0], &testcase.first.in2_low[0], &out.out_low[0]);

  for (int i = 0; i < 81; i++) {
    if (sntrup761::utils::center_lift(out.out_low[i]) != testcase.second.out_low[i]) {
      dump(testcase, out);
      return false;
    }
  }

  return true;
}

output lowmul_ref(input in) {
  std::array<int16_t, 96> out = {};
  for (int i = 0; i < 81; i++) {
    for (int j = 0; j < 81; j++) {
      if (i + j < 81) {
        out[i + j] = sntrup761::utils::center_lift(out[i + j] + int32_t(in.in1_low[i]) * in.in2_low[j]);
      }
    }
  }
  for (int i = 0; i < 81; i++) {
    out[i] = sntrup761::utils::center_lift(170 * out[i]);
  }
  return output{out};
}

inout testcase1() {
  inout res = {};
  for (int i = 0; i < 81; i++) {
    res.first.in1_low[i] = 1;
    res.first.in2_low[i] = 1;
  }
  res.second = lowmul_ref(res.first);
  return res;
}

inout testcase2() {
  inout res = {};
  for (int i = 0; i < 40; i++) {
    res.first.in1_low[i] = 2295;
    res.first.in2_low[i] = 2295;
  }
  for (int i = 40; i < 81; i++) {
    res.first.in1_low[i] = -2295;
    res.first.in2_low[i] = -2295;
  }
  res.second = lowmul_ref(res.first);
  return res;
}

int main() {
  assert(run_testcase(testcase1()));
  assert(run_testcase(testcase2()));
}
