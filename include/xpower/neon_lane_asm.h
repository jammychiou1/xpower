#ifndef NEON_LANE_ASM_H
#define NEON_LANE_ASM_H

#include <cstdint>

template <int LANE>
inline int16x8_t vqrdmulhq_laneq_s16_asm(int16x8_t v1, int16x8_t v2) {
  int16x8_t vd;
  asm ("sqrdmulh %[vd].8h, %[v1].8h, %[v2].h[%[lane]]"
      : [vd] "=w" (vd)
      : [v1] "w" (v1), [v2] "x" (v2), [lane] "I" (LANE));
  return vd;
}

template <int LANE>
inline int16x8_t vqdmulhq_laneq_s16_asm(int16x8_t v1, int16x8_t v2) {
  int16x8_t vd;
  asm ("sqdmulh %[vd].8h, %[v1].8h, %[v2].h[%[lane]]"
      : [vd] "=w" (vd)
      : [v1] "w" (v1), [v2] "x" (v2), [lane] "I" (LANE));
  return vd;
}

template <int LANE>
inline int16x8_t vmulq_laneq_s16_asm(int16x8_t v1, int16x8_t v2) {
  int16x8_t vd;
  asm ("mul %[vd].8h, %[v1].8h, %[v2].h[%[lane]]"
      : [vd] "=w" (vd)
      : [v1] "w" (v1), [v2] "x" (v2), [lane] "I" (LANE));
  return vd;
}

template <int LANE>
inline int16x8_t vmlaq_laneq_s16_asm(int16x8_t vd, int16x8_t v1, int16x8_t v2) {
  asm ("mla %[vd].8h, %[v1].8h, %[v2].h[%[lane]]"
      : [vd] "+w" (vd)
      : [v1] "w" (v1), [v2] "x" (v2), [lane] "I" (LANE));
  return vd;
}

template <int LANE>
inline int16x8_t vmlsq_laneq_s16_asm(int16x8_t vd, int16x8_t v1, int16x8_t v2) {
  asm ("mls %[vd].8h, %[v1].8h, %[v2].h[%[lane]]"
      : [vd] "+w" (vd)
      : [v1] "w" (v1), [v2] "x" (v2), [lane] "I" (LANE));
  return vd;
}

#endif // NEON_LANE_ASM_H
