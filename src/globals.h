#ifndef TANKS_GLOBALS_H
#define TANKS_GLOBALS_H

#include <stdint.h>
#include "tank.h"
#include "level.h"

typedef struct {
    level_t level; //level currently being played
    uint8_t mission; //The mission number, always displayed 1 higher than stored. Also used as an index for levels.
    uint8_t lives; //Number of remaining tanks. This includes the tank that is currently in use, so a value of 1 means that the game will end the next time the tank is hit.
    uint8_t total_kills; //Number of enemy tanks destroyed.
    uint8_t kills[NUM_TANK_TYPES];
    uint24_t timer; //Game time, probably used for physics stuff.
    uint16_t cursor_x; //If I decide to implement a cursor mode, this will represent the position of the crosshairs on the screen.
    uint8_t cursor_y;  //TODO: Otherwise, this will be removed
    uint24_t lastCycle; //Time the last physics cycle started
    uint8_t status; //Game status enum
    uint8_t shotCooldown; //How many more ticks before we can fire another shot
    uint8_t mineCooldown;
} game_t;

extern tank_t* tanks;
extern tile_t tiles[LEVEL_SIZE_Y][LEVEL_SIZE_X];
extern game_t game;

#endif //TANKS_GLOBALS_H
