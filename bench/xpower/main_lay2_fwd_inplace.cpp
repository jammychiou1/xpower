#include <cstdint>

#include "main_lay2.h"

int16_t main_arr[10][2][9][8];

int main() {
  for (int t = 0; t < 1000000; t++) {
    xpower::main_lay2::fwd_inplace(main_arr);
  }
}
