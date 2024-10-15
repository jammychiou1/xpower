#include <cstdint>

namespace xpower::rqmul {
  void scale_freeze(int16_t out_poly[768]);
};

int16_t out_poly[768];

int main() {
  for (int t = 0; t < 1000000; t++) {
    xpower::rqmul::scale_freeze(out_poly);
  }
}
