#ifndef TANKS_TRANSITION_H
#define TANKS_TRANSITION_H

#include <cstdint>
#include <string.h>

void mission_start_screen(uint8_t mission, uint8_t lives, uint8_t num_tanks);

void extra_life_screen(uint8_t old_lives);

#endif //TANKS_TRANSITION_H
