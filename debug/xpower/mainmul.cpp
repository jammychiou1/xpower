#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "mainmul.h"

int16_t in1_main[768];
int16_t in2_main[768];
int16_t out_full[1528];

int main() {
  int n;
  std::cin >> n;
  for (int i = 0; i <= n; i++) {
    std::cin >> in1_main[i];
  }
  for (int i = 0; i <= n; i++) {
    std::cin >> in2_main[i];
  }

  xpower::mainmul::mainmul(in1_main, in2_main, out_full);

  // for (int i = 0; i <= 2 * n; i++) {
  //   std::cout << out_main[i] << " \n"[i == 2 * n];
  // }
  for (int i = 0; i < 1528; i++) {
    std::cout << out_full[i] << " \n"[i % 8 == 7];
  }
}
