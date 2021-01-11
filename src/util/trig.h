#ifndef TANKS_TRIG_H
#define TANKS_TRIG_H

#include "util.h"

typedef uint angle_t;

/* Converts a constant integer number of degrees to an angle_t */
#define DEGREES_TO_ANGLE(deg) ((angle_t)((float)(deg) * (1 << (INT_BITS - 3)) / 45))

void gen_lookups();

int fast_sin(angle_t angle);
#define fast_cos(rot) fast_sin(DEGREES_TO_ANGLE(90) - (rot))

int fast_sec(angle_t angle);
#define fast_csc(rot) fast_sec(DEGREES_TO_ANGLE(90) - (rot))

#define TRIG_SCALE 256
#define TRIG_PRECISION_BITS 8
#define TRIG_PRECISION (1 << TRIG_PRECISION_BITS)

uint fast_atan2(int y, int x);

#endif //TANKS_TRIG_H
