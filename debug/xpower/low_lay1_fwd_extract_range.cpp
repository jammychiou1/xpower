#include <cstdint>
#include <iostream>
#include <random>
#include <algorithm>

#include "low_lay1.h"

const int in_range = 2295;

int16_t roll() {
  static std::default_random_engine generator;
  static std::uniform_int_distribution<int16_t> distribution(-in_range, in_range);
  return distribution(generator);
}

void gen_input(int16_t low_poly[80]) {
  for (int i = 0; i < 80; i++) {
    low_poly[i] = roll();
  }
}

int main() {
  int16_t low_poly[80];
  int16_t arr[10][2][8];
  int16_t mns[10] = {};
  int16_t mxs[10] = {};
  for (int t = 0; t < 1000000; t++) {
    gen_input(low_poly);

    xpower::low_lay1::fwd_extract(arr, low_poly);

    for (int i = 0; i < 10; i++) {
      for (int k0 = 0; k0 < 2; k0++) {
        for (int k = 0; k < 8; k++) {
          mns[i] = std::min(mns[i], arr[i][k0][k]);
          mxs[i] = std::max(mxs[i], arr[i][k0][k]);
        }
      }
    }
  }

  for (int i = 0; i < 10; i++) {
    std::cout << i << ": " << mns[i] << ' ' << mxs[i] << '\n';
  }
}
