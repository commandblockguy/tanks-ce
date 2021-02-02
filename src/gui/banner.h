#ifndef TANKS_BANNER_H
#define TANKS_BANNER_H

void update_game_kill_counter(uint8_t kills, bool force);

void display_game_kill_counter();

void display_game_banner(uint8_t mission, uint8_t lives);

#include <cstdint>
#include <string.h>

#endif //TANKS_BANNER_H
