#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "rqmul.h"

int16_t in1_poly[768];
int16_t in2_poly[768];
int16_t out_poly[768];

int main() {
  for (int i = 0; i < 761; i++) {
    std::cin >> in1_poly[i];
  }
  for (int i = 0; i < 761; i++) {
    std::cin >> in2_poly[i];
  }

  xpower::rqmul::rqmul(in1_poly, in2_poly, out_poly);

  for (int i = 0; i < 761; i++) {
    std::cout << out_poly[i] << " \n"[i == 760];
  }
}

