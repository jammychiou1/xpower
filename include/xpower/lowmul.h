#ifndef LOWMUL_H
#define LOWMUL_H

#include <cstdint>

namespace xpower::lowmul {
  void lowmul(const int16_t in1_low[81], const int16_t in2_low[81], int16_t out_low[96]);
}

#endif // LOWMUL_H
