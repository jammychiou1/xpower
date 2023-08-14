#include <cstdint>
#include <arm_neon.h>
#include <array>

#include "inline/main_lay1.cpp"
#include "zq_neon.h"
#include "utils/gen_consts.h"

namespace xpower::main_lay1 {
  const std::array<std::array<int16_t, 8>, 9> table = [] {
    std::array<std::array<int16_t, 8>, 9> res = {};
    int16_t prim_root = 11;
    int16_t w10 = sntrup761::utils::gen_pow(prim_root, (sntrup761::q - 1) / 10);
    for (int j = 0; j < 9; j++) {
      res[j][0] = sntrup761::utils::gen_pow(w10, j + 8);
      res[j][1] = sntrup761::utils::gen_bar(res[j][0]);

      res[j][2] = sntrup761::utils::gen_pow(w10, 2 * (j + 8));
      res[j][3] = sntrup761::utils::gen_bar(res[j][2]);

      res[j][4] = sntrup761::utils::gen_pow(w10, 3 * (j + 8));
      res[j][5] = sntrup761::utils::gen_bar(res[j][4]);

      res[j][6] = sntrup761::utils::gen_pow(w10, 4 * (j + 8));
      res[j][7] = sntrup761::utils::gen_bar(res[j][6]);
    }
    return res;
  } ();

  constexpr int poly_base(int i, int j) {
    return (81 * i + 10 * j) % 90 * 16;
  }

  void fwd_extract(int16_t arr[10][2][9][8], int16_t main_poly[768]) {
    for (int j = 0; j < 9; j++) {
      for (int k0 = 0; k0 < 2; k0++) {
        int16x8_t h0_4x, h1_4x, h2_4x, h3_4x, h4_4x, h5_4x, h6_4x, h7_4x, h8_4x, h9_4x;

        if (j <= 2) {
          int16x8_t f7 = vld1q_s16(&main_poly[poly_base(j + 5, j) + 8 * k0]);
          int16x8_t f8 = vld1q_s16(&main_poly[poly_base(j + 6, j) + 8 * k0]);
          int16x8_t f9 = vld1q_s16(&main_poly[poly_base(j + 7, j) + 8 * k0]);
          int16x8_t f0 = vld1q_s16(&main_poly[poly_base(j + 8, j) + 8 * k0]);
          int16x8_t f1 = vld1q_s16(&main_poly[poly_base(j + 9, j) + 8 * k0]);
          int16x8_t f2 = vld1q_s16(&main_poly[poly_base(j + 10, j) + 8 * k0]);

          ntt10_4x_nof3546(
              f0, f1, f2, f7, f8, f9,
              h0_4x, h1_4x, h2_4x, h3_4x, h4_4x,
              h5_4x, h6_4x, h7_4x, h8_4x, h9_4x);
        }
        else {
          int16x8_t f8 = vld1q_s16(&main_poly[poly_base(j + 6, j) + 8 * k0]);
          int16x8_t f9 = vld1q_s16(&main_poly[poly_base(j + 7, j) + 8 * k0]);
          int16x8_t f0 = vld1q_s16(&main_poly[poly_base(j + 8, j) + 8 * k0]);
          int16x8_t f1 = vld1q_s16(&main_poly[poly_base(j + 9, j) + 8 * k0]);
          int16x8_t f2 = vld1q_s16(&main_poly[poly_base(j + 10, j) + 8 * k0]);

          ntt10_4x_nof35467(
              f0, f1, f2, f8, f9,
              h0_4x, h1_4x, h2_4x, h3_4x, h4_4x,
              h5_4x, h6_4x, h7_4x, h8_4x, h9_4x);
        }

        if (j != 2) {
          int16x8_t twist_consts = vld1q_s16(&table[j][0]);

          h0_4x = barret::reduce<2, 0>(h0_4x, shared_consts, shared_consts);
          h5_4x = barret::reduce<2, 0>(h5_4x, shared_consts, shared_consts);

          h1_4x = barret::multiply<0, 1, 0>(h1_4x, twist_consts, twist_consts, shared_consts);
          h2_4x = barret::multiply<2, 3, 0>(h2_4x, twist_consts, twist_consts, shared_consts);
          h3_4x = barret::multiply<4, 5, 0>(h3_4x, twist_consts, twist_consts, shared_consts);
          h4_4x = barret::multiply<6, 7, 0>(h4_4x, twist_consts, twist_consts, shared_consts);

          h6_4x = barret::multiply<0, 1, 0>(h6_4x, twist_consts, twist_consts, shared_consts);
          h7_4x = barret::multiply<2, 3, 0>(h7_4x, twist_consts, twist_consts, shared_consts);
          h8_4x = barret::multiply<4, 5, 0>(h8_4x, twist_consts, twist_consts, shared_consts);
          h9_4x = barret::multiply<6, 7, 0>(h9_4x, twist_consts, twist_consts, shared_consts);

          if (j % 2 == 1) {
            h5_4x = vnegq_s16(h5_4x);
            h6_4x = vnegq_s16(h6_4x);
            h7_4x = vnegq_s16(h7_4x);
            h8_4x = vnegq_s16(h8_4x);
            h9_4x = vnegq_s16(h9_4x);
          }
        }

        vst1q_s16(&arr[0][k0][j][0], h0_4x);
        vst1q_s16(&arr[1][k0][j][0], h1_4x);
        vst1q_s16(&arr[2][k0][j][0], h2_4x);
        vst1q_s16(&arr[3][k0][j][0], h3_4x);
        vst1q_s16(&arr[4][k0][j][0], h4_4x);
        vst1q_s16(&arr[5][k0][j][0], h5_4x);
        vst1q_s16(&arr[6][k0][j][0], h6_4x);
        vst1q_s16(&arr[7][k0][j][0], h7_4x);
        vst1q_s16(&arr[8][k0][j][0], h8_4x);
        vst1q_s16(&arr[9][k0][j][0], h9_4x);
      }
    }
  }
}
