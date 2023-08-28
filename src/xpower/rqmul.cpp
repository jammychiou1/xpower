#include "rqmul.h"

#include <cstdint>
#include <arm_neon.h>

#include "consts.h"
#include "zq_neon.h"
#include "mainmul.h"
#include "lowmul.h"

namespace xpower::rqmul {
  void crt(int16_t out_poly[768], int16_t out_main[1441], int16_t out_low[81]) {
    out_main[1440] = out_main[0];
    for (int i = 0; i < 680; i += 8) {
      int16x8_t a = vld1q_s16(&out_main[i + 760]);
      int16x8_t b = vld1q_s16(&out_main[i + 761]);
      int16x8_t c;
      c = vaddq_s16(a, b);
      vst1q_s16(&out_poly[i], c);
    }
    for (int i = 680; i < 760; i += 8) {
      int16x8_t a = vld1q_s16(&out_main[i - 680]);
      int16x8_t b = vld1q_s16(&out_main[i - 679]);
      int16x8_t c;
      c = vaddq_s16(a, b);
      vst1q_s16(&out_poly[i], c);
    }
    out_poly[760] = out_main[80];
    out_poly[0] -= out_main[760];
    for (int i = 81; i < 761; i += 8) {
      int16x8_t a = vld1q_s16(&out_main[i]);
      int16x8_t c = vld1q_s16(&out_poly[i]);
      c = vaddq_s16(c, a);
      vst1q_s16(&out_poly[i], c);
    }
    for (int i = 680; i < 760; i += 8) {
      int16x8_t a = vld1q_s16(&out_low[i - 680]);
      int16x8_t b = vld1q_s16(&out_low[i - 679]);
      a = vaddq_s16(a, b);
      int16x8_t c = vld1q_s16(&out_poly[i]);
      c = vsubq_s16(c, a);
      vst1q_s16(&out_poly[i], c);
    }
    out_poly[679] -= out_low[0];
    out_poly[760] -= out_low[80];
    for (int i = 0; i < 80; i += 8) {
      int16x8_t a = vld1q_s16(&out_low[i]);
      int16x8_t c = vld1q_s16(&out_poly[i]);
      c = vaddq_s16(c, a);
      vst1q_s16(&out_poly[i], c);
    }
    out_poly[80] += out_low[80];
  }

  const int16_t inv_170 = -27;
  const int16_t inv_170_bar = sntrup761::utils::gen_bar(inv_170);
  void scale_freeze(int16_t out_poly[768]) {
    int16x8_t qs = vdupq_n_s16(shared::q);
    int16x8_t half_qs = vdupq_n_s16((shared::q - 1) / 2);
    int16x8_t neg_half_qs = vdupq_n_s16(-(shared::q - 1) / 2);
    int16x8_t consts = {inv_170, inv_170_bar, 0, 0, 0, 0, 0, 0};

    for (int i = 0; i < 768; i += 8 * 4) {
      int16x8x4_t chunks = vld1q_s16_x4(&out_poly[i]);

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
    static int16_t out_main[1441];
    static int16_t out_low[96];

    mainmul::mainmul(in1_poly, in2_poly, out_main);
    lowmul::lowmul(in1_poly, in2_poly, out_low);
    crt(out_poly, out_main, out_low);
    scale_freeze(out_poly);
  }
}

