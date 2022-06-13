#include <stdbool.h>
#include <cstddef>
#include <cstdint>

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

// Division-by-repeated-subtraction
// Faster than regular division when the result is less than about 150
// About 10x faster than regular division when the result is 1, and 5x faster when the result is 20
inline uint8_t fast_div(uint24_t num, uint24_t den) {
    uint8_t result;
    asm("xor\ta,a\n"
        "loop%=:\n"
        "\tinc\ta\n"
        "\tsbc\thl,de\n"
        "\tjq\tnc,loop%=\n"
        "\tdec\ta\n"
    : "=a" (result)
    : "l" (num), "e" (den)
    : "cc");
    return result;
}

extern uint8_t div256_24_buf[4];
inline uint24_t div256_24(uint24_t num) {
    uint24_t result;
    asm("ld\t(%2),hl\n"
        "\tld\thl,(%2 + 1)"
    : "+l" (result)
    : "l" (num), "iyl" (div256_24_buf)
    :);
    return result;
}

inline uint min(uint a, uint b) { return (a > b) ? b : a; }

inline uint max(uint a, uint b) { return (a > b) ? a : b; }

#endif
