#include "rqmul.h"

#include <cstdint>
#include <arm_neon.h>

#include "consts.h"
#include "zq_neon.h"
#include "mainmul.h"
#include "lowmul.h"

namespace xpower::rqmul {

  void crt(int16_t out_full[1528]) {
    int16x8_t consts = {shared::q, shared::_1_bar, 0, 0, 0, 0, 0, 0};

    for (int i = 760; i < 1520; i += 8) {
      int16x8_t a = vld1q_s16(&out_full[i]);
      a = barret::reduce<1, 0>(a, consts, consts);
      vst1q_s16(&out_full[i], a);
    }
    {
      // TODO: use better scaler instructions
      int32_t esti = (int32_t(out_full[1520]) * shared::_1_bar + (1 << 14)) >> 15;
      out_full[1520] -= esti * shared::q;
    }

    for (int i = 0; i < 760; i += 8) {
      int16x8_t a = vld1q_s16(&out_full[i + 760]);
      int16x8_t b = vld1q_s16(&out_full[i + 761]);
      int16x8_t c = vld1q_s16(&out_full[i]);
      vst1q_s16(&out_full[i], vaddq_s16(c, vaddq_s16(a, b)));
    }
    out_full[0] -= out_full[760];
    out_full[760] += out_full[1520];
  }

  const int16_t inv_170 = -27;
  const int16_t inv_170_bar = sntrup761::utils::gen_bar(inv_170);
  void scale_freeze(int16_t out_full[1528], int16_t out_poly[768]) {
    int16x8_t qs = vdupq_n_s16(shared::q);
    int16x8_t half_qs = vdupq_n_s16((shared::q - 1) / 2);
    int16x8_t neg_half_qs = vdupq_n_s16(-(shared::q - 1) / 2);
    int16x8_t consts = {inv_170, inv_170_bar, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < 768; i += 8 * 4) {
      int16x8x4_t chunks = vld1q_s16_x4(&out_full[i]);

      chunks.val[0] = barret::multiply<0, 1, 0>(chunks.val[0], consts, consts, qs);
      chunks.val[1] = barret::multiply<0, 1, 0>(chunks.val[1], consts, consts, qs);
      chunks.val[2] = barret::multiply<0, 1, 0>(chunks.val[2], consts, consts, qs);
      chunks.val[3] = barret::multiply<0, 1, 0>(chunks.val[3], consts, consts, qs);

      chunks.val[0] = vsubq_s16(chunks.val[0], vandq_s16(vreinterpretq_s16_u16(vcgtq_s16(chunks.val[0], half_qs)), qs));
      chunks.val[1] = vsubq_s16(chunks.val[1], vandq_s16(vreinterpretq_s16_u16(vcgtq_s16(chunks.val[1], half_qs)), qs));
      chunks.val[2] = vsubq_s16(chunks.val[2], vandq_s16(vreinterpretq_s16_u16(vcgtq_s16(chunks.val[2], half_qs)), qs));
      chunks.val[3] = vsubq_s16(chunks.val[3], vandq_s16(vreinterpretq_s16_u16(vcgtq_s16(chunks.val[3], half_qs)), qs));

      chunks.val[0] = vaddq_s16(chunks.val[0], vandq_s16(vreinterpretq_s16_u16(vcltq_s16(chunks.val[0], neg_half_qs)), qs));
      chunks.val[1] = vaddq_s16(chunks.val[1], vandq_s16(vreinterpretq_s16_u16(vcltq_s16(chunks.val[1], neg_half_qs)), qs));
      chunks.val[2] = vaddq_s16(chunks.val[2], vandq_s16(vreinterpretq_s16_u16(vcltq_s16(chunks.val[2], neg_half_qs)), qs));
      chunks.val[3] = vaddq_s16(chunks.val[3], vandq_s16(vreinterpretq_s16_u16(vcltq_s16(chunks.val[3], neg_half_qs)), qs));

      vst1q_s16_x4(&out_poly[i], chunks);
    }
  }

  void rqmul(const int16_t in1_poly[768], const int16_t in2_poly[768], int16_t out_poly[768]) {
    static int16_t out_full[1528];

    mainmul::mainmul(in1_poly, in2_poly, out_full);
    lowmul::lowmul(in1_poly, in2_poly, out_full);
    crt(out_full);
    scale_freeze(out_full, out_poly);
  }
}

