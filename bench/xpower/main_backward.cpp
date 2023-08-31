#include <cstdint>

#include "main_lay1.h"
#include "main_lay2.h"

int16_t main_poly[768];
int16_t main_arr[10][2][9][8];

int main() {
  for (int t = 0; t < 1000000; t++) {
    xpower::main_lay2::bwd_inplace(main_arr);
    xpower::main_lay1::bwd_insert(main_arr, main_poly);
  }
}