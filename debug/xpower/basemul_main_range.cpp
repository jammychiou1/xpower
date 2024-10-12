#include <cstdint>
#include <algorithm>
#include <iostream>
#include <random>

#include "basemul.h"

const int in_range = 8420;

int16_t roll() {
  static std::default_random_engine generator;
  static std::uniform_int_distribution<int16_t> distribution(-in_range, in_range);
  return distribution(generator);
}

void gen_input(int16_t in1_arr[10][2][9][8], int16_t in2_arr[10][2][9][8]) {
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          in1_arr[i][k0][j][k] = roll();
          in2_arr[i][k0][j][k] = roll();
        }
      }
    }
  }
}

int main() {
  int16_t in1_arr[10][2][9][8];
  int16_t in2_arr[10][2][9][8];
  int16_t out_arr[10][2][9][8];
  int16_t mns[10][9] = {};
  int16_t mxs[10][9] = {};
  for (int t = 0; t < 1000000; t++) {
    gen_input(in1_arr, in2_arr);

    xpower::basemul::main_basemul(in1_arr, in2_arr, out_arr);
    for (int i = 0; i < 10; i++) {
      for (int k0 = 0; k0 < 2; k0++) {
        for (int j = 0; j < 9; j++) {
          for (int k = 0; k < 8; k++) {
            mns[i][j] = std::min(mns[i][j], out_arr[i][k0][j][k]);
            mxs[i][j] = std::max(mxs[i][j], out_arr[i][k0][j][k]);
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
