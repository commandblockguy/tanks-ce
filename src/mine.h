#ifndef TANKS_MINE_H
#define TANKS_MINE_H

#include <stdbool.h>
#include <intce.h>
#include "collision.h"

#define MINE_SIZE TILE_SIZE

//10 seconds until detonation
#define MINE_COUNTDOWN (10 * TARGET_TICK_RATE + EXPLOSION_ANIM)
//2 seconds spent pulsing
#define MINE_WARNING (2 * TARGET_TICK_RATE + EXPLOSION_ANIM)
//TODO: better data for this
//time after a enemy enters the range of a mine
#define MINE_TRIGGERED (TARGET_TICK_RATE * 2 / 5 + EXPLOSION_ANIM)
//2/15ths of a second per pulse
#define PULSE_TIME ((uint8_t)(2 * TARGET_TICK_RATE / 15))

//Amount of time the explosion takes
//1/2 second in the original, may reduce to save sprite size
#define EXPLOSION_ANIM (TARGET_TICK_RATE / 2)

//TODO:
#define MINE_DETECT_RANGE (2 * TILE_SIZE)

//120 pixels / 48 px/tile = 2.5 tiles
#define MINE_EXPLOSION_RADIUS (2.5 * TILE_SIZE)

typedef struct {
    bool alive; //Whether this mine is processed
    physics_body_t phys;
    uint24_t countdown; //Number of physics loops until explosions occur
} mine_t;

void detonate(mine_t *mine);

#endif //TANKS_MINE_H
