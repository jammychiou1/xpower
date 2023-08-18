#include <cstdint>

#include "lowmul.h"

int16_t in1_low[81];
int16_t in2_low[81];
int16_t out_low[96];

int main() {
  for (int t = 0; t < 1000000; t++) {
    xpower::lowmul::lowmul(in1_low, in2_low, out_low);
  }
}
