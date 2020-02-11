#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#ifndef H_UTIL
#define H_UTIL

/* *is immediately stabbed* */
#define max(a, b) a < b ? a : b

typedef uint24_t angle_t;

/* Converts a constant integer number of degrees to an angle_t */
#define DEGREES_TO_ANGLE(deg) ((angle_t)((uint32_t)(deg) * (uint32_t)0x200000 / 45))

#define Q_FROM_FLOAT(f) ((int24_t)((float)(f) * (float)(1l << 23l)))
int24_t qmul(int24_t a, int24_t b);
int24_t qdiv(int24_t a, int24_t b);

int24_t fast_sin(angle_t angle);
#define fast_cos(rot) fast_sin(DEGREES_TO_ANGLE(90) - rot)

#define TRIG_SCALE 256

uint24_t fast_atan2(int24_t y, int24_t x);

uint24_t fpsCounter(void);

#endif