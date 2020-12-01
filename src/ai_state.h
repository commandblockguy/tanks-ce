#ifndef TANKS_AI_STATE_H
#define TANKS_AI_STATE_H

#include <stdbool.h>
#include <stdint.h>
#include "util.h"

typedef struct {
    bool clockwise;
} ai_fire_random_state_t;

typedef struct {} ai_fire_current_state_t;

typedef struct {
    bool scan_dir; //0 = X, 1 = Y
    uint8_t scan_pos;
} ai_fire_reflect_state_t;

typedef struct {} ai_fire_future_state_t;

typedef union {
    ai_fire_random_state_t random;
    ai_fire_current_state_t current;
    ai_fire_reflect_state_t reflect;
    ai_fire_future_state_t future;
} ai_fire_state_t;

typedef struct {
    uint8_t cur_dir;
} ai_move_random_state_t;

typedef struct {} ai_move_toward_state_t;

typedef struct {
    uint target_x;
    uint target_y;
} ai_move_away_state_t;

typedef union ai_move {
    ai_move_random_state_t random;
    ai_move_toward_state_t toward;
    ai_move_away_state_t away;
} ai_move_state_t;

#endif //TANKS_AI_STATE_H
