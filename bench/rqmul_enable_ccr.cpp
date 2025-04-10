#include <cstdint>
#include <iostream>
#include <algorithm>

#include "rqmul.h"

int16_t in1_poly[768];
int16_t in2_poly[768];
int16_t out_poly[768];

uint64_t records[1000];

uint64_t get_time() {
  uint64_t t;
  __asm__ volatile("mrs %0, PMCCNTR_EL0" : "=r" (t));
  return t;
}

int main() {
  for (int t = 0; t < 1000; t++) {
    uint64_t start = get_time();

    xpower::rqmul::rqmul(in1_poly, in2_poly, out_poly);

    uint64_t end = get_time();

    records[t] = end - start;
  }

  std::sort(records, records + 1000);
  std::cout << records[500]  << '\n';
  std::cerr << records[0] << ' ' << records[999] << '\n';
  std::cerr << records[250] << ' ' << records[750] << '\n';
}

