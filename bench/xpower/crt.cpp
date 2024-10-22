#include <cstdint>

namespace xpower::rqmul {
  void crt(int16_t out_full[1528]);
};

int16_t out_full[1528];

int main() {
  for (int t = 0; t < 1000000; t++) {
    xpower::rqmul::crt(out_full);
  }
}
