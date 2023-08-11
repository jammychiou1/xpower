#include <cstdint>
#include <arm_neon.h>
#include <array>

#include "interface/sntrup761.h"
#include "utils/gen_consts.h"
#include "zq_neon.h"

namespace xpower::shared {
  const int16_t q_prim = 15631;
}
namespace xpower::basemul {
  constexpr std::array<std::array<std::pair<int16_t, int16_t>, 9>, 10> table = [] {
    std::array<std::array<std::pair<int16_t, int16_t>, 9>, 10> res = {};
    int16_t prim_root = 11;
    int16_t w10 = sntrup761::utils::gen_pow(prim_root, (sntrup761::q - 1) / 10);
    int16_t w9 = sntrup761::utils::gen_pow(prim_root, (sntrup761::q - 1) / 9);
    for (int i = 0; i < 10; i++) {
      for (int j = 0; j < 9; j++) {
        res[i][j].first = sntrup761::utils::center_lift(sntrup761::utils::gen_pow(w10, i) * sntrup761::utils::gen_pow(w9, j));
        res[i][j].second = sntrup761::utils::gen_bar(res[i][j].first);
      }
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

  inline void main_schoolbook(
      int16x8_t a0, int16x8_t a1, int16x8_t b0, int16x8_t b1, int i, int j,
      int16x8_t &c0, int16x8_t &c1) {

    int16_t tw = table[i][j].first;
    int16_t tw_bar = table[i][j].second;

    int16x8_t consts = {tw, tw_bar, sntrup761::q, shared::q_prim, 0, 0, 0, 0};

    a0 = barret::crude_redc<2>(a0, consts);
    a1 = barret::crude_redc<2>(a1, consts);
    b0 = barret::crude_redc<2>(b0, consts);
    b1 = barret::crude_redc<2>(b1, consts);

    int16x8_t tb0 = barret::multiply<0, 1, 2>(b0, consts, consts, consts);
    int16x8_t tb1 = barret::multiply<0, 1, 2>(b1, consts, consts, consts);

    int32x4_t acc0 = {};
    int32x4_t acc1 = {};
    int32x4_t acc2 = {};
    int32x4_t acc3 = {};

    mla_tmv_8(tb1, b0, a0, acc0, acc1);
    mla_tmv_8(tb1, b0, a1, acc2, acc3);
    mla_tmv_8(b0, b1, a0, acc2, acc3);
    mla_tmv_8(tb0, tb1, a1, acc0, acc1);

    int16x8_t c0_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(acc0), vreinterpretq_s16_s32(acc1));
    int16x8_t c0_hhalf = vuzp2q_s16(vreinterpretq_s16_s32(acc0), vreinterpretq_s16_s32(acc1));
    int16x8_t c1_lhalf = vuzp1q_s16(vreinterpretq_s16_s32(acc2), vreinterpretq_s16_s32(acc3));
    int16x8_t c1_hhalf = vuzp2q_s16(vreinterpretq_s16_s32(acc2), vreinterpretq_s16_s32(acc3));

    c0 = xpower::montgomery::redc<3, 2>(c0_lhalf, c0_hhalf, consts, consts);
    c1 = xpower::montgomery::redc<3, 2>(c1_lhalf, c1_hhalf, consts, consts);
  }

  inline void main_karatsuba(
      int16x8_t a0, int16x8_t a1, int16x8_t b0, int16x8_t b1, int i, int j,
      int16x8_t &c0, int16x8_t &c1) {

    int16_t tw = table[i][j].first;
    int16_t tw_bar = table[i][j].second;

    int16x8_t consts = {tw, tw_bar, sntrup761::q, shared::q_prim, 0, 0, 0, 0};

    a0 = barret::crude_redc<2>(a0, consts);
    a1 = barret::crude_redc<2>(a1, consts);
    b0 = barret::crude_redc<2>(b0, consts);
    b1 = barret::crude_redc<2>(b1, consts);

    int16x8_t tb0 = barret::multiply<0, 1, 2>(b0, consts, consts, consts);
    int16x8_t tb1 = barret::multiply<0, 1, 2>(b1, consts, consts, consts);

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

    c0 = xpower::montgomery::redc<3, 2>(c0_lhalf, c0_hhalf, consts, consts);
    c1 = xpower::montgomery::redc<3, 2>(c1_lhalf, c1_hhalf, consts, consts);
  }
}
