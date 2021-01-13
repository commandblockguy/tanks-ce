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

// Division-by-repeated-subtraction
// Faster than regular division when the result is less than about 150
// About 10x faster than regular division when the result is 1, and 5x faster when the result is 20
inline uint8_t fast_div(uint24_t num, uint24_t den) {
    uint8_t result;
    asm("xor a,a\n"
        "loop%=:\n"
        "\tinc a\n"
        "\tsbc hl,de\n"
        "\tjq nc,loop%=\n"
        "\tdec a\n"
    : "=a" (result)
    : "l" (num), "e" (den)
    : "cc");
    return result;
}

inline uint min(uint a, uint b) { return (a > b) ? b : a; }

inline uint max(uint a, uint b) { return (a > b) ? a : b; }

#endif
