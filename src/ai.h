#ifndef H_AI
#define H_AI

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "level.h"
#include "tank.h"

void ai_process_move(tank_t *tank);

void ai_process_fire(tank_t *tank);

void move_random(tank_t *tank);

void move_away(tank_t *tank);

void move_toward(tank_t *tank);

tile_t get_tile_at_offset(tank_t *tank, angle_t angle_offset, int24_t distance);

bool raycast(uint24_t startX, uint24_t startY, angle_t angle, line_seg_t *result);

bool pointing_at_target(tank_t *tank, physics_body_t *target, uint8_t max_bounces, bool future);

void point_at_player(tank_t *tank, physics_body_t *target);

void aim_random(tank_t *tank);

void aim_reflect(tank_t *tank);

void aim_current(tank_t *tank);

void aim_future(tank_t *tank);

#endif /* H_AI */