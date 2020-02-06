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

#include "constants.h"
#include "level.h"
#include "objects.h"

void ai_process_move(Tank* tank);
void ai_process_fire(Tank* tank);

void move_random(Tank* tank);
void move_away(Tank* tank);
void move_toward(Tank* tank);

bool raycast(uint24_t startX, uint24_t startY, angle_t angle, LineSeg* result);
bool pointingAtTarget(Tank* tank, PhysicsBody* target, uint8_t max_bounces, bool future);
void pointAtPlayer(Tank *tank, PhysicsBody *target);

void aim_random(Tank* tank);

void aim_reflect(Tank* tank);

void aim_current(Tank* tank);

void aim_future(Tank* tank);

#endif /* H_AI */