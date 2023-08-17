#include "lowmul.h"

#include <cstdint>

#include "low_lay1.cpp"
#include "basemul.h"
#include "interface/sntrup761.h"
#include "consts.h"

namespace xpower::lowmul {
  void lowmul(const int16_t in1_low[81], const int16_t in2_low[81], int16_t out_low[96]) {
    static int16_t in1_arr[10][2][8];
    static int16_t in2_arr[10][2][8];
    static int16_t out_arr[10][2][8];

    low_lay1::fwd_extract(in1_arr, in1_low);
    low_lay1::fwd_extract(in2_arr, in2_low);
    basemul::low_basemul(in1_arr, in2_arr, out_arr);
    low_lay1::bwd_insert(out_arr, out_low);

    int64_t tmp = out_low[80] - 429 * (int64_t(in1_low[0]) * in2_low[80] + int64_t(in1_low[80]) * in2_low[0]);
    int64_t esti = (tmp * 935519 + (int64_t(1) << 31)) >> 32;
    out_low[80] = tmp - esti * shared::q;
  }
}
