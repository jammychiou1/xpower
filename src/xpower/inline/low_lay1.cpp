#include <cstdint>
#include <arm_neon.h>

#include "interface/sntrup761.h"
#include "zq_neon.h"
#include "consts.h"

namespace xpower::low_lay1 {
  const static int16x8_t ntt5_consts = {
    ntt5::red, ntt5::green, ntt5::blue, ntt5::yellow,
    ntt5::red_bar, ntt5::green_bar, ntt5::blue_bar, ntt5::yellow_bar
  };

  const static int16x8_t shared_consts = {
    shared::q, shared::_4_bar, shared::_1_bar, 0, 0, 0, 0, 0
  };

  inline int16x8_t bar_mul_red(int16x8_t in) {
    return barret::multiply<0, 4, 0>(in, ntt5_consts, ntt5_consts, shared_consts);
  }

  inline int16x8_t bar_mul_green(int16x8_t in) {
    return barret::multiply<1, 5, 0>(in, ntt5_consts, ntt5_consts, shared_consts);
  }

  inline int16x8_t bar_mul_blue(int16x8_t in) {
    return barret::multiply<2, 6, 0>(in, ntt5_consts, ntt5_consts, shared_consts);
  }

  inline int16x8_t bar_mul_yellow(int16x8_t in) {
    return barret::multiply<3, 7, 0>(in, ntt5_consts, ntt5_consts, shared_consts);
  }

  inline int16x8_t bar_mul_4(int16x8_t in) {
    return barret::shift_left<2, 1, 0>(in, shared_consts, shared_consts);
  }

  inline int16x8_t bar_crude_redc(int16x8_t in) {
    return barret::crude_redc<0>(in, shared_consts);
  }

  inline int16x8_t bar_use_esti(int16x8_t lhalf, int16x8_t esti) {
    return barret::use_esti<0>(lhalf, esti, shared_consts);
  }

  inline int16x8_t bar_esti_green(int16x8_t in) {
    return barret::gen_esti<5>(in, ntt5_consts);
  }

  inline int16x8_t bar_esti_blue(int16x8_t in) {
    return barret::gen_esti<6>(in, ntt5_consts);
  }

  inline int16x8_t bar_esti_yellow(int16x8_t in) {
    return barret::gen_esti<7>(in, ntt5_consts);
  }

  inline int16x8_t lhalf_mul_green(int16x8_t in) {
    return lhalf::multiply<1>(in, ntt5_consts);
  }

  inline int16x8_t lhalf_mls_blue(int16x8_t acc, int16x8_t in) {
    return lhalf::mul_sub<2>(acc, in, ntt5_consts);
  }

  inline int16x8_t lhalf_mul_yellow(int16x8_t in) {
    return lhalf::multiply<3>(in, ntt5_consts);
  }

  inline void nega_part(int16x8_t ni0, int16x8_t ni1, int16x8_t &no0_n, int16x8_t &no1_n) {
    int16x8_t nt0i = ni0;
    int16x8_t nt1i = vaddq_s16(ni0, ni1);
    int16x8_t nt2i = ni1;

    int16x8_t nt0o_lhalf = lhalf_mul_green(nt0i);
    int16x8_t nt2o_lhalf = lhalf_mul_yellow(nt2i);
    int16x8_t nt0o_esti = bar_esti_green(nt0i);
    int16x8_t nt2o_esti = bar_esti_yellow(nt2i);

    int16x8_t no0_lhalf_n = vsubq_s16(nt2o_lhalf, nt0o_lhalf);
    int16x8_t no1_lhalf_n = vaddq_s16(nt2o_lhalf, nt0o_lhalf);
    int16x8_t no0_esti_n = vsubq_s16(nt2o_esti, nt0o_esti);
    int16x8_t no1_esti_n = vaddq_s16(nt2o_esti, nt0o_esti);

    int16x8_t nt1_esti = bar_esti_blue(nt1i);

    no1_esti_n = vsubq_s16(no1_esti_n, nt1_esti);
    no1_lhalf_n = lhalf_mls_blue(no1_lhalf_n, nt1i);

    no0_n = bar_use_esti(no0_lhalf_n, no0_esti_n);
    no1_n = bar_use_esti(no1_lhalf_n, no1_esti_n);
  }

  inline void ntt5_4x_nof14(
      int16x8_t f0, int16x8_t f2, int16x8_t f3,
      int16x8_t &h0_4x, int16x8_t &h1_4x, int16x8_t &h2_4x, int16x8_t &h3_4x, int16x8_t &h4_4x) {

    int16x8_t ci1 = vaddq_s16(f2, f3);

    int16x8_t ct0i = ci1;
    int16x8_t h0 = vaddq_s16(f0, ct0i);
    h0_4x = bar_mul_4(h0);
    int16x8_t f0_4x = vshlq_n_s16(f0, 2);

    int16x8_t ct0o = vsubq_s16(f0_4x, ct0i);
    ct0o = bar_crude_redc(ct0o);

    int16x8_t ct1i_n = ci1;

    int16x8_t ct1o_n = bar_mul_red(ct1i_n);

    int16x8_t co0 = vsubq_s16(ct0o, ct1o_n);
    int16x8_t co1 = vaddq_s16(ct0o, ct1o_n);

    int16x8_t ni1 = vsubq_s16(f2, f3);

    int16x8_t nt1i = ni1;
    int16x8_t nt2i = ni1;

    int16x8_t nt1o = bar_mul_blue(nt1i);
    int16x8_t nt2o = bar_mul_yellow(nt2i);

    int16x8_t no0_n = nt2o;
    int16x8_t no1_n = vsubq_s16(nt2o, nt1o);

    h1_4x = vsubq_s16(co0, no0_n);
    h3_4x = vsubq_s16(co1, no1_n);
    h4_4x = vaddq_s16(co0, no0_n);
    h2_4x = vaddq_s16(co1, no1_n);
  }

  inline void ntt5_4x_nof023(
      int16x8_t f1, int16x8_t f4,
      int16x8_t &h0_4x, int16x8_t &h1_4x, int16x8_t &h2_4x, int16x8_t &h3_4x, int16x8_t &h4_4x) {

    int16x8_t ci0 = vaddq_s16(f1, f4);

    int16x8_t ct0i = ci0;
    int16x8_t h0 = ct0i;
    h0_4x = bar_mul_4(h0);

    int16x8_t ct0o = vnegq_s16(ct0i);
    ct0o = bar_crude_redc(ct0o);

    int16x8_t ct1i = ci0;

    int16x8_t ct1o = bar_mul_red(ct1i);

    int16x8_t co0 = vaddq_s16(ct0o, ct1o);
    int16x8_t co1 = vsubq_s16(ct0o, ct1o);

    int16x8_t ni0 = vsubq_s16(f1, f4);

    int16x8_t nt0i = ni0;
    int16x8_t nt1i = ni0;

    int16x8_t nt0o = bar_mul_green(nt0i);
    int16x8_t nt1o = bar_mul_blue(nt1i);

    int16x8_t no0 = nt0o;
    int16x8_t no1 = vsubq_s16(nt1o, nt0o);

    h1_4x = vaddq_s16(co0, no0);
    h3_4x = vaddq_s16(co1, no1);
    h4_4x = vsubq_s16(co0, no0);
    h2_4x = vsubq_s16(co1, no1);
  }

  inline void ntt5_4x_noh04(
      int16x8_t f0, int16x8_t f1, int16x8_t f2, int16x8_t f3, int16x8_t f4,
      int16x8_t &h1_4x, int16x8_t &h2_4x, int16x8_t &h3_4x) {

    int16x8_t ci0 = vaddq_s16(f1, f4);
    int16x8_t ci1 = vaddq_s16(f2, f3);

    int16x8_t ct0i = vaddq_s16(ci0, ci1);
    int16x8_t f0_4x = vshlq_n_s16(f0, 2);

    int16x8_t ct0o = vsubq_s16(f0_4x, ct0i);
    ct0o = bar_crude_redc(ct0o);

    int16x8_t ct1i = vsubq_s16(ci0, ci1);

    int16x8_t ct1o = bar_mul_red(ct1i);

    int16x8_t co0 = vaddq_s16(ct0o, ct1o);
    int16x8_t co1 = vsubq_s16(ct0o, ct1o);

    int16x8_t ni0 = vsubq_s16(f1, f4);
    int16x8_t ni1 = vsubq_s16(f2, f3);

    int16x8_t nt0i = vaddq_s16(ni1, ni0);
    int16x8_t nt1i = ni1;
    int16x8_t nt2i = vsubq_s16(ni1, ni0);

    int16x8_t nt0o = bar_mul_green(nt0i);
    int16x8_t nt1o = bar_mul_blue(nt1i);
    int16x8_t nt2o = bar_mul_yellow(nt2i);

    int16x8_t no0 = vsubq_s16(nt0o, nt1o);
    int16x8_t no1 = vsubq_s16(nt2o, nt1o);

    h1_4x = vaddq_s16(co0, no0);
    h2_4x = vaddq_s16(co1, no1);
    h3_4x = vsubq_s16(co1, no1);
  }

  inline void ntt5_4x_noh23(
      int16x8_t f0, int16x8_t f1, int16x8_t f2, int16x8_t f3, int16x8_t f4,
      int16x8_t &h0_4x, int16x8_t &h1_4x, int16x8_t &h4_4x) {

    int16x8_t ci0 = vaddq_s16(f1, f4);
    int16x8_t ci1 = vaddq_s16(f2, f3);

    int16x8_t ct0i = vaddq_s16(ci0, ci1);
    int16x8_t h0 = vaddq_s16(f0, ct0i);
    h0_4x = bar_mul_4(h0);
    int16x8_t f0_4x = vshlq_n_s16(f0, 2);

    int16x8_t ct0o = vsubq_s16(f0_4x, ct0i);
    ct0o = bar_crude_redc(ct0o);

    int16x8_t ct1i = vsubq_s16(ci0, ci1);

    int16x8_t ct1o = bar_mul_red(ct1i);

    int16x8_t co0 = vaddq_s16(ct0o, ct1o);

    int16x8_t ni0 = vsubq_s16(f1, f4);
    int16x8_t ni1 = vsubq_s16(f2, f3);

    int16x8_t nt0i = vaddq_s16(ni1, ni0);
    int16x8_t nt1i = ni1;

    int16x8_t nt0o = bar_mul_green(nt0i);
    int16x8_t nt1o = bar_mul_blue(nt1i);

    int16x8_t no0 = vsubq_s16(nt0o, nt1o);

    h1_4x = vaddq_s16(co0, no0);
    h4_4x = vsubq_s16(co0, no0);
  }

  inline void ntt10_4x_nof35467(
      int16x8_t f0, int16x8_t f1, int16x8_t f2, int16x8_t f8, int16x8_t f9,
      int16x8_t &h0_4x, int16x8_t &h1_4x, int16x8_t &h2_4x, int16x8_t &h3_4x, int16x8_t &h4_4x,
      int16x8_t &h5_4x, int16x8_t &h6_4x, int16x8_t &h7_4x, int16x8_t &h8_4x, int16x8_t &h9_4x) {

    int16x8_t h00_4x, h10_4x, h20_4x, h30_4x, h40_4x;
    ntt5_4x_nof14(f0, f2, f8, h00_4x, h10_4x, h20_4x, h30_4x, h40_4x);
    int16x8_t h01_4x, h11_4x, h21_4x, h31_4x, h41_4x;
    ntt5_4x_nof023(f1, f9, h01_4x, h11_4x, h21_4x, h31_4x, h41_4x);
    h0_4x = vaddq_s16(h00_4x, h01_4x);
    h5_4x = vsubq_s16(h00_4x, h01_4x);
    h2_4x = vaddq_s16(h10_4x, h11_4x);
    h7_4x = vsubq_s16(h10_4x, h11_4x);
    h4_4x = vaddq_s16(h20_4x, h21_4x);
    h9_4x = vsubq_s16(h20_4x, h21_4x);
    h6_4x = vaddq_s16(h30_4x, h31_4x);
    h1_4x = vsubq_s16(h30_4x, h31_4x);
    h8_4x = vaddq_s16(h40_4x, h41_4x);
    h3_4x = vsubq_s16(h40_4x, h41_4x);
  }

  inline void ntt10_4x_noh3456(
      int16x8_t f0, int16x8_t f1, int16x8_t f2, int16x8_t f3, int16x8_t f4,
      int16x8_t f5, int16x8_t f6, int16x8_t f7, int16x8_t f8, int16x8_t f9,
      int16x8_t &h0_4x, int16x8_t &h1_4x, int16x8_t &h2_4x,
      int16x8_t &h7_4x, int16x8_t &h8_4x, int16x8_t &h9_4x) {

    int16x8_t f00 = vaddq_s16(f0, f5);
    int16x8_t f10 = vaddq_s16(f6, f1);
    int16x8_t f20 = vaddq_s16(f2, f7);
    int16x8_t f30 = vaddq_s16(f8, f3);
    int16x8_t f40 = vaddq_s16(f4, f9);
    ntt5_4x_noh23(f00, f10, f20, f30, f40, h0_4x, h2_4x, h8_4x);
    int16x8_t f01 = vsubq_s16(f0, f5);
    int16x8_t f11 = vsubq_s16(f6, f1);
    int16x8_t f21 = vsubq_s16(f2, f7);
    int16x8_t f31 = vsubq_s16(f8, f3);
    int16x8_t f41 = vsubq_s16(f4, f9);
    ntt5_4x_noh04(f01, f11, f21, f31, f41, h7_4x, h9_4x, h1_4x);
  }

  inline void intt10_40x_nof3456(
      int16x8_t h0, int16x8_t h1, int16x8_t h2, int16x8_t h3, int16x8_t h4,
      int16x8_t h5, int16x8_t h6, int16x8_t h7, int16x8_t h8, int16x8_t h9,
      int16x8_t &f0_40x, int16x8_t &f1_40x, int16x8_t &f2_40x,
      int16x8_t &f7_40x, int16x8_t &f8_40x, int16x8_t &f9_40x) {

    ntt10_4x_noh3456(h0, h9, h8, h7, h6, h5, h4, h3, h2, h1,
        f0_40x, f1_40x, f2_40x, f7_40x, f8_40x, f9_40x);
  }
}

