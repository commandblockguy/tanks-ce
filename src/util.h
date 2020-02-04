#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#ifndef H_UTIL
#define H_UTIL

//Shifted by 6 bits
typedef uint24_t ufix_t;
typedef int24_t fix_t;
#define SHIFT_AMOUNT 6
#define SHIFT_MASK ((1 << SHIFT_AMOUNT) - 1)
#define float_to_ufix(x)   ((uint24_t)((x) * (1 << SHIFT_AMOUNT)))
#define float_to_fix(x)    (( int24_t)((x) * (1 << SHIFT_AMOUNT)))
#define float_from_ufix(x) ((float)(x) / (1 << SHIFT_AMOUNT))
#define to_ufix(x) ((x) << SHIFT_AMOUNT)
#define from_ufix(x) ((x) >> SHIFT_AMOUNT)
#define to_fix(x) ((fix_t)to_ufix(x))
#define from_fix(x) ((x) / (1 << SHIFT_AMOUNT))
#define multiply_ufix(a, b) (from_ufix(a * b))
#define divide_ufix(a, b) (to_ufix(a)/b)
#define divide_fix(a, b) (to_fix(a)/b)

fix_t fast_sin(uint8_t rot);

float fast_atan2(float x, float y);

uint24_t fpsCounter(void);

#define fast_cos(rot) fast_sin(64 - rot)

#endif