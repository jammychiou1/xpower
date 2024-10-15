#include <cstdint>

namespace xpower::rqmul {
  void crt(int16_t out_poly[768], int16_t out_main[1441], int16_t out_low[81]);
};

int16_t out_poly[768];
int16_t out_main[1441];
int16_t out_low[81];

int main() {
  for (int t = 0; t < 1000000; t++) {
    xpower::rqmul::crt(out_poly, out_main, out_low);
  }
}
