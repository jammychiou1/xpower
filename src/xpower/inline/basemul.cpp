#include <cstdint>
#include <arm_neon.h>
#include <array>

#include "interface/sntrup761.h"
#include "utils/gen_consts.h"
#include "consts.h"
#include "zq_neon.h"

namespace xpower::basemul {
  struct table_entry_t {
    int32_t t0_bar;
    int32_t t1_bar;
    int16_t t0_coef;
    int16_t t1_coef;
    int16_t bar;
    int16_t coef;
  };

  const int16x8_t shared_consts = { shared::q };

  const std::array<std::array<table_entry_t, 9>, 10> table = [] {
    std::array<std::array<table_entry_t, 9>, 10> res = {};
    int16_t w10 = shared::w10;
    int16_t w9 = shared::w9;
    int64_t inv_2 = shared::inv_2;
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 9; j++) {
        if (i % 2 == 0) {
          res[i][j].coef = sntrup761::utils::center_lift(sntrup761::utils::gen_pow(w10, 3 * i) * sntrup761::utils::gen_pow(w9, 5 * j));
          res[i][j].bar = sntrup761::utils::gen_bar(res[i][j].coef);

          res[i][j].t0_coef = inv_2;
          res[i][j].t0_bar = sntrup761::utils::gen_bar_narrow(res[i][j].t0_coef);

          res[i][j].t1_coef = sntrup761::utils::center_lift(inv_2 * sntrup761::utils::gen_pow(w10, 7 * i) * sntrup761::utils::gen_pow(w9, 4 * j));
          res[i][j].t1_bar = sntrup761::utils::gen_bar_narrow(res[i][j].t1_coef);
        }
        else {
          res[i][j].coef = sntrup761::utils::center_lift(sntrup761::utils::gen_pow(w10, i) * sntrup761::utils::gen_pow(w9, j));
          res[i][j].bar = sntrup761::utils::gen_bar(res[i][j].coef);

          res[i][j].t0_bar = sntrup761::utils::gen_bar_narrow(1);
        }
      }
    }
    return res;
  } ();

  const std::array<std::array<int16_t, 8>, 10> twist_table = [] {
    std::array<std::array<int16_t, 8>, 10> res = {};
    int16_t w10 = shared::w10;
    int64_t inv_2 = shared::inv_2;
    for (int i = 0; i < 10; i++) {
      if (i % 2 == 0) {
        res[i][0] = sntrup761::utils::center_lift(sntrup761::utils::gen_pow(w10, 3 * i));
        res[i][1] = sntrup761::utils::gen_bar(res[i][0]);

        res[i][2] = sntrup761::utils::center_lift(int64_t(-956) * inv_2 * sntrup761::utils::gen_pow(w10, i));
        res[i][3] = sntrup761::utils::gen_bar(res[i][2]);

        res[i][4] = sntrup761::utils::center_lift(int64_t(-956) * inv_2 * sntrup761::utils::gen_pow(w10, 8 * i));
        res[i][5] = sntrup761::utils::gen_bar(res[i][4]);
      }
      else {
        res[i][0] = sntrup761::utils::center_lift(sntrup761::utils::gen_pow(w10, i));
        res[i][1] = sntrup761::utils::gen_bar(res[i][0]);

        res[i][2] = sntrup761::utils::center_lift(int64_t(-956) * sntrup761::utils::gen_pow(w10, i));
        res[i][3] = sntrup761::utils::gen_bar(res[i][2]);
      }

      res[i][6] = shared::q;
      res[i][7] = shared::q_prim;
    }
    return res;
  } ();

  template <int LANE>
  inline void mla_col_8(int16x8_t col, int16x8_t scl, int32x4_t &acc0, int32x4_t &acc1) {
    acc0 = vmlal_laneq_s16(acc0, vget_low_s16(col), scl, LANE);
    acc1 = vmlal_high_laneq_s16(acc1, col, scl, LANE);
  }

  template <int LANE>
  inline void mls_col_8(int16x8_t col, int16x8_t scl, int32x4_t &acc0, int32x4_t &acc1) {
    acc0 = vmlsl_laneq_s16(acc0, vget_low_s16(col), scl, LANE);
    acc1 = vmlsl_high_laneq_s16(acc1, col, scl, LANE);
  }

  inline void mla_tmv_8(int16x8_t tm_row, int16x8_t tm_col, int16x8_t vec,
      int32x4_t &acc0, int32x4_t &acc1) {

    mla_col_8<0>(tm_col, vec, acc0, acc1);
    mla_col_8<1>(vextq_s16(tm_row, tm_col, 7), vec, acc0, acc1);
    mla_col_8<2>(vextq_s16(tm_row, tm_col, 6), vec, acc0, acc1);
    mla_col_8<3>(vextq_s16(tm_row, tm_col, 5), vec, acc0, acc1);
    mla_col_8<4>(vextq_s16(tm_row, tm_col, 4), vec, acc0, acc1);
    mla_col_8<5>(vextq_s16(tm_row, tm_col, 3), vec, acc0, acc1);
    mla_col_8<6>(vextq_s16(tm_row, tm_col, 2), vec, acc0, acc1);
    mla_col_8<7>(vextq_s16(tm_row, tm_col, 1), vec, acc0, acc1);
  }

  inline void mls_tmv_8(int16x8_t tm_row, int16x8_t tm_col, int16x8_t vec,
      int32x4_t &acc0, int32x4_t &acc1) {

    mls_col_8<0>(tm_col, vec, acc0, acc1);
    mls_col_8<1>(vextq_s16(tm_row, tm_col, 7), vec, acc0, acc1);
    mls_col_8<2>(vextq_s16(tm_row, tm_col, 6), vec, acc0, acc1);
    mls_col_8<3>(vextq_s16(tm_row, tm_col, 5), vec, acc0, acc1);
    mls_col_8<4>(vextq_s16(tm_row, tm_col, 4), vec, acc0, acc1);
    mls_col_8<5>(vextq_s16(tm_row, tm_col, 3), vec, acc0, acc1);
    mls_col_8<6>(vextq_s16(tm_row, tm_col, 2), vec, acc0, acc1);
    mls_col_8<7>(vextq_s16(tm_row, tm_col, 1), vec, acc0, acc1);
  }

  inline void karatsuba(
      int16x8_t a0, int16x8_t a1, int16x8_t b0, int16x8_t b1, int i, int j,
      int16x8_t &c0, int16x8_t &c1) {

    int16x8_t consts = vld1q_s16((int16_t *) &table[i][j]);

    int16x8_t tb0 = barret::multiply<7, 6, 0>(b0, consts, consts, shared_consts);
    int16x8_t tb1 = barret::multiply<7, 6, 0>(b1, consts, consts, shared_consts);

    int32x4_t acc0 = {};
    int32x4_t acc1 = {};

    mla_tmv_8(tb1, b0, vaddq_s16(a0, a1), acc0, acc1);

    int32x4_t acc2 = acc0;
    int32x4_t acc3 = acc1;

    mla_tmv_8(vsubq_s16(tb0, tb1), vsubq_s16(tb1, b0), a1, acc0, acc1);
    mls_tmv_8(vsubq_s16(tb1, b0), vsubq_s16(b0, b1), a0, acc2, acc3);

    c0 = barret::reduce_narrow<0, 0>(acc0, acc1, vreinterpretq_s32_s16(consts), shared_consts);
    c1 = barret::reduce_narrow<0, 0>(acc2, acc3, vreinterpretq_s32_s16(consts), shared_consts);
  }

  // |i| needs to be even
  inline void radix2(
      int16x8_t a0, int16x8_t a1, int16x8_t b0, int16x8_t b1, int i, int j,
      int16x8_t &c0, int16x8_t &c1) {

    int16x8_t consts = vld1q_s16((int16_t *) &table[i][j]);

    int16x8_t sqta1 = barret::multiply<7, 6, 0>(a1, consts, consts, shared_consts);
    int16x8_t sqtb1 = barret::multiply<7, 6, 0>(b1, consts, consts, shared_consts);

    int16x8_t ha0 = vaddq_s16(a0, sqta1);
    int16x8_t ha1 = vsubq_s16(a0, sqta1);
    int16x8_t hb0 = vaddq_s16(b0, sqtb1);
    int16x8_t hb1 = vsubq_s16(b0, sqtb1);

    int16x8_t sqthb0 = barret::multiply<7, 6, 0>(hb0, consts, consts, shared_consts);
    int16x8_t sqthb1 = barret::multiply<7, 6, 0>(hb1, consts, consts, shared_consts);
    int16x8_t nsqthb1 = vnegq_s16(sqthb1);

    int32x4_t acc00 = {};
    int32x4_t acc01 = {};

    mla_tmv_8(sqthb0, hb0, ha0, acc00, acc01);

    int32x4_t acc10 = {};
    int32x4_t acc11 = {};

    mla_tmv_8(nsqthb1, hb1, ha1, acc10, acc11);

    int32x4_t acc0 = vaddq_s32(acc00, acc10);
    int32x4_t acc1 = vaddq_s32(acc01, acc11);
    int32x4_t acc2 = vsubq_s32(acc00, acc10);
    int32x4_t acc3 = vsubq_s32(acc01, acc11);

    c0 = barret::multiply_narrow<4, 0, 0>(acc0, acc1, consts, vreinterpretq_s32_s16(consts), shared_consts);
    c1 = barret::multiply_narrow<5, 1, 0>(acc2, acc3, consts, vreinterpretq_s32_s16(consts), shared_consts);
  }

  inline void karatsuba_twist(
      int16x8_t a0, int16x8_t a1, int16x8_t b0, int16x8_t b1, int i,
      int16x8_t &c0, int16x8_t &c1) {

    int16x8_t consts = vld1q_s16(&twist_table[i][0]);

    a0 = barret::crude_redc<6>(a0, consts);
    a1 = barret::crude_redc<6>(a1, consts);
    b0 = barret::crude_redc<6>(b0, consts);
    b1 = barret::crude_redc<6>(b1, consts);

    int16x8_t tb0 = barret::multiply<0, 1, 6>(b0, consts, consts, consts);
    int16x8_t tb1 = barret::multiply<0, 1, 6>(b1, consts, consts, consts);

    int32x4_t acc0 = {};
    int32x4_t acc1 = {};

    mla_tmv_8(tb1, b0, vaddq_s16(a0, a1), acc0, acc1);

    int32x4_t acc2 = acc0;
    int32x4_t acc3 = acc1;

    mla_tmv_8(vsubq_s16(tb0, tb1), vsubq_s16(tb1, b0), a1, acc0, acc1);
    mls_tmv_8(vsubq_s16(tb1, b0), vsubq_s16(b0, b1), a0, acc2, acc3);

    int16x8_t c0_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(acc0), vreinterpretq_s16_s32(acc1));
    int16x8_t c0_hhalf = vuzp2q_s16(vreinterpretq_s16_s32(acc0), vreinterpretq_s16_s32(acc1));
    int16x8_t c1_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(acc2), vreinterpretq_s16_s32(acc3));
    int16x8_t c1_hhalf = vuzp2q_s16(vreinterpretq_s16_s32(acc2), vreinterpretq_s16_s32(acc3));

    c0 = montgomery::redc<7, 6>(c0_lhalf, c0_hhalf, consts, consts);
    c0 = barret::multiply<2, 3, 6>(c0, consts, consts, consts);
    c1 = montgomery::redc<7, 6>(c1_lhalf, c1_hhalf, consts, consts);
    c1 = barret::multiply<2, 3, 6>(c1, consts, consts, consts);
  }

  // |i| needs to be even
  inline void radix2_twist(
      int16x8_t a0, int16x8_t a1, int16x8_t b0, int16x8_t b1, int i,
      int16x8_t &c0, int16x8_t &c1) {

    int16x8_t consts = vld1q_s16(&twist_table[i][0]);

    a0 = barret::crude_redc<6>(a0, consts);
    b0 = barret::crude_redc<6>(b0, consts);
    int16x8_t sqta1 = barret::multiply<0, 1, 6>(a1, consts, consts, consts);
    int16x8_t sqtb1 = barret::multiply<0, 1, 6>(b1, consts, consts, consts);

    int16x8_t ha0 = vaddq_s16(a0, sqta1);
    int16x8_t ha1 = vsubq_s16(a0, sqta1);
    int16x8_t hb0 = vaddq_s16(b0, sqtb1);
    int16x8_t hb1 = vsubq_s16(b0, sqtb1);

    int16x8_t sqthb0 = barret::multiply<0, 1, 6>(hb0, consts, consts, consts);
    int16x8_t sqthb1 = barret::multiply<0, 1, 6>(hb1, consts, consts, consts);
    int16x8_t nsqthb1 = vnegq_s16(sqthb1);

    int32x4_t acc00 = {};
    int32x4_t acc01 = {};

    mla_tmv_8(sqthb0, hb0, ha0, acc00, acc01);

    int32x4_t acc10 = {};
    int32x4_t acc11 = {};

    mla_tmv_8(nsqthb1, hb1, ha1, acc10, acc11);

    int32x4_t acc0 = vaddq_s32(acc00, acc10);
    int32x4_t acc1 = vaddq_s32(acc01, acc11);
    int32x4_t acc2 = vsubq_s32(acc00, acc10);
    int32x4_t acc3 = vsubq_s32(acc01, acc11);

    int16x8_t c0_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(acc0), vreinterpretq_s16_s32(acc1));
    int16x8_t c0_hhalf = vuzp2q_s16(vreinterpretq_s16_s32(acc0), vreinterpretq_s16_s32(acc1));
    int16x8_t c1_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(acc2), vreinterpretq_s16_s32(acc3));
    int16x8_t c1_hhalf = vuzp2q_s16(vreinterpretq_s16_s32(acc2), vreinterpretq_s16_s32(acc3));

    c0 = montgomery::redc<7, 6>(c0_lhalf, c0_hhalf, consts, consts);
    c0 = barret::multiply<2, 3, 6>(c0, consts, consts, consts);
    c1 = montgomery::redc<7, 6>(c1_lhalf, c1_hhalf, consts, consts);
    c1 = barret::multiply<4, 5, 6>(c1, consts, consts, consts);
  }
}
