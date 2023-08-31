#include <cstdint>
#include <iostream>
#include <random>
#include <algorithm>

#include "main_lay2.h"

const int in_range = 3600;

int16_t roll() {
  static std::default_random_engine generator;
  static std::uniform_int_distribution<int16_t> distribution(-in_range, in_range);
  return distribution(generator);
}

void gen_input(int16_t arr[10][2][9][8]) {
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          arr[i][k0][j][k] = roll();
        }
      }
    }
  }
}

int main() {
  int16_t arr[10][2][9][8];
  int16_t mns[10][9] = {};
  int16_t mxs[10][9] = {};
  for (int t = 0; t < 1000000; t++) {
    gen_input(arr);

    xpower::main_lay2::fwd_inplace(arr);

    for (int i = 0; i < 10; i++) {
      for (int k0 = 0; k0 < 2; k0++) {
        for (int j = 0; j < 9; j++) {
          for (int k = 0; k < 8; k++) {
            mns[i][j] = std::min(mns[i][j], arr[i][k0][j][k]);
            mxs[i][j] = std::max(mxs[i][j], arr[i][k0][j][k]);
          }
        }
      }
    }
  }

  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 9; j++) {
      std::cout << i << ", " << j << ": " << mns[i][j] << ' ' << mxs[i][j] << '\n';
    }
  }
}
