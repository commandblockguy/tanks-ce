#ifndef H_AI
#define H_AI

#include <stdbool.h>
#include <cstddef>
#include <cstdint>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <cstdlib>
#include <string.h>

#include "../level.h"
#include "../objects/tank.h"

void ai_process_move(Tank *tank);

void ai_process_fire(Tank *tank);

void move_random(Tank *tank);

void move_away(Tank *tank);

void move_toward(Tank *tank);

tile_t get_tile_at_offset(Tank *tank, angle_t angle_offset, int distance);

bool pointing_at_target(Tank *tank, PhysicsBody *target, uint8_t max_bounces, bool future);

void point_at_player(Tank *tank, PhysicsBody *target);

void aim_random(Tank *tank);

void aim_reflect(Tank *tank);

void aim_current(Tank *tank);

void aim_future(Tank *tank);

#endif /* H_AI */