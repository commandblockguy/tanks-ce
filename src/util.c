#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#undef NDEBUG
#include <debug.h>
#include "util.h"
#include "graphx.h"

void gen_lookups(void);

int24_t sin_table[64];

bool lookups_set = false;

void gen_lookups(void) {
	uint8_t i;
	for(i = 0; i < 64; i++) {
		sin_table[i] = sin(i * M_PI / 128) * 256;
	}
	lookups_set = true;
}

int24_t fast_sin(angle_t angle) {
    //todo: fix angle
    angle = angle >> 16;
	if(angle < 64) return sin_table[angle];
	if(angle == 64) return 256;
	if(angle < 128) return sin_table[128 - angle];
	if(angle < 192) return -sin_table[angle - 128];
	if(angle == 192) return -256;
				  return -sin_table[256 - angle];
}

int24_t qmul(int24_t a, int24_t b) {
    return (a >> 12) * (b >> 11);
}

int24_t qdiv(int24_t a, int24_t b) {
    int24_t a1 = a * (1 << 7);
    int24_t d = (a1 / b);
    return d * (1 << 16);
}

int24_t nabs(int24_t x) {
    if(x > 0) x = -x;
    return x;
}

// Credit: https://geekshavefeelings.com/posts/fixed-point-atan2
uint24_t fast_atan2(int24_t y, int24_t x) {
    if (x == y) { // x/y or y/x would return -1 since 1 isn't representable
        if (y > 0) { // 1/8
            return DEGREES_TO_ANGLE(360 / 8);
        } else if (y < 0) { // 5/8
            return DEGREES_TO_ANGLE(360 / 8 * 5);
        } else { // x = y = 0
            return 0;
        }
    }
    int24_t nabs_y = nabs(y);
    int24_t nabs_x = nabs(x);
    if (nabs_x < nabs_y) { // octants 1, 4, 5, 8
        int24_t y_over_x = qdiv(y, x);
        int24_t correction = qmul(Q_FROM_FLOAT(0.273 * M_1_PI), nabs(y_over_x));
        int24_t unrotated = qmul(Q_FROM_FLOAT(0.25 + 0.273 * M_1_PI) + correction, y_over_x);
        if (x > 0) { // octants 1, 8
            return unrotated;
        } else { // octants 4, 5
            return DEGREES_TO_ANGLE(180) + unrotated;
        }
    } else { // octants 2, 3, 6, 7
        int24_t x_over_y = qdiv(x, y);
        int24_t correction = qmul(Q_FROM_FLOAT(0.273 * M_1_PI), nabs(x_over_y));
        int24_t unrotated = qmul(Q_FROM_FLOAT(0.25 + 0.273 * M_1_PI) + correction, x_over_y);
        if (y > 0) { // octants 2, 3
            return DEGREES_TO_ANGLE(90) - unrotated;
        } else { // octants 6, 7
            return DEGREES_TO_ANGLE(270) - unrotated;
        }
    }
}

//ms since last function call
uint24_t fpsCounter(void) {
	uint24_t r;

	//Disable timer temporarily
	timer_Control &= ~TIMER1_ENABLE;

	r = timer_1_Counter / 33;

	timer_1_Counter = 0;
	/* Reenable the timer, set it to the 32768 kHz clock */
	timer_Control |= TIMER1_ENABLE | TIMER1_32K | TIMER1_NOINT | TIMER1_UP;

	return r;
}
