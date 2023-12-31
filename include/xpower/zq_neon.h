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

  template <int BAR_LANE>
  inline int32x4_t gen_esti_32(int32x4_t in, int32x4_t bar) {
    return vqrdmulhq_laneq_s32_asm<BAR_LANE>(in, bar);
  }

  template <int BAR_LANE>
  inline int16x8_t gen_esti_shl(int16x8_t in, int16x8_t bar) {
    return vqdmulhq_laneq_s16_asm<BAR_LANE>(in, bar);
  }

  template <int SHIFT, int BAR_LANE>
  inline int16x8_t gen_esti_precise(int16x8_t in, int16x8_t bar) {
    int16x8_t esti_shl = gen_esti_shl<BAR_LANE>(in, bar);
    return vrshrq_n_s16(esti_shl, SHIFT);
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

  template <int SHIFT, int _1_BAR_LANE, int MOD_LANE>
  inline int16x8_t precise_redc(int16x8_t in, int16x8_t _1_bar, int16x8_t mod) {
    int16x8_t esti = gen_esti<SHIFT, _1_BAR_LANE>(in, _1_bar);
    return use_esti<MOD_LANE>(in, esti, mod);
  }

  template <int _1_BAR_LANE, int MOD_LANE>
  inline int16x8_t reduce_narrow(int32x4_t in1, int32x4_t in2, int32x4_t _1_bar, int16x8_t mod) {
    int32x4_t esti1 = gen_esti_32<_1_BAR_LANE>(in1, _1_bar);
    int32x4_t esti2 = gen_esti_32<_1_BAR_LANE>(in2, _1_bar);
    int16x8_t in_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(in1), vreinterpretq_s16_s32(in2));
    int16x8_t esti_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(esti1), vreinterpretq_s16_s32(esti2));
    return use_esti<MOD_LANE>(in_lhalf, esti_lhalf, mod);
  }

  template <int COEF_LANE, int BAR_LANE, int MOD_LANE>
  inline int16x8_t multiply_narrow(int32x4_t in1, int32x4_t in2, int16x8_t coef, int32x4_t bar, int16x8_t mod) {
    int32x4_t esti1 = gen_esti_32<BAR_LANE>(in1, bar);
    int32x4_t esti2 = gen_esti_32<BAR_LANE>(in2, bar);
    int16x8_t in_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(in1), vreinterpretq_s16_s32(in2));
    int16x8_t esti_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(esti1), vreinterpretq_s16_s32(esti2));
    int16x8_t out_lhalf = vmulq_laneq_s16_asm<COEF_LANE>(in_lhalf, coef);
    return use_esti<MOD_LANE>(out_lhalf, esti_lhalf, mod);
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

namespace xpower::montgomery {
  template <int PRIM_LANE, int MOD_LANE>
  inline int16x8_t redc(int16x8_t lhalf, int16x8_t hhalf, int16x8_t prim, int16x8_t mod) {
    int16x8_t l = vmulq_laneq_s16_asm<PRIM_LANE>(lhalf, prim);
    int16x8_t diff_2x = vqdmulhq_laneq_s16_asm<MOD_LANE>(l, mod);
    int16x8_t diff = vshrq_n_s16(diff_2x, 1);
    return vsubq_s16(hhalf, diff);
  }
}

#endif // ZQ_NEON_H
