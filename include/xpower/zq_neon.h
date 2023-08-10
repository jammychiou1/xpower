#ifndef ZQ_NEON_H
#define ZQ_NEON_H

#include <cstdint>
#include <arm_neon.h>

#include "neon_lane_asm.h"

namespace xpower::barret {
  template <int BAR_LANE>
  inline int16x8_t gen_esti(int16x8_t in, int16x8_t bar) {
    return vqrdmulhq_laneq_s16_asm<BAR_LANE>(in, bar);
  }

  template <int MOD_LANE>
  inline int16x8_t use_esti(int16x8_t lhalf, int16x8_t esti, int16x8_t mod) {
    return vmlsq_laneq_s16_asm<MOD_LANE>(lhalf, esti, mod);
  }

  template <int COEF_LANE, int BAR_LANE, int MOD_LANE>
  inline int16x8_t multiply(int16x8_t in, int16x8_t coef, int16x8_t bar, int16x8_t mod) {
    int16x8_t esti = gen_esti<BAR_LANE>(in, bar);
    int16x8_t lhalf = vmulq_laneq_s16_asm<COEF_LANE>(in, coef);
    return use_esti<MOD_LANE>(lhalf, esti, mod);
  }

  template <int SHIFT, int BAR_LANE, int MOD_LANE>
  inline int16x8_t shift_left(int16x8_t in, int16x8_t bar, int16x8_t mod) {
    int16x8_t esti = gen_esti<BAR_LANE>(in, bar);
    int16x8_t lhalf = vshlq_n_s16(in, SHIFT);
    return use_esti<MOD_LANE>(lhalf, esti, mod);
  }

  template <int _1_BAR_LANE, int MOD_LANE>
  inline int16x8_t reduce(int16x8_t in, int16x8_t _1_bar, int16x8_t mod) {
    int16x8_t esti = gen_esti<_1_BAR_LANE>(in, _1_bar);
    return use_esti<MOD_LANE>(in, esti, mod);
  }

  inline int16x8_t gen_esti_crude_redc(int16x8_t in) {
    int16x8_t esti = vshrq_n_s16(in, 12);
    int16x8_t sgn_bit = vshrq_n_s16(in, 15);
    return vsubq_s16(esti, sgn_bit);
  }

  template <int MOD_LANE>
  inline int16x8_t crude_redc(int16x8_t in, int16x8_t mod) {
    int16x8_t esti = gen_esti_crude_redc(in);
    return use_esti<MOD_LANE>(in, esti, mod);
  }
}

namespace xpower::lhalf {
  template <int COEF_LANE>
  inline int16x8_t multiply(int16x8_t in, int16x8_t coef) {
    return vmulq_laneq_s16_asm<COEF_LANE>(in, coef);
  }

  template <int COEF_LANE>
  inline int16x8_t mul_add(int16x8_t acc, int16x8_t in, int16x8_t coef) {
    return vmlaq_laneq_s16_asm<COEF_LANE>(acc, in, coef);
  }

  template <int COEF_LANE>
  inline int16x8_t mul_sub(int16x8_t acc, int16x8_t in, int16x8_t coef) {
    return vmlsq_laneq_s16_asm<COEF_LANE>(acc, in, coef);
  }
}

#endif // ZQ_NEON_H
