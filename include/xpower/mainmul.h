#ifndef MAINMUL_H
#define MAINMUL_H

#include <cstdint>

namespace xpower::mainmul {
  void mainmul(const int16_t in1_main[768], const int16_t in2_main[768], int16_t out_full[1528]);
}

#endif // MAINMUL_H
