#ifndef TANKS_GAME_H
#define TANKS_GAME_H

#include "objects/tank.h"

// Game status
enum {
    QUIT = 1, NEXT_LEVEL, RETRY, LOSE, ERROR
};

struct game {
    struct level level; //level currently being played
    uint8_t mission; //The mission number, always displayed 1 higher than stored. Also used as an index for levels.
    tile_t tiles[LEVEL_SIZE_Y][LEVEL_SIZE_X]; //Currently active tilemap data
    uint8_t lives; //Number of remaining tanks. This includes the tank that is currently in use, so a value of 1 means that the game will end the next time the tank is hit.
    uint8_t total_kills; //Number of enemy tanks destroyed.
    uint8_t kills[NUM_TANK_TYPES];
    uint8_t num_tanks;
    bool alive_tanks[MAX_NUM_TANKS];
    Tank *player;
    uint24_t tick;
};

extern struct game game;

bool start_mission(const void *comp_tiles, const struct serialized_tank *ser_tanks); //Start a mission and reset various tank things.
uint8_t play_level(const void *comp_tiles, const struct serialized_tank *ser_tanks);
uint8_t play_mission(const void *comp_tiles, const struct serialized_tank *ser_tanks);

#endif //TANKS_GAME_H
