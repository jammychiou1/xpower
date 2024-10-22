#ifndef LOW_LAY1_H
#define LOW_LAY1_H

#include <cstdint>

namespace xpower::low_lay1 {
  void fwd_extract(int16_t arr[10][2][8], const int16_t low_poly[81]);
  void bwd_insert(int16_t arr[10][2][8], int16_t out_full[1528]);
}

#endif // LOW_LAY1_H
