#include <cstdint>
#include <arm_neon.h>
#include <iostream>

#include "mainmul.h"

int16_t in1_main[768];
int16_t in2_main[768];
int16_t out_main[1440];

int main() {
  int n;
  std::cin >> n;
  for (int i = 0; i <= n; i++) {
    std::cin >> in1_main[i];
  }
  for (int i = 0; i <= n; i++) {
    std::cin >> in2_main[i];
  }

  xpower::mainmul::mainmul(in1_main, in2_main, out_main);

  for (int i = 0; i <= 2 * n; i++) {
    std::cout << out_main[i] << " \n"[i == 2 * n];
  }
}
