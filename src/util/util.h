#include <stdbool.h>
#include <cstddef>
#include <cstdint>
#include <tice.h>

#ifndef H_UTIL
#define H_UTIL

// Since "unsigned int" is far too long
typedef unsigned int uint;

#define INT_BITS (sizeof(int) * 8)

#define print_line() dbg_printf("%s:%u\n", __FILE__, __LINE__)

enum {
    AXIS_X,
    AXIS_Y
};

void init_timer();

void limit_framerate();

inline uint min(uint a, uint b) { return (a > b) ? b : a; }

inline uint max(uint a, uint b) { return (a > b) ? a : b; }

#endif
