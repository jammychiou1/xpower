#include <cstdint>
#include <arm_neon.h>

#include "interface/sntrup761.h"
#include "consts.h"
#include "zq_neon.h"

namespace xpower::main_lay2 {
  const static int16x8_t ntt9_consts = {
    ntt9::red, ntt9::green, ntt9::blue, 0,
    ntt9::red_bar, ntt9::green_bar, ntt9::blue_bar, ntt9::red_bar_shl2
  };

  const static int16x8_t shared_consts = {
    shared::q, shared::_1_bar_shl12, 0, 0, 0, 0, 0, 0
  };

  inline int16x8_t bar_mul_red(int16x8_t in) {
    return barret::multiply<0, 4, 0>(in, ntt9_consts, ntt9_consts, shared_consts);
  }

  inline int16x8_t bar_mul_green(int16x8_t in) {
    return barret::multiply<1, 5, 0>(in, ntt9_consts, ntt9_consts, shared_consts);
  }

  inline int16x8_t bar_mul_blue(int16x8_t in) {
    return barret::multiply<2, 6, 0>(in, ntt9_consts, ntt9_consts, shared_consts);
  }

  inline int16x8_t bar_crude_redc(int16x8_t in) {
    return barret::crude_redc<0>(in, shared_consts);
  }

  inline int16x8_t bar_redc(int16x8_t in) {
    int16x8_t esti_shl12 = barret::gen_esti_shl<1>(in, shared_consts);
    int16x8_t esti = vrshrq_n_s16(esti_shl12, 12);
    return barret::use_esti<0>(in, esti, shared_consts);
  }

  inline int16x8_t bar_mul_2(int16x8_t in) {
    int16x8_t esti_shl12 = barret::gen_esti_shl<1>(in, shared_consts);
    int16x8_t esti = vrshrq_n_s16(esti_shl12, 11);
    int16x8_t out = vshlq_n_s16(in, 1);
    return barret::use_esti<0>(out, esti, shared_consts);
  }

  inline void bar_redc_mul_2(int16x8_t in, int16x8_t &out_redc, int16x8_t &out_mul_2) {
    int16x8_t esti_shl12 = barret::gen_esti_shl<1>(in, shared_consts);
    int16x8_t esti_redc = vrshrq_n_s16(esti_shl12, 12);
    int16x8_t esti_mul2 = vrshrq_n_s16(esti_shl12, 11);
    out_redc = in;
    out_redc = barret::use_esti<0>(out_redc, esti_redc, shared_consts);
    out_mul_2 = vshlq_n_s16(in, 1);
    out_mul_2 = barret::use_esti<0>(out_mul_2, esti_mul2, shared_consts);
  }

  inline void bar_mla_red_mls_red(int16x8_t in, int16x8_t acc, int16x8_t &out_mla, int16x8_t &out_mls) {
    int16x8_t esti_acc_shl12 = barret::gen_esti_shl<1>(acc, shared_consts);
    int16x8_t esti_acc_shl2 = vrshrq_n_s16(esti_acc_shl12, 10);
    int16x8_t esti_mul_red_shl2 = barret::gen_esti<7>(in, ntt9_consts);
    int16x8_t esti_mla_red = vrshrq_n_s16(vaddq_s16(esti_acc_shl2, esti_mul_red_shl2), 2);
    int16x8_t esti_mls_red = vrshrq_n_s16(vsubq_s16(esti_acc_shl2, esti_mul_red_shl2), 2);
    int16x8_t lhalf_mul_red = lhalf::multiply<0>(in, ntt9_consts);
    out_mla = vaddq_s16(acc, lhalf_mul_red);
    out_mla = barret::use_esti<0>(out_mla, esti_mla_red, shared_consts);
    out_mls = vsubq_s16(acc, lhalf_mul_red);
    out_mls = barret::use_esti<0>(out_mls, esti_mls_red, shared_consts);
  }

  inline void ntt9_2x(
      int16x8_t f0, int16x8_t f1, int16x8_t f2, int16x8_t f3, int16x8_t f4,
      int16x8_t f5, int16x8_t f6, int16x8_t f7, int16x8_t f8,
      int16x8_t &h0_2x, int16x8_t &h1_2x, int16x8_t &h2_2x, int16x8_t &h3_2x, int16x8_t &h4_2x,
      int16x8_t &h5_2x, int16x8_t &h6_2x, int16x8_t &h7_2x, int16x8_t &h8_2x) {

    int16x8_t a0t0i = vaddq_s16(f3, f6);
    int16x8_t a0t1i = vsubq_s16(f3, f6);

    int16x8_t a0o0 = vaddq_s16(f0, a0t0i);
    a0o0 = bar_mul_2(a0o0);
    int16x8_t a0t0o = vshlq_n_s16(f0, 1);
    a0t0o = vsubq_s16(a0t0o, a0t0i);

    int16x8_t a0o1, a0o2;
    bar_mla_red_mls_red(a0t1i, a0t0o, a0o1, a0o2);


    int16x8_t a1t0i = vaddq_s16(f7, f4);
    int16x8_t a1t1i = vsubq_s16(f7, f4);

    int16x8_t a1o0 = vaddq_s16(f1, a1t0i);
    int16x8_t a1t0o = vshlq_n_s16(f1, 1);
    a1t0o = vsubq_s16(a1t0o, a1t0i);
    int16x8_t a1t1o = bar_mul_red(a1t1i);

    int16x8_t a1o1 = vaddq_s16(a1t0o, a1t1o);
    int16x8_t a1o2 = vsubq_s16(a1t0o, a1t1o);


    int16x8_t a2t0i = vaddq_s16(f2, f5);
    int16x8_t a2t1i = vsubq_s16(f2, f5);

    int16x8_t a2o0 = vaddq_s16(f8, a2t0i);
    int16x8_t a2t0o = vshlq_n_s16(f8, 1);
    a2t0o = vsubq_s16(a2t0o, a2t0i);
    int16x8_t a2t1o = bar_mul_red(a2t1i);

    int16x8_t a2o1 = vaddq_s16(a2t0o, a2t1o);
    int16x8_t a2o2 = vsubq_s16(a2t0o, a2t1o);


    int16x8_t b0i0 = a0o0;
    int16x8_t b0i1 = a1o0;
    int16x8_t b0i2 = a2o0;

    int16x8_t b0t0i = vaddq_s16(b0i1, b0i2);
    int16x8_t b0t1i = vsubq_s16(b0i1, b0i2);

    b0t0i = bar_redc(b0t0i);
    h0_2x = vaddq_s16(b0i0, vshlq_n_s16(b0t0i, 1));
    int16x8_t b0t0o = vsubq_s16(b0i0, b0t0i);
    int16x8_t b0t1o = bar_mul_red(b0t1i);

    h3_2x = vaddq_s16(b0t0o, b0t1o);
    h6_2x = vsubq_s16(b0t0o, b0t1o);


    int16x8_t b1i0 = a0o1;
    int16x8_t b1i1 = bar_mul_green(a2o1);
    int16x8_t b1i2 = bar_mul_blue(a1o2);

    int16x8_t b1t0i = vaddq_s16(b1i1, b1i2);
    int16x8_t b1t1i = vsubq_s16(b1i1, b1i2);

    b1t0i = bar_redc(b1t0i);
    h1_2x = vaddq_s16(b1i0, vshlq_n_s16(b1t0i, 1));
    int16x8_t b1t0o = vsubq_s16(b1i0, b1t0i);
    int16x8_t b1t1o = bar_mul_red(b1t1i);

    h7_2x = vaddq_s16(b1t0o, b1t1o);
    h4_2x = vsubq_s16(b1t0o, b1t1o);


    int16x8_t b2i0 = a0o2;
    int16x8_t b2i1 = bar_mul_green(a1o1);
    int16x8_t b2i2 = bar_mul_blue(a2o2);

    int16x8_t b2t0i = vaddq_s16(b2i1, b2i2);
    int16x8_t b2t1i = vsubq_s16(b2i1, b2i2);

    b2t0i = bar_redc(b2t0i);
    h8_2x = vaddq_s16(b2i0, vshlq_n_s16(b2t0i, 1));
    int16x8_t b2t0o = vsubq_s16(b2i0, b2t0i);
    int16x8_t b2t1o = bar_mul_red(b2t1i);

    h2_2x = vaddq_s16(b2t0o, b2t1o);
    h5_2x = vsubq_s16(b2t0o, b2t1o);
  }

  inline void intt9_18x(
      int16x8_t h0, int16x8_t h1, int16x8_t h2, int16x8_t h3, int16x8_t h4,
      int16x8_t h5, int16x8_t h6, int16x8_t h7, int16x8_t h8,
      int16x8_t &f0_18x, int16x8_t &f1_18x, int16x8_t &f2_18x, int16x8_t &f3_18x, int16x8_t &f4_18x,
      int16x8_t &f5_18x, int16x8_t &f6_18x, int16x8_t &f7_18x, int16x8_t &f8_18x) {

    ntt9_2x(
        h0, h8, h7, h6, h5, h4, h3, h2, h1,
        f0_18x, f1_18x, f2_18x, f3_18x, f4_18x,
        f5_18x, f6_18x, f7_18x, f8_18x);
  }
}
