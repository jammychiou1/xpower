#include <cstdint>
#include <arm_neon.h>
#include <array>
#include <utility>
#include <cassert>
#include <iostream>

#include "utils/gen_consts.h"
#include "ntt_ref.h"
#include "main_lay2.h"

struct io {
  int16_t arr[10][2][9][8];
};
typedef std::pair<io, io> inout;

void dump(inout testcase, io out) {
  std::cerr << "input\n";
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          std::cerr << testcase.first.arr[i][k0][j][k] << " \n"[k == 7];
        }
      }
    }
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
  io out = testcase.first;
  xpower::main_lay2::fwd_inplace(out.arr);

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

void fwd_inplace_ref(int16_t out_arr[10][2][9][8], int16_t in_arr[10][2][9][8]) {
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int k = 0; k < 8; k++) {
        std::array<int16_t, 9> fjs = {};

        for (int j = 0; j < 9; j++) {
          fjs[j] = in_arr[i][k0][j][k];
        }

        std::array<int16_t, 9> hjs = ntt_ref<9>(fjs);

        for (int j = 0; j < 9; j++) {
          out_arr[i][k0][j][k] = sntrup761::utils::center_lift(int64_t(2) * hjs[j]);
        }
      }
    }
  }
}

inout testcase1() {
  inout res = {};
  for (int i = 0; i < 10; i++) {
    res.first.arr[i][0][0][0] = 1;
    res.first.arr[i][0][1][0] = 1;
  }
  fwd_inplace_ref(res.second.arr, res.first.arr);
  return res;
}

inout testcase2() {
  inout res = {};
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          if (i * j & 2) {
            res.first.arr[i][k0][j][k] = 3000;
          }
          else {
            res.first.arr[i][k0][j][k] = -3000;
          }
        }
      }
    }
  }
  fwd_inplace_ref(res.second.arr, res.first.arr);
  return res;
}

int main() {
  assert(run_testcase(testcase1()));
  assert(run_testcase(testcase2()));
}
