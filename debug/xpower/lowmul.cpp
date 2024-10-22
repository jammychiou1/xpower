#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "lowmul.h"

int16_t in1_low[81];
int16_t in2_low[81];
int16_t out_full[1528];

int main() {
  int n;
  std::cin >> n;
  for (int i = 0; i <= n; i++) {
    std::cin >> in1_low[i];
  }
  for (int i = 0; i <= n; i++) {
    std::cin >> in2_low[i];
  }

  xpower::lowmul::lowmul(in1_low, in2_low, out_full);

  // for (int i = 0; i <= 2 * n; i++) {
  //   std::cout << out_full[i] << " \n"[i == 2 * n];
  // }
  for (int i = 0; i < 1528; i++) {
    std::cout << out_full[i] << " \n"[i % 8 == 7];
  }
}
