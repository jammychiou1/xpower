#include "low_lay1.h"

#include <cstdint>
#include <arm_neon.h>
#include <array>

#include "inline/low_lay1.cpp"
#include "zq_neon.h"
#include "utils/gen_consts.h"

namespace xpower::low_lay1 {
  void fwd_extract(int16_t arr[10][2][8], const int16_t low_poly[80]) {
    for (int k0 = 0; k0 < 2; k0++) {
      int16x8_t h0_4x, h1_4x, h2_4x, h3_4x, h4_4x, h5_4x, h6_4x, h7_4x, h8_4x, h9_4x;

      int16x8_t f8 = vld1q_s16(&low_poly[8 * k0]);
      int16x8_t f9 = vld1q_s16(&low_poly[16 + 8 * k0]);
      int16x8_t f0 = vld1q_s16(&low_poly[32 + 8 * k0]);
      int16x8_t f1 = vld1q_s16(&low_poly[48 + 8 * k0]);
      int16x8_t f2 = vld1q_s16(&low_poly[64 + 8 * k0]);

      ntt10_4x_nof35467(
          f0, f1, f2, f8, f9,
          h0_4x, h1_4x, h2_4x, h3_4x, h4_4x,
          h5_4x, h6_4x, h7_4x, h8_4x, h9_4x);

      vst1q_s16(&arr[0][k0][0], h0_4x);
      vst1q_s16(&arr[1][k0][0], h1_4x);
      vst1q_s16(&arr[2][k0][0], h2_4x);
      vst1q_s16(&arr[3][k0][0], h3_4x);
      vst1q_s16(&arr[4][k0][0], h4_4x);
      vst1q_s16(&arr[5][k0][0], h5_4x);
      vst1q_s16(&arr[6][k0][0], h6_4x);
      vst1q_s16(&arr[7][k0][0], h7_4x);
      vst1q_s16(&arr[8][k0][0], h8_4x);
      vst1q_s16(&arr[9][k0][0], h9_4x);
    }
  }

  void bwd_insert(int16_t arr[10][2][8], int16_t out_full[1528]) {
    for (int k0 = 0; k0 < 2; k0++) {
      int16x8_t f0_40x, f1_40x, f2_40x, f3_40x, f4_40x, f5_40x, f6_40x, f7_40x, f8_40x, f9_40x;

      int16x8_t h0 = vld1q_s16(&arr[0][k0][0]);
      int16x8_t h1 = vld1q_s16(&arr[1][k0][0]);
      int16x8_t h2 = vld1q_s16(&arr[2][k0][0]);
      int16x8_t h3 = vld1q_s16(&arr[3][k0][0]);
      int16x8_t h4 = vld1q_s16(&arr[4][k0][0]);
      int16x8_t h5 = vld1q_s16(&arr[5][k0][0]);
      int16x8_t h6 = vld1q_s16(&arr[6][k0][0]);
      int16x8_t h7 = vld1q_s16(&arr[7][k0][0]);
      int16x8_t h8 = vld1q_s16(&arr[8][k0][0]);
      int16x8_t h9 = vld1q_s16(&arr[9][k0][0]);

      intt10_40x_nof3456(h0, h1, h2, h3, h4, h5, h6, h7, h8, h9,
          f0_40x, f1_40x, f2_40x, f7_40x, f8_40x, f9_40x);

      int16x8_t g0 = vld1q_s16(&out_full[     1440 + 8 * k0]);
      int16x8_t g1 = vld1q_s16(&out_full[16 + 1440 + 8 * k0]);
      int16x8_t g2 = vld1q_s16(&out_full[32 + 1440 + 8 * k0]);
      int16x8_t g3 = vld1q_s16(&out_full[48 + 1440 + 8 * k0]);
      int16x8_t g4 = vld1q_s16(&out_full[64 + 1440 + 8 * k0]);

      vst1q_s16(&out_full[     8 * k0], f7_40x);
      vst1q_s16(&out_full[16 + 8 * k0], f8_40x);
      vst1q_s16(&out_full[32 + 8 * k0], f9_40x);
      vst1q_s16(&out_full[48 + 8 * k0], f0_40x);
      vst1q_s16(&out_full[64 + 8 * k0], f1_40x);

      vst1q_s16(&out_full[     1440 + 8 * k0], vsubq_s16(g0, f7_40x));
      vst1q_s16(&out_full[16 + 1440 + 8 * k0], vsubq_s16(g1, f8_40x));
      vst1q_s16(&out_full[32 + 1440 + 8 * k0], vsubq_s16(g2, f9_40x));
      vst1q_s16(&out_full[48 + 1440 + 8 * k0], vsubq_s16(g3, f0_40x));
      vst1q_s16(&out_full[64 + 1440 + 8 * k0], vsubq_s16(g4, f1_40x));

      if (k0 == 0) {
        out_full[80] = vgetq_lane_s16(f2_40x, 0);
        // out_full[1520] -= out_full[80];
      }
    }
  }
}
