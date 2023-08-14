#include <cstdint>
#include <arm_neon.h>
#include <array>
#include <utility>
#include <cassert>
#include <iostream>

#include "utils/gen_consts.h"
#include "ntt_ref.h"
#include "main_lay1.h"

struct input {
  int16_t main_poly[768];
};
struct output {
  int16_t arr[10][2][9][8];
};
typedef std::pair<input, output> inout;

void dump(inout testcase, output out) {
  std::cerr << "input\n";
  for (int i = 0; i < 761; i++) {
    std::cerr << testcase.first.main_poly[i] << " \n"[i == 760];
  }

  std::cerr << "ref output\n";
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          std::cerr << testcase.second.arr[i][k0][j][k] << " \n"[k == 7];
        }
      }
    }
  }

  std::cerr << "output\n";
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          std::cerr << out.arr[i][k0][j][k] << " \n"[k == 7];
        }
      }
    }
  }

  std::cerr << "output (center lifted)\n";
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          std::cerr << sntrup761::utils::center_lift(out.arr[i][k0][j][k]) << " \n"[k == 7];
        }
      }
    }
  }
}

bool run_testcase(inout testcase) {
  output out = {};
  xpower::main_lay1::fwd_extract(out.arr, testcase.first.main_poly);

  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          if (sntrup761::utils::center_lift(out.arr[i][k0][j][k]) != testcase.second.arr[i][k0][j][k]) {
            dump(testcase, out);
            std::cerr << i << ' ' << k0 << ' ' << j << ' ' << k << '\n';
            std::cerr << testcase.second.arr[i][k0][j][k] << ' ' << sntrup761::utils::center_lift(out.arr[i][k0][j][k]) << '\n';
            return false;
          }
        }
      }
    }
  }

  return true;
}

void fwd_extract_ref(int16_t arr[10][2][9][8], int16_t main_poly[768]) {
  for (int j = 0; j < 9; j++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int k = 0; k < 8; k++) {
        std::array<int16_t, 10> fis = {};

        for (int i = 0; i < 10; i++) {
          int idx = (81 * i + 10 * j) % 90 * 16 + 8 * k0 + k;
          if (idx < 761) {
            fis[i] = main_poly[idx];
          }
        }

        std::array<int16_t, 10> his = ntt_ref<10>(fis);

        for (int i = 0; i < 10; i++) {
          arr[i][k0][j][k] = sntrup761::utils::center_lift(int64_t(4) * his[i]);
        }
      }
    }
  }
}

inout testcase1() {
  inout res = {};
  for (int i = 0; i < 761; i++) {
    res.first.main_poly[i] = 1;
  }
  fwd_extract_ref(res.second.arr, res.first.main_poly);
  return res;
}

inout testcase2() {
  inout res = {};
  for (int i = 0; i < 380; i++) {
    res.first.main_poly[i] = 2295;
  }
  for (int i = 380; i < 761; i++) {
    res.first.main_poly[i] = -2295;
  }
  fwd_extract_ref(res.second.arr, res.first.main_poly);
  return res;
}

int main() {
  assert(run_testcase(testcase1()));
  assert(run_testcase(testcase2()));
}
