#include <cstdint>

#include "mainmul.h"

int16_t in1_main[768];
int16_t in2_main[768];
int16_t out_full[1528];

int main() {
  for (int t = 0; t < 1000000; t++) {
    xpower::mainmul::mainmul(in1_main, in2_main, out_full);
  }
}
