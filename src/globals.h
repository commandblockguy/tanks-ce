#ifndef TANKS_GLOBALS_H
#define TANKS_GLOBALS_H

#include <cstdint>
#include "objects/tank.h"
#include "level.h"

// Game status
enum {
    QUIT = 1, NEXT_LEVEL, RETRY, LOSE, ERROR
};

typedef struct {
    level_t level; //level currently being played
    uint8_t mission; //The mission number, always displayed 1 higher than stored. Also used as an index for levels.
    uint8_t lives; //Number of remaining tanks. This includes the tank that is currently in use, so a value of 1 means that the game will end the next time the tank is hit.
    uint8_t total_kills; //Number of enemy tanks destroyed.
    uint8_t kills[NUM_TANK_TYPES];
    uint8_t shot_cooldown; //How many more ticks before we can fire another shot
    uint8_t mine_cooldown;
    bool player_alive;
    uint8_t num_tanks;
    bool alive_tanks[MAX_NUM_TANKS];
    Tank *player;
    uint24_t tick;
} game_t;

extern tile_t tiles[LEVEL_SIZE_Y][LEVEL_SIZE_X];
extern game_t game;

#endif //TANKS_GLOBALS_H
