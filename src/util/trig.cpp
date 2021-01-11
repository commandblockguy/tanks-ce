#include <math.h>
#include <limits.h>
#include "trig.h"

int sin_table[TRIG_PRECISION / 4];
int sec_table[TRIG_PRECISION / 4];

void gen_lookups() {
    for(uint i = 0; i < TRIG_PRECISION / 4; i++) {
        sin_table[i] = sin(i * M_PI / (TRIG_PRECISION / 2.0)) * TRIG_SCALE;
        sec_table[i] = 1.0 / cos(i * M_PI / (TRIG_PRECISION / 2.0)) * TRIG_SCALE;
    }
}

int fast_sin(angle_t angle) {
    angle >>= INT_BITS - TRIG_PRECISION_BITS;
    if(angle < TRIG_PRECISION / 4) return sin_table[angle];
    if(angle == TRIG_PRECISION / 4) return TRIG_SCALE;
    if(angle < TRIG_PRECISION / 2) return sin_table[TRIG_PRECISION / 2 - angle];
    if(angle < TRIG_PRECISION * 3 / 4) return -sin_table[angle - TRIG_PRECISION / 2];
    if(angle == TRIG_PRECISION * 3 / 4) return -TRIG_SCALE;
    return -sin_table[TRIG_PRECISION - angle];
}

int fast_sec(angle_t angle) {
    angle >>= INT_BITS - TRIG_PRECISION_BITS;
    if(angle < TRIG_PRECISION / 4) return sec_table[angle];
    if(angle == TRIG_PRECISION / 4) return INT_MAX;
    if(angle < TRIG_PRECISION / 2) return -sec_table[TRIG_PRECISION / 2 - angle];
    if(angle < TRIG_PRECISION * 3 / 4) return -sec_table[angle - TRIG_PRECISION / 2];
    if(angle == TRIG_PRECISION * 3 / 4) return INT_MIN;
    return sec_table[TRIG_PRECISION - angle];
}

#define Q_FROM_FLOAT(f) ((int)((float)(f) * (float)(1l << 23l)))

inline int qmul(int a, int b) {
    return (a >> 12) * (b >> 11);
}

inline int qdiv(int a, int b) {
    int a1 = a * (1 << 7);
    int d = (a1 / b);
    return d * (1 << 16);
}

inline int nabs(int x) {
    if(x > 0) x = -x;
    return x;
}

// Credit: https://geekshavefeelings.com/posts/fixed-point-atan2
uint fast_atan2(int y, int x) {
    if(x == y) { // x/y or y/x would return -1 since 1 isn't representable
        if(y > 0) { // 1/8
            return DEGREES_TO_ANGLE(360.0 / 8);
        } else if(y < 0) { // 5/8
            return DEGREES_TO_ANGLE(360.0 * 5 / 8);
        } else { // x = y = 0
            return 0;
        }
    }
    int nabs_y = nabs(y);
    int nabs_x = nabs(x);
    if(nabs_x < nabs_y) { // octants 1, 4, 5, 8
        int y_over_x = qdiv(y, x);
        int correction = qmul(Q_FROM_FLOAT(0.273 * M_1_PI), nabs(y_over_x));
        int unrotated = qmul(Q_FROM_FLOAT(0.25 + 0.273 * M_1_PI) + correction, y_over_x);
        if(x > 0) { // octants 1, 8
            return unrotated;
        } else { // octants 4, 5
            return DEGREES_TO_ANGLE(180) + unrotated;
        }
    } else { // octants 2, 3, 6, 7
        int x_over_y = qdiv(x, y);
        int correction = qmul(Q_FROM_FLOAT(0.273 * M_1_PI), nabs(x_over_y));
        int unrotated = qmul(Q_FROM_FLOAT(0.25 + 0.273 * M_1_PI) + correction, x_over_y);
        if(y > 0) { // octants 2, 3
            return DEGREES_TO_ANGLE(90) - unrotated;
        } else { // octants 6, 7
            return DEGREES_TO_ANGLE(270) - unrotated;
        }
    }
}
