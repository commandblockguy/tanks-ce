#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#ifndef H_UTIL
#define H_UTIL

typedef uint24_t angle_t;

/* Converts a constant integer number of degrees to an angle_t */
#define DEGREES_TO_ANGLE(deg) ((angle_t)((uint32_t)(deg) * (uint32_t)0x200000 / 45))

int24_t fast_sin(angle_t angle);
#define fast_cos(rot) fast_sin(DEGREES_TO_ANGLE(90) - rot)

#define TRIG_SCALE 256

float fast_atan2(float x, float y);

uint24_t fpsCounter(void);

#endif