#ifndef MAIN_LAY1_H
#define MAIN_LAY1_H

#include <cstdint>

namespace xpower::main_lay1 {
  void fwd_extract(int16_t arr[10][2][9][8], const int16_t main_poly[768]);
  void bwd_insert(int16_t arr[10][2][9][8], int16_t out_full[1528]);
}

#endif // MAIN_LAY1_H
