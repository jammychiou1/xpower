#ifndef BASEMUL_H
#define BASEMUL_H

#include <cstdint>

namespace xpower::basemul {
  void main_basemul(int16_t in1_arr[10][2][9][8], int16_t in2_arr[10][2][9][8], int16_t out_arr[10][2][9][8]);
  void low_basemul(int16_t in1_arr[10][2][8], int16_t in2_arr[10][2][8], int16_t out_arr[10][2][8]);
}

#endif // BASEMUL_H
