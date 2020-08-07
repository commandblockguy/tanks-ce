#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#ifndef H_UTIL
#define H_UTIL

#undef NDEBUG
#include "debug.h"

// todo: maybe turn this into a function that can be called on a physical calc, printing to serial or to a log
#define printf_(...) dbg_sprintf((char*)dbgout, __VA_ARGS__)

typedef uint24_t angle_t;

/* Converts a constant integer number of degrees to an angle_t */
#define DEGREES_TO_ANGLE(deg) ((angle_t)((uint32_t)(deg) * (uint32_t)0x200000 / 45))

void gen_lookups(void);

int24_t fast_sin(angle_t angle);

#define fast_cos(rot) fast_sin(DEGREES_TO_ANGLE(90) - (rot))

#define TRIG_SCALE 256
#define TRIG_PRECISION_BITS 8
#define TRIG_PRECISION (1 << TRIG_PRECISION_BITS)

uint24_t fast_atan2(int24_t y, int24_t x);

void init_timer(void);

void limit_framerate(void);

void wait_ms_or_keypress(uint24_t ms);

inline uint24_t min(uint24_t a, uint24_t b) { return (a > b) ? b : a; }

inline uint24_t max(uint24_t a, uint24_t b) { return (a > b) ? a : b; }

#endif
