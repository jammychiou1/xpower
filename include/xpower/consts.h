#ifndef CONSTS_H
#define CONSTS_H

#include <cstdint>

#include "interface/sntrup761.h"

namespace xpower::shared {
  const int16_t q = sntrup761::q;
  const int16_t q_prim = 15631;
  const int16_t _1_bar = 7;
  const int16_t _2_bar = 14;
  const int16_t _4_bar = 29;
}

namespace xpower::ntt5 {
  const int16_t red = 1005;
  const int16_t green = 918;
  const int16_t blue = -818;
  const int16_t yellow = -1736;
  const int16_t red_bar = 7173;
  const int16_t green_bar = 6552;
  const int16_t blue_bar = -5838;
  const int16_t yellow_bar = -12391;
}

namespace xpower::ntt9 {
  const int16_t red = -621;
  const int16_t blue = -803;
  const int16_t green = 1891;
  const int16_t red_bar = -4432;
  const int16_t blue_bar = -5731;
  const int16_t green_bar = 13497;
}

#endif // CONSTS_H
