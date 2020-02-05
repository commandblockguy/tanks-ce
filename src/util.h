#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#ifndef H_UTIL
#define H_UTIL

int24_t fast_sin(uint8_t rot);
#define fast_cos(rot) fast_sin(64 - rot)
#define TRIG_SCALE 256

float fast_atan2(float x, float y);

uint24_t fpsCounter(void);

#endif