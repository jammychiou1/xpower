#include "mainmul.h"

#include <cstdint>

#include "main_lay1.h"
#include "main_lay2.h"
#include "basemul.h"

namespace xpower::mainmul {
  void mainmul(const int16_t in1_main[768], const int16_t in2_main[768], int16_t out_full[1528]) {
    int16_t in1_arr[10][2][9][8];
    int16_t in2_arr[10][2][9][8];
    int16_t out_arr[10][2][9][8];

    main_lay1::fwd_extract(in1_arr, in1_main);
    main_lay2::fwd_inplace(in1_arr);
    main_lay1::fwd_extract(in2_arr, in2_main);
    main_lay2::fwd_inplace(in2_arr);
    basemul::main_basemul(in1_arr, in2_arr, out_arr);
    main_lay2::bwd_inplace(out_arr);
    main_lay1::bwd_insert(out_arr, out_full);
  }
}
