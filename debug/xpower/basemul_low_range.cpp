#include <cstdint>
#include <algorithm>
#include <iostream>
#include <random>

#include "basemul.h"

const int in_range = 20000;

int16_t roll() {
  static std::default_random_engine generator;
  static std::uniform_int_distribution<int16_t> distribution(-in_range, in_range);
  return distribution(generator);
}

void gen_input(int16_t in1_arr[10][2][8], int16_t in2_arr[10][2][8]) {
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int k = 0; k < 8; k++) {
        in1_arr[i][k0][k] = roll();
        in2_arr[i][k0][k] = roll();
      }
    }
  }
}

int main() {
  int16_t in1_arr[10][2][8];
  int16_t in2_arr[10][2][8];
  int16_t out_arr[10][2][8];
  int16_t mns[10] = {};
  int16_t mxs[10] = {};
  for (int t = 0; t < 1000000; t++) {
    gen_input(in1_arr, in2_arr);

    xpower::basemul::low_basemul(in1_arr, in2_arr, out_arr);
    for (int i = 0; i < 10; i++) {
      for (int k0 = 0; k0 < 2; k0++) {
        for (int k = 0; k < 8; k++) {
          mns[i] = std::min(mns[i], out_arr[i][k0][k]);
          mxs[i] = std::max(mxs[i], out_arr[i][k0][k]);
        }
      }
    }
  }

  for (int i = 0; i < 10; i++) {
    std::cout << i << ": " << mns[i] << ' ' << mxs[i] << '\n';
  }
}
