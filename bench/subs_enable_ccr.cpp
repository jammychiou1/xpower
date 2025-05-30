#include <cstdint>
#include <iostream>
#include <algorithm>

#include "rqmul.h"
#include "mainmul.h"
#include "main_lay1.h"
#include "main_lay2.h"
#include "lowmul.h"
#include "low_lay1.h"
#include "basemul.h"

/* Non-exposed functions. Define them ourselves. */
namespace xpower::rqmul {
  void crt(int16_t out_full[1528]);
  void scale_freeze(int16_t out_full[1528], int16_t out_poly[768]);
}


#define BENCH_SUBROUTINE(EXPRESSION) \
  do { \
    uint64_t records[1000]; \
    for (int t = 0; t < 1000; t++) { \
      uint64_t start = get_time(); \
      EXPRESSION; \
      uint64_t end = get_time(); \
      records[t] = end - start; \
    } \
    std::sort(records, records + 1000); \
    std::cout << "=== " #EXPRESSION " ===\n"; \
    std::cout << records[500]  << '\n'; \
    std::cerr << records[0] << ' ' << records[999] << '\n'; \
    std::cerr << records[250] << ' ' << records[750] << '\n'; \
  } while (0)

int16_t in1_poly[768];
int16_t in2_poly[768];

int16_t in1_arr_main[10][2][9][8];
int16_t in2_arr_main[10][2][9][8];
int16_t out_arr_main[10][2][9][8];

int16_t in1_arr_low[10][2][8];
int16_t in2_arr_low[10][2][8];
int16_t out_arr_low[10][2][8];

int16_t out_full[1528];

int16_t out_poly[768];

uint64_t get_time() {
  uint64_t t;
  __asm__ volatile("mrs %0, PMCCNTR_EL0" : "=r" (t));
  return t;
}

int main() {
  BENCH_SUBROUTINE(xpower::rqmul::rqmul(in1_poly, in2_poly, out_poly));

  BENCH_SUBROUTINE(xpower::mainmul::mainmul(in1_poly, in2_poly, out_full));

  BENCH_SUBROUTINE(xpower::main_lay1::fwd_extract(in1_arr_main, in1_poly));
  BENCH_SUBROUTINE(xpower::main_lay2::fwd_inplace(in1_arr_main));
  BENCH_SUBROUTINE(xpower::basemul::main_basemul(in1_arr_main, in2_arr_main, out_arr_main));
  BENCH_SUBROUTINE(xpower::main_lay2::bwd_inplace(out_arr_main));
  BENCH_SUBROUTINE(xpower::main_lay1::bwd_insert(out_arr_main, out_full));

  BENCH_SUBROUTINE(xpower::lowmul::lowmul(in1_poly, in2_poly, out_full));

  BENCH_SUBROUTINE(xpower::low_lay1::fwd_extract(in1_arr_low, in1_poly));
  BENCH_SUBROUTINE(xpower::basemul::low_basemul(in1_arr_low, in2_arr_low, out_arr_low));
  BENCH_SUBROUTINE(xpower::low_lay1::bwd_insert(out_arr_low, out_full));

  BENCH_SUBROUTINE(xpower::rqmul::crt(out_full));
  BENCH_SUBROUTINE(xpower::rqmul::scale_freeze(out_full, out_poly));
}
