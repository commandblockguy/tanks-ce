#ifndef TANKS_PHYSICS_H
#define TANKS_PHYSICS_H

#include <cstdint>

// Tick / Frame rate
#define TARGET_TICK_RATE 30

enum {
    NONE = 0, UP = 1, DOWN = 2, LEFT = 4, RIGHT = 8
};
typedef uint8_t direction_t;

#endif //TANKS_PHYSICS_H
