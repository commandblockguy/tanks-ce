#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <tice.h>

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"
#include "objects.h"
#include "level.h"
#include "collision.h"
#include "util.h"
#include "debug.h"

extern Game game;
extern Tank* tanks;

const uint8_t max_shells[]  = 	{5, 1, 1, 1, 1, 3, 2, 5, 5, 2};
const uint8_t max_mines[]   = 	{2, 0, 0, 0, 4, 0, 0, 2, 2, 2};
const uint8_t max_bounces[] = 	{1, 1, 1, 0, 1, 1, 2, 1, 1, 0};

bool fire_shell(Tank* tank) {
	int i;
	for(i = max_shells[tank->type] - 1; i >= 0; i--) {
		Shell* shell = &tank->shells[i];
		fix_t vector_x, vector_y;

		if(shell->alive) continue;
		
		shell->alive = true;
		shell->left_tank_hitbox = false;
		shell->bounces = max_bounces[tank->type];

		vector_x = fast_cos(tank->barrel_rot);
		vector_y = fast_sin(tank->barrel_rot);

		shell->phys.position_x = center_x(&tank->phys) + BARREL_LENGTH * vector_x;
		shell->phys.position_y = center_y(&tank->phys) + BARREL_LENGTH * vector_y;
		
		shell->phys.width = shell->phys.height = SHELL_SIZE;
		shell->phys.type = ShellPhysics;
		if(tank->type == MISSILE || tank->type == IMMOB_MISSILE) {
			shell->phys.velocity_x = SHELL_SPEED_MISSILE * vector_x;
			shell->phys.velocity_y = SHELL_SPEED_MISSILE * vector_y;
		} else {
			shell->phys.velocity_x = SHELL_SPEED_STANDARD * vector_x;
			shell->phys.velocity_y = SHELL_SPEED_STANDARD * vector_y;
		}
		return true;
	}
	return false;
}

bool lay_mine(Tank* tank) {
	int i;
	if(!max_mines[tank->type]) return false;
	for(i = max_mines[tank->type] - 1; i >= 0; i--) {
		Mine* mine = &tank->mines[i];
		if(mine->alive) continue;
		mine->alive = true;
		mine->countdown = MINE_COUNTDOWN;
		mine->phys.position_x = tank->phys.position_x + to_ufix(TANK_SIZE - MINE_SIZE) / 2;
		mine->phys.position_y = tank->phys.position_y + to_ufix(TANK_SIZE - MINE_SIZE) / 2;
		mine->phys.width = mine->phys.height = MINE_SIZE;
		return true;
	}
	return false;
}

void calc_bullet_spawn(Tank* tank) {
	tank->bullet_spawn_x = from_ufix(BARREL_LENGTH * fast_cos(tank->barrel_rot));
	tank->bullet_spawn_y = from_ufix(BARREL_LENGTH * fast_sin(tank->barrel_rot));
}

void detonate(Mine* mine, uint8_t* tiles) {
	int j, k;
	
	mine->alive = false;
	mine->countdown = EXPLOSION_ANIM - 1;

	//The original game uses a radius, not a square
	//Don't tell anyone.

	for(j  = ptToXTile(center_x(&mine->phys) - float_to_ufix(MINE_EXPLOSION_RADIUS));
		j <= ptToXTile(center_x(&mine->phys) + float_to_ufix(MINE_EXPLOSION_RADIUS)); j++)
	{
		if(j < 0 || j > LEVEL_SIZE_X) continue;
		for(k  = ptToYTile(center_y(&mine->phys) - float_to_ufix(MINE_EXPLOSION_RADIUS));
			k <= ptToYTile(center_y(&mine->phys) + float_to_ufix(MINE_EXPLOSION_RADIUS)); k++)
		{
			if(k < 0 || k > LEVEL_SIZE_Y) continue;
			if(tiles[j + LEVEL_SIZE_X * k] == DESTRUCTIBLE) tiles[j + LEVEL_SIZE_X * k] = DESTROYED;
		}
	}

	for(j = 0; j < game.level.num_tanks; j++) {
		Tank* tank = &tanks[j];
		if(tank->alive && center_distance_lt(&mine->phys, &tank->phys, float_to_ufix(MINE_EXPLOSION_RADIUS))) {
			tank->alive = false;
		}
		for(k = 0; k < max_shells[tank->type]; k++) {
			Shell* shell = &tank->shells[k];
			if(shell->alive && center_distance_lt(&mine->phys, &shell->phys, float_to_ufix(MINE_EXPLOSION_RADIUS))) {
				shell->alive = false;
			}
		}
		for(k = 0; k < max_mines[tank->type]; k++) {
			Mine* mine2 = &tank->mines[k];
			if(mine2->alive && center_distance_lt(&mine->phys, &mine2->phys, float_to_ufix(MINE_EXPLOSION_RADIUS))) {
				detonate(mine2, tiles);
			}
		}
	}
}

bool shell_ricochet(Shell* shell, Direction dir) {
	//Determine if shell explodes here, and subtracts 1 from the bounces left
	if(!shell->bounces--) {
		shell->alive = false;
		return false;
	}
	//Shell is still alive
	if(dir & UP || dir & DOWN)
		shell->phys.velocity_y *= -1;
	if(dir & LEFT || dir & RIGHT)
		shell->phys.velocity_x *= -1;
	return true;
}
