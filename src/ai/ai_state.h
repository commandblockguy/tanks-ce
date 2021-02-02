#ifndef TANKS_AI_STATE_H
#define TANKS_AI_STATE_H

#include <stdbool.h>
#include <cstdint>
#include "../util/util.h"

struct ai_fire_random_state {
    bool clockwise;
};

struct ai_fire_current_state {};

struct ai_fire_reflect_state {
    uint8_t scan_dir; //0 = X, 1 = Y
    uint8_t scan_pos;
};

struct ai_fire_future_state {};

union ai_fire_state {
    struct ai_fire_random_state random;
    struct ai_fire_current_state current;
    struct ai_fire_reflect_state reflect;
    struct ai_fire_future_state future;
};

struct ai_move_random_state {
    uint8_t cur_dir;
};

struct ai_move_toward_state {};

struct ai_move_away_state {
    uint target_x;
    uint target_y;
};

union ai_move_state {
    struct ai_move_random_state random;
    struct ai_move_toward_state toward;
    struct ai_move_away_state away;
};

#endif //TANKS_AI_STATE_H
