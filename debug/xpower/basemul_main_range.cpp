#include <cstdint>
#include <algorithm>
#include <iostream>

#include "basemul.h"

int16_t in1_arr[10][2][9][8];
int16_t in2_arr[10][2][9][8];
int16_t out_arr[10][2][9][8];

int main() {
  int mag;
  std::cin >> mag;
  for (int i = 0; i < 10; i++) {
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          in1_arr[i][k0][j][k] = mag;
          in2_arr[i][k0][j][k] = mag;
        }
      }
    }
  }
  xpower::basemul::main_basemul(in1_arr, in2_arr, out_arr);

  for (int i = 0; i < 10; i++) {
    int16_t mx = -32768;
    int16_t mn = 32767;
    for (int k0 = 0; k0 < 2; k0++) {
      for (int j = 0; j < 9; j++) {
        for (int k = 0; k < 8; k++) {
          mx = std::max(mx, out_arr[i][k0][j][k]);
          mn = std::min(mn, out_arr[i][k0][j][k]);
        }
      }
    }
    std::cout << mn << ' ' << mx << '\n';
  }
}
