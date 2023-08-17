#ifndef RQMUL_H
#define RQMUL_H

#include <cstdint>

namespace xpower::rqmul {
  void rqmul(const int16_t in1_poly[768], const int16_t in2_poly[768], int16_t out_poly[768]);
}

#endif // RQMUL_H

