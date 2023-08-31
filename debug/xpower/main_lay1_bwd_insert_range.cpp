#include <cstdint>
#include <iostream>
#include <random>
#include <algorithm>

#include "main_lay1.h"

const int in_range = 8100;

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
  int16_t main_poly[1440];
  int16_t arr[10][2][9][8];
  int16_t mn = 0;
  int16_t mx = 0;
  for (int t = 0; t < 1000000; t++) {
    gen_input(arr);

    xpower::main_lay1::bwd_insert(arr, main_poly);

    for (int i = 0; i < 1440; i++) {
      mn = std::min(mn, main_poly[i]);
      mx = std::max(mx, main_poly[i]);
    }
  }

  std::cout << mn << ' ' << mx << '\n';
}
