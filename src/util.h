#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#ifndef H_UTIL
#define H_UTIL

#undef NDEBUG
#include "debug.h"

// Since "unsigned int" is far too long
typedef unsigned int uint;

#define INT_BITS (sizeof(int) * 8)

// todo: maybe turn this into a function that can be called on a physical calc, printing to serial or to a log
#define printf_(...) dbg_sprintf((char*)dbgout, __VA_ARGS__)

#define print_line() dbg_sprintf(dbgout, "%s:%u\n", __FILE__, __LINE__)

typedef uint angle_t;

/* Converts a constant integer number of degrees to an angle_t */
#define DEGREES_TO_ANGLE(deg) ((angle_t)((float)(deg) * (1 << (INT_BITS - 3)) / 45))

void gen_lookups(void);

int fast_sin(angle_t angle);
#define fast_cos(rot) fast_sin(DEGREES_TO_ANGLE(90) - (rot))

int fast_sec(angle_t angle);
#define fast_csc(rot) fast_sec(DEGREES_TO_ANGLE(90) - (rot))

#define TRIG_SCALE 256
#define TRIG_PRECISION_BITS 8
#define TRIG_PRECISION (1 << TRIG_PRECISION_BITS)

#ifdef __EZ80__
#define set_timer_Counter(n, x) timer_##n##_Counter = (x)
#define get_timer_Counter(n) timer_##n##_Counter
#define set_timer_ReloadValue(n, x) timer_##n##_ReloadValue = (x)
#endif

uint fast_atan2(int y, int x);

void init_timer(void);

void limit_framerate(void);

void wait_ms_or_keypress(uint ms);

inline uint min(uint a, uint b) { return (a > b) ? b : a; }

inline uint max(uint a, uint b) { return (a > b) ? a : b; }

#endif
