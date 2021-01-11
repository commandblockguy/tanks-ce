#ifndef TANKS_GAME_H
#define TANKS_GAME_H

#include "objects/tank.h"

bool start_mission(const serialized_tank_t *ser_tanks); //Start a mission and reset various tank things.
uint8_t play_level(const void *comp_tiles, const serialized_tank_t *ser_tanks);
uint8_t play_mission(const serialized_tank_t *ser_tanks);

#endif //TANKS_GAME_H
