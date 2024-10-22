#include <cstdint>

#include "rqmul.h"

int16_t in1_poly[768];
int16_t in2_poly[768];
int16_t out_poly[768];

int main() {
  for (int t = 0; t < 1000000; t++) {
    xpower::rqmul::rqmul(in1_poly, in2_poly, out_poly);
  }
}
