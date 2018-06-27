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

extern Game game;
extern Tank* tanks;

const uint8_t max_shells[]  = 	{5, 1, 1, 1, 1, 3, 2, 5, 5, 2};
const uint8_t max_mines[]   = 	{2, 0, 0, 0, 4, 0, 0, 2, 2, 2};
const uint8_t max_bounces[] = 	{1, 1, 1, 0, 1, 1, 2, 1, 1, 0};

bool fire_shell(Tank* tank) {
	int i;
	for(i = max_shells[tank->type] - 1; i >= 0; i--) {
		Shell* shell = &tank->shells[i];
		float vector_x, vector_y;
		if(shell->alive) continue;
		shell->alive = true;
		shell->left_tank_hitbox = false;
		shell->bounces = max_bounces[tank->type];
		vector_x = cos(tank->barrel_rot * ROT_UNITS_TO_RADIANS);
		vector_y = sin(tank->barrel_rot * ROT_UNITS_TO_RADIANS);
		shell->phys.position_x = tank->phys.position_x + (TANK_SIZE / 2 + BARREL_LENGTH * vector_x) * (1 << SHIFT_AMOUNT);
		shell->phys.position_y = tank->phys.position_y + (TANK_SIZE / 2 + BARREL_LENGTH * vector_y) * (1 << SHIFT_AMOUNT);
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
		mine->phys.position_x = tank->phys.position_x + ((TANK_SIZE - MINE_SIZE) << SHIFT_AMOUNT) / 2;
		mine->phys.position_y = tank->phys.position_y + ((TANK_SIZE - MINE_SIZE) << SHIFT_AMOUNT) / 2;
		mine->phys.width = mine->phys.height = MINE_SIZE;
		return true;
	}
	return false;
}

void calc_bullet_spawn(Tank* tank) {
	tank->bullet_spawn_x = BARREL_LENGTH * cos(tank->barrel_rot * ROT_UNITS_TO_RADIANS);
	tank->bullet_spawn_y = BARREL_LENGTH * sin(tank->barrel_rot * ROT_UNITS_TO_RADIANS);
}

//TODO: fix this ugly mess
void detonate(Mine* mine, uint8_t* tiles) {
	int j, k;
	//TODO: explosions
	mine->alive = false;
	//Mines and tiles must be the same size for this to work
	//(this is Bad Code)
	for(j = ((mine->phys.position_x >> SHIFT_AMOUNT) - MINE_EXPLOSION_RADIUS - MAP_OFFSET_X) / TILE_SIZE; j <= ((mine->phys.position_x >> SHIFT_AMOUNT) + MINE_EXPLOSION_RADIUS - MAP_OFFSET_X) / TILE_SIZE; j++) {
		for(k = ((mine->phys.position_y >> SHIFT_AMOUNT) - MINE_EXPLOSION_RADIUS) / TILE_SIZE; k <= ((mine->phys.position_y >> SHIFT_AMOUNT) + MINE_EXPLOSION_RADIUS) / TILE_SIZE; k++) {
			if(tiles[j + LEVEL_SIZE_X * k] == DESTRUCTIBLE) tiles[j + LEVEL_SIZE_X * k] = DESTROYED;
		}
	}
	for(j = 0; j < game.level.num_tanks; j++) {
		Tank* tank = &tanks[j];
		AABB bb1 = getAABB(&mine->phys);
		AABB bb2 = getAABB(&tank->phys);
		if(!tank->alive) continue;
		if(center_distance(bb1, bb2) < MINE_EXPLOSION_RADIUS) {
			tank->alive = false;
		}
		for(k = 0; k < max_shells[tank->type]; k++) {
			Shell* shell = &tank->shells[k];
			if(shell->alive && center_distance(getAABB(&mine->phys), getAABB(&shell->phys)) < MINE_EXPLOSION_RADIUS) {
				shell->alive = false;
			}
		}
		for(k = 0; k < max_mines[tank->type]; k++) {
			Mine* mine2 = &tank->mines[k];
			if(mine2->alive && center_distance(getAABB(&mine->phys), getAABB(&mine2->phys)) < MINE_EXPLOSION_RADIUS) {
				detonate(mine2, tiles);
			}
		}
	}
}

bool shell_ricochet(Shell* shell, Direction dir, uint8_t distance) {
	//Determine if shell explodes here, and subtracts 1 from the bounces left
	if(!shell->bounces--) {
		shell->alive = false;
		return false;
	}
	//Shell is still alive
	switch(dir) {
		case UP:
			shell->phys.position_y += 2 * distance << SHIFT_AMOUNT; //Reflect the position of the shell
			shell->phys.velocity_y *= -1; //Invert the velocity of the shell
			break;
		case DOWN:
			shell->phys.position_y -= 2 * distance << SHIFT_AMOUNT;
			shell->phys.velocity_y *= -1;
			break;
		case LEFT:
			shell->phys.position_x += 2 * distance << SHIFT_AMOUNT;
			shell->phys.velocity_x *= -1;
			break;
		case RIGHT:
			shell->phys.position_x -= 2 * distance << SHIFT_AMOUNT;
			shell->phys.velocity_x *= -1;
			break;
	}
	return true;
}
